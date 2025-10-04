#pragma once

#include <string>
#include <vector>
#include <memory>

namespace fnshell {

struct Expr;
using ExprPtr = std::shared_ptr<Expr>;

enum class ExprType { Literal, Identifier, Call, Pipe };

struct Expr {
    ExprType type;
    std::string value;          // Literal/Identifier
    std::string name;           // Call
    std::vector<ExprPtr> args;  // Call
    ExprPtr left;               // Pipe
    ExprPtr right;              // Pipe

    static ExprPtr literal(const std::string& v);
    static ExprPtr ident(const std::string& name);
    static ExprPtr call(const std::string& name, const std::vector<ExprPtr>& args);
    static ExprPtr pipe(ExprPtr left, ExprPtr right);
};

} // namespace fnshell

