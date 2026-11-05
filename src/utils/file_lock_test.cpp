// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <atomic>
#include <chrono>
#include <fstream>
#include <thread>

#include "lib/doctest/doctest.h"
#include "utils/file_lock.h"

namespace linpipe {

namespace {

std::filesystem::path test_dir() {
  auto dir = std::filesystem::temp_directory_path() / "linpipe_file_lock_test";
  std::filesystem::create_directories(dir);
  return dir;
}

} // namespace

TEST_CASE("FileLock creates the lock file and removes it on release") {
  auto path = test_dir() / "create.lock";
  std::filesystem::remove(path);
  {
    FileLock lock(path);
    CHECK(std::filesystem::is_regular_file(path));
  }
  CHECK(!std::filesystem::exists(path));
}

TEST_CASE("FileLock reuses and removes a stale lock file") {
  auto path = test_dir() / "stale.lock";
  {
    std::ofstream stale(path);
  }
  REQUIRE(std::filesystem::is_regular_file(path));
  {
    FileLock lock(path);
  }
  CHECK(!std::filesystem::exists(path));
}

TEST_CASE("FileLock can be taken again after release") {
  auto path = test_dir() / "relock.lock";
  {
    FileLock lock(path);
  }
  {
    FileLock lock(path);
  }
  CHECK(true);
}

TEST_CASE("FileLock excludes other threads") {
  // With the lock file removed during unlocking, a possible problem is a waiter ending up
  // locking an already deleted file while a newcomer locks a newly-created one. We use more than
  // the three threads needed to manifest this issue to make it more likely to happen.
  auto path = test_dir() / "contention.lock";
  std::filesystem::remove(path);

  constexpr int threads = 8, iterations = 500;
  std::atomic<int> inside = 0, violations = 0;
  std::vector<std::thread> workers;
  workers.reserve(threads);
  for (int t = 0; t < threads; t++)
    workers.emplace_back([&] {
      for (int i = 0; i < iterations; i++) {
        FileLock lock(path);
        if (inside.fetch_add(1) != 0)
          violations++;
        std::this_thread::yield();
        inside.fetch_sub(1);
      }
    });
  for (auto& worker : workers)
    worker.join();

  CHECK(violations == 0);
  CHECK(!std::filesystem::exists(path));
}

TEST_CASE("FileLock throws when the directory does not exist") {
  auto path = test_dir() / "missing_dir" / "x.lock";
  std::filesystem::remove_all(path.parent_path());
  CHECK_THROWS_WITH_AS(FileLock{path}, doctest::Contains("Cannot open lock file"), LinpipeError);
}

} // namespace linpipe
