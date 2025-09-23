#include "nex/ai/ai_core.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <regex>
#include <filesystem>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <future>
#include <sys/wait.h>

namespace nex::ai {

// LlamaEngine Implementation (Placeholder for llama.cpp integration)
class LlamaEngine::Impl {
public:
    bool model_loaded = false;
    ModelConfig config;
    
    // TODO: 集成实际的 llama.cpp API
    // 这里先提供占位符实现
    
    bool loadModel(const ModelConfig& cfg) {
        config = cfg;
        
        // 检查模型文件是否存在
        if (!std::filesystem::exists(cfg.model_path)) {
            std::cerr << "Model file not found: " << cfg.model_path << std::endl;
            return false;
        }
        
        std::cout << "Loading model: " << cfg.model_path << std::endl;
        std::cout << "Model type: " << cfg.model_type << std::endl;
        std::cout << "Quantized: " << (cfg.quantized ? "Yes" : "No") << std::endl;
        
        // 模拟加载时间
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        model_loaded = true;
        return true;
    }
    
    std::string generate(const std::string& prompt, int max_tokens) {
        if (!model_loaded) {
            return "ERROR: Model not loaded";
        }
        
        // TODO: 调用实际的 llama.cpp 推理
        // 现在返回模拟响应
        
        if (prompt.find("下载") != std::string::npos && prompt.find("gcc") != std::string::npos) {
            return "sudo apt update && sudo apt install gcc";
        }
        
        if (prompt.find("安装") != std::string::npos && prompt.find("python") != std::string::npos) {
            return "sudo apt install python3 python3-pip";
        }
        
        if (prompt.find("查找") != std::string::npos && prompt.find(".cpp") != std::string::npos) {
            return "find . -name '*.cpp' -type f";
        }
        
        if (prompt.find("系统状态") != std::string::npos || prompt.find("监控") != std::string::npos) {
            return "系统运行正常。CPU使用率较低，内存充足，建议定期清理临时文件。";
        }
        
        return "我理解您的请求，但需要更多上下文信息来生成准确的命令。";
    }
};

LlamaEngine::LlamaEngine() : pImpl(std::make_unique<Impl>()) {}
LlamaEngine::~LlamaEngine() = default;

bool LlamaEngine::loadModel(const ModelConfig& config) {
    return pImpl->loadModel(config);
}

bool LlamaEngine::unloadModel() {
    pImpl->model_loaded = false;
    return true;
}

bool LlamaEngine::isModelLoaded() const {
    return pImpl->model_loaded;
}

std::string LlamaEngine::generate(const std::string& prompt, int max_tokens) {
    return pImpl->generate(prompt, max_tokens);
}

std::future<std::string> LlamaEngine::generateAsync(const std::string& prompt, int max_tokens) {
    return std::async(std::launch::async, [this, prompt, max_tokens]() {
        return this->generate(prompt, max_tokens);
    });
}

void LlamaEngine::generateStream(const std::string& prompt, 
                                std::function<void(const std::string&)> callback,
                                int max_tokens) {
    // 模拟流式输出
    std::string response = generate(prompt, max_tokens);
    
    // 分块发送
    const size_t chunk_size = 10;
    for (size_t i = 0; i < response.length(); i += chunk_size) {
        std::string chunk = response.substr(i, chunk_size);
        callback(chunk);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

std::string LlamaEngine::getModelInfo() const {
    if (!pImpl->model_loaded) {
        return "No model loaded";
    }
    
    std::stringstream ss;
    ss << "Model: " << pImpl->config.model_type << std::endl;
    ss << "Path: " << pImpl->config.model_path << std::endl;
    ss << "Context Length: " << pImpl->config.context_length << std::endl;
    ss << "Quantized: " << (pImpl->config.quantized ? "Yes" : "No") << std::endl;
    return ss.str();
}

size_t LlamaEngine::getMemoryUsage() const {
    // TODO: 返回实际内存使用量
    return pImpl->model_loaded ? 2048 * 1024 * 1024 : 0; // 2GB 模拟值
}

// CommandParser Implementation
CommandParser::CommandParser(std::shared_ptr<LlamaEngine> engine) 
    : ai_engine_(engine) {
    
    // 初始化预定义命令映射
    CommandMapping gcc_install = {
        "下载gcc", "sudo apt install gcc", "安装GCC编译器",
        {"安装gcc", "gcc下载", "安装编译器"}, true, "package"
    };
    command_mappings_["gcc"] = gcc_install;
    
    CommandMapping python_install = {
        "安装python", "sudo apt install python3 python3-pip", "安装Python解释器",
        {"下载python", "python安装", "安装python3"}, true, "package"
    };
    command_mappings_["python"] = python_install;
    
    CommandMapping find_cpp = {
        "查找cpp文件", "find . -name '*.cpp' -type f", "查找所有C++源文件",
        {"找cpp", "搜索cpp", "列出cpp文件"}, false, "file"
    };
    command_mappings_["find_cpp"] = find_cpp;
}

AIResponse CommandParser::parseCommand(const std::string& natural_input) {
    AIResponse response;
    response.success = false;
    
    if (!ai_engine_ || !ai_engine_->isModelLoaded()) {
        response.error_message = "AI模型未加载";
        return response;
    }
    
    // 首先尝试匹配预定义命令
    auto matches = findMatches(natural_input);
    if (!matches.empty()) {
        response.success = true;
        response.command = matches[0].shell_command;
        response.content = matches[0].description;
        response.confidence = 0.9f;
        return response;
    }
    
    // 使用AI生成命令
    std::string prompt = buildPrompt(natural_input);
    std::string ai_response = ai_engine_->generate(prompt, 256);
    
    response.success = true;
    response.command = ai_response;
    response.content = "AI生成的命令";
    response.confidence = 0.7f;
    
    return response;
}

std::string CommandParser::buildPrompt(const std::string& input, const std::string& context) {
    std::stringstream prompt;
    prompt << "你是一个Linux命令行专家。用户说：\"" << input << "\"" << std::endl;
    prompt << "请生成对应的shell命令，只返回命令本身，不要解释。" << std::endl;
    prompt << "如果需要管理员权限，请加上sudo。" << std::endl;
    prompt << "常见映射：" << std::endl;
    prompt << "- 下载/安装gcc -> sudo apt install gcc" << std::endl;
    prompt << "- 安装python -> sudo apt install python3 python3-pip" << std::endl;
    prompt << "- 查找文件 -> find命令" << std::endl;
    prompt << "- 查看状态 -> ps, top, df等" << std::endl;
    prompt << std::endl << "命令：";
    
    return prompt.str();
}

std::vector<CommandMapping> CommandParser::findMatches(const std::string& input) {
    std::vector<CommandMapping> matches;
    
    for (const auto& [key, mapping] : command_mappings_) {
        // 检查主要关键词
        if (input.find("gcc") != std::string::npos && key == "gcc") {
            matches.push_back(mapping);
        } else if (input.find("python") != std::string::npos && key == "python") {
            matches.push_back(mapping);
        } else if (input.find("cpp") != std::string::npos && key == "find_cpp") {
            matches.push_back(mapping);
        }
        
        // 检查别名
        for (const auto& alias : mapping.aliases) {
            if (input.find(alias) != std::string::npos) {
                matches.push_back(mapping);
                break;
            }
        }
    }
    
    return matches;
}

// SystemMonitorAI Implementation
SystemMonitorAI::SystemMonitorAI(std::shared_ptr<LlamaEngine> engine) 
    : ai_engine_(engine) {}

SystemStatus SystemMonitorAI::collectSystemStatus() {
    SystemStatus status;
    
    status.cpu_usage = getCPUUsage();
    status.memory_usage = getMemoryUsage();
    status.disk_usage = getDiskUsage();
    status.running_processes = getRunningProcesses();
    status.system_info = getSystemInfo();
    status.timestamp = std::to_string(std::time(nullptr));
    
    // 保存到历史记录
    status_history_.push_back(status);
    if (status_history_.size() > 100) {
        status_history_.erase(status_history_.begin());
    }
    
    return status;
}

float SystemMonitorAI::getCPUUsage() {
    // 简单的CPU使用率获取
    std::ifstream stat_file("/proc/stat");
    if (!stat_file.is_open()) return -1.0f;
    
    std::string line;
    std::getline(stat_file, line);
    
    // 解析 /proc/stat 第一行
    std::istringstream iss(line);
    std::string cpu;
    long user, nice, system, idle;
    iss >> cpu >> user >> nice >> system >> idle;
    
    static long prev_idle = 0, prev_total = 0;
    long total = user + nice + system + idle;
    long diff_idle = idle - prev_idle;
    long diff_total = total - prev_total;
    
    float usage = 100.0f * (1.0f - (float)diff_idle / diff_total);
    
    prev_idle = idle;
    prev_total = total;
    
    return usage;
}

float SystemMonitorAI::getMemoryUsage() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) return -1.0f;
    
    long total_mem = info.totalram * info.mem_unit;
    long free_mem = info.freeram * info.mem_unit;
    long used_mem = total_mem - free_mem;
    
    return 100.0f * used_mem / total_mem;
}

float SystemMonitorAI::getDiskUsage() {
    std::filesystem::space_info space = std::filesystem::space(".");
    if (space.capacity == static_cast<std::uintmax_t>(-1)) return -1.0f;
    
    return 100.0f * (space.capacity - space.available) / space.capacity;
}

std::vector<std::string> SystemMonitorAI::getRunningProcesses() {
    std::vector<std::string> processes;
    
    // 获取当前用户的进程
    FILE* pipe = popen("ps -u $(whoami) -o pid,comm,pcpu --no-headers | head -10", "r");
    if (!pipe) return processes;
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        processes.push_back(std::string(buffer));
    }
    pclose(pipe);
    
