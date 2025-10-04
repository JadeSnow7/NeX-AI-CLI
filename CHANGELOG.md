# 更新日志

## [0.1.0] - 2025-10-04

### 已实现

#### 核心推理
- ✅ 集成 llama.cpp 本地推理引擎
- ✅ 支持 GGUF 模型加载、量化格式（Q4_K_M 等）
- ✅ 温度/Top-p 采样与流式输出
- ✅ 环境变量配置（AICLI_CTX、AICLI_THREADS、AICLI_SEED）
- ✅ 降低 llama/ggml 日志噪声

#### 会话与上下文
- ✅ 多会话管理（/session 命令）
- ✅ 单上下文多会话切换与 KV 重建
- ✅ ChatML 对话模板渲染（<|im_start|> 风格）
- ✅ 会话历史 SQLite 持久化（sessions、messages 表）

#### 工具系统
- ✅ 工具注册中心与内置工具（echo、fs.read_file、fs.write_file、shell.exec）
- ✅ JSON Schema 字段校验与友好中文错误
- ✅ 白名单、超时、输出限额（可配置）
- ✅ 对话内联工具占位符 `{{tool:name args}}`
- ✅ 工具调用日志落库（tool_invocations 表）

#### 观测与指标
- ✅ sys box 事件记录（JSONL + SQLite）
- ✅ 计时器与指标上报（tokens/s、p50/p95）
- ✅ 推理各阶段计时（load_model、prefill、decode）

#### 函数式 Shell
- ✅ 词法/语法解析器（管道、调用、字面量）
- ✅ 求值器与运行时环境
- ✅ 高阶函数（map、filter）
- ✅ 错误处理（Error 类型、unwrap_or）
- ✅ 内置函数库（files、text.read、split_lines、contains、sys.events）
- ✅ REPL `/fn` 命令

#### CLI 交互
- ✅ REPL Shell（/help、/exit、/model、/session、/tools、/render、/stop、/fn）
- ✅ 分轨渲染（思考/最终）
- ✅ 中断支持（abort_callback + 原子标志）
- ✅ 友好错误提示

#### 文档与配置
- ✅ 12 篇详细文档（usage、architecture、dataflow、routing、models、sysbox、fnshell、config、security、plugins、benchmark、troubleshooting）
- ✅ 3 个配置文件示例（aicli.yaml、routing_rules.yaml、tools.yaml）
- ✅ 3 个 JSON Schema（tool、fnshell.ast、sysbox.event）
- ✅ 5 个运维脚本（setup_env、get_llama、fetch_models、run_local、run_benchmark）

#### 云端推理与路由
- ✅ HTTP 客户端（SSE 流式）
- ✅ OpenAI Provider（Chat Completions API）
- ✅ Gemini Provider（streamGenerateContent API）
- ✅ 路由器（token 阈值、失败回退）
- ✅ REPL `/cloud` 命令（openai/gemini/disable）

### 待实现

- ⏳ 增强路由策略（成本模型、延迟预算、断路器）
- ⏳ RAG 与嵌入模型集成
- ⏳ 函数式 Shell 完整高阶函数（fold、真实 filter 谓词）
- ⏳ Web TUI 可视化面板
- ⏳ 配置文件 YAML 解析（当前为硬编码默认值）

### 已知问题

- 编译警告：部分函数参数未使用（session_id 在无 SQLITE 时）
- filter 当前为占位实现（全保留，未真正调用谓词）
- 函数式 Shell 的 lambda 与闭包支持待扩展


