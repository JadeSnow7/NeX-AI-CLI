#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

namespace nex::cli {

/**
 * @brief 命令执行结果
 */
struct CommandResult {
    bool success;
    int exit_code;
    std::string output;
    std::string error_message;
    
    CommandResult(bool success = true, int code = 0, 
                 const std::string& output = "", 
                 const std::string& error = "")
        : success(success), exit_code(code), output(output), error_message(error) {}
};

/**
 * @brief 命令参数
 */
struct CommandArgs {
    std::vector<std::string> positional_args;
    std::map<std::string, std::string> options;
    std::map<std::string, bool> flags;
    
    bool hasOption(const std::string& name) const {
        return options.find(name) != options.end();
    }
    
    std::string getOption(const std::string& name, const std::string& default_value = "") const {
        auto it = options.find(name);
        return (it != options.end()) ? it->second : default_value;
    }
    
    bool hasFlag(const std::string& name) const {
        auto it = flags.find(name);
        return (it != flags.end()) ? it->second : false;
    }
};

/**
 * @brief 命令处理器接口
 */
class ICommandHandler {
public:
    virtual ~ICommandHandler() = default;
    virtual CommandResult execute(const CommandArgs& args) = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
};

/**
 * @brief 命令处理函数类型
 */
using CommandFunction = std::function<CommandResult(const CommandArgs&)>;

/**
 * @brief 简单的函数式命令处理器
 */
class FunctionCommandHandler : public ICommandHandler {
private:
    CommandFunction function_;
    std::string description_;
    std::string usage_;
    
public:
    FunctionCommandHandler(CommandFunction func, const std::string& desc, const std::string& usage)
        : function_(std::move(func)), description_(desc), usage_(usage) {}
    
    CommandResult execute(const CommandArgs& args) override {
        return function_(args);
    }
    
    std::string getDescription() const override { return description_; }
    std::string getUsage() const override { return usage_; }
};

/**
 * @brief 命令注册器和调度器
 */
class CommandDispatcher {
private:
    std::map<std::string, std::unique_ptr<ICommandHandler>> commands_;
    
public:
    void registerCommand(const std::string& name, std::unique_ptr<ICommandHandler> handler) {
        commands_[name] = std::move(handler);
    }
    
    void registerCommand(const std::string& name, CommandFunction func, 
                        const std::string& description, const std::string& usage) {
        auto handler = std::make_unique<FunctionCommandHandler>(std::move(func), description, usage);
        registerCommand(name, std::move(handler));
    }
    
    CommandResult dispatch(const std::string& command, const CommandArgs& args) {
        auto it = commands_.find(command);
        if (it != commands_.end()) {
            return it->second->execute(args);
        }
        
        return CommandResult(false, 1, "", "Unknown command: " + command);
    }
    
    bool hasCommand(const std::string& name) const {
        return commands_.find(name) != commands_.end();
    }
    
    std::vector<std::string> getCommandNames() const {
        std::vector<std::string> names;
        for (const auto& [name, handler] : commands_) {
            names.push_back(name);
        }
        return names;
    }
    
    std::string getCommandDescription(const std::string& name) const {
        auto it = commands_.find(name);
        return (it != commands_.end()) ? it->second->getDescription() : "";
    }
    
    std::string getCommandUsage(const std::string& name) const {
        auto it = commands_.find(name);
        return (it != commands_.end()) ? it->second->getUsage() : "";
    }
};

/**
 * @brief CLI应用程序接口
 */
class ICLIApplication {
public:
    virtual ~ICLIApplication() = default;
    
    virtual int run(int argc, char* argv[]) = 0;
    virtual void setupCommands() = 0;
    virtual void showHelp() const = 0;
    virtual void showVersion() const = 0;
};

} // namespace nex::cli
