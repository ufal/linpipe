// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <atomic>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <random>
#include <thread>

#include "lib/doctest/doctest.h"
#include "lib/httplib/httplib.h"
#include "lib/json/json.h"
#include "models/model_hub.h"
#include "utils/path_utf8.h"

namespace linpipe {

namespace {

// Sets (or unsets, when value is nullptr) an environment variable and
// restores its original value on destruction.
class EnvGuard {
 public:
  EnvGuard(const char* name, const char* value) : name_(name) {
    if (const char* old = std::getenv(name)) old_value_ = old;
    set(name, value);
  }
  ~EnvGuard() { set(name_.c_str(), old_value_ ? old_value_->c_str() : nullptr); }

  EnvGuard(const EnvGuard&) = delete;
  EnvGuard& operator=(const EnvGuard&) = delete;

 private:
  static void set(const char* name, const char* value) {
#ifdef _WIN32
    _putenv_s(name, value ? value : "");  // An empty value removes the variable.
#else
    if (value) setenv(name, value, 1); else unsetenv(name);
#endif
  }

  std::string name_;
  std::optional<std::string> old_value_;
};

// Sets the logging level and restores the original one on destruction.
class LoggingGuard {
 public:
  explicit LoggingGuard(int level) : old_level_(logging_level) { logging_level = level; }
  ~LoggingGuard() { logging_level = old_level_; }

  LoggingGuard(const LoggingGuard&) = delete;
  LoggingGuard& operator=(const LoggingGuard&) = delete;

 private:
  int old_level_;
};

// Creates a unique temporary directory path and removes it on destruction.
// The directory itself is not created, so that ModelHub can create it.
class TempDir {
 public:
  TempDir() {
    std::random_device random;
    path_ = std::filesystem::temp_directory_path() / ("linpipe_model_hub_test_" + std::to_string(random()));
    std::filesystem::remove_all(path_);
  }
  ~TempDir() { std::error_code ec; std::filesystem::remove_all(path_, ec); }

  TempDir(const TempDir&) = delete;
  TempDir& operator=(const TempDir&) = delete;

  const std::filesystem::path& path() const { return path_; }
  std::string utf8() const { return path_to_utf8(path_); }

 private:
  std::filesystem::path path_;
};

// A local HTTP server running in a background thread, serving given contents
// on given paths and counting the requests; any other path returns 404.
class TestServer {
 public:
  TestServer() {
    server_.Get(".*", [this](const httplib::Request& req, httplib::Response& res) {
      std::lock_guard<std::mutex> lock(mutex_);
      requests_++;
      auto it = contents_.find(req.path);
      if (it == contents_.end()) {
        res.status = 404;
      } else {
        res.status = 200;
        res.set_content(it->second, "application/json");
      }
    });
    port_ = server_.bind_to_any_port("127.0.0.1");
    REQUIRE_MESSAGE(port_ > 0, "cannot bind the test HTTP server");
    thread_ = std::thread([this] { server_.listen_after_bind(); });
    server_.wait_until_ready();
  }
  ~TestServer() { stop(); }

  TestServer(const TestServer&) = delete;
  TestServer& operator=(const TestServer&) = delete;

  void serve(const std::string& path, const std::string& content) {
    std::lock_guard<std::mutex> lock(mutex_);
    contents_[path] = content;
  }

  std::string url(const std::string& path) const { return "http://127.0.0.1:" + std::to_string(port_) + path; }

  int requests() const { return requests_; }

  // Stops the server; afterwards, connections to its port are refused.
  void stop() {
    if (thread_.joinable()) {
      server_.stop();
      thread_.join();
    }
  }

