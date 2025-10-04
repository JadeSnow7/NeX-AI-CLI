# 安全与沙箱

## 文件系统沙箱

- **根路径限制**：`config/tools.yaml` 中 `fs.sandbox_root` 指定可访问根，默认为当前目录。
- **路径校验**：拒绝包含 `..` 的路径，防止目录遍历攻击。
- **写入开关**：`fs.allow_write` 可全局禁用写操作。

## Shell 命令白名单

- **白名单**：`config/tools.yaml` 中 `shell.whitelist` 定义允许的命令，默认：`ls, cat, echo, stat, wc, pwd, date`。
- **参数注入防护**：参数由简单 JSON 解析构造，避免 shell 注入。
- **超时与限额**：
  - `timeout_ms`（默认 5000）限制执行时长。
  - `max_output_bytes`（默认 262144）限制输出体量。

## API Key 管理

- **环境变量**：从 `OPENAI_API_KEY`、`GEMINI_API_KEY` 读取，不写入配置文件。
- **日志脱敏**：`security.log_redaction=true` 时自动脱敏敏感字段（计划中）。

## 网络访问

- **开关**：`security.allow_network` 控制云端 Provider 是否启用。
- **代理支持**：尊重 `http_proxy`、`https_proxy`、`all_proxy` 环境变量。

## 审计日志

- **sys box**：记录所有工具调用、路由决策、错误事件到 `data/sysbox.jsonl` 或 SQLite。
- **工具调用表**：`tool_invocations` 记录工具名、参数、结果、成功与否、耗时。

## Prompt 注入防护

- **占位符解析**：用户输入中的 `{{tool:...}}` 在推理前执行，避免模型伪造。
- **角色隔离**：ChatML 模板明确区分 `system/user/assistant`。

## 最佳实践

- 定期审计 `data/aicli.db` 中 `tool_invocations` 与 `events`。
- 仅在受信环境开启 `fs.allow_write` 与 `shell.exec`。
- 限制 `fs.sandbox_root` 到工作目录，避免全盘访问。
- 敏感数据不要通过云端 Provider（使用 `routing.prefer_local=true`）。



