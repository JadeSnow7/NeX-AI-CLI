#include "session.h"

#include <algorithm>

namespace conversation {

const std::string& SessionManager::ensure_session(const std::string& name) {
    if (!name.empty()) {
        session_to_messages_.emplace(name, std::vector<Message>{});
        current_ = name;
        return current_;
    }
    session_to_messages_.emplace(current_, std::vector<Message>{});
    return current_;
}

void SessionManager::set_current(const std::string& name) {
    if (!name.empty()) {
        session_to_messages_.emplace(name, std::vector<Message>{});
        current_ = name;
    }
}

const std::string& SessionManager::current() const { return current_; }

std::vector<std::string> SessionManager::list() const {
    std::vector<std::string> r;
    r.reserve(session_to_messages_.size());
    for (auto& kv : session_to_messages_) r.push_back(kv.first);
    std::sort(r.begin(), r.end());
    return r;
}

void SessionManager::add_message(const std::string& session, const Message& msg) {
    session_to_messages_[session].push_back(msg);
}

const std::vector<Message>& SessionManager::history(const std::string& session) const {
    static const std::vector<Message> empty;
    auto it = session_to_messages_.find(session);
    if (it == session_to_messages_.end()) return empty;
    return it->second;
}

} // namespace conversation




