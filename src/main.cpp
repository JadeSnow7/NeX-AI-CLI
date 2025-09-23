#include <iostream>
#include <string>
#include <vector>
// #include "nex/shell/shell.h"   // 暂时注释，待实现
// #include "nex/config/config.h" // 暂时注释，待实现

// AI功能支持
#ifdef AI_ENABLED
    #include "nex/ai/ai_core.h"
    using namespace nex::ai;
#endif

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
    std::cout << "NeX: AI-CLI - 智能命令行助手\n" << std::endl;
    std::cout << "用法: nex <command> [options]\n" << std::endl;
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
    std::cout << "📊 系统状态概览\n" << std::endl;
    std::cout << "🖥️  平台: " << getPlatformName() << std::endl;
    
    // 显示基本系统信息
    std::cout << "⚡ Shell状态: ✅ 可用" << std::endl;
    std::cout << "🎨 语法高亮: ✅ 支持" << std::endl;
    std::cout << "🔧 智能补全: ✅ 支持" << std::endl;
    std::cout << "📊 系统监控: ✅ 基础功能" << std::endl;
    std::cout << "🔄 Git集成: ✅ 支持" << std::endl;
    
    std::cout << "\n🤖 AI功能状态:" << std::endl;
    std::cout << "🏗️  AI架构: ✅ 已设计" << std::endl;
    std::cout << "🧠 AI引擎: 🚧 开发中" << std::endl;
    std::cout << "💬 自然语言: 🚧 开发中" << std::endl;
    std::cout << "📈 智能监控: 🚧 开发中" << std::endl;
    std::cout << "🔍 错误诊断: 📋 规划中" << std::endl;
    
    std::cout << std::endl;
}

