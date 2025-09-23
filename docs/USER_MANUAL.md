# NeX: AI-CLI 使用手册

## 📖 简介

NeX（Next-generation eXperience）是一个现代化的AI增强命令行界面工具，专为Linux/Unix系统设计。它结合了传统Shell的强大功能与现代化的用户体验，为未来的AI集成奠定基础。

## 🚀 快速开始

### 安装要求

- **操作系统**: Linux (Ubuntu 20.04+, CentOS 8+, Arch Linux)
- **编译器**: GCC 10+ 或 Clang 12+ (支持C++20)
- **依赖库**: 
  - `libreadline-dev` (命令行编辑)
  - `cmake` 或 `xmake` (构建系统)
  - `git` (可选，用于Git集成功能)

### 构建安装

#### 方法一：CMake构建（推荐）

```bash
# 克隆仓库
git clone https://github.com/your-repo/nex-ai-cli.git
cd nex-ai-cli

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake ..
make -j$(nproc)

# 运行
./nex help
```

#### 方法二：XMake构建

```bash
# 安装XMake (如果未安装)
curl -fsSL https://xmake.io/shget.text | bash

# 构建
xmake build

# 运行
./build/linux/x86_64/debug/nex help
```

## 📋 命令参考

### 基本命令

| 命令 | 描述 | 示例 |
|------|------|------|
| `help` | 显示帮助信息 | `nex help` |
| `version` | 显示版本信息 | `nex version` |
| `status` | 显示系统状态概览 | `nex status` |
| `shell` | 启动交互式Shell | `nex shell` |
| `config` | 配置管理 | `nex config` |
| `analyze` | AI系统分析（开发中） | `nex analyze` |

### Shell模式

启动交互式Shell模式：

```bash
nex shell
```

#### 内置命令

| 命令 | 描述 | 示例 |
|------|------|------|
| `help` | Shell帮助 | `help` |
| `cd [path]` | 切换目录 | `cd /home/user` |
| `pwd` | 显示当前目录 | `pwd` |
| `echo [text]` | 输出文本 | `echo "Hello World"` |
| `export VAR=value` | 设置环境变量 | `export PATH=$PATH:/new/path` |
| `env` | 显示环境变量 | `env` |
| `history` | 命令历史 | `history` |
| `clear` | 清屏 | `clear` |
| `exit` | 退出Shell | `exit` |

#### 高级功能

**管道操作**：
```bash
ls -la | grep .txt | wc -l
```

**I/O重定向**：
```bash
echo "Hello" > output.txt
cat input.txt | sort > sorted.txt
find . -name "*.cpp" 2>/dev/null
```

**后台执行**：
```bash
long_running_command &
```

**环境变量展开**：
```bash
echo $HOME
echo ${USER}_backup
```

## ⚙️ 配置管理

### 配置文件位置

- **用户配置**: `~/.nex/config.ini`
- **系统配置**: `/etc/nex/config.ini`

### 配置文件格式

```ini
# NeX配置文件

[shell.prompt]
show_time = true
show_git_info = true
show_system_status = false
use_colors = true
format = \u@\h \w$ 
time_format = %H:%M:%S

[shell.highlight]
enabled = true
command_color = bold_blue
path_color = yellow
flag_color = cyan
string_color = green
number_color = magenta

[shell.completion]
enabled = true
case_sensitive = false
max_suggestions = 20
show_descriptions = true

[shell.history]
enabled = true
max_entries = 1000
ignore_duplicates = true
ignore_space_prefix = true
history_file = ~/.nex_history

[system.monitoring]
enabled = false
update_interval = 5
show_cpu = true
show_memory = true
show_load = true
show_processes = false

[system.logging]
enabled = true
level = info
file = ~/.nex.log
max_file_size = 10
max_backup_files = 5
```

### 配置命令

```bash
# 查看当前配置
nex config

# 编辑配置文件
nex config --edit

# 重置为默认配置
nex config --reset

# 验证配置文件
nex config --validate
```

## 🎨 界面特性

### 智能提示符

NeX提供丰富的提示符信息：

```
[12:34:56] user@hostname /home/user/project (main●) ➤ 
```

**组成元素**：
- `[12:34:56]` - 当前时间
- `user@hostname` - 用户名和主机名
- `/home/user/project` - 当前目录（支持~缩写）
- `(main●)` - Git分支信息
  - `main` - 当前分支
  - `●` - 工作区状态（●已暂存 ●未暂存 ●未跟踪）
  - `↑2` - 领先远程2个提交
  - `↓1` - 落后远程1个提交
