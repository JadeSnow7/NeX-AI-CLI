#pragma once

#include <string>
#include <unordered_map>
#include <vector>

namespace conversation {

struct Message {
    std::string role;  // system/user/assistant
    std::string content;
};

class SessionManager {
public:
    const std::string& ensure_session(const std::string& name);
    void set_current(const std::string& name);
    const std::string& current() const;
    std::vector<std::string> list() const;

    void add_message(const std::string& session, const Message& msg);
    const std::vector<Message>& history(const std::string& session) const;

private:
    std::unordered_map<std::string, std::vector<Message>> session_to_messages_;
    std::string current_ = "default";
};

} // namespace conversation




