#pragma once

#include "core/inference/engine.h"
#include <string>

namespace inference {

class GeminiClient : public Engine {
public:
    GeminiClient(const std::string& api_key, const std::string& base_url, const std::string& model);
    ~GeminiClient() override = default;

    bool load_model(const std::string& model_path, std::string& err) override;
    void unload_model() override;
    bool is_loaded() const override;

    bool generate(const std::string& prompt,
                  const GenerateOptions& options,
                  const StreamCallback& on_token,
                  std::string& err) override;

private:
    std::string api_key_;
    std::string base_url_;
    std::string model_;
    bool loaded_ = true;
};

std::unique_ptr<Engine> create_gemini_engine(const std::string& api_key,
                                             const std::string& base_url = "https://generativelanguage.googleapis.com/v1beta",
                                             const std::string& model = "gemini-1.5-flash");

} // namespace inference

