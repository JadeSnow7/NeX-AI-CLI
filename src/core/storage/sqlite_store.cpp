#include "sqlite_store.h"

#include <cstdlib>
#include <mutex>

#if AICLI_WITH_SQLITE
#include <sqlite3.h>
#endif

namespace storage {

#if AICLI_WITH_SQLITE

static std::mutex& mu() { static std::mutex m; return m; }
static sqlite3* db = nullptr;

bool sqlite_available() { return true; }

static void ensure_db() {
    if (db) return;
    std::lock_guard<std::mutex> lk(mu());
    if (db) return;
    const char* env = std::getenv("AICLI_DATA_DIR");
    std::string path = env ? std::string(env) + "/aicli.db" : std::string("data/aicli.db");
    sqlite3_open(path.c_str(), &db);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS sessions (name TEXT PRIMARY KEY)", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY AUTOINCREMENT, session TEXT, role TEXT, content TEXT, ts INTEGER DEFAULT (strftime('%s','now')))", nullptr, nullptr, nullptr);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS tool_invocations (id INTEGER PRIMARY KEY AUTOINCREMENT, tool TEXT, args TEXT, result TEXT, ok INTEGER, duration_ms REAL, ts INTEGER DEFAULT (strftime('%s','now')))", nullptr, nullptr, nullptr);
}

void init() { ensure_db(); }

void save_message(const std::string& session, const conversation::Message& msg) {
    ensure_db();
    sqlite3_stmt* st = nullptr;
    sqlite3_prepare_v2(db, "INSERT OR IGNORE INTO sessions(name) VALUES (?)", -1, &st, nullptr);
    sqlite3_bind_text(st, 1, session.c_str(), -1, SQLITE_TRANSIENT); sqlite3_step(st); sqlite3_finalize(st);
    sqlite3_prepare_v2(db, "INSERT INTO messages(session,role,content) VALUES (?,?,?)", -1, &st, nullptr);
    sqlite3_bind_text(st, 1, session.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, msg.role.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, msg.content.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(st); sqlite3_finalize(st);
}

std::vector<conversation::Message> load_history(const std::string& session) {
    ensure_db();
    std::vector<conversation::Message> out;
    sqlite3_stmt* st = nullptr;
    sqlite3_prepare_v2(db, "SELECT role,content FROM messages WHERE session=? ORDER BY id", -1, &st, nullptr);
    sqlite3_bind_text(st, 1, session.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(st) == SQLITE_ROW) {
        const char* role = (const char*)sqlite3_column_text(st, 0);
        const char* content = (const char*)sqlite3_column_text(st, 1);
        out.push_back({role ? role : "", content ? content : ""});
    }
    sqlite3_finalize(st);
    return out;
}

std::vector<std::string> list_sessions() {
    ensure_db();
    std::vector<std::string> out;
    sqlite3_stmt* st = nullptr;
    sqlite3_prepare_v2(db, "SELECT name FROM sessions ORDER BY name", -1, &st, nullptr);
    while (sqlite3_step(st) == SQLITE_ROW) {
        const char* name = (const char*)sqlite3_column_text(st, 0);
        if (name) out.emplace_back(name);
    }
    sqlite3_finalize(st);
    return out;
}

void log_tool_invocation(const std::string& tool_name, const std::string& args_json, const std::string& result_json, bool ok, double duration_ms) {
    ensure_db();
    sqlite3_stmt* st = nullptr;
    sqlite3_prepare_v2(db, "INSERT INTO tool_invocations(tool,args,result,ok,duration_ms) VALUES (?,?,?,?,?)", -1, &st, nullptr);
    sqlite3_bind_text(st, 1, tool_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, args_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, result_json.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(st, 4, ok ? 1 : 0);
    sqlite3_bind_double(st, 5, duration_ms);
    sqlite3_step(st); sqlite3_finalize(st);
}

#else

bool sqlite_available() { return false; }
void init() {}
void save_message(const std::string&, const conversation::Message&) {}
std::vector<conversation::Message> load_history(const std::string&) { return {}; }
std::vector<std::string> list_sessions() { return {}; }
void log_tool_invocation(const std::string&, const std::string&, const std::string&, bool, double) {}

#endif

} // namespace storage




