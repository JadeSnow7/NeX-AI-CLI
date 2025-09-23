#include "nex/shell/prompt_utils.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <vector>
#include <string>

namespace nex::shell {

// SyntaxHighlighter实现
bool SyntaxHighlighter::enabled_ = true;

std::string SyntaxHighlighter::highlight(const std::string& line) {
    if (!enabled_) {
        return line;
    }
    
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    
    // 简单的分词
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return line;
    }
    
    std::string result;
    
    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& word = tokens[i];
        
        if (i == 0 && isCommand(word)) {
            // 命令高亮
            result += Colors::BOLD + Colors::BLUE + word + Colors::RESET;
        } else if (isFlag(word)) {
            // 选项高亮
            result += Colors::CYAN + word + Colors::RESET;
        } else if (isString(word)) {
            // 字符串高亮
            result += Colors::GREEN + word + Colors::RESET;
        } else if (isNumber(word)) {
            // 数字高亮
            result += Colors::MAGENTA + word + Colors::RESET;
        } else if (isPath(word)) {
            // 路径高亮
            result += Colors::YELLOW + word + Colors::RESET;
        } else {
            // 普通文本
            result += word;
        }
        
        if (i < tokens.size() - 1) {
            result += " ";
        }
    }
    
    return result;
}

bool SyntaxHighlighter::isCommand(const std::string& word) {
    // 检查是否为常见命令
    static const std::vector<std::string> commands = {
        "ls", "cd", "pwd", "cat", "grep", "find", "awk", "sed", "sort",
        "uniq", "head", "tail", "wc", "cp", "mv", "rm", "mkdir", "rmdir",
        "chmod", "chown", "ps", "top", "kill", "killall", "jobs", "bg", "fg",
        "git", "make", "cmake", "gcc", "g++", "clang", "python", "node",
        "npm", "pip", "docker", "ssh", "scp", "rsync", "curl", "wget"
    };
    
    return std::find(commands.begin(), commands.end(), word) != commands.end();
}

bool SyntaxHighlighter::isPath(const std::string& word) {
    return word.find('/') != std::string::npos || 
           word.find('.') == 0 ||  // 相对路径
           word.find('~') == 0;    // home路径
}

bool SyntaxHighlighter::isFlag(const std::string& word) {
    return word.length() >= 2 && word[0] == '-';
}

bool SyntaxHighlighter::isString(const std::string& word) {
    return (word.length() >= 2 && 
            ((word.front() == '"' && word.back() == '"') ||
             (word.front() == '\'' && word.back() == '\'')));
}

bool SyntaxHighlighter::isNumber(const std::string& word) {
    if (word.empty()) return false;
    
    size_t start = 0;
    if (word[0] == '-' || word[0] == '+') start = 1;
    
    if (start >= word.length()) return false;
    
    bool has_dot = false;
    for (size_t i = start; i < word.length(); ++i) {
        if (word[i] == '.') {
            if (has_dot) return false;
            has_dot = true;
        } else if (!std::isdigit(word[i])) {
            return false;
        }
    }
    
    return true;
}

// AutoCompleter实现
std::vector<std::string> AutoCompleter::getCompletions(const std::string& line, int cursor_pos) {
    if (cursor_pos < 0 || cursor_pos > static_cast<int>(line.length())) {
        return {};
    }
    
    // 找到当前单词的开始位置
    int word_start = cursor_pos;
    while (word_start > 0 && line[word_start - 1] != ' ' && line[word_start - 1] != '\t') {
        word_start--;
    }
    
    std::string partial = line.substr(word_start, cursor_pos - word_start);
    
    // 判断是否为第一个单词（命令）
    bool is_first_word = true;
    for (int i = 0; i < word_start; ++i) {
        if (line[i] != ' ' && line[i] != '\t') {
            is_first_word = false;
            break;
        }
    }
    
    if (is_first_word) {
        return completeCommand(partial);
    } else {
        // 尝试路径补全
        return completePath(partial);
    }
}

