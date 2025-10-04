# 贡献指南

## 开发环境

- C++20、CMake 3.24+
- 可选：CUDA Toolkit（GPU）、libsqlite3-dev（持久化）
- 代码风格：遵循现有缩进与命名惯例

## 构建与测试

```bash
# 完整构建（本地推理 + SQLite）
cmake -S . -B build -DAICLI_WITH_LLAMA=ON -DAICLI_WITH_SQLITE=ON -DAICLI_BUILD_TESTS=ON
cmake --build build -j

# 运行测试
./build/test_cli_repl

# 运行主程序
./build/aicli
```

## 代码结构

- `src/cli/`：REPL 与命令处理
- `src/core/inference/`：推理引擎（本地 llama、远端 Provider）
- `src/core/conversation/`：会话与模板
- `src/core/tools/`：工具系统与 Schema
- `src/core/fnshell/`：函数式 Shell 解析器与运行时
- `src/core/sysbox/`：观测与指标
- `src/core/storage/`：SQLite 持久化
- `src/utils/`：日志、配置、文件系统

## 提交规范

- 提交信息：`[模块] 简短描述`
- 示例：`[tools] 增加 fs.list_dir 工具`、`[fnshell] 修复 filter 谓词调用`

## 添加新工具

1. 在 `src/core/tools/tools.cpp` 中实现函数
2. 使用 `validate_required_fields` 校验参数
3. 调用 `sysbox::record_json` 与 `storage::log_tool_invocation`
4. 在 `register_builtin_tools()` 中注册
5. 更新 `docs/usage.md` 与 `assets/schema/tool.schema.json`

## 添加函数式 Shell 函数

1. 在 `src/core/fnshell/stdlib.cpp` 实现
2. 签名：`static ValuePtr fn_name(const std::vector<ValuePtr>& args)`
3. 在 `register_stdlib()` 中注册
4. 更新 `docs/fnshell.md`

## 性能优化

- 使用 `sysbox::ScopedTimer` 测量新增路径
- 避免频繁上下文重建（优化会话切换）
- KV cache 更精细控制（考虑 llama_memory_seq_* API）

## 测试

- 单元测试：在 `tests/unit/` 添加
- 端到端：在 `tests/e2e/` 编写回放脚本
- 基准：更新 `scripts/run_benchmark.sh`


