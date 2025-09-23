# 构建指南

## 环境要求

### Linux
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y build-essential unzip git

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install unzip git
```

### Windows
- Visual Studio 2019+ 或 Visual Studio Build Tools
- Git for Windows
- 7-Zip 或其他解压工具

### macOS
```bash
# 安装Xcode命令行工具
xcode-select --install

# 使用Homebrew安装依赖
brew install git
```

## 安装XMake

### Linux/macOS
```bash
curl -fsSL https://xmake.io/shget.text | bash
```

### Windows
```powershell
Invoke-Expression (Invoke-Webrequest 'https://xmake.io/psget.text' -UseBasicParsing).Content
```

## 构建步骤

1. **克隆项目**
```bash
git clone <repository-url>
cd nex
```

2. **配置项目**
```bash
# Debug模式
xmake f -m debug

# Release模式
xmake f -m release

# 指定平台（如果需要）
xmake f -p [linux|windows|macosx] -a [x86_64|arm64]
```

3. **编译项目**
```bash
xmake
```

4. **运行程序**
```bash
# 直接运行
xmake run nex

# 运行特定命令
xmake run nex help
xmake run nex status
```

5. **安装（可选）**
```bash
# 安装到系统目录
xmake install

# 安装到指定目录
xmake install -o /usr/local
```

## 开发模式

### 启用详细输出
```bash
xmake -v
```

### 清理构建文件
```bash
xmake clean
```

### 重新配置
```bash
xmake f -c
```

### 运行测试
```bash
xmake run nex_test
```

## 跨平台编译

### Linux交叉编译到Windows
```bash
# 需要安装mingw-w64
sudo apt install mingw-w64
xmake f -p windows --toolchain=mingw
xmake
```

### 静态链接（减少依赖）
```bash
xmake f -m release --static=yes
xmake
```

## 故障排除

### 常见问题

1. **缺少unzip工具**
```bash
# Ubuntu/Debian
sudo apt install unzip

# CentOS/RHEL
sudo yum install unzip
```

2. **C++20支持问题**
确保使用足够新的编译器版本：
- GCC 10+
- Clang 10+
- MSVC 2019 16.8+

3. **依赖包下载失败**
```bash
# 更新包仓库
xmake repo --update

# 手动安装依赖
xmake require nlohmann_json cli11 spdlog fmt
```

4. **权限问题**
```bash
# Linux/macOS - 给予执行权限
chmod +x build/linux/x86_64/debug/aicli
```

## 性能优化

### Release构建优化
```bash
xmake f -m release --optimize=fastest
xmake
```

### 链接时优化（LTO）
```bash
xmake f -m release --lto=yes
xmake
```

## 调试

### 使用GDB调试
```bash
xmake f -m debug
xmake
gdb build/linux/x86_64/debug/nex
```

### 使用Valgrind检查内存
```bash
valgrind --leak-check=full ./build/linux/x86_64/debug/nex status
```

## 持续集成

项目支持以下CI/CD平台：
- GitHub Actions
- GitLab CI
- Jenkins

示例GitHub Actions配置在 `.github/workflows/` 目录中。
