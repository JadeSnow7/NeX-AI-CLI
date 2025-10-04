#include "schema.h"
#include <algorithm>
#include <vector>

namespace tools {

std::optional<SchemaError> validate_required_fields(const std::string& json, const std::vector<std::string>& required) {
    for (const auto& field : required) {
        std::string key = "\"" + field + "\"";
        if (json.find(key) == std::string::npos) {
            return SchemaError{field, "required field missing"};
        }
    }
    return std::nullopt;
}

std::optional<std::string> extract_string_field(const std::string& json, const std::string& key) {
    auto p = json.find("\"" + key + "\"");
    if (p == std::string::npos) return std::nullopt;
    auto s = json.find('"', p + key.size() + 2);
    if (s == std::string::npos) return std::nullopt;
    auto e = json.find('"', s + 1);
    if (e == std::string::npos) return std::nullopt;
    return json.substr(s + 1, e - s - 1);
}

} // namespace tools



