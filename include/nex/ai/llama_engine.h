#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <future>

namespace nex {
namespace ai {

struct LlamaConfig {
    std::string model_path;
    int n_threads = -1;        // CPU线程数，-1为自动
    int n_ctx = 4096;          // 上下文长度
    int n_batch = 512;         // 批处理大小
    bool use_mmap = true;      // 使用内存映射
    bool use_mlock = false;    // 锁定内存
    float temperature = 0.7f;  // 生成温度
    int top_k = 40;           // top-k采样
    float top_p = 0.9f;       // top-p采样
    int max_tokens = 512;     // 最大生成长度
};

class LlamaEngine {
public:
    LlamaEngine();
    ~LlamaEngine();
    
    // 禁用拷贝
    LlamaEngine(const LlamaEngine&) = delete;
    LlamaEngine& operator=(const LlamaEngine&) = delete;
    
    // 模型加载和管理
    bool loadModel(const LlamaConfig& config);
    bool unloadModel();
    bool isModelLoaded() const;
    
    // 同步推理
    std::string generate(const std::string& prompt, int max_tokens = -1);
    
    // 异步推理
    std::future<std::string> generateAsync(const std::string& prompt, int max_tokens = -1);
    
    // 流式推理
    void generateStream(const std::string& prompt, 
                       std::function<void(const std::string&)> callback,
                       int max_tokens = -1);
    
    // 聊天接口
    std::string chat(const std::string& user_input, 
                    const std::string& system_prompt = "");
    
    // 模型信息
    std::string getModelInfo() const;
    size_t getModelSize() const;
    int getContextLength() const;
    
    // 配置管理
    void updateConfig(const LlamaConfig& config);
    LlamaConfig getConfig() const;
    
    // 性能统计
    struct Stats {
        double tokens_per_second = 0.0;
        size_t total_tokens = 0;
        size_t prompt_tokens = 0;
        size_t completion_tokens = 0;
        double load_time_ms = 0.0;
        double generation_time_ms = 0.0;
    };
    
    Stats getStats() const;
    void resetStats();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

// 便捷函数
std::vector<std::string> getAvailableModels(const std::string& models_dir = "~/.nex/models");
bool validateModelFile(const std::string& model_path);
LlamaConfig getDefaultConfig();

} // namespace ai
} // namespace nex
