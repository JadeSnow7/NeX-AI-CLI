#include "nex/shell/shell.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <errno.h>

namespace nex::shell {

CommandExecutor::CommandExecutor() {
    // 注册基本的内置命令
    registerBuiltin("cd", [](const ParsedCommand& cmd) -> CommandResult {
        std::string path = cmd.args.empty() ? Environment::getInstance().getVariable("HOME") : cmd.args[0];
        if (Environment::getInstance().changeDirectory(path)) {
            return CommandResult(0);
        } else {
            return CommandResult(1, "", "cd: " + path + ": No such file or directory");
        }
    });
    
    registerBuiltin("pwd", [](const ParsedCommand& cmd) -> CommandResult {
        return CommandResult(0, Environment::getInstance().getCurrentDirectory() + "\n");
    });
    
    registerBuiltin("echo", [](const ParsedCommand& cmd) -> CommandResult {
        std::string output;
        for (size_t i = 0; i < cmd.args.size(); ++i) {
            if (i > 0) output += " ";
            output += cmd.args[i];
        }
        output += "\n";
        return CommandResult(0, output);
    });
    
    registerBuiltin("export", [](const ParsedCommand& cmd) -> CommandResult {
        for (const auto& arg : cmd.args) {
            size_t eq_pos = arg.find('=');
            if (eq_pos != std::string::npos) {
                std::string name = arg.substr(0, eq_pos);
                std::string value = arg.substr(eq_pos + 1);
                Environment::getInstance().setVariable(name, value);
            }
        }
        return CommandResult(0);
    });
    
    registerBuiltin("env", [](const ParsedCommand& cmd) -> CommandResult {
        std::string output;
        auto vars = Environment::getInstance().getAllVariables();
        for (const auto& [name, value] : vars) {
            output += name + "=" + value + "\n";
        }
        return CommandResult(0, output);
    });
    
    registerBuiltin("exit", [](const ParsedCommand& cmd) -> CommandResult {
        int code = cmd.args.empty() ? 0 : std::stoi(cmd.args[0]);
        std::exit(code);
        return CommandResult(code);
    });
}

CommandResult CommandExecutor::executePipeline(const Pipeline& pipeline) {
    if (pipeline.commands.empty()) {
        return CommandResult(0);
    }
    
    if (pipeline.commands.size() == 1) {
        return executeCommand(pipeline.commands[0]);
    }
    
    return executePipelineInternal(pipeline.commands);
}

CommandResult CommandExecutor::executeCommand(const ParsedCommand& command) {
    if (command.program.empty()) {
        return CommandResult(0);
    }
    
    if (isBuiltin(command.program)) {
        return executeBuiltin(command);
    } else {
        return executeExternal(command);
    }
}

void CommandExecutor::registerBuiltin(const std::string& name, BuiltinHandler handler) {
    builtins_[name] = handler;
}

bool CommandExecutor::isBuiltin(const std::string& name) const {
    return builtins_.find(name) != builtins_.end();
}

CommandResult CommandExecutor::executeBuiltin(const ParsedCommand& command) {
    auto it = builtins_.find(command.program);
    if (it != builtins_.end()) {
        return it->second(command);
    }
    return CommandResult(1, "", "Internal error: builtin not found");
}

CommandResult CommandExecutor::executeExternal(const ParsedCommand& command) {
    // 查找可执行文件
    std::string executable = Environment::getInstance().findExecutable(command.program);
    if (executable.empty()) {
        return CommandResult(127, "", command.program + ": command not found");
    }
    
    // 准备参数
    std::vector<char*> argv;
    argv.push_back(const_cast<char*>(command.program.c_str()));
    for (const auto& arg : command.args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);
    
    // 保存文件描述符
    int stdin_backup = dup(STDIN_FILENO);
    int stdout_backup = dup(STDOUT_FILENO);
    int stderr_backup = dup(STDERR_FILENO);
    
    // 设置重定向
    setupRedirections(command, stdin_backup, stdout_backup, stderr_backup);
    
    pid_t pid = fork();
    if (pid == 0) {
        // 子进程
        execv(executable.c_str(), argv.data());
        perror("execv failed");
        _exit(127);
    } else if (pid > 0) {
        // 父进程
        restoreRedirections(stdin_backup, stdout_backup, stderr_backup);
        
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            return CommandResult(WEXITSTATUS(status));
        } else {
            return CommandResult(1, "", "Process terminated abnormally");
        }
    } else {
        // fork失败
        restoreRedirections(stdin_backup, stdout_backup, stderr_backup);
        return CommandResult(1, "", "Failed to fork process");
    }
}