- `➤` - 命令提示符

### 语法高亮

NeX支持实时语法高亮：

- **命令**: <span style="color: blue; font-weight: bold;">ls</span>
- **选项**: <span style="color: cyan;">-la</span>
- **路径**: <span style="color: yellow;">/home/user</span>
- **字符串**: <span style="color: green;">"hello world"</span>
- **数字**: <span style="color: magenta;">123</span>

### 智能补全

**Tab补全功能**：
- 命令补全：`ls` + Tab → 显示所有以`ls`开头的命令
- 路径补全：`cd /ho` + Tab → 补全为`/home/`
- 选项补全：`ls -` + Tab → 显示可用选项

**使用示例**：
```bash
# 命令补全
gi<Tab>     → git
ls --h<Tab> → ls --help

# 路径补全
cd ~/Doc<Tab>     → cd ~/Documents/
cat /etc/pas<Tab> → cat /etc/passwd

# 智能补全
git che<Tab> → git checkout
```

## 🔧 高级用法

### Git集成

NeX自动检测Git仓库并显示状态：

```bash
# 在Git仓库中工作
cd my-project/
nex shell

# 提示符将显示：
# [14:30:25] user@host ~/my-project (feature-branch●↑1) ➤
```

**状态指示器**：
- `●` 绿色：已暂存的更改
- `●` 红色：未暂存的更改  
- `●` 黄色：未跟踪的文件
- `↑N` 青色：领先远程N个提交
- `↓N` 青色：落后远程N个提交

### 系统监控

启用系统监控功能：

```ini
[system.monitoring]
enabled = true
update_interval = 5
show_cpu = true
show_memory = true
```

提示符将显示：
```
[14:30:25] user@host ~/project (main) [CPU:15% MEM:45%] ➤
```

### 自定义主题

创建自定义颜色主题：

```ini
[shell.highlight]
enabled = true
command_color = bright_green
path_color = bright_yellow
flag_color = bright_cyan
string_color = bright_magenta
number_color = bright_blue
```

**可用颜色**：
- 基本颜色：`red`, `green`, `blue`, `yellow`, `cyan`, `magenta`, `white`, `black`
- 明亮颜色：`bright_red`, `bright_green`, 等
- 样式：`bold`, `dim`, `underline`
- 组合：`bold_blue`, `dim_green`

## 🐛 故障排除

### 常见问题

**1. 命令找不到**
```bash
# 检查PATH设置
echo $PATH

# 重新加载配置
source ~/.bashrc
```

**2. 颜色显示异常**
```bash
# 检查终端支持
echo $TERM

# 禁用颜色（临时）
nex config --set shell.prompt.use_colors=false
```

**3. Git信息不显示**
```bash
# 检查Git可用性
which git

# 检查是否在Git仓库中
git status
```

**4. 补全功能不工作**
```bash
# 检查readline支持
nex config --set shell.completion.enabled=true

# 重启Shell
exit
nex shell
```

### 日志和调试

**查看日志**：
```bash
# 查看系统日志
tail -f ~/.nex.log

# 调试模式启动
NEX_DEBUG=1 nex shell
```

**性能分析**：
```bash
# 启用性能监控
nex config --set system.monitoring.enabled=true

# 查看资源使用
nex status
```

## 🔌 扩展开发

### 添加自定义命令

创建插件目录：
```bash
mkdir -p ~/.nex/plugins
```

示例插件 (`~/.nex/plugins/hello.cpp`)：
```cpp
#include "nex/shell/shell.h"

extern "C" {
    nex::shell::CommandResult hello_command(const nex::shell::ParsedCommand& cmd) {
        return nex::shell::CommandResult(0, "Hello from custom plugin!\n");
    }
    
    void register_plugin() {
        // 注册自定义命令
        auto& executor = nex::shell::CommandExecutor::getInstance();
        executor.registerBuiltin("hello", hello_command);
    }
}
```

## 📞 支持和社区

- **GitHub**: https://github.com/your-repo/nex-ai-cli
- **文档**: https://nex-cli.readthedocs.io
- **Issues**: https://github.com/your-repo/nex-ai-cli/issues
- **Wiki**: https://github.com/your-repo/nex-ai-cli/wiki

## 📄 许可证

NeX: AI-CLI 采用 MIT 许可证发布。详见 [LICENSE](LICENSE) 文件。

---

*NeX: 下一代AI增强命令行体验* 🚀
