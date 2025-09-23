#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <future>

namespace nex::ai {

// 模型类型枚举
enum class ModelType {
    LOCAL_LLAMA_CPP,  // 本地 llama.cpp 模型
    ONLINE_OPENAI,    // OpenAI API
    ONLINE_ANTHROPIC, // Anthropic API
    ONLINE_CUSTOM     // 自定义在线API
};

// 在线API配置
struct OnlineAPIConfig {
    std::string api_url;
    std::string api_key;
    std::string model_name;
    int timeout_seconds = 30;
    int max_retries = 3;
    bool use_streaming = false;
    std::map<std::string, std::string> headers;  // 自定义请求头
};

// AI模型配置结构
struct ModelConfig {
    ModelType type = ModelType::LOCAL_LLAMA_CPP;
    
    // 本地模型配置
    std::string model_path;
    std::string model_type;  // "qwen3-coder", "codellama", etc.
    int context_length = 4096;
    int n_threads = 4;
    bool use_gpu = false;
    bool quantized = true;  // 使用量化模型
    
    // 通用推理参数
    float temperature = 0.1;
    int top_k = 40;
    float top_p = 0.9f;
    int max_tokens = 512;
    
    // 在线API配置
    OnlineAPIConfig online_config;
    
    // 验证配置是否有效
    bool isValid() const {
        if (type == ModelType::LOCAL_LLAMA_CPP) {
            return !model_path.empty() && !model_type.empty();
        } else {
            return !online_config.api_key.empty() && !online_config.model_name.empty();
        }
    }
};

// 系统状态结构
struct SystemStatus {
    float cpu_usage;
    float memory_usage;
    float disk_usage;
    std::vector<std::string> running_processes;
    std::map<std::string, std::string> system_info;
    std::string timestamp;
};

// AI响应结构
struct AIResponse {
    bool success;
    std::string content;
    std::string command;  // 生成的shell命令
    std::vector<std::string> suggestions;
    std::string error_message;
    float confidence = 0.0f;
};

// 命令映射结构
struct CommandMapping {
    std::string natural_language;
    std::string shell_command;
    std::string description;
    std::vector<std::string> aliases;
    bool requires_sudo = false;
    std::string category;  // "package", "file", "system", etc.
};

// 在线模型接口基类
class OnlineModelInterface {
public:
    virtual ~OnlineModelInterface() = default;
    
    // 初始化API连接
    virtual bool initialize(const OnlineAPIConfig& config) = 0;
    
    // 生成响应
    virtual std::string generate(const std::string& prompt, int max_tokens = 512) = 0;
    
    // 异步生成
    virtual std::future<std::string> generateAsync(const std::string& prompt, int max_tokens = 512) = 0;
    
    // 流式生成
    virtual void generateStream(const std::string& prompt, 
                              std::function<void(const std::string&)> callback,
                              int max_tokens = 512) = 0;
    
    // 检查连接状态
    virtual bool isConnected() const = 0;
    
    // 获取模型信息
    virtual std::string getModelInfo() const = 0;
    
    // 获取使用情况统计
    virtual std::map<std::string, int> getUsageStats() const = 0;
};

// OpenAI API 实现
class OpenAIModel : public OnlineModelInterface {
public:
    OpenAIModel();
    ~OpenAIModel();
    
    bool initialize(const OnlineAPIConfig& config) override;
    std::string generate(const std::string& prompt, int max_tokens = 512) override;
    std::future<std::string> generateAsync(const std::string& prompt, int max_tokens = 512) override;
    void generateStream(const std::string& prompt, 
                       std::function<void(const std::string&)> callback,
                       int max_tokens = 512) override;
    bool isConnected() const override;
    std::string getModelInfo() const override;
    std::map<std::string, int> getUsageStats() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 通用HTTP API 实现（用于其他在线服务）
class GenericOnlineModel : public OnlineModelInterface {
public:
    GenericOnlineModel();
    ~GenericOnlineModel();
    
