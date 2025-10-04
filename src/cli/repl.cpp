#include "repl.h"

#include <iostream>
#include <memory>
#include <string>

#include "core/inference/engine.h"
#include "core/inference/local_llama/llama_engine.h"
#include "core/conversation/session.h"
#include "core/conversation/template.h"
#include "core/sysbox/sysbox.h"
#include "core/storage/sqlite_store.h"
#include "core/tools/tools.h"
#include "utils/config.h"
#include "core/fnshell/parser.h"
#include "core/fnshell/evaluator.h"
#include "core/inference/remote/openai_client.h"
#include "core/inference/remote/gemini_client.h"
#include "core/router/router.h"

namespace cli {

static std::shared_ptr<inference::Engine>& local_engine() {
    static std::shared_ptr<inference::Engine> engine = inference::create_local_engine();
    return engine;
}

static std::shared_ptr<inference::Engine>& cloud_engine() {
    static std::shared_ptr<inference::Engine> engine;
    return engine;
}

static std::unique_ptr<router::Router>& router_singleton() {
    static std::unique_ptr<router::Router> r;
    return r;
}

Repl::~Repl() = default;

void Repl::run() {
    tools::register_builtin_tools();
    sessions_.reset(new conversation::SessionManager());
    sessions_->ensure_session("default");

    std::cout << "aicli \n";
    std::cout << "输入以 '/' 开头的命令，例如 /help，其他输入将与 AI 对话。\n";
    std::cout << "输入 /exit 退出。\n";

    std::string line;
    while (true) {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line)) {
            break;
        }
        if (line.rfind('/', 0) == 0) {
            handle_command(line);
        } else {
            handle_chat(line);
        }
    }
}

void Repl::handle_command(const std::string& line) {
    if (line == "/exit") {
        std::cout << "再见。\n";
        std::exit(0);
    } else if (line == "/help") {
        std::cout << "可用命令：\n";
        std::cout << "  /help                       显示帮助\n";
        std::cout << "  /exit                       退出\n";
        std::cout << "  /config                     显示当前配置(占位)\n";
        std::cout << "  /model <path>               加载本地 gguf 模型\n";
        std::cout << "  /session [name|list|clear|rm <name>]\n";
        std::cout << "  /render think on|off        切换显示 <think> 轨\n";
        std::cout << "  /stop                       中断当前生成\n";
        std::cout << "  /tools list                 列出可用工具\n";
        std::cout << "  /tools run <name> <args-json> 运行工具\n";
        std::cout << "  /fn <expr>                  执行函数式 Shell 表达式\n";
        std::cout << "  /cloud openai|gemini|disable 启用/禁用云端 Provider\n";
    } else if (line == "/config") {
        std::cout << "[占位] 显示配置：来自环境变量与 config/aicli.yaml\n";
    } else if (line.rfind("/model", 0) == 0) {
        auto args = line.substr(std::string("/model").size());
        cmd_model(args);
    } else if (line.rfind("/session", 0) == 0) {
        auto args = line.substr(std::string("/session").size());
        cmd_session(args);
    } else if (line.rfind("/render", 0) == 0) {
        auto args = line.substr(std::string("/render").size());
        cmd_render(args);
    } else if (line == "/stop") {
        cmd_stop();
    } else if (line.rfind("/tools", 0) == 0) {
        auto rest = line.substr(std::string("/tools").size());
        auto ltrim = [](std::string& s){ s.erase(0, s.find_first_not_of(" \t")); };
        auto rtrim = [](std::string& s){ s.erase(s.find_last_not_of(" \t") + 1); };
        ltrim(rest); rtrim(rest);
        if (rest == "list") {
            auto v = tools::Registry::instance().list();
            for (auto& t : v) std::cout << t.name << " - " << t.description << "\n";
            return;
        }
        if (rest.rfind("run ", 0) == 0) {
            auto payload = rest.substr(4); ltrim(payload);
            // 拆 name 与 args-json
            auto sp = payload.find(' ');
            if (sp == std::string::npos) { std::cout << "用法：/tools run <name> <args-json>\n"; return; }
            std::string name = payload.substr(0, sp);
            std::string args = payload.substr(sp + 1);
            auto td = tools::Registry::instance().find(name);
            if (!td) { std::cout << "未找到工具：" << name << "\n"; return; }
            auto res = td->run(args);
            if (res.ok) std::cout << res.output_json << "\n"; else std::cout << "[工具错误] " << res.error << "\n";
            return;
        }
        std::cout << "用法：/tools list 或 /tools run <name> <args-json>\n";
    } else if (line.rfind("/fn", 0) == 0) {
        auto expr = line.substr(std::string("/fn").size());
        auto ltrim = [](std::string& s){ s.erase(0, s.find_first_not_of(" \t")); };
        ltrim(expr);
        if (expr.empty()) { std::cout << "用法：/fn <expression>\n"; return; }
        fnshell::Parser parser;
        std::string err;
        auto ast = parser.parse(expr, err);
        if (!ast) { std::cout << "[解析错误] " << err << "\n"; return; }
        fnshell::Evaluator evaluator;
        auto result = evaluator.eval(*ast);
        std::cout << result->to_string() << "\n";
    } else if (line.rfind("/cloud", 0) == 0) {
        auto args = line.substr(std::string("/cloud").size());
        cmd_cloud(args);
    } else {
        std::cout << "未知命令：" << line << "，输入 /help 查看帮助。\n";
    }
}

