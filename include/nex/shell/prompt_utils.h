#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <vector>

namespace nex::shell {

/**
 * @brief Git仓库状态
 */
struct GitStatus {
    std::string branch;
    bool is_dirty = false;
    int ahead = 0;
    int behind = 0;
    int staged = 0;
    int unstaged = 0;
    int untracked = 0;
    
    bool isValid() const { return !branch.empty(); }
};

/**
 * @brief 系统状态信息
 */
struct SystemStatus {
    double cpu_usage = 0.0;
    double memory_usage = 0.0;
    std::string load_average;
    int running_processes = 0;
};

/**
 * @brief ANSI颜色代码
 */
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    
    // 基本颜色
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    
    // 明亮颜色
    const std::string BRIGHT_BLACK = "\033[90m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BRIGHT_WHITE = "\033[97m";
    
    // 背景颜色
    const std::string BG_BLACK = "\033[40m";
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
    const std::string BG_WHITE = "\033[47m";
}

/**
 * @brief 提示符工具类
 */
class PromptBuilder {
public:
    PromptBuilder();
    
    /**
     * @brief 构建丰富的提示符
     */
    std::string buildPrompt() const;
    
    /**
     * @brief 设置提示符格式
     */
    void setFormat(const std::string& format);
    
    /**
     * @brief 启用/禁用Git信息显示
     */
    void setGitEnabled(bool enabled) { git_enabled_ = enabled; }
    
    /**
     * @brief 启用/禁用系统状态显示
     */
    void setSystemStatusEnabled(bool enabled) { system_status_enabled_ = enabled; }
    
    /**
     * @brief 启用/禁用时间显示
     */
    void setTimeEnabled(bool enabled) { time_enabled_ = enabled; }
    
    /**
     * @brief 设置是否使用颜色
     */
    void setColorEnabled(bool enabled) { color_enabled_ = enabled; }

private:
    std::string format_;
    bool git_enabled_;
    bool system_status_enabled_;
    bool time_enabled_;
    bool color_enabled_;
    
    std::optional<GitStatus> getGitStatus() const;
    SystemStatus getSystemStatus() const;
    std::string getCurrentTime() const;
    std::string expandPromptVariables(const std::string& format) const;
    std::string colorize(const std::string& text, const std::string& color) const;
};

/**
 * @brief Git工具类
 */
class GitUtils {
public:
    /**
     * @brief 检查当前目录是否在Git仓库中
     */
    static bool isGitRepository();
    
    /**
     * @brief 获取当前分支名
     */
    static std::string getCurrentBranch();
    
    /**
     * @brief 获取Git状态
     */
    static GitStatus getStatus();
    
    /**
     * @brief 获取仓库根目录
     */
    static std::string getRepositoryRoot();

private:
    static std::string executeGitCommand(const std::string& command);
};

/**
 * @brief 系统监控工具类
 */
class SystemMonitor {
public:
    /**
     * @brief 获取CPU使用率
     */
    static double getCpuUsage();
    
    /**
     * @brief 获取内存使用率
     */
    static double getMemoryUsage();
    
    /**
     * @brief 获取系统负载
     */
    static std::string getLoadAverage();
    
    /**
     * @brief 获取运行进程数
     */
    static int getRunningProcesses();
    
    /**
     * @brief 获取完整系统状态
     */
    static SystemStatus getSystemStatus();
};

/**
 * @brief 语法高亮器
 */
class SyntaxHighlighter {
public:
    /**
     * @brief 对命令行进行语法高亮
     */
    static std::string highlight(const std::string& line);
    
    /**
     * @brief 启用/禁用语法高亮
     */
    static void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /**
     * @brief 检查是否启用
     */
    static bool isEnabled() { return enabled_; }

private:
    static bool enabled_;
    
    static bool isCommand(const std::string& word);
    static bool isPath(const std::string& word);
    static bool isFlag(const std::string& word);
    static bool isString(const std::string& word);
    static bool isNumber(const std::string& word);
};

/**
 * @brief 自动补全器
 */
class AutoCompleter {
public:
    /**
     * @brief 获取补全建议
     */
    static std::vector<std::string> getCompletions(const std::string& line, int cursor_pos);
    
    /**
     * @brief 补全命令
     */
    static std::vector<std::string> completeCommand(const std::string& partial);
    
    /**
     * @brief 补全文件路径
     */
    static std::vector<std::string> completePath(const std::string& partial);
    
    /**
     * @brief 补全选项
     */
    static std::vector<std::string> completeOption(const std::string& command, const std::string& partial);

private:
    static std::vector<std::string> getAvailableCommands();
    static std::vector<std::string> getFilesInDirectory(const std::string& dir, const std::string& prefix = "");
};

} // namespace nex::shell
