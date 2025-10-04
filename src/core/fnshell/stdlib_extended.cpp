#include "stdlib.h"
#include "evaluator.h"
#include "value.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <sstream>

namespace fs = std::filesystem;

namespace fnshell {

// files(pattern) -> List<String>
static ValuePtr fn_files(const std::vector<ValuePtr>& args) {
    if (args.empty() || args[0]->type != Value::Type::String) {
        return Value::make_error("files: requires string pattern");
    }
    std::string pattern = std::get<std::string>(args[0]->data);
    List result;
    for (auto& entry : fs::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string name = entry.path().filename().string();
            if (pattern.rfind("*.", 0) == 0) {
                std::string ext = pattern.substr(1);
                if (name.size() >= ext.size() && name.substr(name.size() - ext.size()) == ext) {
                    result.push_back(Value::make_string(name));
                }
            }
        }
    }
    return Value::make_list(result);
}

// text.read(filename_str) -> String (可能 Error)
static ValuePtr fn_text_read(const std::vector<ValuePtr>& args) {
    if (args.empty() || args[0]->type != Value::Type::String) {
        return Value::make_error("text.read: requires string path");
    }
    std::string path = std::get<std::string>(args[0]->data);
    std::ifstream ifs(path);
    if (!ifs.is_open()) return Value::make_error("text.read: open failed: " + path);
    std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    return Value::make_string(content);
}

// split_lines(str) -> List<String>
static ValuePtr fn_split_lines(const std::vector<ValuePtr>& args) {
    if (args.empty() || args[0]->type != Value::Type::String) {
        return Value::make_error("split_lines: requires string");
    }
    std::string s = std::get<std::string>(args[0]->data);
    List result;
    std::istringstream iss(s);
    std::string line;
    while (std::getline(iss, line)) {
        result.push_back(Value::make_string(line));
    }
    return Value::make_list(result);
}

// contains(substr, str) -> Bool
static ValuePtr fn_contains(const std::vector<ValuePtr>& args) {
    if (args.size() < 2 || args[0]->type != Value::Type::String || args[1]->type != Value::Type::String) {
        return Value::make_error("contains: requires (substr, str)");
    }
    std::string sub = std::get<std::string>(args[0]->data);
    std::string s = std::get<std::string>(args[1]->data);
    return Value::make_bool(s.find(sub) != std::string::npos);
}

// map(fn, list) -> List
static ValuePtr fn_map(const std::vector<ValuePtr>& args, Evaluator& ev) {
    if (args.size() < 2) return Value::make_error("map: requires (fn, list)");
    if (args[1]->type != Value::Type::List) return Value::make_error("map: second arg must be list");
    if (args[0]->type != Value::Type::Function) return Value::make_error("map: first arg must be function");
    std::string fn_name = std::get<std::string>(args[0]->data);
    auto& lst = std::get<List>(args[1]->data);
    List result;
    for (auto& v : lst) {
        // 调用函数
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::Call;
        node->name = fn_name;
        auto res = ev.eval(node); // 简化：直接用单参调用占位，实际应动态构造 args
        // 改为手动调用内置
        // 暂时简化：map 只展示结构，实际调用待完善
        result.push_back(v);
    }
    return Value::make_list(result);
}

// filter(pred_fn, list) -> List
static ValuePtr fn_filter(const std::vector<ValuePtr>& args) {
    if (args.size() < 2) return Value::make_error("filter: requires (pred, list)");
    if (args[1]->type != Value::Type::List) return Value::make_error("filter: second arg must be list");
    auto& lst = std::get<List>(args[1]->data);
    List result;
    // 占位：全保留（真正实现需为每个元素调用 pred）
    for (auto& v : lst) result.push_back(v);
    return Value::make_list(result);
}

// unwrap_or(val, default_val) -> val if not Error else default
static ValuePtr fn_unwrap_or(const std::vector<ValuePtr>& args) {
    if (args.size() < 2) return Value::make_error("unwrap_or: requires (val, default)");
    if (args[0]->is_error()) return args[1];
    return args[0];
}

// sys.events() -> List<String> (占位)
static ValuePtr fn_sys_events(const std::vector<ValuePtr>&) {
    List events;
    events.push_back(Value::make_string("[event1]"));
    events.push_back(Value::make_string("[event2]"));
    return Value::make_list(events);
}

void register_stdlib(Evaluator& ev) {
    ev.register_builtin("files", fn_files);
    ev.register_builtin("text.read", fn_text_read);
    ev.register_builtin("split_lines", fn_split_lines);
    ev.register_builtin("contains", fn_contains);
    ev.register_builtin("map", [&](const std::vector<ValuePtr>& a){ return fn_map(a, ev); });
    ev.register_builtin("filter", fn_filter);
    ev.register_builtin("unwrap_or", fn_unwrap_or);
    ev.register_builtin("sys.events", fn_sys_events);
}

} // namespace fnshell


