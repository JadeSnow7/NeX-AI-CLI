#include "logging.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

namespace logging {

static Level g_level = Level::Info;

static const char* level_name(Level lv) {
    switch (lv) {
        case Level::Trace: return "TRACE";
        case Level::Debug: return "DEBUG";
        case Level::Info:  return "INFO";
        case Level::Warn:  return "WARN";
        case Level::Error: return "ERROR";
    }
    return "INFO";
}

void initialize() {
    if (const char* env = std::getenv("AICLI_LOG_LEVEL")) {
        std::string v(env);
        if (v == "trace") g_level = Level::Trace;
        else if (v == "debug") g_level = Level::Debug;
        else if (v == "info") g_level = Level::Info;
        else if (v == "warn") g_level = Level::Warn;
        else if (v == "error") g_level = Level::Error;
    }
}

void log(Level level, const std::string& message) {
    if (static_cast<int>(level) < static_cast<int>(g_level)) {
        return;
    }
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    std::cerr << '[' << buf << "] " << level_name(level) << ": " << message << '\n';
}

} // namespace logging





