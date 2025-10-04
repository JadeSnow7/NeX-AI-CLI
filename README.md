# aicli

**基于 C++ 的智能 AI CLI**：本地集成 `llama.cpp`，云端支持 OpenAI/Gemini，智能路由分流。提供类似 Claude Code 的交互式 CLI Shell、多会话管理、工具调用系统、函数式 Shell，以及完整的观测与持久化能力。

---

## 核心特性

- ✅ **本地推理**：llama.cpp 集成（GGUF、量化、流式、GPU 可选）
- ✅ **云端推理**：OpenAI/Gemini 支持、流式 SSE 解析
- ✅ **智能路由**：token 阈值分流、失败回退、路由决策记录
- ✅ **多会话管理**：独立上下文、KV 复用、ChatML 模板
- ✅ **工具系统**：内置 echo/fs.read/fs.write/shell.exec，白名单、超时、Schema 校验
- ✅ **分轨渲染**：隐藏/显示 `<think>` 思考轨、流式输出
- ✅ **函数式 Shell**：解析器、高阶函数（map/filter）、管道、错误处理
- ✅ **sys box**：事件记录、计时、tokens/s、p50/p95 指标
- ✅ **SQLite 持久化**：会话历史、工具调用、事件存储

---

## 快速开始

### 1. 构建（最小依赖）
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/aicli
```

### 2. 启用完整功能

#### 本地推理（llama.cpp）
```bash
# 拉取子模块
bash scripts/get_llama.sh

# 构建（CPU）
cmake -S . -B build -DAICLI_WITH_LLAMA=ON
cmake --build build -j

# 或启用 GPU（CUDA）
cmake -S . -B build -DAICLI_WITH_LLAMA=ON -DLLAMA_CUBLAS=ON
cmake --build build -j
```

#### SQLite 持久化
```bash
# 安装依赖
sudo apt-get install -y libsqlite3-dev

# 构建
cmake -S . -B build -DAICLI_WITH_LLAMA=ON -DAICLI_WITH_SQLITE=ON
cmake --build build -j
```

---

## 使用示例

### 本地推理
```bash
./build/aicli
> /model /path/to/model.gguf
> /render think off
> 你好
AI：你好！有什么可以帮你的吗？😊
> /exit
```

### 云端推理（OpenAI）
```bash
export OPENAI_API_KEY=sk-your-key
./build/aicli
> /cloud openai
已启用 OpenAI（https://api.openai.com/v1，模型：gpt-4o-mini）
> 你好
AI：你好！我能为你做什么？
```

### 云端推理（Gemini）
```bash
export GEMINI_API_KEY=your-key
./build/aicli
> /cloud gemini
已启用 Gemini（...，模型：gemini-1.5-flash）
> 你好
```

### 混合路由（本地 + 云端）
```bash
export OPENAI_API_KEY=sk-...
export AICLI_INPUT_TOKEN_THRESHOLD=500
./build/aicli
> /model /path/to/local.gguf
> /cloud openai
# 短输入走本地，长输入走云端
> 你好（本地）
> 请帮我写一篇2000字的技术文档...（云端）
```

### 工具调用
```bash
> /tools list
> /tools run fs.read_file {"path":"/etc/hostname"}
> /tools run shell.exec {"cmd":"ls","args":["-l"]}

# 对话中内联
> {{tool:fs.read_file {"path":"README.md"}}} 帮我总结这个文件
```

### 函数式 Shell
```bash
> /fn files("*.md")
["README.md", "CHANGELOG.md"]

> /fn files("*.md") | map(text.read)
["# aicli...", "# 更新日志..."]

> /fn text.read("not_exist.txt") | unwrap_or("fallback")
"fallback"

> /fn text.read("README.md") | split_lines
["# aicli", "", "**基于 C++ 的智能 AI CLI**...", ...]
```

### 会话管理
```bash
> /session projA
> 这是 projA 的上下文
> /session projB
> 这是 projB 的上下文
> /session list
当前会话：projB
会话列表(2)： default projA projB
> /session clear
已清空当前会话历史：projB
```

---

## 命令速查

```bash
/help                           # 显示帮助
/model <path>                   # 加载本地 GGUF 模型
/cloud openai|gemini|disable    # 启用/禁用云端 Provider
/session [name|list|clear|rm]   # 会话管理
/render think on|off            # 切换思考轨显示
/tools list                     # 列出工具
/tools run <name> <args-json>   # 运行工具
/fn <expression>                # 执行函数式 Shell 表达式
/stop                           # 中断生成
/exit                           # 退出
```

---

## 环境变量

- `AICLI_CTX`：上下文长度（默认 4096）
- `AICLI_THREADS`：推理线程数（默认 CPU 核数）
- `AICLI_SEED`：采样随机种子
- `AICLI_TOOL_TIMEOUT_MS`：工具超时（默认 5000）
- `AICLI_INPUT_TOKEN_THRESHOLD`：路由阈值（默认 800）
- `AICLI_PREFER_LOCAL`：优先本地（默认 true）
- `AICLI_DATA_DIR`：数据目录（默认 data）
- `AICLI_LOG_LEVEL`：日志级别
- `OPENAI_API_KEY`：OpenAI 密钥
- `OPENAI_BASE_URL`：OpenAI 端点（可选）
- `GEMINI_API_KEY`：Gemini 密钥
- `GEMINI_BASE_URL`：Gemini 端点（可选）

---

## 文档

- [使用指南](docs/usage.md)
- [架构概览](docs/architecture.md)
- [数据流](docs/dataflow.md)
- [路由策略](docs/routing.md)
- [模型与后端](docs/models.md)
- [sys box](docs/sysbox.md)
- [函数式 Shell](docs/fnshell.md)
- [配置详解](docs/config.md)
- [安全与沙箱](docs/security.md)
- [插件扩展](docs/plugins.md)
- [基准测试](docs/benchmark.md)
- [优化建议](docs/optimization.md)
- [故障排查](docs/troubleshooting.md)
- [更新日志](CHANGELOG.md)
- [贡献指南](CONTRIBUTING.md)
- [项目总结](PROJECT_SUMMARY.md)

---

## 项目结构

```
aicli/
├── src/
│   ├── main.cpp
│   ├── cli/                # REPL Shell
│   ├── core/
│   │   ├── inference/      # 本地/云端推理引擎
│   │   ├── router/         # 智能路由器
│   │   ├── conversation/   # 会话与模板
│   │   ├── tools/          # 工具系统
│   │   ├── fnshell/        # 函数式 Shell
│   │   ├── sysbox/         # 观测系统
│   │   └── storage/        # SQLite 持久化
│   └── utils/              # 日志、配置
├── docs/                   # 13+ 篇文档
├── config/                 # 配置示例
├── scripts/                # 运维脚本
├── tests/                  # 单元测试
└── third_party/llama.cpp   # 子模块
```

---

## 数据存储

### SQLite（启用时）
- `data/aicli.db`
  - `sessions`：会话列表
  - `messages`：对话历史
  - `tool_invocations`：工具调用日志
  - `events`：系统事件

### JSONL（始终）
- `data/sysbox.jsonl`：事件与计时日志

---

## 许可证

MIT（见 `LICENSE`）

---

## 统计

- **源码**：2500+ 行 C++20
- **文档**：800+ 行 Markdown
- **可执行文件**：3.0MB（Release 模式）
- **已验证**：构建通过、功能测试通过、多场景回归通过
