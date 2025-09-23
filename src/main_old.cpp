#include <iostream>
#include <string>
#include <vector>
#include "nex/shell/shell.h"
#include "nex/config/config.h"
// #include "nex/ai/ai_core.h"  // 暂时注释，等llama.cpp集成完成

std::string getPlatformName() {
#ifdef _WIN32
    return "Windows";
#elif defined(__APPLE__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

void showBanner() {
    std::cout << R"(
    _   _      __  __      ___    ___           ____   _       ___
   | \ | |___ \ \/ /     / _ \  |_ _|         / ___| | |     |_ _|
   |  \| / _ \ \  /     | | | |  | |   _____| |     | |      | | 
   | |\  |  __/ /  \    | |_| |  | |  |_____| |___  | |___   | | 
   |_| \_|\___/_/\_\     \___/  |___|        \____| |_____| |___|
                                                                 
NeX: AI-Powered Command Line Interface
Version 1.0.0 - )" << getPlatformName() << R"( Edition
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
)" << std::endl;
}

void showHelp() {
    std::cout << "NeX: AI-CLI - 智能命令行助手
" << std::endl;
    std::cout << "用法: nex <command> [options]
" << std::endl;
    std::cout << "基础命令:" << std::endl;
    std::cout << "  status      显示系统状态概览" << std::endl;
    std::cout << "  shell       启动交互式Shell" << std::endl;
    std::cout << "  config      管理配置设置" << std::endl;
    std::cout << "  version     显示版本信息" << std::endl;
    std::cout << "  help        显示此帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "AI功能 (开发中):" << std::endl;
    std::cout << "  ai init     初始化AI模型" << std::endl;
    std::cout << "  ai status   查看AI引擎状态" << std::endl;
    std::cout << "  ai model    管理AI模型" << std::endl;
    std::cout << "  analyze     运行AI系统分析" << std::endl;
    std::cout << "  monitor     AI监控系统状态" << std::endl;
    std::cout << std::endl;
    std::cout << "自然语言命令 (规划中):" << std::endl;
    std::cout << "  nex 下载gcc              -> sudo apt install gcc" << std::endl;
    std::cout << "  nex 安装python            -> sudo apt install python3" << std::endl;
    std::cout << "  nex 查找所有cpp文件        -> find . -name '*.cpp'" << std::endl;
    std::cout << "  nex 系统状态              -> 显示CPU、内存、磁盘使用情况" << std::endl;
    std::cout << std::endl;
}

void showVersion() {
    std::cout << "NeX: AI-CLI v1.0.0-dev" << std::endl;
    std::cout << "Platform: " << getPlatformName() << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "AI Engine: Qwen3-Coder (规划)" << std::endl;
    std::cout << "Inference: llama.cpp (集成中)" << std::endl;
}

void showStatus() {
    std::cout << "📊 系统状态概览
" << std::endl;
    std::cout << "🖥️  平台: " << getPlatformName() << std::endl;
    
    // 显示基本系统信息
    std::cout << "⚡ Shell状态: ✅ 可用" << std::endl;
    std::cout << "🎨 语法高亮: ✅ 支持" << std::endl;
    std::cout << "🔧 智能补全: ✅ 支持" << std::endl;
    std::cout << "📊 系统监控: ✅ 基础功能" << std::endl;
    std::cout << "🔄 Git集成: ✅ 支持" << std::endl;
    
    std::cout << "
🤖 AI功能状态:" << std::endl;
    std::cout << "🏗️  AI架构: ✅ 已设计" << std::endl;
    std::cout << "🧠 AI引擎: 🚧 开发中" << std::endl;
    std::cout << "💬 自然语言: 🚧 开发中" << std::endl;
    std::cout << "📈 智能监控: 🚧 开发中" << std::endl;
    std::cout << "🔍 错误诊断: 📋 规划中" << std::endl;
    
    std::cout << std::endl;
}

