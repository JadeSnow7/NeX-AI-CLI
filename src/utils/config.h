#pragma once

#include <optional>
#include <string>

namespace config {

void initialize_from_env();
std::optional<std::string> get_env(const std::string& key);

} // namespace config







