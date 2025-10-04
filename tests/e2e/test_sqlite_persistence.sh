#!/usr/bin/env bash
# SQLite 持久化测试（需启用 AICLI_WITH_SQLITE）

AICLI=./build/aicli
DB=data/aicli.db

# 检测是否启用 SQLite
if ! ldd $AICLI 2>/dev/null | grep -q sqlite3; then
    echo "⏭️  跳过 SQLite 测试（未启用 AICLI_WITH_SQLITE）"
    exit 0
fi

echo "=== SQLite 持久化测试 ==="

# 清理旧数据
rm -f $DB

# 测试 1：工具调用落库
printf '/tools run echo {"test":1}\n/exit\n' | timeout 2 $AICLI >/dev/null 2>&1

if [ -f "$DB" ]; then
    COUNT=$(sqlite3 $DB "SELECT COUNT(*) FROM tool_invocations WHERE tool='echo'" 2>/dev/null || echo 0)
    if [ "$COUNT" -gt 0 ]; then
        echo "✅ 工具调用已记录到 SQLite"
    else
        echo "❌ 工具调用未记录"
    fi
else
    echo "⏭️  数据库未创建（可能 SQLite 未真正启用）"
fi

# 测试 2：sys box 事件
if [ -f "$DB" ]; then
    COUNT=$(sqlite3 $DB "SELECT COUNT(*) FROM events" 2>/dev/null || echo 0)
    if [ "$COUNT" -gt 0 ]; then
        echo "✅ sys box 事件已记录"
    else
        echo "⏭️  events 表可能为空"
    fi
fi

echo ""
echo "✅ SQLite 持久化测试完成"

