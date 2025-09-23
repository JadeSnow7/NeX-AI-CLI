#include "nex/shell/prompt_utils.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <algorithm>
#include <regex>
#include <optional>
#include <cstring>

namespace nex::shell {

// PromptBuilder实现
PromptBuilder::PromptBuilder() 
    : format_("\\u@\\h \\w$ ")
    , git_enabled_(true)
    , system_status_enabled_(false)
    , time_enabled_(true)
    , color_enabled_(true) {
}

std::string PromptBuilder::buildPrompt() const {
    if (!color_enabled_) {
        return expandPromptVariables(format_);
    }
    
    std::string prompt;
    
    // 时间信息
    if (time_enabled_) {
        prompt += colorize("[" + getCurrentTime() + "]", Colors::DIM + Colors::CYAN);
        prompt += " ";
    }
    
    // 用户和主机名
    char* username = getenv("USER");
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    
    if (username) {
        if (strcmp(username, "root") == 0) {
            prompt += colorize(username, Colors::BOLD + Colors::RED);
        } else {
            prompt += colorize(username, Colors::BOLD + Colors::GREEN);
        }
        prompt += colorize("@", Colors::DIM);
        prompt += colorize(hostname, Colors::BOLD + Colors::BLUE);
        prompt += " ";
    }
    
    // 当前目录
    std::string cwd = std::filesystem::current_path();
    char* home = getenv("HOME");
    if (home && cwd.find(home) == 0) {
        cwd = "~" + cwd.substr(strlen(home));
    }
    prompt += colorize(cwd, Colors::BOLD + Colors::YELLOW);
    
    // Git信息
    if (git_enabled_) {
        auto git_status = getGitStatus();
        if (git_status.has_value()) {
            const auto& git = git_status.value();
            
            prompt += " ";
            prompt += colorize("(", Colors::DIM);
            
            // 分支名
            std::string branch_color = Colors::BRIGHT_MAGENTA;
            if (git.is_dirty) {
                branch_color = Colors::BRIGHT_RED;
            }
            prompt += colorize(git.branch, branch_color);
            
            // 状态指示
            if (git.staged > 0) {
                prompt += colorize("●", Colors::GREEN);  // 已暂存
            }
            if (git.unstaged > 0) {
                prompt += colorize("●", Colors::RED);    // 未暂存
            }
            if (git.untracked > 0) {
                prompt += colorize("●", Colors::YELLOW); // 未跟踪
            }
            
            // 远程状态
            if (git.ahead > 0) {
                prompt += colorize("↑" + std::to_string(git.ahead), Colors::CYAN);
            }
            if (git.behind > 0) {
                prompt += colorize("↓" + std::to_string(git.behind), Colors::CYAN);
            }
            
            prompt += colorize(")", Colors::DIM);
        }
    }
    
    // 系统状态
    if (system_status_enabled_) {
        auto sys_status = getSystemStatus();
        prompt += " ";
        prompt += colorize("[", Colors::DIM);
        
        // CPU使用率
        std::string cpu_color = Colors::GREEN;
        if (sys_status.cpu_usage > 80.0) cpu_color = Colors::RED;
        else if (sys_status.cpu_usage > 60.0) cpu_color = Colors::YELLOW;
        
        prompt += colorize("CPU:" + std::to_string((int)sys_status.cpu_usage) + "%", cpu_color);
        
        // 内存使用率
        std::string mem_color = Colors::GREEN;
        if (sys_status.memory_usage > 80.0) mem_color = Colors::RED;
        else if (sys_status.memory_usage > 60.0) mem_color = Colors::YELLOW;
        
        prompt += " ";
        prompt += colorize("MEM:" + std::to_string((int)sys_status.memory_usage) + "%", mem_color);
        
        prompt += colorize("]", Colors::DIM);
    }
    
    // 提示符结尾
    prompt += "\n";
    prompt += colorize("➤ ", Colors::BOLD + Colors::BRIGHT_GREEN);
    
    return prompt;
}

void PromptBuilder::setFormat(const std::string& format) {
    format_ = format;
}

std::optional<GitStatus> PromptBuilder::getGitStatus() const {
    return GitUtils::isGitRepository() ? 
           std::make_optional(GitUtils::getStatus()) : 
           std::nullopt;
}

SystemStatus PromptBuilder::getSystemStatus() const {
    return SystemMonitor::getSystemStatus();
}

std::string PromptBuilder::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
    return ss.str();
}

