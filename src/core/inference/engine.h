#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace inference {

struct GenerateOptions {
    int max_new_tokens = 256;
    float temperature = 0.7f;
    float top_p = 0.95f;
};

using StreamCallback = std::function<void(const std::string&)>;

class Engine {
public:
    virtual ~Engine() = default;
    virtual bool load_model(const std::string& model_path, std::string& err) = 0;
    virtual void unload_model() = 0;
    virtual bool is_loaded() const = 0;

    virtual bool generate(const std::string& prompt,
                          const GenerateOptions& options,
                          const StreamCallback& on_token,
                          std::string& err) = 0;

    // 基于会话的生成：按 session_id 复用 KV 与上下文
    virtual bool generate_with_session(const std::string& session_id,
                                       const std::string& prompt,
                                       const GenerateOptions& options,
                                       const StreamCallback& on_token,
                                       std::string& err) {
        // 默认回退到无会话
        (void)session_id;
        return generate(prompt, options, on_token, err);
    }

    // 重置会话：清理对应序列的 KV/状态
    virtual void reset_session(const std::string& session_id) {
        (void)session_id;
    }

    // 请求取消当前推理；默认空实现
    virtual void request_abort() {}
};

// 获取本地引擎实例（llama 或占位实现）
std::unique_ptr<Engine> create_local_engine();

} // namespace inference