    return processes;
}

std::map<std::string, std::string> SystemMonitorAI::getSystemInfo() {
    std::map<std::string, std::string> info;
    
    // 获取系统信息
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    info["hostname"] = hostname;
    
    info["user"] = getenv("USER") ? getenv("USER") : "unknown";
    
    // 获取系统版本
    std::ifstream version_file("/proc/version");
    if (version_file.is_open()) {
        std::string version;
        std::getline(version_file, version);
        info["kernel"] = version.substr(0, version.find(' ', 14)); // 简化版本信息
    }
    
    return info;
}

AIResponse SystemMonitorAI::analyzeSystemStatus(const SystemStatus& status) {
    AIResponse response;
    
    if (!ai_engine_ || !ai_engine_->isModelLoaded()) {
        response.success = false;
        response.error_message = "AI模型未加载";
        return response;
    }
    
    std::stringstream prompt;
    prompt << "分析以下系统状态并提供优化建议：" << std::endl;
    prompt << "CPU使用率: " << status.cpu_usage << "%" << std::endl;
    prompt << "内存使用率: " << status.memory_usage << "%" << std::endl;
    prompt << "磁盘使用率: " << status.disk_usage << "%" << std::endl;
    prompt << "运行进程数: " << status.running_processes.size() << std::endl;
    prompt << "请提供简洁的分析和建议。";
    
    std::string ai_analysis = ai_engine_->generate(prompt.str(), 512);
    
    response.success = true;
    response.content = ai_analysis;
    return response;
}