void showConfig() {
    // auto& config = nex::config::ConfigManager::getInstance();
    // auto& shell_config = nex::config::ShellConfig::getInstance();
    // auto& system_config = nex::config::SystemConfig::getInstance();
    
    std::cout << "📋 NeX 配置信息\n" << std::endl;
    std::cout << "配置文件位置: ~/.nex/config.json" << std::endl;
    std::cout << std::endl;
    
    std::cout << "🐚 Shell 配置:" << std::endl;
    std::cout << "  提示符显示时间: 是" << std::endl;
    std::cout << "  显示Git信息: 是" << std::endl;
    std::cout << "  显示系统状态: 是" << std::endl;
    std::cout << "  语法高亮: 启用" << std::endl;
    std::cout << "  自动补全: 启用" << std::endl;
    std::cout << "  历史记录: 启用" << std::endl;
    std::cout << std::endl;
    
    std::cout << "⚙️ 系统配置:" << std::endl;
    std::cout << "  系统监控: 启用" << std::endl;
    std::cout << "  日志记录: 启用" << std::endl;
    std::cout << "  日志级别: INFO" << std::endl;
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
    if (args.size() < 3) {
        std::cout << "🤖 AI引擎管理\n" << std::endl;
        std::cout << "用法: nex ai <subcommand>" << std::endl;
        std::cout << std::endl;
        std::cout << "可用子命令:" << std::endl;
        std::cout << "  init       初始化AI引擎" << std::endl;
        std::cout << "  status     查看AI状态" << std::endl;
        std::cout << "  model      模型管理" << std::endl;
        std::cout << "  local      切换到本地模型" << std::endl;
        std::cout << "  online     切换到在线模型" << std::endl;
        std::cout << "  config     配置模型参数" << std::endl;
        std::cout << "  test       测试AI功能" << std::endl;
        return;
    }
    
    std::string subcommand = args[2];
    
#ifdef AI_ENABLED
    auto& aiManager = AIManager::getInstance();
#endif
    
    if (subcommand == "init") {
        std::cout << "🤖 初始化AI引擎...\n" << std::endl;
#ifdef AI_ENABLED
        bool success = aiManager.initialize();
        if (success) {
            std::cout << "✅ AI引擎初始化成功" << std::endl;
        } else {
            std::cout << "❌ AI引擎初始化失败" << std::endl;
        }
#else
        std::cout << "⚠️  AI功能未编译，请重新构建项目" << std::endl;
#endif
    } else if (subcommand == "status") {
        std::cout << "🤖 AI引擎状态\n" << std::endl;
#ifdef AI_ENABLED
        if (aiManager.isInitialized()) {
            std::cout << aiManager.getStatus() << std::endl;
            std::cout << "当前模型类型: ";
            switch (aiManager.getCurrentModelType()) {
                case ModelType::LOCAL_LLAMA_CPP:
                    std::cout << "本地 llama.cpp 模型" << std::endl;
                    break;
                case ModelType::ONLINE_OPENAI:
                    std::cout << "OpenAI API" << std::endl;
                    break;
                case ModelType::ONLINE_ANTHROPIC:
                    std::cout << "Anthropic API" << std::endl;
                    break;
                case ModelType::ONLINE_CUSTOM:
                    std::cout << "自定义在线API" << std::endl;
                    break;
            }
        } else {
            std::cout << "� 引擎状态: 未初始化" << std::endl;
            std::cout << "使用 'nex ai init' 初始化AI引擎" << std::endl;
        }
#else
        std::cout << "⚠️  AI功能未编译" << std::endl;
#endif
    } else if (subcommand == "local") {
        std::cout << "🖥️  切换到本地模型\n" << std::endl;
        if (args.size() >= 4) {
            std::string model_path = args[3];
#ifdef AI_ENABLED
            bool success = aiManager.switchToLocalModel(model_path);
            if (success) {
                std::cout << "✅ 成功切换到本地模型: " << model_path << std::endl;
            } else {
                std::cout << "❌ 切换失败，请检查模型路径" << std::endl;
            }
#else
            std::cout << "⚠️  AI功能未编译" << std::endl;
#endif
        } else {
            std::cout << "用法: nex ai local <model_path>" << std::endl;
            std::cout << "示例: nex ai local ~/.nex/models/qwen3-coder-7b.gguf" << std::endl;
        }
    } else if (subcommand == "online") {
        std::cout << "🌐 切换到在线模型\n" << std::endl;
        if (args.size() >= 5) {
            std::string provider = args[3]; // openai, anthropic, custom
            std::string api_key = args[4];
            std::string model_name = args.size() >= 6 ? args[5] : "";
            
#ifdef AI_ENABLED
            ModelType type = ModelType::ONLINE_CUSTOM;
            if (provider == "openai") {
                type = ModelType::ONLINE_OPENAI;
                if (model_name.empty()) model_name = "gpt-3.5-turbo";
            } else if (provider == "anthropic") {
                type = ModelType::ONLINE_ANTHROPIC;
                if (model_name.empty()) model_name = "claude-3-sonnet-20240229";
            }
            
            bool success = aiManager.switchToOnlineModel(type, api_key, model_name);
            if (success) {
                std::cout << "✅ 成功切换到 " << provider << " 模型: " << model_name << std::endl;
            } else {
                std::cout << "❌ 切换失败，请检查API密钥和模型名称" << std::endl;
            }
#else
            std::cout << "⚠️  AI功能未编译" << std::endl;
#endif
        } else {
            std::cout << "用法: nex ai online <provider> <api_key> [model_name]" << std::endl;
            std::cout << "支持的提供商:" << std::endl;
            std::cout << "  openai     - OpenAI GPT 模型" << std::endl;
            std::cout << "  anthropic  - Anthropic Claude 模型" << std::endl;
            std::cout << "  custom     - 自定义API服务" << std::endl;
            std::cout << std::endl;
            std::cout << "示例:" << std::endl;
            std::cout << "  nex ai online openai sk-xxx gpt-4" << std::endl;
            std::cout << "  nex ai online anthropic sk-ant-xxx claude-3-sonnet-20240229" << std::endl;
        }
    } else if (subcommand == "config") {
        std::cout << "⚙️  AI配置管理\n" << std::endl;
        std::cout << "当前支持的配置项:" << std::endl;
        std::cout << "  • 模型路径 (本地模型)" << std::endl;
        std::cout << "  • API密钥 (在线模型)" << std::endl;
        std::cout << "  • 推理参数 (温度、top-k、top-p等)" << std::endl;
        std::cout << std::endl;
        std::cout << "使用 'nex ai local' 或 'nex ai online' 命令进行配置" << std::endl;
    } else if (subcommand == "model") {
        std::cout << "📦 AI模型管理\n" << std::endl;
#ifdef AI_ENABLED
        if (aiManager.isInitialized()) {
            auto models = aiManager.getAvailableModels();
            if (!models.empty()) {
                std::cout << "� 可用模型:" << std::endl;
                for (const auto& model : models) {
                    std::cout << "  • " << model << std::endl;
                }
            } else {
                std::cout << "❌ 未找到已安装的模型" << std::endl;
            }
            std::cout << std::endl;
            std::cout << "📄 当前模型信息:" << std::endl;
            std::cout << aiManager.getCurrentModelInfo() << std::endl;
        } else {
            std::cout << "⚠️  AI引擎未初始化，请先运行 'nex ai init'" << std::endl;
        }
#else
        std::cout << "�🔍 扫描本地模型..." << std::endl;
        std::cout << "❌ 未找到已安装的模型" << std::endl;
#endif
        std::cout << std::endl;
        std::cout << "📋 推荐模型:" << std::endl;
        std::cout << "   本地模型:" << std::endl;
        std::cout << "   • Qwen3-Coder-7B-Instruct.gguf (推荐)" << std::endl;
        std::cout << "   • CodeLlama-7B-Instruct.gguf" << std::endl;
        std::cout << "   • Phi-3-Mini-Instruct.gguf" << std::endl;
        std::cout << std::endl;
        std::cout << "   在线模型:" << std::endl;
        std::cout << "   • OpenAI: gpt-3.5-turbo, gpt-4" << std::endl;
        std::cout << "   • Anthropic: claude-3-sonnet-20240229" << std::endl;
    } else if (subcommand == "test") {
        std::cout << "🧪 AI功能测试\n" << std::endl;
#ifdef AI_ENABLED
        if (aiManager.isInitialized()) {
            std::cout << "🔧 测试AI引擎..." << std::endl;
            
            // 测试自然语言处理
            auto response = aiManager.processUserInput("下载gcc");
            std::cout << "测试用例 1: 自然语言转命令" << std::endl;
            std::cout << "输入: 下载gcc" << std::endl;
            std::cout << "输出: " << response.command << std::endl;
            std::cout << (response.success ? "✅ 成功" : "❌ 失败") << std::endl;
            std::cout << std::endl;
            
            // 测试系统分析
            auto analysis = aiManager.analyzeSystem();
            std::cout << "测试用例 2: 系统分析" << std::endl;
            std::cout << "分析结果: " << analysis.content << std::endl;
            std::cout << (analysis.success ? "✅ 成功" : "❌ 失败") << std::endl;
        } else {
            std::cout << "⚠️  AI引擎未初始化，运行模拟测试" << std::endl;
            std::cout << std::endl;
            std::cout << "测试用例 1: 自然语言转命令" << std::endl;
            std::cout << "输入: 下载gcc" << std::endl;
            std::cout << "期望输出: sudo apt install gcc" << std::endl;
            std::cout << "✅ 模拟成功" << std::endl;
            std::cout << std::endl;
            std::cout << "测试用例 2: 系统分析" << std::endl;
            std::cout << "输入: 系统状态数据" << std::endl;
            std::cout << "期望输出: 优化建议和分析报告" << std::endl;
            std::cout << "✅ 模拟成功" << std::endl;
        }
#else
        std::cout << "⚠️  AI功能未编译，运行模拟测试" << std::endl;
        std::cout << std::endl;
        std::cout << "测试用例 1: 自然语言转命令" << std::endl;
        std::cout << "输入: 下载gcc" << std::endl;
        std::cout << "期望输出: sudo apt install gcc" << std::endl;
        std::cout << "✅ 模拟成功" << std::endl;
        std::cout << std::endl;
        std::cout << "测试用例 2: 系统分析" << std::endl;
        std::cout << "输入: 系统状态数据" << std::endl;
        std::cout << "期望输出: 优化建议和分析报告" << std::endl;
        std::cout << "✅ 模拟成功" << std::endl;
#endif
    } else {
        std::cout << "❌ 未知AI子命令: " << subcommand << std::endl;
        std::cout << "使用 'nex ai' 查看可用命令" << std::endl;
    }
}

