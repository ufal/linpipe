// This file is part of LinPipe <http://github.com/ufal/linpipe/>.
//
// Copyright 2022-2023 Institute of Formal and Applied Linguistics, Faculty
// of Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <ctime>
#include <fstream>

#include "utils/logging.h"

namespace linpipe {

int logging_level = LOGGING_INFO;
bool logging_to_file = false;

bool logging_sources = false;

namespace {

static bool logging_last_progress = false;
static ofstream logging_file;

class LoggingInit {
 private:
  LoggingInit() {
    auto env_log_level = getenv("LINPIPE_LOG_LEVEL");
    if (env_log_level)
      logging_set_level(env_log_level);

    auto env_log_file = getenv("LINPIPE_LOG_FILE");
    if (env_log_file)
      logging_set_file(filesystem::u8path(env_log_file));
  }
  static LoggingInit singleton;
};
LoggingInit LoggingInit::singleton;

}

void logging_set_level(string_view level) {
  logging_sources = false;
  if (level.size() >= 2 && (level.compare(level.size() - 2, 2, "+s") == 0 || level.compare(level.size() - 2, 2, "+S") == 0)) {
    logging_sources = true;
    level.remove_suffix(2);
  } else if (level.size() >= 8 && (level.compare(level.size() - 8, 8, "+sources") == 0 || level.compare(level.size() - 8, 8, "+SOURCES") == 0)) {
    logging_sources = true;
    level.remove_suffix(8);
  }

  if (level == "t" || level == "T" || level == "trace" || level == "TRACE")
    logging_level = LOGGING_TRACE;
  else if (level == "i" || level == "I" || level == "info" || level == "INFO")
    logging_level = LOGGING_INFO;
  else if (level == "p" || level == "P" || level == "progress" || level == "PROGRESS")
    logging_level = LOGGING_PROGRESS;
  else if (level == "w" || level == "W" || level == "warn" || level == "WARN")
    logging_level = LOGGING_WARN;
  else if (level == "e" || level == "E" || level == "error" || level == "ERROR")
    logging_level = LOGGING_ERROR;
  else if (level == "f" || level == "F" || level == "fatal" || level == "FATAL" || level == "none" || level == "NONE")
    logging_level = LOGGING_FATAL;
  else
    throw LinpipeError{"logging_set_level: Cannot parse logging level '", level, "'"};
}

void logging_set_file(filesystem::path path) {
  logging_file.open(path, ios::out | ios::app);
  if (!logging_file.is_open())
    throw LinpipeError{"logging_set_file: Cannot redirect logs to file '", path.u8string(), "'"};

  logging_to_file = true;
}

ostream& logging_start(int level, const char* source, int line) {
  ostream& logger = logging_to_file ? logging_file : cerr;

  if (level != LOGGING_PROGRESS && logging_last_progress) logger.put('\n');
  logging_last_progress = level == LOGGING_PROGRESS;

  time_t now;
  time(&now);
  char date_time[6 + 1 + 6 + 1];
  strftime(date_time, size(date_time), "%y%m%d-%H%M%S", localtime(&now));
  logger.write(date_time, sizeof(date_time) - 1);

  if (logging_sources)
    logger << ' ' << source << ':' << line;

  logger.put(' ');
  logger.put("TIPWEF"[level]);
  logger.put(' ');

  return logger;
}

} // namespace linpipe
