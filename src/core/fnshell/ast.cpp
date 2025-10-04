#include "ast.h"

namespace fnshell {

ExprPtr Expr::literal(const std::string& v) {
    auto e = std::make_shared<Expr>();
    e->type = ExprType::Literal;
    e->value = v;
    return e;
}

ExprPtr Expr::ident(const std::string& name) {
    auto e = std::make_shared<Expr>();
    e->type = ExprType::Identifier;
    e->value = name;
    return e;
}

ExprPtr Expr::call(const std::string& name, const std::vector<ExprPtr>& args) {
    auto e = std::make_shared<Expr>();
    e->type = ExprType::Call;
    e->name = name;
    e->args = args;
    return e;
}

ExprPtr Expr::pipe(ExprPtr left, ExprPtr right) {
    auto e = std::make_shared<Expr>();
    e->type = ExprType::Pipe;
    e->left = left;
    e->right = right;
    return e;
}

} // namespace fnshell