static void split_think_final(const std::string& full, bool show_think) {
    if (!show_think) {
        // 去除 <think>...</think> 片段
        std::string out;
        out.reserve(full.size());
        size_t i = 0;
        while (i < full.size()) {
            size_t start = full.find("<think>", i);
            if (start == std::string::npos) {
                out.append(full.substr(i));
                break;
            }
            out.append(full.substr(i, start - i));
            size_t end = full.find("</think>", start);
            if (end == std::string::npos) {
                // 无闭合则丢弃后续
                break;
            }
            i = end + 8; // 跳过 </think>
        }
        std::cout << out;
    } else {
        // 原样输出
        std::cout << full;
    }
}

static bool run_inline_tools(std::string& user_text) {
    // 匹配 {{tool:name args_json}}，只做简单线性查找，不支持嵌套
    bool changed = false;
    size_t pos = 0;
    while (true) {
        auto l = user_text.find("{{tool:", pos);
        if (l == std::string::npos) break;
        auto sp = user_text.find(' ', l);
        auto rb = user_text.find("}}", l);
        if (sp == std::string::npos || rb == std::string::npos || sp > rb) break;
        std::string name = user_text.substr(l + 7, sp - (l + 7));
        std::string args = user_text.substr(sp + 1, rb - sp - 1);
        auto td = tools::Registry::instance().find(name);
        if (!td) { pos = rb + 2; continue; }
        auto res = td->run(args);
        std::string inject;
        if (res.ok) { inject = std::string("[tool ") + name + ": " + res.output_json + "]"; }
        else { inject = std::string("[tool ") + name + " error: " + res.error + "]"; }
        user_text.replace(l, rb - l + 2, inject);
        pos = l + inject.size();
        changed = true;
    }
    return changed;
}

void Repl::handle_chat(const std::string& line) {
    if (line.empty()) {
        return;
    }
    // 优先使用路由器，回退到本地引擎
    auto& rt = router_singleton();
    auto& local_eng = local_engine();
    
    if (!rt && !local_eng) {
        std::cout << "[提示] 请先使用 /model 或 /cloud 配置引擎。\n";
        return;
    }
    if (!rt && (!local_eng || !local_eng->is_loaded())) {
        std::cout << "[提示] 请先使用 /model <path> 加载本地模型。\n";
        return;
    }

    const std::string& sname = sessions_->current();
    std::string user_text = line;
    // 执行内联工具
    run_inline_tools(user_text);

    conversation::Message umsg{"user", user_text};
    sessions_->add_message(sname, umsg);
    if (storage::sqlite_available()) storage::save_message(sname, umsg);

    conversation::RenderOptions ropts; ropts.use_chatml = true;
    std::string prompt;
    if (ropts.use_chatml) {
        prompt = conversation::TemplateBuilder::render_chatml(sessions_->history(sname), ropts);
    } else {
        prompt = conversation::TemplateBuilder::render_plain(sessions_->history(sname), ropts);
    }

    std::string buffer;
    std::string err;
    inference::GenerateOptions opt;
    bool ok = false;
    
    if (rt) {
        // 使用路由器
        ok = rt->generate(sname, prompt, opt, [&](const std::string& tok){ buffer += tok; }, err);
    } else if (local_eng && local_eng->is_loaded()) {
        // 仅本地
        ok = local_eng->generate_with_session(sname, prompt, opt, [&](const std::string& tok){ buffer += tok; }, err);
    }
    
    if (!ok) {
        std::cout << "[错误] 推理失败：" << err << "\n";
        return;
    }
    std::cout << "AI：";
    split_think_final(buffer, show_think_);
    std::cout << "\n";

    conversation::Message amsg{"assistant", buffer};
    sessions_->add_message(sname, amsg);
    if (storage::sqlite_available()) storage::save_message(sname, amsg);
}

void Repl::cmd_model(const std::string& args) {
    std::string path = args;
    auto ltrim = [](std::string& s){ s.erase(0, s.find_first_not_of(" \t")); };
    auto rtrim = [](std::string& s){ s.erase(s.find_last_not_of(" \t") + 1); };
    std::string tmp = path; ltrim(tmp); rtrim(tmp); path = tmp;
    if (path.empty()) { std::cout << "用法：/model <path-to-gguf>\n"; return; }
    auto& eng = local_engine(); if (!eng) { eng = inference::create_local_engine(); }
    std::string err; if (eng->load_model(path, err)) { std::cout << "已加载模型：" << path << "\n"; sysbox::record({"cli","info","model loaded: "+path}); } else { std::cout << "加载失败：" << err << "\n"; sysbox::record({"cli","error","model load failed: "+err}); }
}

