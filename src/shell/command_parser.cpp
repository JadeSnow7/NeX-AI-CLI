#include "nex/shell/shell.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <cctype>

namespace nex::shell {

Pipeline CommandParser::parseLine(const std::string& line) {
    Pipeline pipeline;
    
    // 处理空行和注释
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    if (trimmed.empty() || trimmed[0] == '#') {
        return pipeline;
    }
    
    // 检查后台执行
    if (!trimmed.empty() && trimmed.back() == '&') {
        pipeline.background = true;
        trimmed.pop_back();
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    }
    
    // 按管道符分割命令
    std::vector<std::string> parts;
    std::stringstream ss(trimmed);
    std::string part;
    
    while (std::getline(ss, part, '|')) {
        part.erase(0, part.find_first_not_of(" \t"));
        part.erase(part.find_last_not_of(" \t") + 1);
        if (!part.empty()) {
            parts.push_back(part);
        }
    }
    
    // 解析每个命令
    for (const auto& commandStr : parts) {
        ParsedCommand cmd = parseCommand(commandStr);
        if (!cmd.program.empty()) {
            pipeline.commands.push_back(std::move(cmd));
        }
    }
    
    return pipeline;
}

ParsedCommand CommandParser::parseCommand(const std::string& command) {
    std::vector<std::string> tokens = tokenize(command);
    return parseTokens(tokens);
}

std::vector<std::string> CommandParser::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::string current_token;
    bool in_quotes = false;
    bool in_double_quotes = false;
    bool escape_next = false;
    
    for (size_t i = 0; i < line.length(); ++i) {
        char c = line[i];
        
        if (escape_next) {
            current_token += c;
            escape_next = false;
            continue;
        }
        
        if (c == '\\' && (in_quotes || in_double_quotes)) {
            escape_next = true;
            continue;
        }
        
        if (c == '\'' && !in_double_quotes) {
            in_quotes = !in_quotes;
            continue;
        }
        
        if (c == '"' && !in_quotes) {
            in_double_quotes = !in_double_quotes;
            continue;
        }
        
        if (in_quotes || in_double_quotes) {
            current_token += c;
            continue;
        }
        
        if (std::isspace(c)) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
            continue;
        }
        
        // 处理特殊字符
        if (c == '>' || c == '<' || c == '&' || c == ';') {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
            
            // 处理 >> 和 2> 等
            if (c == '>' && i + 1 < line.length() && line[i + 1] == '>') {
                tokens.push_back(">>");
                ++i;
            } else if (std::isdigit(current_token.empty() ? '1' : current_token.back()) && c == '>') {
                tokens.push_back(std::string(1, c));
            } else {
                tokens.push_back(std::string(1, c));
            }
            continue;
        }
        
        current_token += c;
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

ParsedCommand CommandParser::parseTokens(const std::vector<std::string>& tokens) {
    ParsedCommand cmd;
    
    if (tokens.empty()) {
        return cmd;
    }
    
    cmd.program = tokens[0];
    
    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& token = tokens[i];
        
        // 处理重定向
        if (token == ">") {
            if (i + 1 < tokens.size()) {
                cmd.redirections[RedirectionType::Output] = tokens[i + 1];
                ++i;
            }
        } else if (token == ">>") {
            if (i + 1 < tokens.size()) {
                cmd.redirections[RedirectionType::Append] = tokens[i + 1];
                ++i;
            }
        } else if (token == "<") {
            if (i + 1 < tokens.size()) {
                cmd.redirections[RedirectionType::Input] = tokens[i + 1];
                ++i;
            }
        } else if (token == "2>") {
            if (i + 1 < tokens.size()) {
                cmd.redirections[RedirectionType::Error] = tokens[i + 1];
                ++i;
            }
        } else if (token == "2>>") {
            if (i + 1 < tokens.size()) {
                cmd.redirections[RedirectionType::ErrorAppend] = tokens[i + 1];
                ++i;
            }
        } else if (token == "&") {
            cmd.background = true;
        } else {
            // 普通参数
            std::string expanded = expandVariables(token);
            cmd.arguments.emplace_back(ArgumentType::String, expanded);
            cmd.args.push_back(expanded);
        }
    }
    
    return cmd;
}

std::string CommandParser::expandVariables(const std::string& str) {
    std::string result = str;
    std::regex var_regex(R"(\$\{([^}]+)\}|\$([A-Za-z_][A-Za-z0-9_]*))");
    std::smatch match;
    
    while (std::regex_search(result, match, var_regex)) {
        std::string var_name = match[1].matched ? match[1].str() : match[2].str();
        std::string var_value = Environment::getInstance().getVariable(var_name);
        
        result.replace(match.position(), match.length(), var_value);
    }
    
    return result;
}

std::string CommandParser::removeQuotes(const std::string& str) {
    if (str.length() >= 2) {
        if ((str.front() == '"' && str.back() == '"') ||
            (str.front() == '\'' && str.back() == '\'')) {
            return str.substr(1, str.length() - 2);
        }
    }
    return str;
}

} // namespace nex::shell
