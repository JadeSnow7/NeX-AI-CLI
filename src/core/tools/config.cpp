#include "config.h"
#include <cstdlib>

namespace tools {

ToolsConfig& get_config() {
    static ToolsConfig cfg;
    static bool loaded = false;
    if (!loaded) {
        loaded = true;
        // 预留：从 config/tools.yaml 解析（需 YAML 库）；暂用默认值
        if (const char* env = std::getenv("AICLI_TOOL_TIMEOUT_MS")) {
            try { cfg.timeout_ms = std::stoi(env); } catch (...) {}
        }
    }
    return cfg;
}

} // namespace tools