void Repl::cmd_session(const std::string& args) {
    std::string name = args; auto ltrim = [](std::string& s){ s.erase(0, s.find_first_not_of(" \t")); }; auto rtrim = [](std::string& s){ s.erase(s.find_last_not_of(" \t") + 1); };
    std::string tmp = name; ltrim(tmp); rtrim(tmp); name = tmp;
    if (name.empty() || name == "list") {
        std::vector<std::string> list = sessions_->list();
        if (storage::sqlite_available()) {
            auto sql_list = storage::list_sessions();
            list.insert(list.end(), sql_list.begin(), sql_list.end());
        }
        // 去重
        std::sort(list.begin(), list.end()); list.erase(std::unique(list.begin(), list.end()), list.end());
        std::cout << "当前会话：" << sessions_->current() << "\n";
        std::cout << "会话列表(" << list.size() << ")：";
        for (auto& n : list) std::cout << ' ' << n;
        std::cout << "\n";
        return;
    }
    if (name == "clear") {
        const std::string& cur = sessions_->current();
        sessions_->ensure_session(cur);
        std::cout << "已清空当前会话历史：" << cur << "\n";
        auto& eng = local_engine(); if (eng) eng->reset_session(cur);
        auto& ceng = cloud_engine(); if (ceng) ceng->reset_session(cur);
        return;
    }
    if (name.rfind("rm ", 0) == 0) {
        std::string rmname = name.substr(3); ltrim(rmname); rtrim(rmname);
        auto& eng = local_engine(); if (eng) eng->reset_session(rmname);
        auto& ceng = cloud_engine(); if (ceng) ceng->reset_session(rmname);
        std::cout << "已重置会话状态：" << rmname << "（历史清理需后续实现）\n";
        return;
    }
    sessions_->set_current(name);
    sessions_->ensure_session(name);
    std::cout << "已切换到会话：" << name << "\n";
}

void Repl::cmd_render(const std::string& args) {
    std::string a = args; auto ltrim = [](std::string& s){ s.erase(0, s.find_first_not_of(" \t")); }; auto rtrim = [](std::string& s){ s.erase(s.find_last_not_of(" \t") + 1); };
    ltrim(a); rtrim(a);
    if (a.rfind("think", 0) == 0) {
        std::string rest = a.substr(5); ltrim(rest); rtrim(rest);
        if (rest == "on") { show_think_ = true; std::cout << "think 渲染：ON\n"; sysbox::record({"cli","info","think=on"}); return; }
        if (rest == "off") { show_think_ = false; std::cout << "think 渲染：OFF\n"; sysbox::record({"cli","info","think=off"}); return; }
    }
    std::cout << "用法：/render think on|off\n";
    sysbox::record({"cli","info", std::string("think=") + (show_think_?"on":"off")});
}

void Repl::cmd_stop() {
    auto& eng = local_engine(); if (eng) eng->request_abort();
    auto& ceng = cloud_engine(); if (ceng) ceng->request_abort();
    std::cout << "[中断] 已请求停止当前生成\n";
    sysbox::record({"cli","warn","abort requested"});
}

void Repl::cmd_cloud(const std::string& args) {
    auto ltrim = [](std::string& s){ s.erase(0, s.find_first_not_of(" \t")); };
    auto rtrim = [](std::string& s){ s.erase(s.find_last_not_of(" \t") + 1); };
    std::string a = args; ltrim(a); rtrim(a);
    
    if (a == "openai") {
        auto key = config::get_env("OPENAI_API_KEY");
        if (!key || key->empty()) {
            std::cout << "[错误] 请设置环境变量 OPENAI_API_KEY\n";
            return;
        }
        auto base_url = config::get_env("OPENAI_BASE_URL");
        std::string url = base_url ? *base_url : "https://api.openai.com/v1";
        cloud_engine() = inference::create_openai_engine(*key, url, "gpt-4o-mini");
        router_singleton() = std::make_unique<router::Router>(local_engine(), cloud_engine());
        std::cout << "已启用 OpenAI（" << url << "，模型：gpt-4o-mini）\n";
        sysbox::record({"cli","info","cloud provider enabled: openai"});
        return;
    }
    if (a == "gemini") {
        auto key = config::get_env("GEMINI_API_KEY");
        if (!key || key->empty()) {
            std::cout << "[错误] 请设置环境变量 GEMINI_API_KEY\n";
            return;
        }
        auto base_url = config::get_env("GEMINI_BASE_URL");
        std::string url = base_url ? *base_url : "https://generativelanguage.googleapis.com/v1beta";
        cloud_engine() = inference::create_gemini_engine(*key, url, "gemini-1.5-flash");
        router_singleton() = std::make_unique<router::Router>(local_engine(), cloud_engine());
        std::cout << "已启用 Gemini（" << url << "，模型：gemini-1.5-flash）\n";
        sysbox::record({"cli","info","cloud provider enabled: gemini"});
        return;
    }
    if (a == "disable") {
        cloud_engine().reset();
        router_singleton().reset();
        std::cout << "已禁用云端 Provider\n";
        sysbox::record({"cli","info","cloud provider disabled"});
        return;
    }
    std::cout << "用法：/cloud openai|gemini|disable\n";
}

} // namespace cli