// AIManager Implementation
AIManager& AIManager::getInstance() {
    static AIManager instance;
    return instance;
}

bool AIManager::initialize(const std::string& config_file) {
    if (initialized_) return true;
    
    // 创建AI引擎
    engine_ = std::make_shared<LlamaEngine>();
    
    // 加载配置
    ModelConfig config;
    config.model_path = "/path/to/qwen3-coder-model.gguf";  // TODO: 从配置文件读取
    config.model_type = "qwen3-coder";
    config.quantized = true;
    config.n_threads = std::thread::hardware_concurrency();
    
    // 这里先不实际加载模型，等 llama.cpp 集成完成
    std::cout << "AI系统初始化中..." << std::endl;
    std::cout << "模型类型: " << config.model_type << std::endl;
    std::cout << "量化模式: " << (config.quantized ? "启用" : "禁用") << std::endl;
    
    // 创建各个组件
    command_parser_ = std::make_shared<CommandParser>(engine_);
    system_monitor_ = std::make_shared<SystemMonitorAI>(engine_);
    conversation_manager_ = std::make_shared<ConversationManager>(engine_);
    command_executor_ = std::make_shared<AICommandExecutor>(command_parser_);
    
    initialized_ = true;
    std::cout << "AI系统初始化完成！" << std::endl;
    
    return true;
}

AIResponse AIManager::processUserInput(const std::string& input) {
    if (!initialized_) {
        AIResponse response;
        response.success = false;
        response.error_message = "AI系统未初始化";
        return response;
    }
    
    // 解析用户输入
    return command_parser_->parseCommand(input);
}

AIResponse AIManager::analyzeSystem() {
    if (!initialized_) {
        AIResponse response;
        response.success = false;
        response.error_message = "AI系统未初始化";
        return response;
    }
    
    auto status = system_monitor_->collectSystemStatus();
    return system_monitor_->analyzeSystemStatus(status);
}

std::string AIManager::getStatus() const {
    if (!initialized_) return "未初始化";
    
    std::stringstream ss;
    ss << "AI系统状态: 运行中" << std::endl;
    ss << "模型加载: " << (engine_->isModelLoaded() ? "已加载" : "未加载") << std::endl;
    ss << "内存使用: " << (engine_->getMemoryUsage() / 1024 / 1024) << " MB" << std::endl;
    
    return ss.str();
}

