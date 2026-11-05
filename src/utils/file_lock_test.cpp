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
#include "utils/test_utils.h"

namespace linpipe {

TEST_CASE("FileLock creates the lock file and removes it on release") {
  TempDir dir("linpipe_file_lock_test", true);
  auto path = dir.path() / "create.lock";
  {
    FileLock lock(path);
    CHECK(std::filesystem::is_regular_file(path));
  }
  CHECK(!std::filesystem::exists(path));
}

TEST_CASE("FileLock reuses and removes a stale lock file") {
  TempDir dir("linpipe_file_lock_test", true);
  auto path = dir.path() / "stale.lock";
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
  TempDir dir("linpipe_file_lock_test", true);
  auto path = dir.path() / "relock.lock";
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
  TempDir dir("linpipe_file_lock_test", true);
  auto path = dir.path() / "contention.lock";

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
  TempDir dir("linpipe_file_lock_test", true);
  auto path = dir.path() / "missing_dir" / "x.lock";
  CHECK_THROWS_WITH_AS(FileLock{path}, doctest::Contains("Cannot open lock file"), LinpipeError);
}

} // namespace linpipe