    bool initialize(const OnlineAPIConfig& config) override;
    std::string generate(const std::string& prompt, int max_tokens = 512) override;
    std::future<std::string> generateAsync(const std::string& prompt, int max_tokens = 512) override;
    void generateStream(const std::string& prompt, 
                       std::function<void(const std::string&)> callback,
                       int max_tokens = 512) override;
    bool isConnected() const override;
    std::string getModelInfo() const override;
    std::map<std::string, int> getUsageStats() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 统一AI引擎接口（原AIEngine重命名为AIEngine）
class AIEngine {
public:
    AIEngine();
    ~AIEngine();

    // 模型管理
    bool loadModel(const ModelConfig& config);
    bool unloadModel();
    bool isModelLoaded() const;
    ModelType getCurrentModelType() const;
    
    // 推理接口
    std::string generate(const std::string& prompt, int max_tokens = 512);
    std::future<std::string> generateAsync(const std::string& prompt, int max_tokens = 512);
    
    // 流式生成
    void generateStream(const std::string& prompt, 
                       std::function<void(const std::string&)> callback,
                       int max_tokens = 512);
    
    // 模型信息
    std::string getModelInfo() const;
    size_t getMemoryUsage() const;
    
    // 在线模型特有功能
    std::map<std::string, int> getUsageStats() const;  // API使用统计
    bool isConnected() const;  // 网络连接状态
    
    // 模型切换
    bool switchToLocalModel(const ModelConfig& config);
    bool switchToOnlineModel(const ModelConfig& config);
    
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 为了兼容性，保留AIEngine别名
using AIEngine = AIEngine;

// 自然语言命令解析器
class CommandParser {
public:
    CommandParser(std::shared_ptr<AIEngine> engine);
    
    // 解析自然语言到shell命令
    AIResponse parseCommand(const std::string& natural_input);
    
    // 加载预定义命令映射
    void loadCommandMappings(const std::string& mappings_file);
    
    // 添加自定义命令映射
    void addCommandMapping(const CommandMapping& mapping);
    
    // 获取建议
    std::vector<std::string> getSuggestions(const std::string& partial_input);
    
    // 解释shell命令
    std::string explainCommand(const std::string& shell_command);
    
private:
    std::shared_ptr<AIEngine> ai_engine_;
    std::map<std::string, CommandMapping> command_mappings_;
    
    // 构建提示词
    std::string buildPrompt(const std::string& input, const std::string& context = "");
    
    // 解析AI响应
    CommandMapping parseAIResponse(const std::string& response);
    
    // 匹配预定义命令
    std::vector<CommandMapping> findMatches(const std::string& input);
};

// 系统监控AI分析器
class SystemMonitorAI {
public:
    SystemMonitorAI(std::shared_ptr<AIEngine> engine);
    
    // 收集系统状态
    SystemStatus collectSystemStatus();
    
    // AI分析系统状态
    AIResponse analyzeSystemStatus(const SystemStatus& status);
    
    // 生成监控报告
    std::string generateMonitoringReport(const SystemStatus& status);
    
    // 性能优化建议
    std::vector<std::string> getOptimizationSuggestions(const SystemStatus& status);
    
    // 异常检测
    std::vector<std::string> detectAnomalies(const SystemStatus& status);
    
    // 资源预测
    std::map<std::string, float> predictResourceUsage(const std::vector<SystemStatus>& history);
    
private:
    std::shared_ptr<AIEngine> ai_engine_;
    std::vector<SystemStatus> status_history_;
    
    // 系统状态收集器
    float getCPUUsage();
    float getMemoryUsage();
    float getDiskUsage();
    std::vector<std::string> getRunningProcesses();
    std::map<std::string, std::string> getSystemInfo();
};

// AI对话管理器
class ConversationManager {
public:
    ConversationManager(std::shared_ptr<AIEngine> engine);
    
    // 开始新对话
    void startConversation();
    
    // 处理用户输入
    AIResponse processInput(const std::string& input);
    
    // 获取对话历史
    std::vector<std::pair<std::string, std::string>> getConversationHistory();
    
    // 清除对话历史
    void clearConversation();
    
    // 设置上下文
    void setContext(const std::string& context);
    
private:
    std::shared_ptr<AIEngine> ai_engine_;
    std::vector<std::pair<std::string, std::string>> conversation_history_;
    std::string current_context_;
    
