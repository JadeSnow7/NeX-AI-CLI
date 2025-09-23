#!/bin/bash

# NeX: AI-CLI 高级功能演示脚本
# 展示各种shell功能和特性

set -e

echo "� NeX: AI-CLI 高级功能演示"
echo "=============================="
echo

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

# 函数定义
print_header() {
    echo -e "${CYAN}� $1${NC}"
    echo "----------------------------------------"
}

print_command() {
    echo -e "${YELLOW}➤ $1${NC}"
}

print_result() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

wait_for_input() {
    echo -e "${WHITE}按回车继续...${NC}"
    read -r
}

# 检查构建状态
check_build() {
    print_header "检查构建状态"
    
    if [ ! -f "build/nex" ]; then
        echo -e "${RED}❌ 未找到构建的可执行文件${NC}"
        echo "请先运行以下命令构建项目:"
        echo "  mkdir build && cd build"
        echo "  cmake .."
        echo "  make"
        exit 1
    fi
    
    print_result "找到可执行文件: build/nex"
    echo
}

# 演示基础命令
demo_basic_commands() {
    print_header "基础命令演示"
    
    print_command "./build/nex help"
    ./build/nex help
    echo
    
    print_command "./build/nex version"
    ./build/nex version
    echo
    
    print_result "基础命令测试完成"
    wait_for_input
}

# 演示配置系统
demo_config_system() {
    print_header "配置系统演示"
    
    # 创建示例配置目录
    mkdir -p ~/.nex
    
    print_command "创建示例配置文件"
    cat > ~/.nex/config.ini << EOF
[shell.prompt]
show_time = true
show_git_info = true
show_system_status = false
use_colors = true
time_format = %H:%M:%S

[shell.highlight]
enabled = true
command_color = bright_blue
path_color = bright_yellow
flag_color = bright_cyan

[shell.completion]
enabled = true
max_suggestions = 20

[system]
max_history_size = 1000
auto_save_history = true
EOF
    
    print_result "配置文件已创建: ~/.nex/config.ini"
    
    print_command "./build/nex config"
    ./build/nex config
    echo
    
    print_result "配置系统测试完成"
    wait_for_input
}

