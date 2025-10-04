#pragma once

#include "parser.h"
#include "value.h"
#include <map>
#include <functional>

namespace fnshell {

using BuiltinFn = std::function<ValuePtr(const std::vector<ValuePtr>&)>;

class Evaluator {
public:
    Evaluator();
    ValuePtr eval(const ASTPtr& node);
    void register_builtin(const std::string& name, BuiltinFn fn);
private:
    std::map<std::string, BuiltinFn> builtins_;
};

} // namespace fnshell
