#pragma once

#include <string>
#include <memory>
#include "core/conversation/session.h"

namespace cli {

class Repl {
public:
    Repl() = default;
    ~Repl();
    void run();

private:
    void handle_command(const std::string& line);
    void handle_chat(const std::string& line);

    // 简易引擎生命周期与调用
    void cmd_model(const std::string& args);

    // 会话控制
    void cmd_session(const std::string& args);
    std::unique_ptr<conversation::SessionManager> sessions_;

    // 渲染控制
    bool show_think_ = false;
    void cmd_render(const std::string& args);
    void cmd_stop();
    
    // 云端 Provider
    void cmd_cloud(const std::string& args);
};

} // namespace cli



