#include "evaluator.h"
#include "stdlib.h"

namespace fnshell {

Evaluator::Evaluator() {
    register_stdlib(*this);
}

void Evaluator::register_builtin(const std::string& name, BuiltinFn fn) {
    builtins_[name] = fn;
}

ValuePtr Evaluator::eval(const ASTPtr& node) {
    if (!node) return Value::make_error("null node");
    switch (node->type) {
        case NodeType::Literal: {
            // 尝试解析为数字或字符串
            if (node->value.find('.') != std::string::npos) {
                try { return Value::make_float(std::stod(node->value)); } catch (...) {}
            }
            try { return Value::make_int(std::stoll(node->value)); } catch (...) {}
            return Value::make_string(node->value);
        }
        case NodeType::Identifier: {
            // 若是已注册函数名，返回函数值；否则视为字符串字面量
            if (builtins_.find(node->value) != builtins_.end()) {
                return Value::make_function(node->value);
            }
            return Value::make_string(node->value);
        }
        case NodeType::Call: {
            auto it = builtins_.find(node->name);
            if (it == builtins_.end()) return Value::make_error("unknown function: " + node->name);
            std::vector<ValuePtr> args;
            for (auto& a : node->args) {
                auto v = eval(a);
                if (v->is_error()) return v;
                args.push_back(v);
            }
            return it->second(args);
        }
        case NodeType::Pipe: {
            auto left_val = eval(node->left);
            // 管道允许传递错误，让右侧处理（如 unwrap_or）
            // if (left_val->is_error()) return left_val;
            // Pipe: 右侧可能是 Call 或 Identifier（函数名），将左侧结果作为首参
            if (!node->right) return Value::make_error("pipe right is null");
            if (node->right->type == NodeType::Identifier) {
                // 简化：视为无参调用，左值作为唯一参数
                std::string fn_name = node->right->value;
                auto it = builtins_.find(fn_name);
                if (it == builtins_.end()) return Value::make_error("unknown function: " + fn_name);
                return it->second({left_val});
            }
            if (node->right->type == NodeType::Call) {
                auto it = builtins_.find(node->right->name);
                if (it == builtins_.end()) return Value::make_error("unknown function: " + node->right->name);
                std::vector<ValuePtr> args;
                args.push_back(left_val);
                for (auto& a : node->right->args) {
                    auto v = eval(a);
                    if (v->is_error()) return v;
                    args.push_back(v);
                }
                return it->second(args);
            }
            return Value::make_error("pipe right must be identifier or call");
        }
    }
    return Value::make_error("unknown node type");
}

} // namespace fnshell
