#pragma once

#include <string>
#include <vector>

namespace tools {

struct ToolsConfig {
    int timeout_ms = 5000;
    std::vector<std::string> shell_whitelist = {"ls","cat","echo","stat","wc","pwd","date"};
    size_t max_output_bytes = 262144;
    std::string fs_sandbox_root = ".";
    bool fs_allow_write = true;
};

// 从 config/tools.yaml 或环境变量加载，返回全局配置实例
ToolsConfig& get_config();

} // namespace tools



