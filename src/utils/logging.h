#pragma once

#include <string>

namespace logging {

enum class Level { Trace, Debug, Info, Warn, Error };

void initialize();
void log(Level level, const std::string& message);

} // namespace logging





