#pragma once

#include "core/inference/engine.h"
#include <memory>
#include <string>

namespace router {

enum class Decision { Local, Cloud };

struct Request {
    std::string prompt;
    int estimated_input_tokens = 0;
    int estimated_output_tokens = 0;
    bool requires_tools = false;
};

class Router {
public:
    Router(std::shared_ptr<inference::Engine> local_engine,
           std::shared_ptr<inference::Engine> cloud_engine);

    Decision decide(const Request& req) const;

    bool generate(const std::string& session_id,
                 const std::string& prompt,
                 const inference::GenerateOptions& options,
                 const inference::StreamCallback& on_token,
                 std::string& err);

private:
    std::shared_ptr<inference::Engine> local_;
    std::shared_ptr<inference::Engine> cloud_;
    bool prefer_local_ = true;
    int input_token_threshold_ = 800;
};

} // namespace router

