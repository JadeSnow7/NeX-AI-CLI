#!/usr/bin/env bash
set -euo pipefail

REPO=https://github.com/ggerganov/llama.cpp.git
DST=third_party/llama.cpp

if [ -d "$DST/.git" ]; then
  echo "[ok] llama.cpp already present"
  exit 0
fi

echo "[info] cloning llama.cpp -> $DST"
mkdir -p third_party

# 支持代理: 使用环境变量 http_proxy/https_proxy/all_proxy
# 重试 3 次
for i in 1 2 3; do
  if git clone --depth 1 "$REPO" "$DST"; then
    (cd "$DST" && git submodule update --init --recursive)
    echo "[ok] llama.cpp ready"
    exit 0
  fi
  echo "[warn] clone attempt $i failed, retrying in 5s..."
  sleep 5
done

echo "[error] failed to clone llama.cpp after 3 attempts"
exit 1




