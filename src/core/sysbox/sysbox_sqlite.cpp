#include "sysbox.h"

#include <string>
#include <mutex>
#include <cstdlib>

#if AICLI_WITH_SQLITE
#include <sqlite3.h>
#endif

namespace sysbox {

#if AICLI_WITH_SQLITE

static std::mutex& mu() { static std::mutex m; return m; }
static sqlite3* db = nullptr;

static void ensure_db() {
    if (db) return;
    std::lock_guard<std::mutex> lk(mu());
    if (db) return;
    const char* env = std::getenv("AICLI_DATA_DIR");
    std::string path = env ? std::string(env) + "/aicli.db" : std::string("data/aicli.db");
    sqlite3_open(path.c_str(), &db);
    const char* create_events = "CREATE TABLE IF NOT EXISTS events (ts INTEGER DEFAULT (strftime('%s','now')), component TEXT, level TEXT, message TEXT, payload TEXT);";
    sqlite3_exec(db, create_events, nullptr, nullptr, nullptr);
    const char* create_metrics = "CREATE TABLE IF NOT EXISTS metrics (ts INTEGER DEFAULT (strftime('%s','now')), name TEXT, tokens INTEGER, ms REAL, tps REAL, p50 REAL, p95 REAL);";
    sqlite3_exec(db, create_metrics, nullptr, nullptr, nullptr);
}

static void insert_event(const char* component, const char* level, const char* message, const char* payload) {
    ensure_db();
    const char* sql = "INSERT INTO events (component,level,message,payload) VALUES (?,?,?,?);";
    sqlite3_stmt* st = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &st, nullptr);
    sqlite3_bind_text(st, 1, component, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 2, level, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 3, message ? message : "", -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(st, 4, payload ? payload : "", -1, SQLITE_TRANSIENT);
    sqlite3_step(st);
    sqlite3_finalize(st);
}

#else
// no-op; fallback handled in sysbox.cpp
static void ensure_db() {}
static void insert_event(const char*, const char*, const char*, const char*) {}
#endif

} // namespace sysbox




