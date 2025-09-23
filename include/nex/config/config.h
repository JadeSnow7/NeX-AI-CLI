#pragma once

#include <string>
#include <map>
#include <memory>
#include <filesystem>

namespace nex::config {

/**
 * @brief 配置键值对类型
 */
using ConfigMap = std::map<std::string, std::string>;
using ConfigSection = std::map<std::string, ConfigMap>;

/**
 * @brief 配置管理器
 */
class ConfigManager {
public:
    static ConfigManager& getInstance();
    
    /**
     * @brief 加载配置文件
     */
    bool loadConfig(const std::string& config_path = "");
    
    /**
     * @brief 保存配置文件
     */
    bool saveConfig(const std::string& config_path = "");
    
    /**
     * @brief 获取配置值
     */
    std::string getValue(const std::string& section, const std::string& key, const std::string& default_value = "") const;
    
    /**
     * @brief 设置配置值
     */
    void setValue(const std::string& section, const std::string& key, const std::string& value);
    
    /**
     * @brief 获取布尔值
     */
    bool getBool(const std::string& section, const std::string& key, bool default_value = false) const;
    
    /**
     * @brief 设置布尔值
     */
    void setBool(const std::string& section, const std::string& key, bool value);
    
    /**
     * @brief 获取整数值
     */
    int getInt(const std::string& section, const std::string& key, int default_value = 0) const;
    
    /**
     * @brief 设置整数值
     */
    void setInt(const std::string& section, const std::string& key, int value);
    
    /**
     * @brief 重置为默认配置
     */
    void resetToDefaults();
    
    /**
     * @brief 获取配置文件路径
     */
    std::string getConfigPath() const;

private:
    ConfigManager() = default;
    
    ConfigSection config_;
    std::string config_path_;
    
    void loadDefaults();
    std::string getDefaultConfigPath() const;
    bool parseConfigFile(const std::string& content);
    std::string generateConfigFile() const;
};

/**
 * @brief Shell配置
 */
class ShellConfig {
public:
    struct PromptSettings {
        bool show_time = true;
        bool show_git_info = true;
        bool show_system_status = false;
        bool use_colors = true;
        std::string format = "\\u@\\h \\w$ ";
        std::string time_format = "%H:%M:%S";
    };
    
    struct HighlightSettings {
        bool enabled = true;
        std::string command_color = "bold_blue";
        std::string path_color = "yellow";
        std::string flag_color = "cyan";
        std::string string_color = "green";
        std::string number_color = "magenta";
    };
    
    struct CompletionSettings {
        bool enabled = true;
        bool case_sensitive = false;
        int max_suggestions = 20;
        bool show_descriptions = true;
    };
    
    struct HistorySettings {
        bool enabled = true;
        int max_entries = 1000;
        bool ignore_duplicates = true;
        bool ignore_space_prefix = true;
        std::string history_file = "~/.nex_history";
    };
    
    static ShellConfig& getInstance();
    
    void loadFromConfig();
    void saveToConfig();
    
    PromptSettings prompt;
    HighlightSettings highlight;
    CompletionSettings completion;
    HistorySettings history;

private:
    ShellConfig() = default;
};

/**
 * @brief 系统配置
 */
class SystemConfig {
public:
    struct MonitoringSettings {
        bool enabled = false;
        int update_interval = 5; // 秒
        bool show_cpu = true;
        bool show_memory = true;
        bool show_load = true;
        bool show_processes = false;
    };
    
    struct LogSettings {
        bool enabled = true;
        std::string level = "info"; // debug, info, warn, error
        std::string file = "~/.nex.log";
        int max_file_size = 10; // MB
        int max_backup_files = 5;
    };
    
    static SystemConfig& getInstance();
    
    void loadFromConfig();
    void saveToConfig();
    
    MonitoringSettings monitoring;
    LogSettings logging;

private:
    SystemConfig() = default;
};

} // namespace nex::config
