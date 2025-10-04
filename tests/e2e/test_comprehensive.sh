#!/usr/bin/env bash
set -euo pipefail

AICLI=${AICLI:-./build/aicli}
REPORT=tests/e2e/test_report.txt

echo "=== aicli 综合测试 ===" | tee "$REPORT"
echo "时间: $(date)" | tee -a "$REPORT"
echo "" | tee -a "$REPORT"

PASS=0
FAIL=0

run_test() {
    local name=$1
    local input=$2
    local expect=$3
    echo "[测试] $name" | tee -a "$REPORT"
    local output=$(printf "$input" | timeout 5 $AICLI 2>&1 || echo "TIMEOUT")
    if echo "$output" | grep -q "$expect"; then
        echo "  ✅ 通过" | tee -a "$REPORT"
        ((PASS++))
    else
        echo "  ❌ 失败（期望：$expect）" | tee -a "$REPORT"
        echo "  输出：$output" | tee -a "$REPORT"
        ((FAIL++))
    fi
    echo "" | tee -a "$REPORT"
}

# 1. 基础命令
run_test "帮助命令" "/help\n/exit\n" "/model"
run_test "退出命令" "/exit\n" "再见"

# 2. 工具系统
run_test "工具列表" "/tools list\n/exit\n" "echo"
run_test "工具 echo" "/tools run echo {\"k\":1}\n/exit\n" '"k"'
run_test "工具 fs.read 缺字段" "/tools run fs.read_file {}\n/exit\n" "字段"
run_test "工具 shell 白名单拒绝" "/tools run shell.exec {\"cmd\":\"rm\"}\n/exit\n" "不在白名单"

# 3. 函数式 Shell
run_test "fnshell files" "/fn files(\"*.md\")\n/exit\n" "README.md"
run_test "fnshell unwrap_or" "/fn text.read(\"not_exist\") | unwrap_or(\"ok\")\n/exit\n" '"ok"'

# 4. 会话管理
run_test "会话切换" "/session testA\n/session list\n/exit\n" "testA"

# 5. 渲染控制
run_test "渲染 think off" "/render think off\n/exit\n" "think 渲染：OFF"

echo "=== 测试总结 ===" | tee -a "$REPORT"
echo "通过: $PASS" | tee -a "$REPORT"
echo "失败: $FAIL" | tee -a "$REPORT"
echo "" | tee -a "$REPORT"

if [ $FAIL -eq 0 ]; then
    echo "✅ 所有测试通过" | tee -a "$REPORT"
    exit 0
else
    echo "❌ 存在失败测试" | tee -a "$REPORT"
    exit 1
fi

