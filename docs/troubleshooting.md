# 故障排查

## 子模块拉取失败
- 设置代理：
  - `export http_proxy=... https_proxy=... all_proxy=...`
- 使用脚本重试：
  - `scripts/get_llama.sh`
- 证书问题：
  - `git config --global http.sslBackend openssl` 或 `git config --global http.sslVerify false`（不建议长期关闭）

## AICLI_WITH_LLAMA=ON 仍然找不到 llama.h
- 确认 `third_party/llama.cpp/CMakeLists.txt` 存在
- 重新配置：
  - `cmake -S . -B build -DAICLI_WITH_LLAMA=ON`
- 若网络不通，先用 OFF（占位实现）保证可编译：
  - `cmake -S . -B build -DAICLI_WITH_LLAMA=OFF`

## 性能与资源
- 环境变量调整：
  - `AICLI_CTX`（上下文长度，默认 4096）
  - `AICLI_THREADS`（线程数，默认 CPU 逻辑核数）
  - `AICLI_SEED`（采样随机种子）
- GPU：
  - CUDA: 在子项目开启 `LLAMA_CUBLAS=ON`
  - Metal: `LLAMA_METAL=ON`




