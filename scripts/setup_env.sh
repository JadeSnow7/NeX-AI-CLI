#!/usr/bin/env bash
set -euo pipefail

# 初始化子模块与基础依赖（占位）
if [ -d third_party/llama.cpp ]; then
  git submodule update --init --recursive
fi

echo "AICLI_ENV=initialized"





