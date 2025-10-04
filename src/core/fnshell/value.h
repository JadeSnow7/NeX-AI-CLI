#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>

namespace fnshell {

struct Value;

using ValuePtr = std::shared_ptr<Value>;
using List = std::vector<ValuePtr>;

struct FunctionValue { std::string name; };

struct Value {
    enum class Type { Nil, Int, Float, String, Bool, List, Error, Function };
    Type type;
    std::variant<int64_t, double, std::string, bool, List, FunctionValue> data;

    static ValuePtr nil() { auto v = std::make_shared<Value>(); v->type = Type::Nil; return v; }
    static ValuePtr make_int(int64_t i) { auto v = std::make_shared<Value>(); v->type = Type::Int; v->data = i; return v; }
    static ValuePtr make_float(double d) { auto v = std::make_shared<Value>(); v->type = Type::Float; v->data = d; return v; }
    static ValuePtr make_string(const std::string& s) { auto v = std::make_shared<Value>(); v->type = Type::String; v->data = s; return v; }
    static ValuePtr make_bool(bool b) { auto v = std::make_shared<Value>(); v->type = Type::Bool; v->data = b; return v; }
    static ValuePtr make_list(const List& l) { auto v = std::make_shared<Value>(); v->type = Type::List; v->data = l; return v; }
    static ValuePtr make_error(const std::string& msg) { auto v = std::make_shared<Value>(); v->type = Type::Error; v->data = msg; return v; }
    static ValuePtr make_function(const std::string& fn_name) { auto v = std::make_shared<Value>(); v->type = Type::Function; v->data = FunctionValue{fn_name}; return v; }

    bool is_error() const { return type == Type::Error; }
    std::string to_string() const;
};

} // namespace fnshell
