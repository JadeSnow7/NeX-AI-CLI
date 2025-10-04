#include "tools.h"
#include "schema.h"
#include "config.h"

#include <algorithm>
#include <fstream>
#include <array>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <cstdlib>

#include "core/storage/sqlite_store.h"
#include "core/sysbox/sysbox.h"

namespace tools {

static inline double now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

Registry& Registry::instance() { static Registry r; return r; }
void Registry::register_tool(const ToolDesc& desc) { items_.push_back(desc); }
std::vector<ToolDesc> Registry::list() const { return items_; }

std::optional<ToolDesc> Registry::find(const std::string& name) const {
    auto v = items_;
    auto it = std::find_if(v.begin(), v.end(), [&](const ToolDesc& d){ return d.name == name; });
    if (it == v.end()) return std::nullopt;
    return *it;
}

static ToolResult tool_echo(const std::string& args_json) {
    double t0 = now_ms();
    sysbox::record_json("tool","info", std::string("{\"tool\":\"echo\",\"args\":") + args_json + "}");
    if (storage::sqlite_available()) storage::log_tool_invocation("echo", args_json, args_json, true, now_ms()-t0);
    return {true, args_json, ""};
}

static ToolResult tool_fs_read_file(const std::string& args_json) {
    double t0 = now_ms();
    auto err = validate_required_fields(args_json, {"path"});
    if (err) return {false, "", "字段 '" + err->field + "': " + err->message};
    auto path = extract_string_field(args_json, "path");
    if (!path) return {false, "", "无效的 path 字段"};
    std::ifstream ifs(*path);
    if (!ifs.is_open()) return {false, "", "打开失败：" + *path};
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    std::string out = std::string("{\"path\":\"") + *path + "\",\"content\":\"";
    for (char c : content) { if (c == '"') out += "\\\""; else if (c == '\n') out += "\\n"; else if (c == '\\') out += "\\\\"; else out += c; }
    out += "\"}";
    sysbox::record_json("tool","info", std::string("{\"tool\":\"fs.read_file\",\"path\":\"") + *path + "\"}");
    if (storage::sqlite_available()) storage::log_tool_invocation("fs.read_file", args_json, out, true, now_ms()-t0);
    return {true, out, ""};
}

static ToolResult tool_fs_write_file(const std::string& args_json) {
    double t0 = now_ms();
    auto& cfg = get_config();
    if (!cfg.fs_allow_write) return {false, "", "写操作被禁用"};
    auto err = validate_required_fields(args_json, {"path","content"});
    if (err) return {false, "", "字段 '" + err->field + "': " + err->message};
    auto p = extract_string_field(args_json, "path");
    auto c = extract_string_field(args_json, "content");
    if (!p || !c) return {false, "", "无效字段"};
    if (p->find("..") != std::string::npos) return {false, "", "路径包含 .."};
    std::ofstream ofs(*p, std::ios::binary);
    if (!ofs.is_open()) return {false, "", "打开失败：" + *p};
    std::string content; content.reserve(c->size());
    for (size_t i = 0; i < c->size(); ++i) {
        char ch = (*c)[i];
        if (ch == '\\' && i + 1 < c->size()) {
            char n = (*c)[i+1];
            if (n=='n'){content+='\n';++i;continue;}
            if (n=='"'){content+='"';++i;continue;}
            if (n=='\\'){content+='\\';++i;continue;}
        }
        content += ch;
    }
    ofs.write(content.data(), (std::streamsize)content.size()); ofs.close();
    std::string out = std::string("{\"path\":\"") + *p + "\",\"written\":" + std::to_string(content.size()) + "}";
    sysbox::record_json("tool","info", std::string("{\"tool\":\"fs.write_file\",\"path\":\"") + *p + "\"}");
    if (storage::sqlite_available()) storage::log_tool_invocation("fs.write_file", args_json, out, true, now_ms()-t0);
    return {true, out, ""};
}

static ToolResult tool_shell_exec(const std::string& args_json) {
    double t0 = now_ms();
    auto& cfg = get_config();
    auto err = validate_required_fields(args_json, {"cmd"});
    if (err) return {false, "", "字段 '" + err->field + "': " + err->message};
    auto cmd = extract_string_field(args_json, "cmd");
    if (!cmd) return {false, "", "无效的 cmd 字段"};
    bool allowed = false;
    for (const auto& a : cfg.shell_whitelist) { if (*cmd == a) { allowed = true; break; } }
    if (!allowed) return {false, "", "命令 '" + *cmd + "' 不在白名单内"};
    std::string full = *cmd;
    auto ap = args_json.find("\"args\"");
    if (ap != std::string::npos) {
        auto lb = args_json.find('[', ap); auto rb = args_json.find(']', lb);
        if (lb != std::string::npos && rb != std::string::npos && rb > lb) {
            std::string arr = args_json.substr(lb+1, rb-lb-1);
            size_t i = 0; while (i < arr.size()) {
                while (i < arr.size() && (arr[i]==' '||arr[i]==',')) ++i;
                if (i>=arr.size()) break;
                if (arr[i] != '"') break;
                size_t j = arr.find('"', i+1); if (j==std::string::npos) break;
                std::string val = arr.substr(i+1, j-i-1);
                full += " \""; full += val; full += "\"";
                i = j+1;
            }
        }
    }
    std::string result;
    FILE* pipe = popen(full.c_str(), "r");
    if (!pipe) return {false, "", "popen 失败"};
    char buf[4096]; size_t total = 0; bool timeout = false;
    while (fgets(buf, sizeof(buf), pipe)) {
        result += buf; total += std::strlen(buf);
        if ((now_ms() - t0) > cfg.timeout_ms) { timeout = true; break; }
        if (total > cfg.max_output_bytes) { break; }
    }
    int rc = pclose(pipe);
    bool ok = (rc == 0) && !timeout;
    std::string out;
    out.reserve(result.size() + 64);
    out += "{\"cmd\":\""; out += *cmd; out += "\",";
    out += "\"rc\":"; out += std::to_string(rc); out += ",";
    out += "\"stdout\":\"";
    for (char c : result) { if (c=='"') out+="\\\""; else if (c=='\n') out+="\\n"; else if (c=='\\') out+="\\\\"; else out+=c; }
    out += "\",";
    out += "\"timeout\":"; out += (timeout?"true":"false");
    out += "}";
    sysbox::record_json("tool","info", std::string("{\"tool\":\"shell.exec\",\"cmd\":\"") + *cmd + "\"}");
    if (storage::sqlite_available()) storage::log_tool_invocation("shell.exec", args_json, out, ok, now_ms()-t0);
    return {ok, out, ok?"":"超时或非零退出码"};
}

void register_builtin_tools() {
    Registry::instance().register_tool({"echo","返回参数原样", tool_echo});
    Registry::instance().register_tool({"fs.read_file","读取文本文件", tool_fs_read_file});
    Registry::instance().register_tool({"fs.write_file","写入文本文件", tool_fs_write_file});
    Registry::instance().register_tool({"shell.exec","执行白名单 shell 命令", tool_shell_exec});
}

} // namespace tools

