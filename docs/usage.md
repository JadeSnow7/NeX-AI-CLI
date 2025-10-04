# 使用指南

## 快速开始

### 启动 REPL
```bash
./build/aicli
```

### 加载本地模型
```
> /model /absolute/path/to/model.gguf
```

### 基本对话
```
> 你好
AI：你好！有什么可以帮你的吗？😊
```

### 会话管理
```
> /session projA          # 切换到 projA 会话
> /session list           # 列出所有会话
> /session clear          # 清空当前会话历史
> /session rm projB       # 重置 projB 会话状态
```

### 思考轨控制
```
> /render think off       # 隐藏 <think> 思考片段（默认）
> /render think on        # 显示完整思考过程
```

### 工具调用

#### 列出可用工具
```
> /tools list
echo - 返回参数原样
fs.read_file - 读取文本文件
fs.write_file - 写入文本文件
shell.exec - 执行白名单 shell 命令
```

#### 手动运行工具
```
> /tools run echo {"msg":"hello"}
{"msg":"hello"}

> /tools run fs.read_file {"path":"/etc/hostname"}
{"path":"/etc/hostname","content":"DESKTOP-3HE7JD5\n"}

> /tools run fs.write_file {"path":"output.txt","content":"hello\n"}
{"path":"output.txt","written":6}

> /tools run shell.exec {"cmd":"ls","args":["-l","/tmp"]}
{"cmd":"ls","rc":0,"stdout":"...","timeout":false}
```

#### 内联工具（对话中）
```
> {{tool:fs.read_file {"path":"README.md"}}} 帮我总结这个文件
AI：[tool fs.read_file: {...}] 根据 README.md，这是...
```

### 中断生成
```
> /stop                   # 停止当前生成（需在另一终端或信号触发）
```

### 退出
```
> /exit
```

## 环境变量

- `AICLI_CTX`：上下文长度（默认 4096）
- `AICLI_THREADS`：推理线程数（默认 CPU 核数）
- `AICLI_SEED`：采样随机种子（可选）
- `AICLI_TOOL_TIMEOUT_MS`：工具超时毫秒（默认 5000）
- `AICLI_DATA_DIR`：数据与数据库目录（默认 data）
- `AICLI_LOG_LEVEL`：日志级别（trace/debug/info/warn/error）

## 数据存储

### SQLite（启用时）
- 会话历史：`data/aicli.db` → `sessions`, `messages`
- 工具日志：`data/aicli.db` → `tool_invocations`
- 系统事件：`data/aicli.db` → `events`

### JSONL（始终）
- `data/sysbox.jsonl`：事件与计时日志

## 查询历史

```bash
# SQLite
sqlite3 data/aicli.db "SELECT * FROM messages WHERE session='default' ORDER BY id DESC LIMIT 10"

# JSONL
tail -n 20 data/sysbox.jsonl | jq .
```



