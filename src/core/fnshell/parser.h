#pragma once

#include <string>
#include <memory>
#include <optional>
#include <vector>

namespace fnshell {

enum class NodeType { Literal, Identifier, Call, Pipe };

struct ASTNode {
    NodeType type;
    std::string value;  // for Literal/Identifier
    std::string name;   // for Call
    std::vector<std::shared_ptr<ASTNode>> args; // for Call
    std::shared_ptr<ASTNode> left;  // for Pipe
    std::shared_ptr<ASTNode> right; // for Pipe
};

using ASTPtr = std::shared_ptr<ASTNode>;

class Parser {
public:
    std::optional<ASTPtr> parse(const std::string& expr, std::string& err);
private:
    std::string input_;
    size_t pos_ = 0;
    void skip_ws();
    std::optional<ASTPtr> parse_expr();
    std::optional<ASTPtr> parse_primary();
};

} // namespace fnshell