std::string PromptBuilder::expandPromptVariables(const std::string& format) const {
    std::string result = format;
    
    // 替换基本变量
    char* username = getenv("USER");
    if (username) {
        size_t pos = result.find("\\u");
        while (pos != std::string::npos) {
            result.replace(pos, 2, username);
            pos = result.find("\\u", pos + strlen(username));
        }
    }
    
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        size_t pos = result.find("\\h");
        while (pos != std::string::npos) {
            result.replace(pos, 2, hostname);
            pos = result.find("\\h", pos + strlen(hostname));
        }
    }
    
    std::string cwd = std::filesystem::current_path();
    size_t pos = result.find("\\w");
    while (pos != std::string::npos) {
        result.replace(pos, 2, cwd);
        pos = result.find("\\w", pos + cwd.length());
    }
    
    return result;
}

std::string PromptBuilder::colorize(const std::string& text, const std::string& color) const {
    if (!color_enabled_) {
        return text;
    }
    return color + text + Colors::RESET;
}

// GitUtils实现
bool GitUtils::isGitRepository() {
    std::string cmd = "git rev-parse --is-inside-work-tree 2>/dev/null";
    std::string result = executeGitCommand(cmd);
    return result.find("true") != std::string::npos;
}

std::string GitUtils::getCurrentBranch() {
    std::string cmd = "git branch --show-current 2>/dev/null";
    std::string result = executeGitCommand(cmd);
    
    // 去除换行符
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result.empty() ? "HEAD" : result;
}

GitStatus GitUtils::getStatus() {
    GitStatus status;
    
    // 获取分支名
    status.branch = getCurrentBranch();
    
    // 获取状态信息
    std::string cmd = "git status --porcelain 2>/dev/null";
    std::string result = executeGitCommand(cmd);
    
    // 解析状态
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.length() >= 2) {
            char staged_char = line[0];
            char unstaged_char = line[1];
            
            if (staged_char != ' ' && staged_char != '?') {
                status.staged++;
            }
            if (unstaged_char != ' ') {
                if (unstaged_char == '?') {
                    status.untracked++;
                } else {
                    status.unstaged++;
                }
            }
        }
    }
    
    status.is_dirty = (status.staged > 0 || status.unstaged > 0 || status.untracked > 0);
    
    // 获取远程状态
    cmd = "git rev-list --count --left-right @{upstream}...HEAD 2>/dev/null";
    result = executeGitCommand(cmd);
    if (!result.empty()) {
        std::istringstream iss(result);
        iss >> status.behind >> status.ahead;
    }
    
    return status;
}

std::string GitUtils::getRepositoryRoot() {
    std::string cmd = "git rev-parse --show-toplevel 2>/dev/null";
    std::string result = executeGitCommand(cmd);
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

std::string GitUtils::executeGitCommand(const std::string& command) {
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    std::string result;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    
    pclose(pipe);
    return result;
}

// SystemMonitor实现
double SystemMonitor::getCpuUsage() {
    static unsigned long long last_idle = 0, last_total = 0;
    
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return 0.0;
    }
    
    std::string line;
    std::getline(file, line);
    
    std::istringstream iss(line);
    std::string cpu;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    
    iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    
    unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
    unsigned long long total_diff = total - last_total;
    unsigned long long idle_diff = idle - last_idle;
    
    double usage = 0.0;
    if (total_diff > 0) {
        usage = 100.0 * (total_diff - idle_diff) / total_diff;
    }
    
    last_total = total;
    last_idle = idle;
    
    return usage;
}

double SystemMonitor::getMemoryUsage() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) {
        return 0.0;
    }
    
    unsigned long long total = 0, available = 0;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") == 0) {
            std::istringstream iss(line);
            std::string key, value;
            iss >> key >> total;
        } else if (line.find("MemAvailable:") == 0) {
            std::istringstream iss(line);
            std::string key, value;
            iss >> key >> available;
            break;
        }
    }
    
    if (total > 0) {
        return 100.0 * (total - available) / total;
    }
    
    return 0.0;
}

std::string SystemMonitor::getLoadAverage() {
    std::ifstream file("/proc/loadavg");
    if (!file.is_open()) {
        return "0.00";
    }
    
    std::string load1, load5, load15;
    file >> load1 >> load5 >> load15;
    
    return load1;
}

int SystemMonitor::getRunningProcesses() {
    int count = 0;
    DIR* proc_dir = opendir("/proc");
    
    if (proc_dir) {
        struct dirent* entry;
        while ((entry = readdir(proc_dir)) != nullptr) {
            if (entry->d_type == DT_DIR && std::isdigit(entry->d_name[0])) {
                count++;
            }
        }
        closedir(proc_dir);
    }
    
    return count;
}

SystemStatus SystemMonitor::getSystemStatus() {
    SystemStatus status;
    status.cpu_usage = getCpuUsage();
    status.memory_usage = getMemoryUsage();
    status.load_average = getLoadAverage();
    status.running_processes = getRunningProcesses();
    return status;
}

} // namespace nex::shell
