#!/bin/bash
AICLI=./build/aicli

echo "=== 基础命令测试 ==="
printf '/help\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q '/model' && echo '✅ 01 帮助' || echo '❌ 01 帮助'
printf '/exit\n' | timeout 2 $AICLI 2>&1 | grep -q '再见' && echo '✅ 02 退出' || echo '❌ 02 退出'

echo ""
echo "=== 工具测试 ==="
printf '/tools list\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q 'echo' && echo '✅ 03 工具列表' || echo '❌ 03 工具列表'
printf '/tools run echo {"k":1}\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q 'k' && echo '✅ 04 工具echo' || echo '❌ 04 工具echo'
printf '/tools run fs.read_file {}\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q '字段' && echo '✅ 05 Schema校验' || echo '❌ 05 Schema校验'
printf '/tools run shell.exec {"cmd":"rm"}\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q '白名单' && echo '✅ 06 白名单' || echo '❌ 06 白名单'

echo ""
echo "=== 函数式 Shell 测试 ==="
printf '/fn files("*.md")\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q 'README' && echo '✅ 07 fnshell-files' || echo '❌ 07 fnshell-files'
printf '/fn text.read("xxx") | unwrap_or("ok")\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q '"ok"' && echo '✅ 08 fnshell-unwrap' || echo '❌ 08 fnshell-unwrap'

echo ""
echo "=== 会话测试 ==="
printf '/session test1\n/session list\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q 'test1' && echo '✅ 09 会话切换' || echo '❌ 09 会话切换'

echo ""
echo "=== 渲染测试 ==="
printf '/render think off\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q 'OFF' && echo '✅ 10 渲染开关' || echo '❌ 10 渲染开关'

echo ""
echo "=== 云端测试 ==="
printf '/cloud openai\n/exit\n' | timeout 2 $AICLI 2>&1 | grep -q 'OPENAI_API_KEY' && echo '✅ 11 云端无key检测' || echo '❌ 11 云端无key检测'
