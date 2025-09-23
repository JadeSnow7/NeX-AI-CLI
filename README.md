# NeX: AI-Powered Command Line Interface

![NeX Logo](docs/images/nex-logo-ascii.png)

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/your-repo/nex-ai-cli)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-orange)](releases)
[![C++](https://img.shields.io/badge/C%2B%2B-20-blue)](https://isocpp.org/)

> 🤖 **下一代智能命令行工具** - 将 AI 的力量带到你的终端

## ✨ 核心特性

### 🎨 现代化 Shell 体验
- **🕐 实时智能提示符** - 显示时间、路径、Git状态
- **🌈 语法高亮** - 彩色命令、参数、路径识别  
- **🔧 智能补全** - Tab键自动补全命令和路径
- **⚙️ 灵活配置** - INI格式配置文件，个性化定制
- **📊 系统监控** - 实时CPU、内存、负载显示

### 🔗 无缝集成
- **🔄 Git集成** - 自动显示分支状态和变更信息
- **🏗️ 双构建支持** - XMake 和 CMake 双重构建系统
- **🐧 跨平台** - Linux/Windows/macOS 原生支持
- **📚 丰富命令** - 内置命令 + 系统命令无缝结合

### 🤖 AI 驱动功能 (即将推出)
- **� 自然语言命令** - "查找所有Python文件" → `find . -name "*.py"`
- **🔍 智能错误诊断** - AI分析错误并提供解决方案
- **� 命令解释** - 复杂命令的智能解释
- **💡 上下文建议** - 基于工作环境的智能建议

## 🚀 快速开始

### 系统要求
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake libreadline-dev git

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake readline-devel git

# Arch Linux
sudo pacman -S base-devel cmake readline git
```

### 安装方式

#### 方式1: CMake 构建 (推荐)
```bash
git clone https://github.com/your-repo/nex-ai-cli.git
cd nex-ai-cli
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### 方式2: XMake 构建
```bash
# 配置项目
xmake f -m release

# 编译
xmake

# 运行
xmake run nex
```

### 立即使用
```bash
# 启动智能Shell
./build/nex shell

# 查看所有功能
./build/nex help

# 系统状态监控
./build/nex status

# 配置管理
./build/nex config
```

## 📚 文档导航

| 文档 | 描述 | 链接 |
|------|------|------|
| 🚀 **快速开始** | 5分钟上手指南 | [QUICK_START.md](docs/QUICK_START.md) |
| 📖 **用户手册** | 完整功能说明 | [USER_MANUAL.md](docs/USER_MANUAL.md) |
| 🏗️ **技术架构** | 系统设计文档 | [TECHNICAL_ARCHITECTURE.md](docs/TECHNICAL_ARCHITECTURE.md) |
| 🤖 **AI集成计划** | AI功能路线图 | [AI_INTEGRATION_PLAN.md](docs/AI_INTEGRATION_PLAN.md) |
| 📋 **开发计划** | 项目进展状态 | [development-plan.md](development-plan.md) |

## 🤖 AI 模型支持规划

### 推荐模型配置

| 模型 | 大小 | 内存需求 | 推理速度 | 适用场景 |
|------|------|----------|----------|----------|
| **CodeLlama-7B-Instruct** | 3.8GB | 8GB RAM | 快速 | 代码生成、命令解释 |
| **Phi-3-Mini-Instruct** | 2.2GB | 4GB RAM | 极快 | 轻量级助手、快速响应 |
| **自定义微调模型** | 可变 | 可变 | 可变 | 专业领域优化 |

### AI 功能预览
```bash
# 自然语言转命令 (规划中)
$ nex ai "查找大于100MB的文件"
🤖 AI: find . -size +100M -type f

# 错误智能诊断 (规划中)
$ failed_command 2>&1 | nex ai diagnose
🤖 分析: 权限不足，建议运行 'sudo failed_command' 或检查文件权限

# 命令解释器 (规划中)
$ nex ai explain "tar -xzf archive.tar.gz" 
🤖 解释: 解压 gzip 压缩的 tar 归档文件
```

## 🛣️ 开发路线图

### ✅ 已完成功能
- [x] 现代化Shell核心
- [x] 智能提示符系统
- [x] 语法高亮引擎
- [x] Tab补全机制
- [x] Git状态集成
- [x] 配置管理系统
- [x] 双构建系统支持
- [x] 跨平台兼容性

### 🚧 开发中功能 (Phase 1 - AI基础)
- [ ] llama.cpp 集成
- [ ] 基础AI命令框架
- [ ] 本地模型管理
- [ ] AI配置扩展

### 🗓️ 计划功能 (Phase 2 - AI增强)
- [ ] 自然语言命令生成
- [ ] 智能错误诊断
- [ ] 上下文感知建议
- [ ] 命令历史分析

## 👥 贡献指南

我们欢迎各种形式的贡献！

### 🔧 技术贡献
```bash
# 1. Fork 项目
git clone https://github.com/your-username/nex-ai-cli.git

# 2. 创建功能分支
git checkout -b feature/amazing-feature

# 3. 提交更改
git commit -m "✨ Add amazing feature"

# 4. 推送分支
git push origin feature/amazing-feature

# 5. 创建 Pull Request
```

## 📄 许可证

本项目基于 [MIT License](LICENSE) 开源协议。

---

<div align="center">

**🚀 立即体验 NeX: AI-CLI，开启智能命令行新时代！**

[![立即下载](https://img.shields.io/badge/立即下载-NeX%201.0.0-brightgreen?style=for-the-badge)](releases)

*用 ❤️ 和 🤖 在开源社区中构建*

</div>
# NeX-AI-CLI
