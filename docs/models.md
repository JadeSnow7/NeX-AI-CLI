# 模型与后端

- 本地：`llama.cpp`（gguf），量化 Q4/Q5/Q8，`gpu_layers` 自适应
- 远端：OpenAI、Gemini，可自定义 base_url、超时与重试
- 嵌入：本地小模型用于 RAG 与分类（可选）
- 生命周期：下载/校验/热加载/卸载、kv-cache 共享与批处理





