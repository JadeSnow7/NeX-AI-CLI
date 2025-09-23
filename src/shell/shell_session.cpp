#include "nex/shell/shell.h"
#include "nex/shell/prompt_utils.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

namespace nex::shell {

ShellSession::ShellSession() 
    : parser_(std::make_unique<CommandParser>())
    , executor_(std::make_unique<CommandExecutor>())
    , prompt_("nex$ ")
    , history_enabled_(true)
    , syntax_highlight_enabled_(true)
    , git_info_enabled_(true)
    , system_status_enabled_(false) {
    
    setupBuiltinCommands();
}

void ShellSession::runInteractive() {
    std::cout << "NeX Shell v1.0.0 - Type 'help' for available commands" << std::endl;
    
    char* input;
    while ((input = readline(getPrompt().c_str())) != nullptr) {
        std::string line(input);
        free(input);
        
        // 跳过空行
        if (line.empty()) {
            continue;
        }
        
        // 添加到历史
        if (history_enabled_) {
            add_history(line.c_str());
            addToHistory(line);
        }
        
        // 执行命令
        CommandResult result = executeLine(line);
        printResult(result);
        
        // 检查退出
        if (line == "exit" || line == "quit") {
            break;
        }
    }
    
    std::cout << "Goodbye!" << std::endl;
}

CommandResult ShellSession::runScript(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return CommandResult(1, "", "Cannot open script file: " + filename);
    }
    
    std::string line;
    int line_number = 0;
    CommandResult last_result(0);
    
    while (std::getline(file, line)) {
        ++line_number;
        
        // 跳过空行和注释
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::cout << "[" << line_number << "] " << line << std::endl;
        
        CommandResult result = executeLine(line);
        if (!result.success) {
            std::cerr << "Error at line " << line_number << ": " << result.error << std::endl;
            return result;
        }
        
        last_result = result;
    }
    
    return last_result;
}

CommandResult ShellSession::executeLine(const std::string& line) {
    try {
        Pipeline pipeline = parser_->parseLine(line);
        
        if (pipeline.commands.empty()) {
            return CommandResult(0);
        }
        
        return executor_->executePipeline(pipeline);
    } catch (const std::exception& e) {
        return CommandResult(1, "", "Shell error: " + std::string(e.what()));
    }
}

void ShellSession::setPrompt(const std::string& prompt) {
    prompt_ = prompt;
}

void ShellSession::setHistoryEnabled(bool enabled) {
    history_enabled_ = enabled;
}

void ShellSession::setSyntaxHighlightEnabled(bool enabled) {
    syntax_highlight_enabled_ = enabled;
}

void ShellSession::setGitInfoEnabled(bool enabled) {
    git_info_enabled_ = enabled;
}

void ShellSession::setSystemStatusEnabled(bool enabled) {
    system_status_enabled_ = enabled;
}

void ShellSession::setupBuiltinCommands() {
    // 添加shell特定的内置命令
    executor_->registerBuiltin("help", [](const ParsedCommand& cmd) -> CommandResult {
        std::string output = R"(NeX Shell - Available Commands:

Built-in Commands:
  help          - Show this help message
  cd [path]     - Change directory
  pwd           - Print working directory
  echo [text]   - Print text to stdout
  export VAR=value - Set environment variable
  env           - Show environment variables
  exit [code]   - Exit shell
  history       - Show command history
  clear         - Clear screen

Shell Features:
  - Command piping: cmd1 | cmd2
  - I/O redirection: cmd > file, cmd < file, cmd >> file
  - Background execution: cmd &
  - Environment variables: $VAR, ${VAR}
  - Command history with arrow keys
  - Tab completion

Examples:
  ls -la | grep txt
  echo "Hello World" > output.txt
  cat input.txt | sort | uniq
  export PATH=$PATH:/new/path
  find . -name "*.cpp" | wc -l

)";
        return CommandResult(0, output);
    });
    
    executor_->registerBuiltin("history", [this](const ParsedCommand& cmd) -> CommandResult {
        std::string output;
        for (size_t i = 0; i < history_.size(); ++i) {
            output += std::to_string(i + 1) + "  " + history_[i] + "\n";
        }
        return CommandResult(0, output);
    });
    
    executor_->registerBuiltin("clear", [](const ParsedCommand& cmd) -> CommandResult {
        std::cout << "\033[2J\033[H";
        return CommandResult(0);
    });
}

std::string ShellSession::getPrompt() const {
    std::string result = prompt_;
    
    // 替换特殊标记
    std::string cwd = Environment::getInstance().getCurrentDirectory();
    std::string home = Environment::getInstance().getVariable("HOME");
    
    // 将HOME路径替换为~
    if (!home.empty() && cwd.find(home) == 0) {
        cwd = "~" + cwd.substr(home.length());
    }
    
    // 替换提示符中的变量
    size_t pos = result.find("\\w");
    if (pos != std::string::npos) {
        result.replace(pos, 2, cwd);
    }
    
    pos = result.find("\\u");
    if (pos != std::string::npos) {
        result.replace(pos, 2, Environment::getInstance().getVariable("USER"));
    }
    
    pos = result.find("\\h");
    if (pos != std::string::npos) {
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            result.replace(pos, 2, hostname);
        }
    }
    
    return result;
}

void ShellSession::addToHistory(const std::string& line) {
    history_.push_back(line);
    
    // 限制历史记录大小
    const size_t MAX_HISTORY = 1000;
    if (history_.size() > MAX_HISTORY) {
        history_.erase(history_.begin());
    }
}

void ShellSession::printResult(const CommandResult& result) {
    if (!result.output.empty()) {
        std::cout << result.output;
    }
    
    if (!result.error.empty()) {
        std::cerr << result.error << std::endl;
    }
    
    // 可选：显示退出码（调试模式）
    if (!result.success && result.exit_code != 0) {
        // std::cerr << "[Exit code: " << result.exit_code << "]" << std::endl;
    }
}

} // namespace nex::shell
