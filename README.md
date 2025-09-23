# NeX: AI-Powered Command Line Interface

![NeX Logo](docs/images/nex-logo-ascii.png)

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/JadeSnow7/NeX-AI-CLI)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.0-orange)](releases)
[![C++](https://img.shields.io/badge/C%2B%2B-20-blue)](https://isocpp.org/)
[![AI Support](https://img.shields.io/badge/AI-双模型支持-purple)](https://github.com/JadeSnow7/NeX-AI-CLI)

> 🤖 **下一代智能命令行工具** - 支持本地和在线AI模型的智能终端

## ✨ 核心特性

### 🎨 现代化 Shell 体验
- **🕐 实时智能提示符** - 显示时间、路径、Git状态
- **🌈 语法高亮** - 彩色命令、参数、路径识别  
- **🔧 智能补全** - Tab键自动补全命令和路径
- **⚙️ 灵活配置** - 个性化定制选项
- **📊 系统监控** - 实时CPU、内存、负载显示

### 🤖 AI 双模型架构 (最新功能!)
- **�️ 本地模型支持** - llama.cpp + Qwen3-Coder/CodeLlama
- **🌐 在线模型支持** - OpenAI GPT、Anthropic Claude
- **💬 自然语言命令** - "下载gcc" → `sudo apt install gcc`
- **🔍 智能错误诊断** - AI分析错误并提供解决方案
- **💡 上下文建议** - 基于工作环境的智能建议
- **🔄 模型自由切换** - 本地/在线模型灵活切换

### �🔗 无缝集成
- **🔄 Git集成** - 自动显示分支状态和变更信息
- **🏗️ 现代构建支持** - CMake 构建系统
- **🐧 跨平台** - Linux/Windows/macOS 原生支持
- **📚 丰富命令** - 内置命令 + 系统命令无缝结合

##  快速开始

### 系统要求
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake libreadline-dev git libcurl4-openssl-dev libjsoncpp-dev

# CentOS/RHEL  
sudo yum groupinstall "Development Tools"
sudo yum install cmake readline-devel git libcurl-devel jsoncpp-devel

# Arch Linux
sudo pacman -S base-devel cmake readline git curl jsoncpp
```

### 安装步骤

```bash
# 1. 克隆项目
git clone https://github.com/JadeSnow7/NeX-AI-CLI.git
cd NeX-AI-CLI

# 2. 配置和构建
cmake -B build
cmake --build build

# 3. 安装 (可选)
sudo cp build/nex /usr/local/bin/

# 4. 立即使用
nex help
```

## 🤖 AI 功能使用指南

### 初始化 AI 系统
```bash
# 初始化AI引擎
nex ai init

# 查看AI状态
nex ai status
```

### 本地模型配置
```bash
# 下载推荐模型 (需要约4-8GB空间)
mkdir -p ~/.nex/models
cd ~/.nex/models

# 下载 Qwen3-Coder 模型 (推荐)
wget https://huggingface.co/Qwen/Qwen3-Coder-7B-Instruct-GGUF/resolve/main/qwen3-coder-7b-instruct.q4_k_m.gguf

# 切换到本地模型
nex ai local ~/.nex/models/qwen3-coder-7b-instruct.q4_k_m.gguf
```

### 在线模型配置
```bash
# OpenAI GPT模型
nex ai online openai YOUR_API_KEY gpt-3.5-turbo

# Anthropic Claude模型  
nex ai online anthropic YOUR_API_KEY claude-3-sonnet-20240229

# 自定义API服务
nex ai online custom YOUR_API_KEY your-model-name
```

### 自然语言命令示例
```bash
# 命令生成模式 (生成并执行Shell命令)
nex command 安装gcc              # → sudo apt install gcc
nex command 查找cpp文件          # → find . -name "*.cpp"
nex command 查看磁盘空间         # → df -h
nex command 监控内存使用         # → free -h

# AI对话模式 (普通对话交流)
nex 你好，今天天气怎么样          # → AI友好回应
nex 请解释什么是Docker          # → AI详细解答
nex 推荐一些学习编程的资源       # → AI建议和指导
nex 如何优化代码性能            # → AI技术建议
```

## 📋 支持的AI模型

### 🖥️ 本地模型 (推荐用于隐私和离线使用)

| 模型 | 大小 | 内存需求 | 推理速度 | 特点 |
|------|------|----------|----------|------|
| **Qwen3-Coder-7B** | 4.2GB | 8GB RAM | 快速 | 专业代码生成，中文支持优秀 |
| **CodeLlama-7B** | 3.8GB | 8GB RAM | 快速 | Meta开源，代码理解能力强 |
| **Phi-3-Mini** | 2.2GB | 4GB RAM | 极快 | 微软轻量级模型，响应迅速 |

### 🌐 在线模型 (推荐用于最新功能)

| 提供商 | 模型 | 特点 | 使用场景 |
|--------|------|------|----------|
| **OpenAI** | gpt-3.5-turbo | 快速、经济 | 日常命令生成 |
| **OpenAI** | gpt-4 | 最强能力 | 复杂任务处理 |
| **Anthropic** | claude-3-sonnet | 安全、准确 | 企业级应用 |

## � 高级用法

### 模型性能对比
```bash
# 测试不同模型的响应速度
nex ai test --benchmark

# 查看模型详细信息
nex ai model --details
```

### 批量命令处理
```bash
# 创建命令脚本
echo "查看磁盘空间
监控CPU使用
查找大文件" > tasks.txt

# 批量执行
nex ai batch tasks.txt
```

### 上下文管理
```bash
# 设置工作上下文
nex ai context set --project cpp-game-engine

# 查看当前上下文
nex ai context show

# 重置上下文
nex ai context reset
```

## 🔍 故障排除

### 常见问题

#### AI 初始化失败
```bash
# 检查AI状态
nex ai status

# 重新初始化
nex ai init --force

# 检查日志
tail -f ~/.nex/logs/ai.log
```

#### 本地模型加载错误
```bash
# 验证模型文件
nex ai local --verify <model_path>

# 检查系统资源
nex system memory

# 降低模型参数
nex ai config set threads 4
nex ai config set context_length 2048
```

#### 在线模型连接问题
```bash
# 测试API连接
nex ai online --test openai YOUR_API_KEY

# 检查网络
curl -s https://api.openai.com/v1/models

# 设置代理 (如需要)
export https_proxy=http://localhost:7890
```

### 性能优化

#### 本地模型优化
```bash
# CPU优化
nex ai config set threads $(nproc)

# 内存优化
nex ai config set mmap true
nex ai config set mlock true

# 量化模型选择
nex ai local model.q4_k_m.gguf  # 平衡性能和质量
nex ai local model.q8_0.gguf    # 更高质量，更大内存
```

#### 在线模型优化
```bash
# 设置超时
nex ai config set timeout 30

# 设置重试
nex ai config set retry_count 3

# 批量请求
nex ai config set batch_size 5
```

## 🏗️ 开发指南

### 构建依赖
```bash
# 必需依赖
- CMake >= 3.16
- C++20 编译器 (GCC 10+/Clang 12+)
- libreadline-dev
- libcurl4-openssl-dev
- libjsoncpp-dev

# 可选依赖 (AI功能)
- CUDA Toolkit (GPU加速)
- OpenBLAS (CPU加速)
```

### 开发环境设置
```bash
# 开发者构建
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DAI_ENABLED=ON
cmake --build build --parallel

# 运行测试
cd build && ctest --verbose

# 调试版本
gdb ./build/nex
```

### 添加新的AI模型支持
```cpp
// 1. 扩展 ModelType 枚举
enum class ModelType {
    LOCAL_LLAMA_CPP,
    ONLINE_OPENAI,
    ONLINE_ANTHROPIC,
    ONLINE_YOUR_NEW_PROVIDER  // 新增
};

// 2. 实现模型接口
class YourNewProviderModel : public OnlineModelInterface {
    // 实现必需方法
};

// 3. 注册到工厂
auto model = std::make_unique<YourNewProviderModel>(config);
```

## 📊 使用统计

### 支持的语言场景

#### 🤖 命令生成模式 (`nex command <描述>`)
专门用于生成和执行Shell命令：
- **系统管理**: `nex command 重启nginx` → `sudo systemctl restart nginx`
- **包管理**: `nex command 安装vim` → `sudo apt install vim`
- **文件操作**: `nex command 查找大文件` → `find . -size +100M`
- **进程管理**: `nex command 杀死firefox` → `killall firefox`
- **网络调试**: `nex command 测试网络` → `ping -c 4 google.com`
- **Git操作**: `nex command 提交代码` → `git add . && git commit -m "update"`

#### 💬 AI对话模式 (`nex <任何文本>`)
用于正常的AI聊天和咨询：
- **技术咨询**: `nex 什么是Docker容器` → AI详细解释
- **学习指导**: `nex 如何学习机器学习` → AI提供学习路径
- **代码建议**: `nex 如何优化这段Python代码` → AI代码建议
- **问题解答**: `nex 为什么我的程序运行缓慢` → AI诊断建议
- **日常对话**: `nex 今天天气怎么样` → AI友好回应
- **技术讨论**: `nex 比较React和Vue的优缺点` → AI深度分析

### 性能基准
```
本地模型响应时间:
- Qwen3-Coder-7B (Q4): ~2-3秒
- CodeLlama-7B (Q4): ~2-4秒  
- Phi-3-Mini (Q4): ~1-2秒

在线模型响应时间:
- GPT-3.5-turbo: ~1-2秒
- GPT-4: ~3-5秒
- Claude-3-Sonnet: ~2-4秒
```

## 🚀 路线图

### v1.1 (即将发布)
- [ ] 🔄 AI模型自动更新
- [ ] 🌐 更多在线模型支持 (Gemini, Cohere)
- [ ] 📝 对话历史管理
- [ ] 🎯 专业领域模型 (DevOps, Security)

### v1.2 (规划中)
- [ ] 🔌 插件系统
- [ ] 🌍 多语言界面 (English, 中文)
- [ ] 📊 使用分析和建议
- [ ] 🎮 交互式教程

### v2.0 (长期目标)
- [ ] 🧠 本地知识库集成
- [ ] 🤝 团队协作功能
- [ ] 🔐 企业级安全特性
- [ ] 📱 移动端支持

## 🤝 贡献指南

我们欢迎各种形式的贡献！

### 报告Bug
1. 检查 [现有问题](https://github.com/JadeSnow7/NeX-AI-CLI/issues)
2. 创建详细的Bug报告
3. 提供重现步骤和系统信息

### 功能请求
1. 描述新功能的用途
2. 提供使用场景示例
3. 考虑向后兼容性

### 代码贡献
```bash
# 1. Fork 项目
# 2. 创建特性分支
git checkout -b feature/amazing-feature

# 3. 提交更改
git commit -m 'Add amazing feature'

# 4. 推送分支
git push origin feature/amazing-feature

# 5. 创建 Pull Request
```

## 📄 许可证

本项目基于 [MIT 许可证](LICENSE) 开源。

## 🙏 致谢

- [llama.cpp](https://github.com/ggerganov/llama.cpp) - 优秀的LLM推理引擎
- [OpenAI](https://openai.com/) - 强大的API服务
- [Anthropic](https://anthropic.com/) - Claude模型支持
- [readline](https://tiswww.case.edu/php/chet/readline/rltop.html) - 终端交互库

---

<div align="center">

**[官方网站](https://github.com/JadeSnow7/NeX-AI-CLI)** • 
**[文档](docs/)** • 
**[示例](examples/)** • 
**[问题反馈](https://github.com/JadeSnow7/NeX-AI-CLI/issues)**

⭐ 如果这个项目对你有帮助，请给我们一个Star！

</div>

## 🛠️ 配置选项

### AI 模型配置
```bash
# 查看当前配置
nex ai status

# 模型切换示例
nex ai local ~/.nex/models/qwen3-coder-7b.gguf
nex ai online openai sk-xxx gpt-4
```

### 性能调优
```bash
# 本地模型参数 (在配置文件中)
{
  "ai": {
    "local": {
      "threads": 8,           # CPU线程数
      "context_length": 4096, # 上下文长度
      "temperature": 0.1      # 生成随机性
    }
  }
}
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
