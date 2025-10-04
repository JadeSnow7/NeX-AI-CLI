#include "router.h"
#include "core/sysbox/sysbox.h"
#include "utils/config.h"

#include <cstdlib>

namespace router {

Router::Router(std::shared_ptr<inference::Engine> local_engine,
              std::shared_ptr<inference::Engine> cloud_engine)
    : local_(local_engine), cloud_(cloud_engine) {
    // 从环境变量读取配置
    if (auto v = config::get_env("AICLI_PREFER_LOCAL")) {
        prefer_local_ = (*v == "true" || *v == "1");
    }
    if (auto v = config::get_env("AICLI_INPUT_TOKEN_THRESHOLD")) {
        try { input_token_threshold_ = std::stoi(*v); } catch (...) {}
    }
}

Decision Router::decide(const Request& req) const {
    // 简单策略：输入 token 超过阈值 → 云端；否则本地
    if (req.estimated_input_tokens > input_token_threshold_) {
        sysbox::record({"router","info","routed to cloud (token threshold)"});
        return Decision::Cloud;
    }
    if (req.requires_tools && !prefer_local_) {
        sysbox::record({"router","info","routed to cloud (tools required)"});
        return Decision::Cloud;
    }
    sysbox::record({"router","info","routed to local"});
    return Decision::Local;
}

bool Router::generate(const std::string& session_id,
                     const std::string& prompt,
                     const inference::GenerateOptions& options,
                     const inference::StreamCallback& on_token,
                     std::string& err) {
    Request req;
    req.prompt = prompt;
    req.estimated_input_tokens = (int)(prompt.size() / 4); // 粗略估计

    Decision d = decide(req);
    std::shared_ptr<inference::Engine> eng = (d == Decision::Local) ? local_ : cloud_;
    
    if (!eng || !eng->is_loaded()) {
        // 回退
        eng = (d == Decision::Local) ? cloud_ : local_;
        if (!eng || !eng->is_loaded()) {
            err = "no engine available";
            sysbox::record({"router","error","no engine available"});
            return false;
        }
        sysbox::record({"router","warn","fallback to alternative engine"});
    }

    bool ok = eng->generate_with_session(session_id, prompt, options, on_token, err);
    if (!ok) {
        sysbox::record({"router","error","generate failed: " + err});
    }
    return ok;
}

} // namespace router