    // 构建对话提示词
    std::string buildConversationPrompt(const std::string& input);
};

// AI配置管理器
class AIConfigManager {
public:
    static AIConfigManager& getInstance();
    
    // 加载AI配置
    bool loadConfig(const std::string& config_file);
    
    // 保存AI配置
    bool saveConfig(const std::string& config_file);
    
    // 获取模型配置
    ModelConfig getModelConfig() const;
    
    // 设置模型配置
    void setModelConfig(const ModelConfig& config);
    
    // 模型管理
    std::vector<ModelConfig> getAvailableModels() const;
    bool addModelConfig(const std::string& name, const ModelConfig& config);
    bool removeModelConfig(const std::string& name);
    ModelConfig getModelConfigByName(const std::string& name) const;
    
    // 在线模型配置
    void setOpenAIConfig(const std::string& api_key, const std::string& model = "gpt-3.5-turbo");
    void setAnthropicConfig(const std::string& api_key, const std::string& model = "claude-3-sonnet-20240229");
    void setCustomAPIConfig(const std::string& api_url, const std::string& api_key, const std::string& model);
    
    // 本地模型配置
    void setLocalModelPath(const std::string& model_path, const std::string& model_type = "qwen3-coder");
    
    // 获取命令映射文件路径
    std::string getCommandMappingsPath() const;
    
    // 获取AI提示词模板
    std::string getPromptTemplate(const std::string& template_name) const;
    
    // 验证配置
    bool validateConfig(const ModelConfig& config) const;
    
private:
    AIConfigManager() = default;
    ModelConfig current_config_;
    std::map<std::string, ModelConfig> named_configs_;
    std::map<std::string, std::string> prompt_templates_;
    std::string command_mappings_path_;
    
    // 初始化默认配置
    void initializeDefaults();
};

// AI命令执行器
class AICommandExecutor {
public:
    AICommandExecutor(std::shared_ptr<CommandParser> parser);
    
    // 执行自然语言命令
    AIResponse executeNaturalCommand(const std::string& input, bool confirm = true);
    
    // 安全检查
    bool isSafeCommand(const std::string& command);
    
    // 用户确认
    bool requestUserConfirmation(const std::string& command, const std::string& description);
    
    // 执行shell命令
    std::pair<int, std::string> executeShellCommand(const std::string& command);
    
    // 设置安全模式
    void setSafeMode(bool enabled);
    
private:
    std::shared_ptr<CommandParser> parser_;
    bool safe_mode_ = true;
    std::vector<std::string> dangerous_commands_;
    
    // 初始化危险命令列表
    void initializeDangerousCommands();
};

// 主AI管理类
class AIManager {
public:
    static AIManager& getInstance();
    
    // 初始化AI系统
    bool initialize(const std::string& config_file = "");
    
    // 关闭AI系统
    void shutdown();
    
    // 获取各个组件
    std::shared_ptr<AIEngine> getEngine();
    std::shared_ptr<CommandParser> getCommandParser();
    std::shared_ptr<SystemMonitorAI> getSystemMonitor();
    std::shared_ptr<ConversationManager> getConversationManager();
    std::shared_ptr<AICommandExecutor> getCommandExecutor();
    
    // 模型管理
    bool switchToLocalModel(const std::string& model_path, const std::string& model_type = "qwen3-coder");
    bool switchToOnlineModel(ModelType type, const std::string& api_key, const std::string& model_name);
    std::vector<std::string> getAvailableModels() const;
    std::string getCurrentModelInfo() const;
    
    // 处理用户输入 (主要接口)
    AIResponse processUserInput(const std::string& input);
    
    // 系统状态分析
    AIResponse analyzeSystem();
    
    // 获取AI状态
    bool isInitialized() const;
    std::string getStatus() const;
    ModelType getCurrentModelType() const;
    
private:
    AIManager() = default;
    
    bool initialized_ = false;
    std::shared_ptr<AIEngine> engine_;
    std::shared_ptr<CommandParser> command_parser_;
    std::shared_ptr<SystemMonitorAI> system_monitor_;
    std::shared_ptr<ConversationManager> conversation_manager_;
    std::shared_ptr<AICommandExecutor> command_executor_;
};

} // namespace nex::ai
