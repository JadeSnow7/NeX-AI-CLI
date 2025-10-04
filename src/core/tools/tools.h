#pragma once

#include <string>
#include <vector>
#include <functional>
#include <optional>

namespace tools {

struct ToolResult {
    bool ok;
    std::string output_json; // 结果 JSON 字符串
    std::string error;
};

using ToolFn = std::function<ToolResult(const std::string& args_json)>;

struct ToolDesc {
    std::string name;
    std::string description;
    ToolFn run;
};

class Registry {
public:
    static Registry& instance();
    void register_tool(const ToolDesc& desc);
    std::vector<ToolDesc> list() const;
    std::optional<ToolDesc> find(const std::string& name) const;
private:
    std::vector<ToolDesc> items_;
};

// 内置工具
void register_builtin_tools();

} // namespace tools