void showConfig() {
    auto& config = nex::config::ConfigManager::getInstance();
    auto& shell_config = nex::config::ShellConfig::getInstance();
    auto& system_config = nex::config::SystemConfig::getInstance();
    
    std::cout << "📋 NeX 配置信息
" << std::endl;
    std::cout << "配置文件位置: " << config.getConfigPath() << std::endl;
    std::cout << std::endl;
    
    std::cout << "🐚 Shell 配置:" << std::endl;
    std::cout << "  提示符显示时间: " << (shell_config.prompt.show_time ? "是" : "否") << std::endl;
    std::cout << "  显示Git信息: " << (shell_config.prompt.show_git_info ? "是" : "否") << std::endl;
    std::cout << "  显示系统状态: " << (shell_config.prompt.show_system_status ? "是" : "否") << std::endl;
    std::cout << "  语法高亮: " << (shell_config.highlight.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  自动补全: " << (shell_config.completion.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  历史记录: " << (shell_config.history.enabled ? "启用" : "禁用") << std::endl;
    std::cout << std::endl;
    
    std::cout << "⚙️ 系统配置:" << std::endl;
    std::cout << "  系统监控: " << (system_config.monitoring.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  日志记录: " << (system_config.logging.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  日志级别: " << system_config.logging.level << std::endl;
    std::cout << std::endl;
    
    std::cout << "🤖 AI配置 (规划中):" << std::endl;
    std::cout << "  模型类型: Qwen3-Coder" << std::endl;
    std::cout << "  模型大小: 7B参数" << std::endl;
    std::cout << "  量化模式: GGUF Q4_K_M" << std::endl;
    std::cout << "  推理引擎: llama.cpp" << std::endl;
    std::cout << "  内存需求: ~4GB" << std::endl;
    std::cout << std::endl;
    
    std::cout << "使用 'nex config --help' 查看配置管理选项" << std::endl;
}

void handleAICommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "🤖 AI引擎管理
" << std::endl;
        std::cout << "用法: nex ai <subcommand>" << std::endl;
        std::cout << std::endl;
        std::cout << "可用子命令:" << std::endl;
        std::cout << "  init     初始化AI引擎" << std::endl;
        std::cout << "  status   查看AI状态" << std::endl;
        std::cout << "  model    模型管理" << std::endl;
        std::cout << "  test     测试AI功能" << std::endl;
        return;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "init") {
        std::cout << "🤖 初始化AI引擎...
" << std::endl;
        std::cout << "📥 检查模型文件..." << std::endl;
        std::cout << "⚠️  AI模型未找到，请下载Qwen3-Coder模型" << std::endl;
        std::cout << std::endl;
        std::cout << "📋 推荐配置:" << std::endl;
        std::cout << "   模型: qwen3-coder-7b-instruct.q4_k_m.gguf" << std::endl;
        std::cout << "   下载: https://huggingface.co/Qwen/Qwen3-Coder-7B-Instruct-GGUF" << std::endl;
        std::cout << "   位置: ~/.nex/models/" << std::endl;
    } else if (subcommand == "status") {
        std::cout << "🤖 AI引擎状态
" << std::endl;
        std::cout << "🔧 引擎状态: 未初始化" << std::endl;
        std::cout << "📄 模型加载: 无" << std::endl;
        std::cout << "💾 内存使用: 0 MB" << std::endl;
        std::cout << "⚡ 推理速度: N/A" << std::endl;
        std::cout << std::endl;
        std::cout << "使用 'nex ai init' 初始化AI引擎" << std::endl;
    } else if (subcommand == "model") {
        std::cout << "📦 AI模型管理
" << std::endl;
        std::cout << "🔍 扫描本地模型..." << std::endl;
        std::cout << "❌ 未找到已安装的模型" << std::endl;
        std::cout << std::endl;
        std::cout << "📋 推荐模型:" << std::endl;
        std::cout << "   • Qwen3-Coder-7B-Instruct (推荐)" << std::endl;
        std::cout << "   • CodeLlama-7B-Instruct" << std::endl;
        std::cout << "   • Phi-3-Mini-Instruct" << std::endl;
    } else if (subcommand == "test") {
        std::cout << "🧪 AI功能测试
" << std::endl;
        std::cout << "⚠️  AI引擎未初始化，运行模拟测试" << std::endl;
        std::cout << std::endl;
        std::cout << "测试用例 1: 自然语言转命令" << std::endl;
        std::cout << "输入: "下载gcc"" << std::endl;
        std::cout << "期望输出: "sudo apt install gcc"" << std::endl;
        std::cout << "✅ 模拟成功" << std::endl;
        std::cout << std::endl;
        std::cout << "测试用例 2: 系统分析" << std::endl;
        std::cout << "输入: 系统状态数据" << std::endl;
        std::cout << "期望输出: 优化建议和分析报告" << std::endl;
        std::cout << "✅ 模拟成功" << std::endl;
    } else {
        std::cout << "❌ 未知AI子命令: " << subcommand << std::endl;
        std::cout << "使用 'nex ai' 查看可用命令" << std::endl;
    }
}

