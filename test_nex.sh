#!/bin/bash

# 测试NeX Shell基本功能

echo "=== NeX Shell 测试脚本 ==="
echo ""

# 启动NeX
cd /home/snow/workspace/aicli

echo "1. 测试help命令:"
./build/linux/x86_64/debug/nex help | head -20
echo ""

echo "2. 测试version命令:"
./build/linux/x86_64/debug/nex version
echo ""

echo "3. 测试status命令:"
./build/linux/x86_64/debug/nex status
echo ""

echo "=== 基本功能测试完成 ==="
echo "要测试交互式shell，请运行: ./build/linux/x86_64/debug/nex shell"