 private:
  httplib::Server server_;
  std::thread thread_;
  int port_ = -1;
  std::mutex mutex_;
  std::map<std::string, std::string> contents_;
  std::atomic<int> requests_ = 0;
};

// Returns an overview JSON with the given timestamp and number of models.
std::string overview(const std::string& timestamp, int models = 1) {
  Json json = {{"timestamp", timestamp}, {"models", Json::object()}};
  for (int i = 0; i < models; i++)
    json["models"]["model_" + std::to_string(i)] = {{"url", "https://example.com/model.json"}, {"date", "2026-10-05"}};
  return json.dump();
}

// Reads the given file; returns an empty string if it cannot be read.
std::string read(const std::filesystem::path& path) {
  std::ifstream is(path, std::ios::binary);
  return {std::istreambuf_iterator<char>(is), std::istreambuf_iterator<char>()};
}

void write(const std::filesystem::path& path, const std::string& content) {
  std::filesystem::create_directories(path.parent_path());
  std::ofstream(path, std::ios::binary) << content;
}

} // namespace

TEST_CASE("ModelHub directory") {
  SUBCASE("throws when the default directory cannot be determined") {
#if defined(_WIN32)
    EnvGuard local_app_data("LOCALAPPDATA", nullptr), user_profile("USERPROFILE", nullptr);
#elif defined(__APPLE__)
    EnvGuard home("HOME", nullptr);
#else
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", nullptr), home("HOME", nullptr);
#endif
    CHECK_THROWS_AS(ModelHub{}, LinpipeError);
    CHECK_THROWS_WITH_AS(ModelHub{}, doctest::Contains("ModelHub directory"), LinpipeError);
  }

#if !defined(_WIN32) && !defined(__APPLE__)
  SUBCASE("ignores relative XDG_CACHE_HOME and throws without HOME") {
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", "relative/cache"), home("HOME", nullptr);
    CHECK_THROWS_AS(ModelHub{}, LinpipeError);
  }
#endif

  SUBCASE("does not throw when the directory is given explicitly") {
#if defined(_WIN32)
    EnvGuard local_app_data("LOCALAPPDATA", nullptr), user_profile("USERPROFILE", nullptr);
#else
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", nullptr), home("HOME", nullptr);
#endif
    CHECK_NOTHROW(ModelHub{"explicit/model_hub"});
    CHECK(ModelHub{"explicit/model_hub"}.get_dir() == "explicit/model_hub");
  }

#if defined(_WIN32)
  SUBCASE("uses LOCALAPPDATA") {
    EnvGuard local_app_data("LOCALAPPDATA", "C:\\Local"), user_profile("USERPROFILE", nullptr);
    CHECK(ModelHub{}.get_dir() == "C:\\Local\\linpipe\\model_hub");
  }

  SUBCASE("falls back to USERPROFILE") {
    EnvGuard local_app_data("LOCALAPPDATA", nullptr), user_profile("USERPROFILE", "C:\\Users\\u");
    CHECK(ModelHub{}.get_dir() == "C:\\Users\\u\\AppData\\Local\\linpipe\\model_hub");
  }
#elif defined(__APPLE__)
  SUBCASE("uses ~/Library/Caches") {
    EnvGuard home("HOME", "/Users/u");
    CHECK(ModelHub{}.get_dir() == "/Users/u/Library/Caches/linpipe/model_hub");
  }
#else
  SUBCASE("uses XDG_CACHE_HOME") {
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", "/xdg"), home("HOME", "/home/u");
    CHECK(ModelHub{}.get_dir() == "/xdg/linpipe/model_hub");
  }

  SUBCASE("falls back to ~/.cache") {
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", nullptr), home("HOME", "/home/u");
    CHECK(ModelHub{}.get_dir() == "/home/u/.cache/linpipe/model_hub");
  }

  SUBCASE("falls back to ~/.cache with relative XDG_CACHE_HOME") {
    EnvGuard xdg_cache_home("XDG_CACHE_HOME", "relative/cache"), home("HOME", "/home/u");
    CHECK(ModelHub{}.get_dir() == "/home/u/.cache/linpipe/model_hub");
  }
#endif
}

TEST_CASE("ModelHub repository overview") {
  LoggingGuard logging_guard(LOGGING_FATAL);
  TempDir dir;
  TestServer server;
  auto cache = dir.path() / ModelHub::repo_json_name;
  auto tmp = cache; tmp += ".tmp";

  const std::string older = overview("2026-10-01T00:00:00Z", 1);
  const std::string newer = overview("2026-10-05T12:00:00Z", 2);
  server.serve("/older.json", older);
  server.serve("/newer.json", newer);

  SUBCASE("creates the directory and downloads the overview") {
    ModelHub hub(dir.utf8(), server.url("/older.json"));
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(std::filesystem::is_directory(dir.path()));
    CHECK(read(cache) == older);
  }

  SUBCASE("creates nested directories") {
    auto nested = dir.path() / "a" / "b";
    ModelHub hub(path_to_utf8(nested), server.url("/older.json"));
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(read(nested / ModelHub::repo_json_name) == older);
  }

  SUBCASE("downloads the overview into an existing directory") {
    std::filesystem::create_directories(dir.path());
    ModelHub hub(dir.utf8(), server.url("/older.json"));
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(read(cache) == older);
  }

  SUBCASE("replaces an older cached overview") {
    write(cache, older);
    ModelHub hub(dir.utf8(), server.url("/newer.json"));
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(read(cache) == newer);
  }

  SUBCASE("keeps a newer cached overview") {
    write(cache, newer);
    ModelHub hub(dir.utf8(), server.url("/older.json"));
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(read(cache) == newer);
  }

  SUBCASE("keeps a cached overview with the same timestamp") {
    auto same = overview("2026-10-01T00:00:00Z", 3);
    write(cache, same);
    ModelHub hub(dir.utf8(), server.url("/older.json"));
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(read(cache) == same);
  }

  SUBCASE("replaces a corrupt cached overview") {
    write(cache, "garbage");
    ModelHub hub(dir.utf8(), server.url("/older.json"));
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(read(cache) == older);
  }

  SUBCASE("fetches the overview only once per instance") {
    ModelHub hub(dir.utf8(), server.url("/older.json"));
    CHECK(server.requests() == 0);
    hub.get_model("model_0");
    hub.get_model("model_0");
    hub.get_model("model_1");
    CHECK(server.requests() == 1);
  }

  SUBCASE("uses the cached overview when the remote one is unusable") {
    server.serve("/portal.json", "<html>Please log in</html>");
    server.serve("/array.json", "[]");
    server.serve("/no_timestamp.json", R"({"models": {}})");
    server.serve("/no_models.json", R"({"timestamp": "2026-12-01T00:00:00Z"})");
    server.serve("/fractional.json", overview("2026-12-01T00:00:00.5Z"));
    server.serve("/offset.json", overview("2026-12-01T00:00:00+02:00"));

    write(cache, older);
    for (auto path : {"/portal.json", "/array.json", "/no_timestamp.json", "/no_models.json",
                      "/fractional.json", "/offset.json", "/missing.json"}) {
      CAPTURE(path);
      ModelHub hub(dir.utf8(), server.url(path));
      CHECK_NOTHROW(hub.get_model("model_0"));
      CHECK(read(cache) == older);
    }

    auto unreachable = server.url("/newer.json");
    server.stop();
    ModelHub hub(dir.utf8(), unreachable);
    CHECK_NOTHROW(hub.get_model("model_0"));
    CHECK(read(cache) == older);
  }

  SUBCASE("throws when there is neither a usable cached nor a remote overview") {
    server.serve("/portal.json", "<html>Please log in</html>");
    for (auto url : {server.url("/portal.json"), server.url("/missing.json"), std::string("no_scheme")}) {
      CAPTURE(url);
      ModelHub hub(dir.utf8(), url);
      CHECK_THROWS_WITH_AS(hub.get_model("model_0"), doctest::Contains("ModelHub::ensure_local_repo:"), LinpipeError);
      CHECK(!std::filesystem::exists(cache));
    }

    server.serve("/array.json", "[]");
    ModelHub array_hub(dir.utf8(), server.url("/array.json"));
    CHECK_THROWS_WITH_AS(array_hub.get_model("model_0"), doctest::Contains("is not a JSON object"), LinpipeError);

    write(cache, "garbage");
    auto unreachable = server.url("/older.json");
    server.stop();
    ModelHub hub(dir.utf8(), unreachable);
    CHECK_THROWS_WITH_AS(hub.get_model("model_0"), doctest::Contains("ModelHub::ensure_local_repo:"), LinpipeError);
  }

  SUBCASE("throws when the directory cannot be created") {
    write(dir.path() / "file", "not a directory");
    ModelHub hub(path_to_utf8(dir.path() / "file"), server.url("/older.json"));
    CHECK_THROWS_WITH_AS(hub.get_model("model_0"), doctest::Contains("Cannot create ModelHub directory"), LinpipeError);
  }

  CHECK(!std::filesystem::exists(tmp));
}

} // namespace linpipe