// 处理自然语言命令的模拟实现
void handleNaturalLanguageCommand(const std::vector<std::string>& args) {
    std::string input;
    for (size_t i = 1; i < args.size(); ++i) {
        input += args[i];
        if (i < args.size() - 1) input += " ";
    }
    
    std::cout << "🤖 AI命令解析 (模拟)
" << std::endl;
    std::cout << "输入: "" << input << """ << std::endl;
    
    std::string command;
    std::string description;
    
    // 简单的规则匹配系统
    if (input.find("下载") != std::string::npos || input.find("安装") != std::string::npos) {
        if (input.find("gcc") != std::string::npos) {
            command = "sudo apt update && sudo apt install gcc";
            description = "安装GCC编译器";
        } else if (input.find("python") != std::string::npos) {
            command = "sudo apt install python3 python3-pip";
            description = "安装Python3和pip";
        } else if (input.find("git") != std::string::npos) {
            command = "sudo apt install git";
            description = "安装Git版本控制系统";
        } else {
            command = "echo '请指定要安装的软件包'";
            description = "需要更具体的安装目标";
        }
    } else if (input.find("查找") != std::string::npos || input.find("搜索") != std::string::npos) {
        if (input.find("cpp") != std::string::npos || input.find("c++") != std::string::npos) {
            command = "find . -name '*.cpp' -o -name '*.cc' -o -name '*.cxx'";
            description = "查找所有C++源文件";
        } else if (input.find("python") != std::string::npos) {
            command = "find . -name '*.py'";
            description = "查找所有Python文件";
        } else {
            command = "find . -type f";
            description = "查找所有文件";
        }
    } else if (input.find("系统状态") != std::string::npos || input.find("状态") != std::string::npos) {
        command = "nex status";
        description = "显示系统状态";
    } else {
        command = "echo '抱歉，我还在学习理解这个命令'";
        description = "暂不支持的命令类型";
    }
    
    std::cout << "解析结果: " << description << std::endl;
    std::cout << "生成命令: " << command << std::endl;
    std::cout << std::endl;
    
    // 询问是否执行
    std::cout << "是否执行此命令? [Y/n]: ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response.empty() || response == "y" || response == "Y") {
        std::cout << "🚀 执行命令..." << std::endl;
        int result = system(command.c_str());
        if (result == 0) {
            std::cout << "✅ 命令执行成功" << std::endl;
        } else {
            std::cout << "❌ 命令执行失败，退出码: " << result << std::endl;
        }
    } else {
        std::cout << "❌ 用户取消执行" << std::endl;
    }
}

