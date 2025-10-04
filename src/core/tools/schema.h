#pragma once

#include <string>
#include <optional>
#include <vector>

namespace tools {

// 简单 JSON 字段校验
struct SchemaError {
    std::string field;
    std::string message;
};

// 校验 JSON 字符串是否包含必需字段，返回 nullopt 表示通过，否则返回错误
std::optional<SchemaError> validate_required_fields(const std::string& json, const std::vector<std::string>& required);

// 提取 JSON 字符串字段值
std::optional<std::string> extract_string_field(const std::string& json, const std::string& key);

} // namespace tools

