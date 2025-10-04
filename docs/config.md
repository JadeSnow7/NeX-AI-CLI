# 配置详解

## 配置文件

### config/aicli.yaml

主配置文件，包含 CLI、本地模型、远端 Provider、路由、内存、安全、遥测等配置。

```yaml
cli:
  theme: default
  history_size: 200
  stream: true

local_model:
  model_dir: ${AICLI_MODEL_DIR:-models}
  default_model: tiny-gguf
  context_length: 4096
  quant: Q4_K_M
  gpu_layers: auto

remote:
  providers:
    openai:
      enabled: true
      base_url: ${OPENAI_BASE_URL:-https://api.openai.com/v1}
      model: gpt-4o-mini
      timeout_ms: 30000
      retries: 2
    gemini:
      enabled: false
      base_url: ${GEMINI_BASE_URL:-https://generativelanguage.googleapis.com}
      model: gemini-1.5-flash
      timeout_ms: 30000
      retries: 2

routing:
  max_cost_usd: 0.02
  latency_budget_ms: 1500
  input_token_threshold: 800
  prefer_local: true
  circuit_breaker:
    failure_threshold: 5
    reset_timeout_ms: 10000

memory:
  store_dir: ${AICLI_DATA_DIR:-data}
  rag:
    enabled: false
    embedder: local-embed-small

security:
  fs_sandbox_root: .
  allow_network: true
  log_redaction: true

telemetry:
  sample_rate: 1.0
  log_level: info
```

### config/routing_rules.yaml

路由规则：按输入 token 数、延迟预算、工具需求自动选择本地或云端。

```yaml
rules:
  - name: prefer-local-short
    when:
      input_tokens_max: 800
      latency_budget_ms_min: 500
    action: local
  - name: send-to-cloud-complex
    when:
      input_tokens_min: 1200
      require_tools: true
    action: cloud
fallback:
  on_local_failure: cloud
  on_cloud_timeout: local_summarized
```

### config/tools.yaml

工具配置：超时、白名单、沙箱。

```yaml
tools:
  timeout_ms: 5000
  shell:
    whitelist: [ls, cat, echo, stat, wc, pwd, date]
    max_output_bytes: 262144
  fs:
    sandbox_root: .
    allow_write: true
```

## 环境变量覆盖

- `AICLI_CONFIG`：指定配置文件路径
- `AICLI_DATA_DIR`：数据目录
- `AICLI_MODEL_DIR`：模型目录
- `AICLI_CTX`：上下文长度
- `AICLI_THREADS`：线程数
- `AICLI_SEED`：随机种子
- `AICLI_TOOL_TIMEOUT_MS`：工具超时
- `AICLI_LOG_LEVEL`：日志级别
- `OPENAI_API_KEY`、`GEMINI_API_KEY`：云端密钥

## 优先级

环境变量 > config/aicli.yaml > 默认值



