#include "value.h"
#include <sstream>

namespace fnshell {

std::string Value::to_string() const {
    switch (type) {
        case Type::Nil: return "nil";
        case Type::Int: return std::to_string(std::get<int64_t>(data));
        case Type::Float: return std::to_string(std::get<double>(data));
        case Type::String: return "\"" + std::get<std::string>(data) + "\"";
        case Type::Bool: return std::get<bool>(data) ? "true" : "false";
        case Type::List: {
            std::ostringstream oss; oss << "[";
            auto& lst = std::get<List>(data);
            for (size_t i = 0; i < lst.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << lst[i]->to_string();
            }
            oss << "]";
            return oss.str();
        }
        case Type::Error: return "Error: " + std::get<std::string>(data);
        case Type::Function: return "<function:" + std::get<FunctionValue>(data).name + ">";
    }
    return "unknown";
}

} // namespace fnshell
