#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <future>
#include <map>
#include <any>
#include <functional>

namespace nex::ai {

/**
 * @brief AI模型类型
 */
enum class ModelType {
    SystemAnalysis,      // 系统分析模型
    ResourceOptimization, // 资源优化模型
    Chatbot,             // 对话模型
    Classification,      // 分类模型
    Prediction,          // 预测模型
    Custom               // 自定义模型
};

/**
 * @brief AI模型信息
 */
struct ModelInfo {
    std::string id;
    std::string name;
    std::string description;
    std::string version;
    ModelType type;
    std::string file_path;
    size_t memory_usage;     // 内存占用（字节）
    bool is_loaded;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief AI推理输入
 */
struct InferenceInput {
    std::string text;
    std::map<std::string, double> numeric_features;
    std::map<std::string, std::string> string_features;
    std::vector<std::vector<float>> tensor_data;
};

/**
 * @brief AI推理输出
 */
struct InferenceOutput {
    std::string text_output;
    std::vector<double> numeric_output;
    std::map<std::string, double> confidence_scores;
    std::vector<std::string> classifications;
    std::map<std::string, std::string> metadata;
    double inference_time_ms;
};

/**
 * @brief AI推理选项
 */
struct InferenceOptions {
    double temperature = 0.7;
    int max_tokens = 1000;
    double top_p = 0.9;
    int top_k = 50;
    std::string stop_sequence = "";
    bool stream_output = false;
};

/**
 * @brief 系统分析结果
 */
struct SystemAnalysisResult {
    std::string overall_status;     // 系统整体状态
    double health_score;            // 健康评分 (0-100)
    std::vector<std::string> issues;         // 发现的问题
    std::vector<std::string> recommendations; // 优化建议
    std::map<std::string, double> metrics;   // 关键指标
    std::string detailed_analysis;           // 详细分析报告
};

/**
 * @brief AI模型管理器接口
 */
class IModelManager {
public:
    virtual ~IModelManager() = default;
    
    // 模型管理
    virtual bool loadModel(const std::string& model_id, const std::string& model_path) = 0;
    virtual bool unloadModel(const std::string& model_id) = 0;
    virtual bool isModelLoaded(const std::string& model_id) const = 0;
    virtual std::vector<ModelInfo> getAvailableModels() const = 0;
    virtual std::vector<ModelInfo> getLoadedModels() const = 0;
    virtual std::optional<ModelInfo> getModelInfo(const std::string& model_id) const = 0;
    
    // 模型切换
    virtual bool setActiveModel(const std::string& model_id) = 0;
    virtual std::string getActiveModelId() const = 0;
    
    // 资源管理
    virtual size_t getTotalMemoryUsage() const = 0;
    virtual void optimizeMemoryUsage() = 0;
};

/**
 * @brief AI推理引擎接口
 */
class IInferenceEngine {
public:
    virtual ~IInferenceEngine() = default;
    
    // 基础推理
    virtual InferenceOutput inference(const InferenceInput& input, 
                                    const InferenceOptions& options = {}) = 0;
    
    // 异步推理
    virtual std::future<InferenceOutput> inferenceAsync(const InferenceInput& input,
                                                       const InferenceOptions& options = {}) = 0;
    
    // 批量推理
    virtual std::vector<InferenceOutput> batchInference(const std::vector<InferenceInput>& inputs,
                                                       const InferenceOptions& options = {}) = 0;
    
    // 流式推理 (用于对话等实时场景)
    virtual void streamInference(const InferenceInput& input,
                               std::function<void(const std::string&)> callback,
                               const InferenceOptions& options = {}) = 0;
    
    // 模型特定方法
    virtual bool supportsModel(const std::string& model_id) const = 0;
    virtual std::vector<std::string> getSupportedFormats() const = 0;
};

/**
 * @brief 系统分析AI引擎
 */
class ISystemAnalysisEngine {
public:
    virtual ~ISystemAnalysisEngine() = default;
    
    // 系统分析
    virtual SystemAnalysisResult analyzeSystem(const std::map<std::string, std::any>& system_data) = 0;
    
    // 性能预测
    virtual std::map<std::string, double> predictPerformance(
        const std::map<std::string, std::vector<double>>& historical_data,
        int prediction_horizon_minutes = 60) = 0;
    
    // 资源优化建议
    virtual std::vector<std::string> generateOptimizationSuggestions(
        const std::map<std::string, double>& current_metrics,
        const std::map<std::string, double>& target_metrics = {}) = 0;
    
    // 异常检测
    virtual std::vector<std::string> detectAnomalies(
        const std::map<std::string, std::vector<double>>& time_series_data) = 0;
    
    // 容量规划
    virtual std::map<std::string, double> planCapacity(
        const std::map<std::string, std::vector<double>>& usage_trends,
        int planning_horizon_days = 30) = 0;
};

/**
 * @brief AI服务管理器
 */
class IAIService {
public:
    virtual ~IAIService() = default;
    
    virtual std::shared_ptr<IModelManager> getModelManager() = 0;
    virtual std::shared_ptr<IInferenceEngine> getInferenceEngine() = 0;
    virtual std::shared_ptr<ISystemAnalysisEngine> getSystemAnalysisEngine() = 0;
    
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    virtual std::string getVersion() const = 0;
    virtual std::map<std::string, std::string> getCapabilities() const = 0;
};

/**
 * @brief 创建AI服务实例
 */
std::unique_ptr<IAIService> createAIService();

} // namespace nex::ai
