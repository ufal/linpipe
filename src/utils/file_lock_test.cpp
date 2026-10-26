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

TEST_CASE("FileLock creates the lock file and keeps it") {
  auto path = test_dir() / "create.lock";
  std::filesystem::remove(path);
  {
    FileLock lock(path);
    CHECK(std::filesystem::is_regular_file(path));
  }
  CHECK(std::filesystem::is_regular_file(path));
}

TEST_CASE("FileLock can be taken again after release") {
  auto path = test_dir() / "relock.lock";
  { FileLock lock(path); }
  { FileLock lock(path); }
  CHECK(true);
}

TEST_CASE("FileLock excludes other threads") {
  auto path = test_dir() / "threads.lock";
  std::atomic<bool> second_locked = false;
  std::thread second;
  {
    FileLock lock(path);
    second = std::thread([&] { FileLock lock(path); second_locked = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    CHECK(!second_locked);
  }
  second.join();
  CHECK(second_locked);
}

TEST_CASE("FileLock throws when the directory does not exist") {
  auto path = test_dir() / "missing_dir" / "x.lock";
  std::filesystem::remove_all(path.parent_path());
  CHECK_THROWS_WITH_AS(FileLock{path}, doctest::Contains("Cannot open lock file"), LinpipeError);
}

} // namespace linpipe