CommandResult CommandExecutor::executePipelineInternal(const std::vector<ParsedCommand>& commands) {
    if (commands.empty()) {
        return CommandResult(0);
    }
    
    std::vector<int[2]> pipes(commands.size() - 1);
    std::vector<pid_t> pids;
    
    // 创建管道
    for (size_t i = 0; i < pipes.size(); ++i) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return CommandResult(1, "", "Failed to create pipe");
        }
    }
    
    // 执行每个命令
    for (size_t i = 0; i < commands.size(); ++i) {
        const auto& cmd = commands[i];
        
        if (isBuiltin(cmd.program)) {
            // 内置命令需要特殊处理
            // 简化处理：如果管道中有内置命令，直接执行
            if (commands.size() == 1) {
                return executeBuiltin(cmd);
            }
            continue;
        }
        
        std::string executable = Environment::getInstance().findExecutable(cmd.program);
        if (executable.empty()) {
            continue;
        }
        
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程
            
            // 设置管道
            if (i > 0) {
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if (i < commands.size() - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            // 关闭所有管道
            for (auto& p : pipes) {
                close(p[0]);
                close(p[1]);
            }
            
            // 设置重定向（仅对最后一个命令）
            if (i == commands.size() - 1) {
                int dummy1, dummy2, dummy3;
                setupRedirections(cmd, dummy1, dummy2, dummy3);
            }
            
            // 准备参数
            std::vector<char*> argv;
            argv.push_back(const_cast<char*>(cmd.program.c_str()));
            for (const auto& arg : cmd.args) {
                argv.push_back(const_cast<char*>(arg.c_str()));
            }
            argv.push_back(nullptr);
            
            execv(executable.c_str(), argv.data());
            perror("execv failed");
            _exit(127);
        } else if (pid > 0) {
            pids.push_back(pid);
        } else {
            perror("fork");
            return CommandResult(1, "", "Failed to fork process");
        }
    }
    
    // 关闭管道
    for (auto& p : pipes) {
        close(p[0]);
        close(p[1]);
    }
    
    // 等待所有进程
    int last_status = 0;
    for (pid_t pid : pids) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            last_status = WEXITSTATUS(status);
        }
    }
    
    return CommandResult(last_status);
}

void CommandExecutor::setupRedirections(const ParsedCommand& command, int& stdin_fd, int& stdout_fd, int& stderr_fd) {
    for (const auto& [type, target] : command.redirections) {
        switch (type) {
            case RedirectionType::Input: {
                int fd = open(target.c_str(), O_RDONLY);
                if (fd != -1) {
                    dup2(fd, STDIN_FILENO);
                    close(fd);
                }
                break;
            }
            case RedirectionType::Output: {
                int fd = open(target.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd != -1) {
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                break;
            }
            case RedirectionType::Append: {
                int fd = open(target.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd != -1) {
                    dup2(fd, STDOUT_FILENO);
                    close(fd);
                }
                break;
            }
            case RedirectionType::Error: {
                int fd = open(target.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd != -1) {
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                break;
            }
            case RedirectionType::ErrorAppend: {
                int fd = open(target.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
                if (fd != -1) {
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                break;
            }
        }
    }
}

void CommandExecutor::restoreRedirections(int stdin_fd, int stdout_fd, int stderr_fd) {
    dup2(stdin_fd, STDIN_FILENO);
    dup2(stdout_fd, STDOUT_FILENO);
    dup2(stderr_fd, STDERR_FILENO);
    close(stdin_fd);
    close(stdout_fd);
    close(stderr_fd);
}

} // namespace nex::shell
