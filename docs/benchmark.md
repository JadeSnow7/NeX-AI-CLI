# 基准测试

## 运行基准

```bash
bash scripts/run_benchmark.sh
```

（脚本当前为占位，待完善）

## 手动基准

### 测试本地推理吞吐

```bash
export AICLI_SEED=42
time printf '/model /path/to/model.gguf\n重复生成100次的测试文本\n/exit\n' | ./build/aicli
```

查看 `data/sysbox.jsonl`：

```bash
jq 'select(.payload.tokens_per_s) | .payload' data/sysbox.jsonl
```

输出示例：
```json
{"tokens":50,"ms":1234,"tokens_per_s":40.5,"p50":1200,"p95":1500}
```

### 指标说明

- `tokens`：生成 token 数
- `ms`：总耗时（毫秒）
- `tokens_per_s`：吞吐（tokens/秒）
- `p50`、`p95`：该指标的中位数与 95 分位（毫秒）

### 查询 SQLite 指标

```bash
sqlite3 data/aicli.db <<EOF
SELECT component, json_extract(payload, '$.tokens_per_s') AS tps
FROM events
WHERE payload LIKE '%tokens_per_s%'
ORDER BY ts DESC LIMIT 10;
EOF
```

## 基准场景

- **Prefill**：大上下文输入（2048+ tokens）
- **Decode**：小批次逐 token 生成
- **多会话并发**：切换会话，验证 KV 复用效率
- **工具调用**：多次工具调用耗时

## 性能调优

- `AICLI_CTX`：增大上下文窗口（需显存）
- `AICLI_THREADS`：调整线程数（推荐物理核数）
- GPU 后端：`-DLLAMA_CUBLAS=ON` 或 `-DLLAMA_METAL=ON`
- 量化：Q4_K_M（平衡） vs Q8_0（质量）

## 回归阈值

建议在 CI/CD 中监控：

- `tokens_per_s` 下降 >20% → 性能退化
- `p95` 上升 >50% → 延迟恶化
- `error` 事件增多 → 稳定性下降



