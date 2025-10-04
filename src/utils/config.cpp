#include "config.h"

#include <cstdlib>
#include <optional>
#include <string>

namespace config {

void initialize_from_env() {
    (void)0; // 预留：读取 AICLI_CONFIG 等环境变量，加载 YAML
}

std::optional<std::string> get_env(const std::string& key) {
    if (const char* v = std::getenv(key.c_str())) {
        return std::string(v);
    }
    return std::nullopt;
}

} // namespace config







