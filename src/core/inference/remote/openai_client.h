#pragma once

#include "core/inference/engine.h"
#include <string>

namespace inference {

class OpenAIClient : public Engine {
public:
    OpenAIClient(const std::string& api_key, const std::string& base_url, const std::string& model);
    ~OpenAIClient() override = default;

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

std::unique_ptr<Engine> create_openai_engine(const std::string& api_key,
                                             const std::string& base_url = "https://api.openai.com/v1",
                                             const std::string& model = "gpt-4o-mini");

} // namespace inference

