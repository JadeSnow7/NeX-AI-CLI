#pragma once

#include "core/inference/engine.h"
#include <memory>
#include <string>

namespace inference {

class LlamaEngine : public Engine {
public:
    LlamaEngine();
    ~LlamaEngine() override;

    bool load_model(const std::string& model_path, std::string& err) override;
    void unload_model() override;
    bool is_loaded() const override;

    bool generate(const std::string& prompt,
                  const GenerateOptions& options,
                  const StreamCallback& on_token,
                  std::string& err) override;

    bool generate_with_session(const std::string& session_id,
                               const std::string& prompt,
                               const GenerateOptions& options,
                               const StreamCallback& on_token,
                               std::string& err) override;

    void reset_session(const std::string& session_id) override;

    void request_abort() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace inference