// ConversationManager Implementation
ConversationManager::ConversationManager(std::shared_ptr<LlamaEngine> engine) 
    : ai_engine_(engine) {}

void ConversationManager::startConversation() {
    conversation_history_.clear();
    current_context_ = "用户开始了新的对话会话";
}

AIResponse ConversationManager::processInput(const std::string& input) {
    AIResponse response;
    
    if (!ai_engine_ || !ai_engine_->isModelLoaded()) {
        response.success = false;
        response.error_message = "AI模型未加载";
        return response;
    }
    
    // 构建对话提示词
    std::string prompt = buildConversationPrompt(input);
    
    // 生成响应
    std::string ai_response = ai_engine_->generate(prompt, 512);
    
    // 保存对话历史
    conversation_history_.emplace_back(input, ai_response);
    if (conversation_history_.size() > 10) {
        conversation_history_.erase(conversation_history_.begin());
    }
    
    response.success = true;
    response.content = ai_response;
    return response;
}

std::string ConversationManager::buildConversationPrompt(const std::string& input) {
    std::stringstream prompt;
    prompt << "你是NeX AI助手，专门帮助用户处理Linux命令行任务。" << std::endl;
    
    // 添加对话历史
    if (!conversation_history_.empty()) {
        prompt << "对话历史：" << std::endl;
        for (const auto& [user_input, ai_response] : conversation_history_) {
            prompt << "用户: " << user_input << std::endl;
            prompt << "助手: " << ai_response << std::endl;
        }
    }
    
    prompt << "当前用户说: " << input << std::endl;
    prompt << "请回应用户并提供有用的建议或命令：" << std::endl;
    
    return prompt.str();
}

// AICommandExecutor Implementation
AICommandExecutor::AICommandExecutor(std::shared_ptr<CommandParser> parser) 
    : parser_(parser) {
    initializeDangerousCommands();
}

void AICommandExecutor::initializeDangerousCommands() {
    dangerous_commands_ = {
        "rm -rf /", "rm -rf /*", ":(){ :|:& };:", 
        "mkfs", "dd if=", "shred", "wipefs",
        "chmod -R 777 /", "chown -R"
    };
}

AIResponse AICommandExecutor::executeNaturalCommand(const std::string& input, bool confirm) {
    AIResponse parse_result = parser_->parseCommand(input);
    
    if (!parse_result.success) {
        return parse_result;
    }
    
    std::string command = parse_result.command;
    
    // 安全检查
    if (safe_mode_ && !isSafeCommand(command)) {
        parse_result.success = false;
        parse_result.error_message = "检测到危险命令，拒绝执行: " + command;
        return parse_result;
    }
    
    // 用户确认
    if (confirm && !requestUserConfirmation(command, parse_result.content)) {
        parse_result.success = false;
        parse_result.error_message = "用户取消执行";
        return parse_result;
    }
    
    // 执行命令
    auto [exit_code, output] = executeShellCommand(command);
    
    AIResponse response;
    response.success = (exit_code == 0);
    response.command = command;
    response.content = output;
    response.error_message = (exit_code != 0) ? "命令执行失败，退出码: " + std::to_string(exit_code) : "";
    
    return response;
}

bool AICommandExecutor::isSafeCommand(const std::string& command) {
    for (const auto& dangerous : dangerous_commands_) {
        if (command.find(dangerous) != std::string::npos) {
            return false;
        }
    }
    return true;
}

bool AICommandExecutor::requestUserConfirmation(const std::string& command, const std::string& description) {
    std::cout << std::endl;
    std::cout << "🤖 AI建议执行以下命令:" << std::endl;
    std::cout << "📋 描述: " << description << std::endl;
    std::cout << "💻 命令: " << command << std::endl;
    std::cout << std::endl;
    std::cout << "是否执行? [Y/n]: ";
    
    std::string response;
    std::getline(std::cin, response);
    
    return response.empty() || response == "y" || response == "Y" || response == "yes";
}

std::pair<int, std::string> AICommandExecutor::executeShellCommand(const std::string& command) {
    std::string full_command = command + " 2>&1";
    FILE* pipe = popen(full_command.c_str(), "r");
    
    if (!pipe) {
        return {-1, "无法执行命令"};
    }
    
    std::string output;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }
    
    int exit_code = pclose(pipe);
    return {WEXITSTATUS(exit_code), output};
}

} // namespace nex::ai
