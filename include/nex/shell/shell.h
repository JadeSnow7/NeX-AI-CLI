#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>

namespace nex::shell {

/**
 * @brief 命令参数类型
 */
enum class ArgumentType {
    String,
    Flag,
    Option,
    Redirection,
    Pipe
};

/**
 * @brief 重定向类型
 */
enum class RedirectionType {
    Input,      // <
    Output,     // >
    Append,     // >>
    Error,      // 2>
    ErrorAppend // 2>>
};

/**
 * @brief 命令参数
 */
struct Argument {
    ArgumentType type;
    std::string value;
    std::string target;  // 用于重定向目标
    
    Argument(ArgumentType t, const std::string& v, const std::string& tgt = "")
        : type(t), value(v), target(tgt) {}
};

/**
 * @brief 解析后的命令
 */
struct ParsedCommand {
    std::string program;
    std::vector<Argument> arguments;
    std::vector<std::string> args;  // 简化的参数列表
    std::map<RedirectionType, std::string> redirections;
    bool background = false;
    
    // 获取所有字符串参数
    std::vector<std::string> getStringArgs() const {
        std::vector<std::string> result;
        for (const auto& arg : arguments) {
            if (arg.type == ArgumentType::String) {
                result.push_back(arg.value);
            }
        }
        return result;
    }
};

/**
 * @brief 管道命令
 */
struct Pipeline {
    std::vector<ParsedCommand> commands;
    bool background = false;
};

/**
 * @brief 命令执行结果
 */
struct CommandResult {
    int exit_code;
    std::string output;
    std::string error;
    bool success;
    
    CommandResult(int code = 0, const std::string& out = "", const std::string& err = "")
        : exit_code(code), output(out), error(err), success(code == 0) {}
};

/**
 * @brief 内置命令处理器
 */
using BuiltinHandler = std::function<CommandResult(const ParsedCommand&)>;

/**
 * @brief 命令解析器
 */
class CommandParser {
public:
    /**
     * @brief 解析命令行
     */
    Pipeline parseLine(const std::string& line);
    
    /**
     * @brief 解析单个命令
     */
    ParsedCommand parseCommand(const std::string& command);
    
private:
    std::vector<std::string> tokenize(const std::string& line);
    ParsedCommand parseTokens(const std::vector<std::string>& tokens);
    std::string expandVariables(const std::string& str);
    std::string removeQuotes(const std::string& str);
};

/**
 * @brief 命令执行器
 */
class CommandExecutor {
public:
    CommandExecutor();
    
    /**
     * @brief 执行管道命令
     */
    CommandResult executePipeline(const Pipeline& pipeline);
    
    /**
     * @brief 执行单个命令
     */
    CommandResult executeCommand(const ParsedCommand& command);
    
    /**
     * @brief 注册内置命令
     */
    void registerBuiltin(const std::string& name, BuiltinHandler handler);
    
    /**
     * @brief 检查是否为内置命令
     */
    bool isBuiltin(const std::string& name) const;
    
private:
    std::map<std::string, BuiltinHandler> builtins_;
    
    CommandResult executeBuiltin(const ParsedCommand& command);
    CommandResult executeExternal(const ParsedCommand& command);
    CommandResult executePipelineInternal(const std::vector<ParsedCommand>& commands);
    
    void setupRedirections(const ParsedCommand& command, int& stdin_fd, int& stdout_fd, int& stderr_fd);
    void restoreRedirections(int stdin_fd, int stdout_fd, int stderr_fd);
};

/**
 * @brief 环境变量管理器
 */
class Environment {
public:
    static Environment& getInstance();
    
    std::string getVariable(const std::string& name) const;
    void setVariable(const std::string& name, const std::string& value);
    void unsetVariable(const std::string& name);
    bool hasVariable(const std::string& name) const;
    
    std::vector<std::string> getPath() const;
    std::string findExecutable(const std::string& name) const;
    
    std::string getCurrentDirectory() const;
    bool changeDirectory(const std::string& path);
    
    std::map<std::string, std::string> getAllVariables() const;
    
private:
    Environment() = default;
    mutable std::map<std::string, std::string> variables_;
    void loadSystemEnvironment();
};

/**
 * @brief Shell会话管理器
 */
class ShellSession {
public:
    ShellSession();
    
    /**
     * @brief 运行交互式shell
     */
    void runInteractive();
    
    /**
     * @brief 执行脚本文件
     */
    CommandResult runScript(const std::string& filename);
    
    /**
     * @brief 执行单行命令
     */
    CommandResult executeLine(const std::string& line);
    
    /**
     * @brief 设置提示符
     */
    void setPrompt(const std::string& prompt);
    
    /**
     * @brief 启用/禁用历史记录
     */
    void setHistoryEnabled(bool enabled);
    
    /**
     * @brief 启用/禁用语法高亮
     */
    void setSyntaxHighlightEnabled(bool enabled);
    
    /**
     * @brief 启用/禁用Git信息显示
     */
    void setGitInfoEnabled(bool enabled);
    
    /**
     * @brief 启用/禁用系统状态显示
     */
    void setSystemStatusEnabled(bool enabled);

private:
    std::unique_ptr<CommandParser> parser_;
    std::unique_ptr<CommandExecutor> executor_;
    std::string prompt_;
    bool history_enabled_;
    bool syntax_highlight_enabled_;
    bool git_info_enabled_;
    bool system_status_enabled_;
    std::vector<std::string> history_;
    
    void setupBuiltinCommands();
    std::string getPrompt() const;
    void addToHistory(const std::string& line);
    void printResult(const CommandResult& result);
};

} // namespace nex::shell
