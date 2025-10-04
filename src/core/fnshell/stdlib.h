#pragma once

namespace fnshell {
class Evaluator;

// 注册标准库函数（files/text/sys/list）
void register_stdlib(Evaluator& ev);

} // namespace fnshell