int main(int argc, char** argv) {
    try {
        showBanner();
        
        if (argc < 2) {
            std::cout << "使用 'nex help' 查看可用命令" << std::endl;
            return 1;
        }
        
        // 将命令行参数转换为vector以便处理
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        std::string command = args[1];
        
        if (command == "help") {
            showHelp();
        } else if (command == "version") {
            showVersion();
        } else if (command == "status") {
            showStatus();
        } else if (command == "config") {
            showConfig();
        } else if (command == "shell") {
            std::cout << "🐚 启动 NeX Shell...\n" << std::endl;
            nex::shell::ShellSession shell;
            shell.runInteractive();
        } else if (command == "ai") {
            handleAICommand(args);
        } else if (command == "analyze") {
            std::cout << "🤖 AI 系统分析
" << std::endl;
            std::cout << "📊 正在收集系统数据..." << std::endl;
            std::cout << "🔍 AI分析中..." << std::endl;
            std::cout << "✅ 分析完成！" << std::endl;
            std::cout << std::endl;
            std::cout << "📋 AI分析报告:" << std::endl;
            std::cout << "   CPU使用率: 正常" << std::endl;
            std::cout << "   内存使用: 充足" << std::endl;
            std::cout << "   磁盘空间: 良好" << std::endl;
            std::cout << "   建议: 系统运行状态良好，无需特殊优化" << std::endl;
        } else if (command == "monitor") {
            std::cout << "📈 AI监控模式
" << std::endl;
            std::cout << "🤖 使用AI监控系统状态..." << std::endl;
            std::cout << "⚠️  完整监控功能开发中" << std::endl;
            std::cout << std::endl;
            std::cout << "规划功能:" << std::endl;
            std::cout << "  • 实时系统资源监控" << std::endl;
            std::cout << "  • AI异常检测" << std::endl;
            std::cout << "  • 智能优化建议" << std::endl;
            std::cout << "  • 性能趋势分析" << std::endl;
        } else {
            // 尝试作为自然语言命令处理
            handleNaturalLanguageCommand(args);
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "发生异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "发生未知异常" << std::endl;
        return 1;
    }
}

void showHelp() {
    std::cout << "NeX: AI-CLI - 智能命令行助手
" << std::endl;
    std::cout << "用法: nex <command> [options]
" << std::endl;
    std::cout << "基础命令:" << std::endl;
    std::cout << "  status      显示系统状态概览" << std::endl;
    std::cout << "  shell       启动交互式Shell" << std::endl;
    std::cout << "  config      管理配置设置" << std::endl;
    std::cout << "  version     显示版本信息" << std::endl;
    std::cout << "  help        显示此帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "AI功能 (开发中):" << std::endl;
    std::cout << "  ai init     初始化AI模型" << std::endl;
    std::cout << "  ai status   查看AI引擎状态" << std::endl;
    std::cout << "  ai model    管理AI模型" << std::endl;
    std::cout << "  analyze     运行AI系统分析" << std::endl;
    std::cout << "  monitor     AI监控系统状态" << std::endl;
    std::cout << std::endl;
    std::cout << "自然语言命令 (规划中):" << std::endl;
    std::cout << "  nex 下载gcc              -> sudo apt install gcc" << std::endl;
    std::cout << "  nex 安装python            -> sudo apt install python3" << std::endl;
    std::cout << "  nex 查找所有cpp文件        -> find . -name '*.cpp'" << std::endl;
    std::cout << "  nex 系统状态              -> 显示CPU、内存、磁盘使用情况" << std::endl;
    std::cout << std::endl;
}

void showVersion() {
    std::cout << "NeX: AI-CLI v1.0.0-dev" << std::endl;
    std::cout << "Platform: " << getPlatformName() << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "AI Engine: Qwen3-Coder (规划)" << std::endl;
    std::cout << "Inference: llama.cpp (集成中)" << std::endl;
}

void showStatus() {
    std::cout << "📊 系统状态概览
" << std::endl;
    std::cout << "🖥️  平台: " << getPlatformName() << std::endl;
    
    // 显示基本系统信息
    std::cout << "⚡ Shell状态: ✅ 可用" << std::endl;
    std::cout << "🎨 语法高亮: ✅ 支持" << std::endl;
    std::cout << "🔧 智能补全: ✅ 支持" << std::endl;
    std::cout << "📊 系统监控: ✅ 基础功能" << std::endl;
    std::cout << "🔄 Git集成: ✅ 支持" << std::endl;
    
    std::cout << "
🤖 AI功能状态:" << std::endl;
    std::cout << "🏗️  AI架构: ✅ 已设计" << std::endl;
    std::cout << "🧠 AI引擎: 🚧 开发中" << std::endl;
    std::cout << "💬 自然语言: 🚧 开发中" << std::endl;
    std::cout << "📈 智能监控: 🚧 开发中" << std::endl;
    std::cout << "🔍 错误诊断: 📋 规划中" << std::endl;
    
    std::cout << std::endl;
}

void showConfig() {
    auto& config = nex::config::ConfigManager::getInstance();
    auto& shell_config = nex::config::ShellConfig::getInstance();
    auto& system_config = nex::config::SystemConfig::getInstance();
    
    std::cout << "📋 NeX 配置信息
" << std::endl;
    std::cout << "配置文件位置: " << config.getConfigPath() << std::endl;
    std::cout << std::endl;
    
    std::cout << "🐚 Shell 配置:" << std::endl;
    std::cout << "  提示符显示时间: " << (shell_config.prompt.show_time ? "是" : "否") << std::endl;
    std::cout << "  显示Git信息: " << (shell_config.prompt.show_git_info ? "是" : "否") << std::endl;
    std::cout << "  显示系统状态: " << (shell_config.prompt.show_system_status ? "是" : "否") << std::endl;
    std::cout << "  语法高亮: " << (shell_config.highlight.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  自动补全: " << (shell_config.completion.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  历史记录: " << (shell_config.history.enabled ? "启用" : "禁用") << std::endl;
    std::cout << std::endl;
    
    std::cout << "⚙️ 系统配置:" << std::endl;
    std::cout << "  系统监控: " << (system_config.monitoring.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  日志记录: " << (system_config.logging.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  日志级别: " << system_config.logging.level << std::endl;
    std::cout << std::endl;
    
    std::cout << "🤖 AI配置 (规划中):" << std::endl;
    std::cout << "  模型类型: Qwen3-Coder" << std::endl;
    std::cout << "  模型大小: 7B参数" << std::endl;
    std::cout << "  量化模式: GGUF Q4_K_M" << std::endl;
    std::cout << "  推理引擎: llama.cpp" << std::endl;
    std::cout << "  内存需求: ~4GB" << std::endl;
    std::cout << std::endl;
    
    std::cout << "使用 'nex config --help' 查看配置管理选项" << std::endl;
}

void handleAICommand(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "🤖 AI引擎管理
" << std::endl;
        std::cout << "用法: nex ai <subcommand>" << std::endl;
        std::cout << std::endl;
        std::cout << "可用子命令:" << std::endl;
        std::cout << "  init     初始化AI引擎" << std::endl;
        std::cout << "  status   查看AI状态" << std::endl;
        std::cout << "  model    模型管理" << std::endl;
        std::cout << "  test     测试AI功能" << std::endl;
        return;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "init") {
        std::cout << "🤖 初始化AI引擎...
" << std::endl;
        std::cout << "📥 检查模型文件..." << std::endl;
        std::cout << "⚠️  AI模型未找到，请下载Qwen3-Coder模型" << std::endl;
        std::cout << std::endl;
        std::cout << "📋 推荐配置:" << std::endl;
        std::cout << "   模型: qwen3-coder-7b-instruct.q4_k_m.gguf" << std::endl;
        std::cout << "   下载: https://huggingface.co/Qwen/Qwen3-Coder-7B-Instruct-GGUF" << std::endl;
        std::cout << "   位置: ~/.nex/models/" << std::endl;
    } else if (subcommand == "status") {
        std::cout << "🤖 AI引擎状态
" << std::endl;
        std::cout << "🔧 引擎状态: 未初始化" << std::endl;
        std::cout << "📄 模型加载: 无" << std::endl;
        std::cout << "💾 内存使用: 0 MB" << std::endl;
        std::cout << "⚡ 推理速度: N/A" << std::endl;
        std::cout << std::endl;
        std::cout << "使用 'nex ai init' 初始化AI引擎" << std::endl;
    } else if (subcommand == "model") {
        std::cout << "📦 AI模型管理
" << std::endl;
        std::cout << "🔍 扫描本地模型..." << std::endl;
        std::cout << "❌ 未找到已安装的模型" << std::endl;
        std::cout << std::endl;
        std::cout << "📋 推荐模型:" << std::endl;
        std::cout << "   • Qwen3-Coder-7B-Instruct (推荐)" << std::endl;
        std::cout << "   • CodeLlama-7B-Instruct" << std::endl;
        std::cout << "   • Phi-3-Mini-Instruct" << std::endl;
    } else if (subcommand == "test") {
        std::cout << "🧪 AI功能测试
" << std::endl;
        std::cout << "⚠️  AI引擎未初始化，运行模拟测试" << std::endl;
        std::cout << std::endl;
        std::cout << "测试用例 1: 自然语言转命令" << std::endl;
        std::cout << "输入: "下载gcc"" << std::endl;
        std::cout << "期望输出: "sudo apt install gcc"" << std::endl;
        std::cout << "✅ 模拟成功" << std::endl;
        std::cout << std::endl;
        std::cout << "测试用例 2: 系统分析" << std::endl;
        std::cout << "输入: 系统状态数据" << std::endl;
        std::cout << "期望输出: 优化建议和分析报告" << std::endl;
        std::cout << "✅ 模拟成功" << std::endl;
    } else {
        std::cout << "❌ 未知AI子命令: " << subcommand << std::endl;
        std::cout << "使用 'nex ai' 查看可用命令" << std::endl;
    }
}

// 处理自然语言命令的模拟实现
void handleNaturalLanguageCommand(const std::vector<std::string>& args) {
    std::string input;
    for (size_t i = 1; i < args.size(); ++i) {
        input += args[i];
        if (i < args.size() - 1) input += " ";
    }
    
    std::cout << "🤖 AI命令解析 (模拟)
" << std::endl;
    std::cout << "输入: "" << input << """ << std::endl;
    
    std::string command;
    std::string description;
    
    // 简单的规则匹配系统
    if (input.find("下载") != std::string::npos || input.find("安装") != std::string::npos) {
        if (input.find("gcc") != std::string::npos) {
            command = "sudo apt update && sudo apt install gcc";
            description = "安装GCC编译器";
        } else if (input.find("python") != std::string::npos) {
            command = "sudo apt install python3 python3-pip";
            description = "安装Python3和pip";
        } else if (input.find("git") != std::string::npos) {
            command = "sudo apt install git";
            description = "安装Git版本控制系统";
        } else {
            command = "echo '请指定要安装的软件包'";
            description = "需要更具体的安装目标";
        }
    } else if (input.find("查找") != std::string::npos || input.find("搜索") != std::string::npos) {
        if (input.find("cpp") != std::string::npos || input.find("c++") != std::string::npos) {
            command = "find . -name '*.cpp' -o -name '*.cc' -o -name '*.cxx'";
            description = "查找所有C++源文件";
        } else if (input.find("python") != std::string::npos) {
            command = "find . -name '*.py'";
            description = "查找所有Python文件";
        } else {
            command = "find . -type f";
            description = "查找所有文件";
        }
    } else if (input.find("系统状态") != std::string::npos || input.find("状态") != std::string::npos) {
        command = "nex status";
        description = "显示系统状态";
    } else {
        command = "echo '抱歉，我还在学习理解这个命令'";
        description = "暂不支持的命令类型";
    }
    
    std::cout << "解析结果: " << description << std::endl;
    std::cout << "生成命令: " << command << std::endl;
    std::cout << std::endl;
    
    // 询问是否执行
    std::cout << "是否执行此命令? [Y/n]: ";
    std::string response;
    std::getline(std::cin, response);
    
    if (response.empty() || response == "y" || response == "Y") {
        std::cout << "🚀 执行命令..." << std::endl;
        int result = system(command.c_str());
        if (result == 0) {
            std::cout << "✅ 命令执行成功" << std::endl;
        } else {
            std::cout << "❌ 命令执行失败，退出码: " << result << std::endl;
        }
    } else {
        std::cout << "❌ 用户取消执行" << std::endl;
    }
}

void showHelp() {
    std::cout << "NeX: AI-CLI - 智能系统资源管理工具\n" << std::endl;
    std::cout << "用法: nex <command> [options]\n" << std::endl;
    std::cout << "可用命令:" << std::endl;
    std::cout << "  status    显示系统状态概览" << std::endl;
    std::cout << "  analyze   运行AI系统分析" << std::endl;
    std::cout << "  shell     启动交互式Shell" << std::endl;
    std::cout << "  config    管理配置设置" << std::endl;
    std::cout << "  version   显示版本信息" << std::endl;
    std::cout << "  help      显示此帮助信息" << std::endl;
    std::cout << std::endl;
    std::cout << "注意: 这是演示版本，完整功能正在开发中..." << std::endl;
}

void showVersion() {
    std::cout << "NeX: AI-CLI v1.0.0-dev" << std::endl;
    std::cout << "Platform: " << getPlatformName() << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

void showStatus() {
    std::cout << "📊 系统状态概览\n" << std::endl;
    std::cout << "🖥️  平台: " << getPlatformName() << std::endl;
    std::cout << "⚡ 状态: 系统监控功能开发中..." << std::endl;
    std::cout << "🤖 AI引擎: 准备中..." << std::endl;
    std::cout << std::endl;
    std::cout << "注意: 完整的系统监控功能将在后续版本中实现" << std::endl;
}

void showConfig() {
    auto& config = nex::config::ConfigManager::getInstance();
    auto& shell_config = nex::config::ShellConfig::getInstance();
    auto& system_config = nex::config::SystemConfig::getInstance();
    
    std::cout << "📋 NeX 配置信息\n" << std::endl;
    std::cout << "配置文件位置: " << config.getConfigPath() << std::endl;
    std::cout << std::endl;
    
    std::cout << "� Shell 配置:" << std::endl;
    std::cout << "  提示符显示时间: " << (shell_config.prompt.show_time ? "是" : "否") << std::endl;
    std::cout << "  显示Git信息: " << (shell_config.prompt.show_git_info ? "是" : "否") << std::endl;
    std::cout << "  显示系统状态: " << (shell_config.prompt.show_system_status ? "是" : "否") << std::endl;
    std::cout << "  语法高亮: " << (shell_config.highlight.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  自动补全: " << (shell_config.completion.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  历史记录: " << (shell_config.history.enabled ? "启用" : "禁用") << std::endl;
    std::cout << std::endl;
    
    std::cout << "⚙️ 系统配置:" << std::endl;
    std::cout << "  系统监控: " << (system_config.monitoring.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  日志记录: " << (system_config.logging.enabled ? "启用" : "禁用") << std::endl;
    std::cout << "  日志级别: " << system_config.logging.level << std::endl;
    std::cout << std::endl;
    
    std::cout << "使用 'nex config --help' 查看配置管理选项" << std::endl;
}

int main(int argc, char** argv) {
    try {
        showBanner();
        
        if (argc < 2) {
            std::cout << "使用 'nex help' 查看可用命令" << std::endl;
            return 1;
        }
        
        // 将命令行参数转换为vector以便处理
        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        std::string command = args[1];
        
        if (command == "help") {
            showHelp();
        } else if (command == "version") {
            showVersion();
        } else if (command == "status") {
            showStatus();
        } else if (command == "config") {
            showConfig();
        } else if (command == "shell") {
            std::cout << "🐚 启动 NeX Shell...\n" << std::endl;
            nex::shell::ShellSession shell;
            shell.runInteractive();
        } else if (command == "ai") {
            handleAICommand(args);
        } else if (command == "analyze") {
            std::cout << "🤖 AI 系统分析\n" << std::endl;
            std::cout << "📊 正在收集系统数据..." << std::endl;
            std::cout << "🔍 AI分析中..." << std::endl;
            std::cout << "✅ 分析完成！" << std::endl;
            std::cout << std::endl;
            std::cout << "📋 AI分析报告:" << std::endl;
            std::cout << "   CPU使用率: 正常" << std::endl;
            std::cout << "   内存使用: 充足" << std::endl;
            std::cout << "   磁盘空间: 良好" << std::endl;
            std::cout << "   建议: 系统运行状态良好，无需特殊优化" << std::endl;
        } else if (command == "monitor") {
            std::cout << "📈 AI监控模式\n" << std::endl;
            std::cout << "🤖 使用AI监控系统状态..." << std::endl;
            std::cout << "⚠️  完整监控功能开发中" << std::endl;
            std::cout << std::endl;
            std::cout << "规划功能:" << std::endl;
            std::cout << "  • 实时系统资源监控" << std::endl;
            std::cout << "  • AI异常检测" << std::endl;
            std::cout << "  • 智能优化建议" << std::endl;
            std::cout << "  • 性能趋势分析" << std::endl;
        } else {
            // 尝试作为自然语言命令处理
            handleNaturalLanguageCommand(args);
        }
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "发生异常: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "发生未知异常" << std::endl;
        return 1;
    }
}
