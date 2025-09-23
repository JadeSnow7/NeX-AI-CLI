# NeX: AI-CLI 快速开始指南

## 🚀 5分钟快速体验

### 系统要求
- Ubuntu 20.04+ / CentOS 8+ / Arch Linux
- GCC 10+ 或 Clang 12+
- 4GB+ 内存 (8GB+ 推荐用于AI功能)
- 2GB+ 可用磁盘空间

### 快速安装

```bash
# 1. 克隆项目
git clone https://github.com/your-repo/nex-ai-cli.git
cd nex-ai-cli

# 2. 安装依赖 (Ubuntu/Debian)
sudo apt update
sudo apt install build-essential cmake libreadline-dev git

# 3. 构建项目
mkdir build && cd build
cmake ..
make -j$(nproc)

# 4. 运行演示
./demo_advanced_features.sh
```

### 立即体验

```bash
# 查看帮助
./build/nex help

# 启动智能Shell
./build/nex shell

# 查看配置
./build/nex config

# 系统状态
./build/nex status
```

## 🎨 核心功能展示

### 1. 智能提示符
启动Shell后，你会看到丰富的提示符信息：

```bash
[14:30:25] user@hostname ~/project (main●) ➤ 
```

**信息解读**:
- `[14:30:25]` - 当前时间
- `user@hostname` - 用户和主机
- `~/project` - 当前目录
- `(main●)` - Git分支和状态
- `➤` - 命令提示符

### 2. 语法高亮
输入命令时自动显示颜色：

![语法高亮示例](images/syntax-highlighting.png)

- **蓝色**: 命令名称
- **青色**: 命令选项 (-l, --help)
- **黄色**: 文件路径
- **绿色**: 字符串
- **紫色**: 数字

### 3. 智能补全
按Tab键获得智能建议：

```bash
ls -<Tab>          # 显示可用选项
cd ~/Doc<Tab>      # 补全为 ~/Documents/
git che<Tab>       # 补全为 git checkout
```

### 4. Git集成
在Git仓库中工作时，提示符自动显示：

```bash
# 清洁仓库
user@host ~/project (main) ➤ 

# 有未提交更改
user@host ~/project (main●) ➤ 

# 领先远程2个提交
user@host ~/project (main↑2) ➤ 
```

### 5. 系统监控
可选择显示系统状态：

```bash
[14:30:25] user@host ~/project (main) [CPU:15% MEM:45%] ➤ 
```

## ⚙️ 基础配置

### 配置文件位置
```bash
~/.nex/config.ini
```

### 快速配置示例

```ini
# 启用所有视觉功能
[shell.prompt]
show_time = true
show_git_info = true
show_system_status = true
use_colors = true

# 启用语法高亮
[shell.highlight]
enabled = true

# 启用智能补全
[shell.completion]
enabled = true
max_suggestions = 20
```

### 应用配置
```bash
# 查看当前配置
./build/nex config

# 重新加载Shell以应用更改
exit
./build/nex shell
```

## 🐚 Shell功能演示

### 管道和重定向
```bash
# 复杂管道
ls -la | grep .txt | wc -l

# 输出重定向
echo "Hello NeX" > output.txt
cat input.txt | sort > sorted.txt

# 错误重定向
find / -name "config" 2>/dev/null
```

### 环境变量
```bash
# 设置变量
export NEX_CONFIG_DIR="~/.nex"

# 使用变量
echo $NEX_CONFIG_DIR
cd ${NEX_CONFIG_DIR}
```

### 后台执行
```bash
# 后台运行
long_running_command &

# 查看后台任务
jobs
```

## 🔧 高级功能

### 自定义提示符
编辑配置文件 `~/.nex/config.ini`:

```ini
[shell.prompt]
format = \u@\h \w$ 
time_format = %H:%M:%S
```

### 自定义颜色主题
```ini
[shell.highlight]
command_color = bright_green
path_color = bright_yellow
flag_color = bright_cyan
string_color = bright_magenta
```

### Git状态详细信息
NeX自动检测Git状态：

- `●` 绿色: 已暂存更改
- `●` 红色: 未暂存更改
- `●` 黄色: 未跟踪文件
- `↑N`: 领先远程N个提交
- `↓N`: 落后远程N个提交

## 🤖 AI功能准备 (即将推出)

### 安装AI模型 (规划中)
```bash
# 下载推荐模型
nex ai model download codellama-7b

# 切换模型
nex ai model switch phi3-mini

# 查看模型状态
nex ai model info
```

### AI功能预览 (规划中)
```bash
# 自然语言查询
nex ai "查找所有Python文件"
# → find . -name "*.py"

# 命令解释
nex ai explain "tar -xzf file.tar.gz"
# → AI解释: 解压gzip压缩的tar文件

# 错误诊断
failed_command 2>&1 | nex ai diagnose
# → AI分析错误原因并提供解决方案
```

## 🔍 故障排除

### 常见问题

**1. 编译错误**
```bash
# 检查编译器版本
gcc --version
# 需要 GCC 10+ 或 Clang 12+

# 更新构建工具
sudo apt update
sudo apt install build-essential cmake
```

**2. readline错误**
```bash
# 安装readline开发库
sudo apt install libreadline-dev

# 重新构建
cd build
make clean
cmake ..
make
```

**3. 权限问题**
```bash
# 确保可执行权限
chmod +x build/nex

# 检查文件权限
ls -la build/nex
```

**4. 颜色显示问题**
```bash
# 检查终端支持
echo $TERM

# 临时禁用颜色
export NEX_NO_COLOR=1
```

### 获取帮助

```bash
# 查看详细帮助
./build/nex help

# Shell内置帮助
nex shell
help

# 查看版本信息
./build/nex version
```

## 📚 学习资源

- 📖 [完整用户手册](USER_MANUAL.md)
- 🏗️ [技术架构文档](TECHNICAL_ARCHITECTURE.md)
- 🤖 [AI集成规划](AI_INTEGRATION_PLAN.md)
- 🔧 [开发计划](development-plan.md)

## 🎯 下一步

1. **熟悉基础功能** - 在日常工作中使用NeX Shell
2. **自定义配置** - 根据个人喜好调整界面和行为
3. **参与开发** - 关注AI功能的开发进展
4. **反馈建议** - 提供使用体验和改进建议

---

🎉 **恭喜！** 你已经成功体验了NeX: AI-CLI的核心功能。现在开始享受智能化的命令行体验吧！

*有问题或建议？欢迎提交Issue或Pull Request！* 🚀