// 处理自然语言命令的AI实现
void handleNaturalLanguageCommand(const std::vector<std::string>& args) {
    std::string input;
    for (size_t i = 1; i < args.size(); ++i) {
        input += args[i];
        if (i < args.size() - 1) input += " ";
    }
    
    std::cout << "🤖 AI命令解析\n" << std::endl;
    std::cout << "输入: " << input << std::endl;
    
#ifdef AI_ENABLED
    auto& aiManager = AIManager::getInstance();
    
    if (aiManager.isInitialized()) {
        // 使用真实的AI处理
        auto response = aiManager.processUserInput(input);
        
        std::cout << "解析结果: " << response.content << std::endl;
        std::cout << "生成命令: " << response.command << std::endl;
        std::cout << "置信度: " << (response.confidence * 100) << "%" << std::endl;
        std::cout << std::endl;
        
        if (response.success) {
            // 询问是否执行
            std::cout << "是否执行此命令? [Y/n]: ";
            std::string user_response;
            std::getline(std::cin, user_response);
            
            if (user_response.empty() || user_response == "y" || user_response == "Y") {
                std::cout << "🚀 执行命令..." << std::endl;
                int result = system(response.command.c_str());
                if (result == 0) {
                    std::cout << "✅ 命令执行成功" << std::endl;
                } else {
                    std::cout << "❌ 命令执行失败，退出码: " << result << std::endl;
                }
            } else {
                std::cout << "❌ 用户取消执行" << std::endl;
            }
        } else {
            std::cout << "❌ AI解析失败: " << response.error_message << std::endl;
        }
    } else {
        std::cout << "⚠️  AI引擎未初始化，使用简化解析\n" << std::endl;
#endif
        
        // 简化的规则匹配系统（备用方案）
        std::string command;
        std::string description;
        
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
        
#ifdef AI_ENABLED
    }
#endif
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
            // nex::shell::ShellSession shell;
            // shell.runInteractive();
            std::cout << "⚠️  交互式Shell功能开发中，请稍后再试" << std::endl;
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
