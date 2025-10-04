# 数据流与时序

1. REPL 接收输入 -> Tokenize/评估复杂度
2. Conversation 聚合上下文/提示词 -> Router 估算成本与延迟
3. Inference 发起请求（本地或云端）-> 流式回调给 CLI
4. SysBox 生成事件（request.start/end、route.decision、error）

时序指标：排队等待、prefill、解码速率、云端 RTT 与吞吐。





