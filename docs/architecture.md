# 架构概览

aicli 由 CLI 交互层、路由与推理层、函数式 Shell、sys box 观测层构成。输入以“/”开头走命令解析器，其余走对话流；根据复杂度/延迟/成本自动选择本地或云端推理。

## 组件
- CLI：REPL、指令解析、流式渲染
- Router：分流策略、失败回退、断路器
- Inference：本地 `llama.cpp` 与远端 OpenAI/Gemini 客户端
- Conversation：会话与记忆、RAG（可选）
- FnShell：函数式运行时、文件对象 `FileRef`、不可变管道
- SysBox：指标/事件采集、查询接口

## 关键流
1. 输入解析 -> 预处理 -> 路由决策
2. 本地/云端推理 -> 流式输出 -> 渲染
3. SysBox 记录路由与延迟、错误、资源事件

## 扩展点
- Provider 插件、工具调用协议、语言/主题/i18n、后端硬件选择





