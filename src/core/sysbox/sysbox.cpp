#include "sysbox.h"

#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>

namespace fs = std::filesystem;

namespace sysbox {

#if AICLI_WITH_SQLITE
// 来自 sysbox_sqlite.cpp
static void ensure_db();
static void insert_event(const char* component, const char* level, const char* message, const char* payload);
#endif

static bool sqlite_enabled() {
#if AICLI_WITH_SQLITE
    const char* env = std::getenv("AICLI_WITH_SQLITE");
    return true; // 已编译开启
#else
    return false;
#endif
}

static std::string& jsonl_path() {
    static std::string p;
    if (p.empty()) {
        fs::create_directories("data");
        p = "data/sysbox.jsonl";
    }
    return p;
}

void record(const Event& ev) {
    std::cerr << "[sysbox] " << ev.component << " " << ev.level << ": " << ev.message << "\n";
#if AICLI_WITH_SQLITE
    if (sqlite_enabled()) insert_event(ev.component.c_str(), ev.level.c_str(), ev.message.c_str(), "");
#endif
    // 旁路 JSONL
    std::ofstream ofs(jsonl_path(), std::ios::app);
    ofs << "{\"component\":\"" << ev.component << "\",\"level\":\"" << ev.level << "\",\"message\":\"";
    for (char c : ev.message) { if (c == '"') ofs << "\\\""; else if (c == '\\') ofs << "\\\\"; else if (c == '\n') ofs << "\\n"; else ofs << c; }
    ofs << "\"}" << '\n';
}

void record_json(const std::string& component, const std::string& level, const std::string& json) {
    std::cerr << "[sysbox] " << component << " " << level << ": " << json << "\n";
#if AICLI_WITH_SQLITE
    if (sqlite_enabled()) insert_event(component.c_str(), level.c_str(), "", json.c_str());
#endif
    std::ofstream ofs(jsonl_path(), std::ios::app);
    ofs << "{\"component\":\"" << component << "\",\"level\":\"" << level << "\",\"payload\":" << json << "}" << '\n';
}

ScopedTimer::ScopedTimer(const std::string& component, const std::string& label)
    : component_(component), label_(label), start_(std::chrono::steady_clock::now()) {}

ScopedTimer::~ScopedTimer() {
    using namespace std::chrono;
    auto dur = duration_cast<milliseconds>(steady_clock::now() - start_).count();
    std::cerr << "[sysbox] " << component_ << " info: timer '" << label_ << "' " << dur << "ms\n";
    record_json(component_, "info", std::string("{\"timer\":\"") + label_ + "\",\"ms\":" + std::to_string(dur) + "}");
}

static std::map<std::string, std::vector<double>>& samples() {
    static std::map<std::string, std::vector<double>> s;
    return s;
}

std::pair<double,double> add_duration_sample(const std::string& name, double ms) {
    auto& v = samples()[name];
    v.push_back(ms);
    std::vector<double> tmp = v; std::sort(tmp.begin(), tmp.end());
    auto pct = [&](double p){ if (tmp.empty()) return 0.0; double idx = p * (tmp.size() - 1); size_t i = (size_t)idx; double frac = idx - i; if (i+1 < tmp.size()) return tmp[i]*(1-frac)+tmp[i+1]*frac; return tmp[i]; };
    return {pct(0.50), pct(0.95)};
}

} // namespace sysbox