# 演示文件操作
demo_file_operations() {
    print_header "文件操作演示"
    
    # 创建测试目录和文件
    TEST_DIR="demo_files"
    mkdir -p "$TEST_DIR"
    
    print_command "创建测试文件"
    echo "Hello NeX Shell!" > "$TEST_DIR/hello.txt"
    echo "print('Python script')" > "$TEST_DIR/script.py"
    echo "int main() { return 0; }" > "$TEST_DIR/main.cpp"
    echo "#!/bin/bash\necho 'Shell script'" > "$TEST_DIR/test.sh"
    chmod +x "$TEST_DIR/test.sh"
    
    print_result "测试文件已创建在 $TEST_DIR/"
    
    print_command "ls -la $TEST_DIR/"
    ls -la "$TEST_DIR/"
    echo
    
    print_command "file $TEST_DIR/*"
    file "$TEST_DIR"/*
    echo
    
    print_result "文件操作演示完成"
    wait_for_input
}

# 演示Git集成
demo_git_integration() {
    print_header "Git集成演示"
    
    # 检查是否在Git仓库中
    if ! git rev-parse --git-dir > /dev/null 2>&1; then
        print_info "当前目录不是Git仓库，创建临时仓库进行演示"
        
        GIT_TEST_DIR="git_demo"
        rm -rf "$GIT_TEST_DIR"
        mkdir "$GIT_TEST_DIR"
        cd "$GIT_TEST_DIR"
        
        git init
        git config user.name "NeX Demo"
        git config user.email "demo@nex.ai"
        
        echo "# NeX Git Demo" > README.md
        git add README.md
        git commit -m "Initial commit"
        
        echo "New feature" >> README.md
        
        print_result "创建了临时Git仓库用于演示"
    fi
    
    print_command "git status"
    git status
    echo
    
    print_command "git log --oneline -5"
    git log --oneline -5 2>/dev/null || echo "没有足够的提交历史"
    echo
    
    print_command "git branch"
    git branch
    echo
    
    # 如果创建了临时目录，返回上级目录
    if [ -d "../git_demo" ]; then
        cd ..
        print_info "返回原目录"
    fi
    
    print_result "Git集成演示完成"
    wait_for_input
}

# 演示系统监控
demo_system_monitoring() {
    print_header "系统监控演示"
    
    print_command "系统信息"
    echo "操作系统: $(uname -o)"
    echo "内核版本: $(uname -r)"
    echo "架构: $(uname -m)"
    echo
    
    print_command "CPU信息"
    if command -v lscpu > /dev/null; then
        lscpu | grep "Model name" || echo "CPU型号信息不可用"
    else
        cat /proc/cpuinfo | grep "model name" | head -1 || echo "CPU信息不可用"
    fi
    echo
    
    print_command "内存使用情况"
    if command -v free > /dev/null; then
        free -h
    else
        echo "内存信息不可用"
    fi
    echo
    
    print_command "磁盘使用情况"
    df -h . 2>/dev/null || echo "磁盘信息不可用"
    echo
    
    print_result "系统监控演示完成"
    wait_for_input
}

# 演示高级Shell功能
demo_advanced_shell() {
    print_header "高级Shell功能演示"
    
    print_command "管道操作示例"
    echo "创建测试数据..."
    echo -e "apple\nbanana\ncherry\ndate\napricot" > fruits.txt
    
    print_command "cat fruits.txt | grep 'a' | sort"
    cat fruits.txt | grep 'a' | sort
    echo
    
    print_command "环境变量操作"
    export NEX_DEMO_VAR="Hello from NeX!"
    echo "NEX_DEMO_VAR = $NEX_DEMO_VAR"
    echo
    
    print_command "命令替换"
    echo "当前时间: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "当前用户: $(whoami)"
    echo "工作目录: $(pwd)"
    echo
    
    # 清理
    rm -f fruits.txt
    
    print_result "高级Shell功能演示完成"
    wait_for_input
}

# 演示语法高亮模拟
demo_syntax_highlighting() {
    print_header "语法高亮演示 (模拟)"
    
    print_info "在实际的NeX Shell中，以下命令会显示彩色语法高亮:"
    echo
    
    # 模拟彩色输出
    echo -e "${BLUE}ls${NC} ${CYAN}-la${NC} ${YELLOW}~/${NC}"
    echo -e "${BLUE}cd${NC} ${YELLOW}/home/user/projects${NC}"
    echo -e "${BLUE}git${NC} ${CYAN}commit${NC} ${CYAN}-m${NC} ${GREEN}\"Fix syntax highlighting\"${NC}"
    echo -e "${BLUE}find${NC} ${YELLOW}.${NC} ${CYAN}-name${NC} ${GREEN}\"*.cpp\"${NC} ${CYAN}-type${NC} ${MAGENTA}f${NC}"
    echo -e "${BLUE}cat${NC} ${YELLOW}config.ini${NC} ${CYAN}|${NC} ${BLUE}grep${NC} ${GREEN}\"highlight\"${NC}"
    echo
    
    print_result "语法高亮演示完成"
    wait_for_input
}

# 演示完成信息
demo_completion() {
    print_header "演示完成"
    
    print_result "所有功能演示已完成！"
    echo
    print_info "你已经体验了NeX: AI-CLI的主要功能:"
    echo "  ✓ 基础命令系统"
    echo "  ✓ 配置管理"
    echo "  ✓ 文件操作"
    echo "  ✓ Git集成"
    echo "  ✓ 系统监控"
    echo "  ✓ 高级Shell功能"
    echo "  ✓ 语法高亮 (模拟)"
    echo
    
    print_info "下一步建议:"
    echo "  1. 阅读用户手册: docs/USER_MANUAL.md"
    echo "  2. 查看技术架构: docs/TECHNICAL_ARCHITECTURE.md"
    echo "  3. 了解AI集成计划: docs/AI_INTEGRATION_PLAN.md"
    echo "  4. 开始使用: ./build/nex shell"
    echo
    
    echo -e "${WHITE}感谢体验NeX: AI-CLI！🚀${NC}"
}

# 清理函数
cleanup() {
    print_info "清理演示文件..."
    rm -rf demo_files git_demo
    rm -f fruits.txt
    print_result "清理完成"
}

# 主演示流程
main() {
    echo -e "${WHITE}"
    cat << "EOF"
    ███╗   ██╗███████╗██╗  ██╗
    ████╗  ██║██╔════╝╚██╗██╔╝
    ██╔██╗ ██║█████╗   ╚███╔╝ 
    ██║╚██╗██║██╔══╝   ██╔██╗ 
    ██║ ╚████║███████╗██╔╝ ██╗
    ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝
    
     AI-Powered Command Line Interface
EOF
    echo -e "${NC}"
    echo
    
    print_info "即将开始NeX功能演示..."
    print_info "演示过程中可以随时按Ctrl+C退出"
    echo
    wait_for_input
    
    # 设置错误处理
    trap cleanup EXIT
    
    # 执行各个演示
    check_build
    demo_basic_commands
    demo_config_system
    demo_file_operations
    demo_git_integration
    demo_system_monitoring
    demo_advanced_shell
    demo_syntax_highlighting
    demo_completion
}

# 运行主程序
main "$@"
