// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2026 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>
#include <fstream>

#include "utils/getenv_utf8.h"
#include "utils/logging.h"
#include "utils/path_utf8.h"

namespace linpipe {

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
LoggingLevel logging_level = LoggingLevel::LEVEL_INFO;
bool logging_to_file = false;
// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

namespace {

bool logging_sources = false;
bool logging_last_progress = false;
std::ofstream logging_file;

class LoggingInit {
 private:
  LoggingInit() {
    auto env_log_level = getenv_utf8("LINPIPE_LOG_LEVEL");
    if (env_log_level)
      logging_set_level(*env_log_level);

    auto env_log_file = getenv_utf8("LINPIPE_LOG_FILE");
    if (env_log_file)
      logging_set_file(path_from_utf8(*env_log_file));
  }
  static LoggingInit singleton;
};
LoggingInit LoggingInit::singleton;

} // namespace

void logging_set_level(std::string_view level) {
  logging_sources = false;
  if (level.size() >= 2 && (level.ends_with("+s") || level.ends_with("+S"))) {
    logging_sources = true;
    level.remove_suffix(2);
  } else if (level.size() >= 8 && (level.ends_with("+sources") || level.ends_with("+SOURCES"))) {
    logging_sources = true;
    level.remove_suffix(8);
  }

  if (level == "t" || level == "T" || level == "trace" || level == "TRACE")
    logging_level = LoggingLevel::LEVEL_TRACE;
  else if (level == "i" || level == "I" || level == "info" || level == "INFO")
    logging_level = LoggingLevel::LEVEL_INFO;
  else if (level == "p" || level == "P" || level == "progress" || level == "PROGRESS")
    logging_level = LoggingLevel::LEVEL_PROGRESS;
  else if (level == "w" || level == "W" || level == "warn" || level == "WARN")
    logging_level = LoggingLevel::LEVEL_WARN;
  else if (level == "e" || level == "E" || level == "error" || level == "ERROR")
    logging_level = LoggingLevel::LEVEL_ERROR;
  else if (level == "f" || level == "F" || level == "fatal" || level == "FATAL")
    logging_level = LoggingLevel::LEVEL_FATAL;
  else
    LOG(ERROR, "logging_set_level: Cannot parse logging level '" << level << "'");
}

void logging_set_file(const std::filesystem::path& path) {
  logging_file.open(path, std::ios::out | std::ios::app);
  if (!logging_file.is_open())
    LOG(ERROR, "logging_set_file: Cannot redirect logs to file '" << path_to_utf8(path) << "'");
  else
    logging_to_file = true;
}

std::ostream& logging_start(LoggingLevel level, const char* source, int line) {
  std::ostream& logger = logging_to_file ? logging_file : std::cerr;

  if (level != LoggingLevel::LEVEL_PROGRESS && logging_last_progress) logger.put('\n');
  logging_last_progress = level == LoggingLevel::LEVEL_PROGRESS;

  time_t now = 0;
  time(&now);
  char date_time[6 + 1 + 6 + 1];
  strftime(date_time, std::size(date_time), "%y%m%d-%H%M%S", localtime(&now));
  logger.write(date_time, sizeof(date_time) - 1);

  if (logging_sources)
    logger << ' ' << source << ':' << line;

  logger.put(' ');
  logger.put("TIPWEF"[static_cast<int>(level)]);
  logger.put(' ');

  return logger;
}

} // namespace linpipe
