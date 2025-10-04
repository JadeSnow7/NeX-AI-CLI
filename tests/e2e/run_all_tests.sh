#!/usr/bin/env bash
set -euo pipefail

AICLI=./build/aicli
REPORT=tests/e2e/test_report.md

echo "# aicli 测试报告" > "$REPORT"
echo "" >> "$REPORT"
echo "**测试时间**: $(date)" >> "$REPORT"
echo "" >> "$REPORT"

PASS=0
FAIL=0

test_case() {
    local name=$1
    local input=$2
    local expect=$3
    echo "## 测试: $name" >> "$REPORT"
    local output=$(printf "$input" | timeout 3 $AICLI 2>&1 || echo "")
    if echo "$output" | grep -q "$expect"; then
        echo "✅ **通过**" >> "$REPORT"
        echo "通过: $name"
        ((PASS++))
    else
        echo "❌ **失败**" >> "$REPORT"
        echo "- 期望: $expect" >> "$REPORT"
        echo "- 输出: \`\`\`" >> "$REPORT"
        echo "$output" | head -10 >> "$REPORT"
        echo "\`\`\`" >> "$REPORT"
        echo "失败: $name"
        ((FAIL++))
    fi
    echo "" >> "$REPORT"
}

echo "开始测试..."

# 基础命令
test_case "01-帮助" "/help\n/exit\n" "/model"
test_case "02-退出" "/exit\n" "再见"

# 工具
test_case "03-工具列表" "/tools list\n/exit\n" "echo"
test_case "04-工具echo" "/tools run echo {\"k\":1}\n/exit\n" "k"
test_case "05-工具缺字段" "/tools run fs.read_file {}\n/exit\n" "字段"
test_case "06-shell白名单" "/tools run shell.exec {\"cmd\":\"rm\"}\n/exit\n" "白名单"
test_case "07-shell成功" "/tools run shell.exec {\"cmd\":\"echo\",\"args\":[\"hi\"]}\n/exit\n" "hi"

# 函数式 Shell
test_case "08-fnshell-files" "/fn files(\"*.md\")\n/exit\n" "README"
test_case "09-fnshell-unwrap" "/fn text.read(\"xxx\") | unwrap_or(\"ok\")\n/exit\n" "ok"

# 会话
test_case "10-会话切换" "/session test1\n/session list\n/exit\n" "test1"
test_case "11-渲染开关" "/render think off\n/exit\n" "OFF"

# 云端
test_case "12-云端无key" "/cloud openai\n/exit\n" "OPENAI_API_KEY"

echo "" >> "$REPORT"
echo "---" >> "$REPORT"
echo "## 总结" >> "$REPORT"
echo "- 通过: $PASS" >> "$REPORT"
echo "- 失败: $FAIL" >> "$REPORT"
echo "" >> "$REPORT"

if [ $FAIL -eq 0 ]; then
    echo "状态: ✅ 全部通过" >> "$REPORT"
    echo "✅ 所有测试通过 ($PASS/$((PASS+FAIL)))"
    exit 0
else
    echo "状态: ❌ 存在失败" >> "$REPORT"
    echo "❌ 部分测试失败 ($PASS/$((PASS+FAIL)))"
    exit 1
fi

