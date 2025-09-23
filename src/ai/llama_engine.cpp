#include "nex/ai/llama_engine.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <regex>

// 这里我们先用模拟实现，等llama.cpp编译完成后再集成真实的API
// #include "llama.h"

namespace nex {
namespace ai {

class LlamaEngine::Impl {
public:
    LlamaConfig config;
    bool model_loaded = false;
    Stats stats;
    std::string model_name;
    
    // 模拟的llama.cpp状态
    // llama_model* model = nullptr;
    // llama_context* ctx = nullptr;
    
    bool loadModelImpl(const LlamaConfig& cfg) {
        config = cfg;
        
        // 验证模型文件
        if (!std::filesystem::exists(config.model_path)) {
            std::cerr << "Model file not found: " << config.model_path << std::endl;
            return false;
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // TODO: 真实的llama.cpp加载逻辑
        // llama_backend_init();
        // llama_model_params model_params = llama_model_default_params();
        // model = llama_load_model_from_file(config.model_path.c_str(), model_params);
        
        // 模拟加载时间
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        auto end = std::chrono::high_resolution_clock::now();
        stats.load_time_ms = std::chrono::duration<double, std::milli>(end - start).count();
        
        model_loaded = true;
        model_name = std::filesystem::path(config.model_path).filename();
        
        std::cout << "🤖 Model loaded: " << model_name << std::endl;
        std::cout << "   Load time: " << stats.load_time_ms << "ms" << std::endl;
        
        return true;
    }
    
    void unloadModelImpl() {
        if (!model_loaded) return;
        
        // TODO: 真实的llama.cpp卸载逻辑
        // if (ctx) {
        //     llama_free(ctx);
        //     ctx = nullptr;
        // }
        // if (model) {
        //     llama_free_model(model);
        //     model = nullptr;
        // }
        // llama_backend_free();
        
        model_loaded = false;
        model_name.clear();
        resetStatsImpl();
        
        std::cout << "🤖 Model unloaded" << std::endl;
    }
    
    std::string generateImpl(const std::string& prompt, int max_tokens) {
        if (!model_loaded) {
            return "Error: No model loaded";
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // TODO: 真实的llama.cpp推理逻辑
        // 这里先用模拟实现
        std::string response = simulateGeneration(prompt, max_tokens);
        
        auto end = std::chrono::high_resolution_clock::now();
        double gen_time = std::chrono::duration<double, std::milli>(end - start).count();
        
        // 更新统计信息
        stats.generation_time_ms += gen_time;
        stats.prompt_tokens += countTokens(prompt);
        stats.completion_tokens += countTokens(response);
        stats.total_tokens = stats.prompt_tokens + stats.completion_tokens;
        
        if (gen_time > 0) {
            stats.tokens_per_second = (stats.completion_tokens / gen_time) * 1000.0;
        }
        
        return response;
    }
    
    void resetStatsImpl() {
        stats = Stats{};
    }
    
private:
    std::string simulateGeneration(const std::string& prompt, int max_tokens) {
        // 模拟AI响应生成
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        
        // 简单的规则响应系统
        if (prompt.find("command") != std::string::npos || 
            prompt.find("命令") != std::string::npos) {
            
            if (prompt.find("install") != std::string::npos || 
                prompt.find("安装") != std::string::npos) {
                if (prompt.find("gcc") != std::string::npos) {
                    return "sudo apt update && sudo apt install gcc build-essential";
                } else if (prompt.find("python") != std::string::npos) {
                    return "sudo apt install python3 python3-pip";
                } else if (prompt.find("nodejs") != std::string::npos) {
                    return "sudo apt install nodejs npm";
                }
            }
            
            if (prompt.find("find") != std::string::npos || 
                prompt.find("查找") != std::string::npos) {
                if (prompt.find("cpp") != std::string::npos || 
                    prompt.find("c++") != std::string::npos) {
                    return "find . -name '*.cpp' -o -name '*.cc' -o -name '*.cxx'";
                } else if (prompt.find("large") != std::string::npos || 
                          prompt.find("大") != std::string::npos) {
                    return "find . -size +100M -type f";
                }
            }
            
            if (prompt.find("status") != std::string::npos || 
                prompt.find("状态") != std::string::npos) {
                return "top -bn1 | head -20";
            }
        }
        
        // 默认聊天响应
        return "您好！我是基于 " + model_name + " 的AI助手。当前使用的是模拟响应，"
               "真实的llama.cpp集成正在进行中。您的输入是：" + prompt;
    }
    
    size_t countTokens(const std::string& text) {
        // 简单的token计数估算（实际应该用tokenizer）
        return text.length() / 4; // 粗略估算
    }
};

// LlamaEngine 实现
LlamaEngine::LlamaEngine() : pImpl(std::make_unique<Impl>()) {}
LlamaEngine::~LlamaEngine() = default;

bool LlamaEngine::loadModel(const LlamaConfig& config) {
    return pImpl->loadModelImpl(config);
}

bool LlamaEngine::unloadModel() {
    pImpl->unloadModelImpl();
    return true;
}

bool LlamaEngine::isModelLoaded() const {
    return pImpl->model_loaded;
}

std::string LlamaEngine::generate(const std::string& prompt, int max_tokens) {
    if (max_tokens <= 0) max_tokens = pImpl->config.max_tokens;
    return pImpl->generateImpl(prompt, max_tokens);
}

std::future<std::string> LlamaEngine::generateAsync(const std::string& prompt, int max_tokens) {
    return std::async(std::launch::async, [this, prompt, max_tokens]() {
        return generate(prompt, max_tokens);
    });
}

void LlamaEngine::generateStream(const std::string& prompt, 
                               std::function<void(const std::string&)> callback,
                               int max_tokens) {
    // 模拟流式输出
    std::string full_response = generate(prompt, max_tokens);
    
    // 分段发送
    const size_t chunk_size = 10;
    for (size_t i = 0; i < full_response.length(); i += chunk_size) {
        std::string chunk = full_response.substr(i, chunk_size);
        callback(chunk);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

std::string LlamaEngine::chat(const std::string& user_input, 
                             const std::string& system_prompt) {
    std::string full_prompt;
    if (!system_prompt.empty()) {
        full_prompt = "System: " + system_prompt + "\n";
    }
    full_prompt += "User: " + user_input + "\nAssistant: ";
    
    return generate(full_prompt);
}

std::string LlamaEngine::getModelInfo() const {
    if (!pImpl->model_loaded) {
        return "No model loaded";
    }
    
    return "Model: " + pImpl->model_name + 
           "\nContext Length: " + std::to_string(pImpl->config.n_ctx) +
           "\nThreads: " + std::to_string(pImpl->config.n_threads) +
           "\nTemperature: " + std::to_string(pImpl->config.temperature);
}

size_t LlamaEngine::getModelSize() const {
    if (!pImpl->model_loaded) return 0;
    
    try {
        return std::filesystem::file_size(pImpl->config.model_path);
    } catch (...) {
        return 0;
    }
}

int LlamaEngine::getContextLength() const {
    return pImpl->config.n_ctx;
}

void LlamaEngine::updateConfig(const LlamaConfig& config) {
    pImpl->config = config;
}

LlamaConfig LlamaEngine::getConfig() const {
    return pImpl->config;
}

LlamaEngine::Stats LlamaEngine::getStats() const {
    return pImpl->stats;
}

void LlamaEngine::resetStats() {
    pImpl->resetStatsImpl();
}

// 便捷函数实现
std::vector<std::string> getAvailableModels(const std::string& models_dir) {
    std::vector<std::string> models;
    
    std::string expanded_dir = models_dir;
    if (expanded_dir.front() == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            expanded_dir = std::string(home) + expanded_dir.substr(1);
        }
    }
    
    try {
        if (std::filesystem::exists(expanded_dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(expanded_dir)) {
                if (entry.is_regular_file()) {
                    std::string ext = entry.path().extension();
                    if (ext == ".gguf" || ext == ".bin") {
                        models.push_back(entry.path().string());
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error scanning models directory: " << e.what() << std::endl;
    }
    
    return models;
}

bool validateModelFile(const std::string& model_path) {
    if (!std::filesystem::exists(model_path)) {
        return false;
    }
    
    // 检查文件大小
    auto size = std::filesystem::file_size(model_path);
    if (size < 1024 * 1024) { // 至少1MB
        return false;
    }
    
    // 检查文件扩展名
    std::string ext = std::filesystem::path(model_path).extension();
    return (ext == ".gguf" || ext == ".bin");
}

LlamaConfig getDefaultConfig() {
    LlamaConfig config;
    config.n_threads = std::thread::hardware_concurrency();
    return config;
}

} // namespace ai
} // namespace nex
