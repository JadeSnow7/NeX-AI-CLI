#pragma once

#include <string>
#include <vector>

#include "core/conversation/session.h"

namespace storage {

// 返回是否编译并启用 SQLite
bool sqlite_available();

// 初始化数据库（创建表）
void init();

// 会话历史
void save_message(const std::string& session, const conversation::Message& msg);
std::vector<conversation::Message> load_history(const std::string& session);
std::vector<std::string> list_sessions();

// 工具调用日志
void log_tool_invocation(const std::string& tool_name,
                         const std::string& args_json,
                         const std::string& result_json,
                         bool ok,
                         double duration_ms);

} // namespace storage




