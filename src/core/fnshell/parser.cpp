#include "parser.h"
#include <cctype>

namespace fnshell {

void Parser::skip_ws() {
    while (pos_ < input_.size() && std::isspace((unsigned char)input_[pos_])) ++pos_;
}

std::optional<ASTPtr> Parser::parse(const std::string& expr, std::string& err) {
    input_ = expr; pos_ = 0;
    auto root = parse_expr();
    if (!root) { err = "parse failed"; return std::nullopt; }
    return root;
}

std::optional<ASTPtr> Parser::parse_expr() {
    skip_ws();
    auto left = parse_primary();
    if (!left) return std::nullopt;
    skip_ws();
    if (pos_ < input_.size() && input_[pos_] == '|') {
        ++pos_; // skip |
        auto right = parse_expr();
        if (!right) return std::nullopt;
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::Pipe;
        node->left = *left;
        node->right = *right;
        return node;
    }
    return left;
}

std::optional<ASTPtr> Parser::parse_primary() {
    skip_ws();
    if (pos_ >= input_.size()) return std::nullopt;
    char c = input_[pos_];
    // 字符串字面量
    if (c == '"') {
        ++pos_; size_t end = pos_;
        while (end < input_.size() && input_[end] != '"') ++end;
        if (end >= input_.size()) return std::nullopt;
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::Literal;
        node->value = input_.substr(pos_, end - pos_);
        pos_ = end + 1;
        return node;
    }
    // 数字字面量
    if (std::isdigit((unsigned char)c) || c == '-') {
        size_t end = pos_;
        if (c == '-') ++end;
        while (end < input_.size() && (std::isdigit((unsigned char)input_[end]) || input_[end] == '.')) ++end;
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::Literal;
        node->value = input_.substr(pos_, end - pos_);
        pos_ = end;
        return node;
    }
    // 标识符或函数调用
    if (std::isalpha((unsigned char)c) || c == '_') {
        size_t end = pos_;
        while (end < input_.size() && (std::isalnum((unsigned char)input_[end]) || input_[end] == '_' || input_[end] == '.')) ++end;
        std::string name = input_.substr(pos_, end - pos_);
        pos_ = end;
        skip_ws();
        // 检查是否函数调用
        if (pos_ < input_.size() && input_[pos_] == '(') {
            ++pos_; // skip (
            std::vector<ASTPtr> args;
            skip_ws();
            if (pos_ < input_.size() && input_[pos_] != ')') {
                while (true) {
                    auto arg = parse_primary();
                    if (!arg) break;
                    args.push_back(*arg);
                    skip_ws();
                    if (pos_ < input_.size() && input_[pos_] == ',') { ++pos_; skip_ws(); continue; }
                    break;
                }
            }
            if (pos_ >= input_.size() || input_[pos_] != ')') return std::nullopt;
            ++pos_; // skip )
            auto node = std::make_shared<ASTNode>();
            node->type = NodeType::Call;
            node->name = name;
            node->args = args;
            return node;
        }
        // 标识符
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::Identifier;
        node->value = name;
        return node;
    }
    return std::nullopt;
}

} // namespace fnshell