std::vector<std::string> AutoCompleter::completeCommand(const std::string& partial) {
    std::vector<std::string> completions;
    auto commands = getAvailableCommands();
    
    for (const auto& cmd : commands) {
        if (cmd.find(partial) == 0) {  // 前缀匹配
            completions.push_back(cmd);
        }
    }
    
    return completions;
}

std::vector<std::string> AutoCompleter::completePath(const std::string& partial) {
    std::string dir_path = ".";
    std::string file_prefix = partial;
    
    size_t last_slash = partial.find_last_of('/');
    if (last_slash != std::string::npos) {
        dir_path = partial.substr(0, last_slash);
        file_prefix = partial.substr(last_slash + 1);
        
        if (dir_path.empty()) {
            dir_path = "/";
        }
    }
    
    // 处理 ~ 路径
    if (dir_path[0] == '~') {
        const char* home = getenv("HOME");
        if (home) {
            if (dir_path.length() == 1) {
                dir_path = home;
            } else {
                dir_path = std::string(home) + dir_path.substr(1);
            }
        }
    }
    
    return getFilesInDirectory(dir_path, file_prefix);
}

std::vector<std::string> AutoCompleter::completeOption(const std::string& command, const std::string& partial) {
    // 简化版本：只返回一些常见选项
    std::vector<std::string> options;
    
    if (command == "ls") {
        std::vector<std::string> ls_options = {"-l", "-a", "-la", "-h", "-t", "-r", "-S"};
        for (const auto& opt : ls_options) {
            if (opt.find(partial) == 0) {
                options.push_back(opt);
            }
        }
    } else if (command == "grep") {
        std::vector<std::string> grep_options = {"-i", "-r", "-n", "-v", "-c", "-l", "-H"};
        for (const auto& opt : grep_options) {
            if (opt.find(partial) == 0) {
                options.push_back(opt);
            }
        }
    }
    // 可以继续添加更多命令的选项...
    
    return options;
}

std::vector<std::string> AutoCompleter::getAvailableCommands() {
    static std::vector<std::string> cached_commands;
    
    if (cached_commands.empty()) {
        // 从PATH中获取可执行文件
        const char* path_env = getenv("PATH");
        if (path_env) {
            std::string path(path_env);
            std::istringstream iss(path);
            std::string dir;
            
            while (std::getline(iss, dir, ':')) {
                if (!dir.empty()) {
                    auto files = getFilesInDirectory(dir);
                    for (const auto& file : files) {
                        // 检查是否可执行
                        std::string full_path = dir + "/" + file;
                        struct stat st;
                        if (stat(full_path.c_str(), &st) == 0 && (st.st_mode & S_IXUSR)) {
                            cached_commands.push_back(file);
                        }
                    }
                }
            }
        }
        
        // 添加内置命令
        std::vector<std::string> builtins = {
            "cd", "pwd", "echo", "export", "env", "exit", "help", "history", "clear"
        };
        cached_commands.insert(cached_commands.end(), builtins.begin(), builtins.end());
        
        // 去重并排序
        std::sort(cached_commands.begin(), cached_commands.end());
        cached_commands.erase(
            std::unique(cached_commands.begin(), cached_commands.end()),
            cached_commands.end()
        );
    }
    
    return cached_commands;
}

std::vector<std::string> AutoCompleter::getFilesInDirectory(const std::string& dir, const std::string& prefix) {
    std::vector<std::string> files;
    
    try {
        if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
            for (const auto& entry : std::filesystem::directory_iterator(dir)) {
                std::string filename = entry.path().filename().string();
                
                if (prefix.empty() || filename.find(prefix) == 0) {
                    // 如果是目录，添加 / 后缀
                    if (entry.is_directory()) {
                        filename += "/";
                    }
                    files.push_back(filename);
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 忽略文件系统错误
    }
    
    std::sort(files.begin(), files.end());
    return files;
}

} // namespace nex::shell
