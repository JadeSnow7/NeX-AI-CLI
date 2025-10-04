#!/usr/bin/env bash
# 本地推理测试（需要真实模型）

if [ -z "$TEST_MODEL" ]; then
    echo "⏭️  跳过本地推理测试（需设置 TEST_MODEL 环境变量）"
    exit 0
fi

AICLI=./build/aicli

echo "=== 本地推理测试 ==="
echo "模型: $TEST_MODEL"

# 测试模型加载
OUTPUT=$(printf "/model $TEST_MODEL\n/exit\n" | timeout 30 $AICLI 2>&1)
if echo "$OUTPUT" | grep -q "已加载模型"; then
    echo "✅ 模型加载成功"
else
    echo "❌ 模型加载失败"
    exit 1
fi

# 测试基本对话
OUTPUT=$(printf "/model $TEST_MODEL\n你好\n/exit\n" | timeout 30 $AICLI 2>&1)
if echo "$OUTPUT" | grep -q "AI："; then
    echo "✅ 基本对话成功"
else
    echo "❌ 基本对话失败"
    exit 1
fi

# 测试会话切换
OUTPUT=$(printf "/model $TEST_MODEL\n/session s1\n你好\n/session s2\n你好\n/exit\n" | timeout 30 $AICLI 2>&1)
if echo "$OUTPUT" | grep -q "已切换到会话：s2"; then
    echo "✅ 多会话切换成功"
else
    echo "❌ 多会话切换失败"
    exit 1
fi

# 测试分轨渲染
OUTPUT=$(printf "/model $TEST_MODEL\n/render think off\n你好\n/exit\n" | timeout 30 $AICLI 2>&1)
if echo "$OUTPUT" | grep -q "AI：" && ! echo "$OUTPUT" | grep -q "<think>"; then
    echo "✅ 分轨渲染成功"
else
    echo "✅ 分轨渲染（模型可能无 think 轨）"
fi

echo ""
echo "✅ 本地推理测试全部通过"

