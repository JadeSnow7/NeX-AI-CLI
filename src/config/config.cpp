#include "nex/config/config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace nex::config {

// ConfigManager实现
ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

bool ConfigManager::loadConfig(const std::string& config_path) {
    config_path_ = config_path.empty() ? getDefaultConfigPath() : config_path;
    
    if (!std::filesystem::exists(config_path_)) {
        loadDefaults();
        return saveConfig();  // 创建默认配置文件
    }
    
    std::ifstream file(config_path_);
    if (!file.is_open()) {
        loadDefaults();
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    if (!parseConfigFile(content)) {
        loadDefaults();
        return false;
    }
    
    return true;
}

bool ConfigManager::saveConfig(const std::string& config_path) {
    std::string path = config_path.empty() ? config_path_ : config_path;
    
    // 确保目录存在
    std::filesystem::path config_file(path);
    std::filesystem::create_directories(config_file.parent_path());
    
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateConfigFile();
    return true;
}

std::string ConfigManager::getValue(const std::string& section, const std::string& key, const std::string& default_value) const {
    auto section_it = config_.find(section);
    if (section_it == config_.end()) {
        return default_value;
    }
    
    auto key_it = section_it->second.find(key);
    if (key_it == section_it->second.end()) {
        return default_value;
    }
    
    return key_it->second;
}

void ConfigManager::setValue(const std::string& section, const std::string& key, const std::string& value) {
    config_[section][key] = value;
}

bool ConfigManager::getBool(const std::string& section, const std::string& key, bool default_value) const {
    std::string value = getValue(section, key);
    if (value.empty()) {
        return default_value;
    }
    
    std::transform(value.begin(), value.end(), value.begin(), ::tolower);
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

void ConfigManager::setBool(const std::string& section, const std::string& key, bool value) {
    setValue(section, key, value ? "true" : "false");
}

int ConfigManager::getInt(const std::string& section, const std::string& key, int default_value) const {
    std::string value = getValue(section, key);
    if (value.empty()) {
        return default_value;
    }
    
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return default_value;
    }
}

void ConfigManager::setInt(const std::string& section, const std::string& key, int value) {
    setValue(section, key, std::to_string(value));
}

void ConfigManager::resetToDefaults() {
    config_.clear();
    loadDefaults();
}

std::string ConfigManager::getConfigPath() const {
    return config_path_;
}

void ConfigManager::loadDefaults() {
    // Shell配置默认值
    setValue("shell.prompt", "show_time", "true");
    setValue("shell.prompt", "show_git_info", "true");
    setValue("shell.prompt", "show_system_status", "false");
    setValue("shell.prompt", "use_colors", "true");
    setValue("shell.prompt", "format", "\\u@\\h \\w$ ");
    setValue("shell.prompt", "time_format", "%H:%M:%S");
    
    setValue("shell.highlight", "enabled", "true");
    setValue("shell.highlight", "command_color", "bold_blue");
    setValue("shell.highlight", "path_color", "yellow");
    setValue("shell.highlight", "flag_color", "cyan");
    setValue("shell.highlight", "string_color", "green");
    setValue("shell.highlight", "number_color", "magenta");
    
    setValue("shell.completion", "enabled", "true");
    setValue("shell.completion", "case_sensitive", "false");
    setValue("shell.completion", "max_suggestions", "20");
    setValue("shell.completion", "show_descriptions", "true");
    
    setValue("shell.history", "enabled", "true");
    setValue("shell.history", "max_entries", "1000");
    setValue("shell.history", "ignore_duplicates", "true");
    setValue("shell.history", "ignore_space_prefix", "true");
    setValue("shell.history", "history_file", "~/.nex_history");
    
    // 系统配置默认值
    setValue("system.monitoring", "enabled", "false");
    setValue("system.monitoring", "update_interval", "5");
    setValue("system.monitoring", "show_cpu", "true");
    setValue("system.monitoring", "show_memory", "true");
    setValue("system.monitoring", "show_load", "true");
    setValue("system.monitoring", "show_processes", "false");
    
    setValue("system.logging", "enabled", "true");
    setValue("system.logging", "level", "info");
    setValue("system.logging", "file", "~/.nex.log");
    setValue("system.logging", "max_file_size", "10");
    setValue("system.logging", "max_backup_files", "5");
}

std::string ConfigManager::getDefaultConfigPath() const {
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/.nex/config.ini";
    }
    return ".nex_config.ini";
}

bool ConfigManager::parseConfigFile(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    std::string current_section;
    
    while (std::getline(stream, line)) {
        // 去除首尾空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // 跳过空行和注释
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        // 处理节
        if (line[0] == '[' && line.back() == ']') {
            current_section = line.substr(1, line.length() - 2);
            continue;
        }
        
        // 处理键值对
        size_t equals_pos = line.find('=');
        if (equals_pos != std::string::npos && !current_section.empty()) {
            std::string key = line.substr(0, equals_pos);
            std::string value = line.substr(equals_pos + 1);
            
            // 去除键值的空白
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            // 去除值的引号
            if ((value.front() == '"' && value.back() == '"') ||
                (value.front() == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.length() - 2);
            }
            
            setValue(current_section, key, value);
        }
    }
    
    return true;
}

std::string ConfigManager::generateConfigFile() const {
    std::ostringstream oss;
    
    oss << "# NeX: AI-CLI Configuration File\n";
    oss << "# Generated automatically - You can edit this file\n\n";
    
    for (const auto& section_pair : config_) {
        oss << "[" << section_pair.first << "]\n";
        
        for (const auto& key_pair : section_pair.second) {
            oss << key_pair.first << " = " << key_pair.second << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

// ShellConfig实现
ShellConfig& ShellConfig::getInstance() {
    static ShellConfig instance;
    return instance;
}

void ShellConfig::loadFromConfig() {
    auto& config = ConfigManager::getInstance();
    
    // 加载提示符设置
    prompt.show_time = config.getBool("shell.prompt", "show_time", true);
    prompt.show_git_info = config.getBool("shell.prompt", "show_git_info", true);
    prompt.show_system_status = config.getBool("shell.prompt", "show_system_status", false);
    prompt.use_colors = config.getBool("shell.prompt", "use_colors", true);
    prompt.format = config.getValue("shell.prompt", "format", "\\u@\\h \\w$ ");
    prompt.time_format = config.getValue("shell.prompt", "time_format", "%H:%M:%S");
    
    // 加载语法高亮设置
    highlight.enabled = config.getBool("shell.highlight", "enabled", true);
    highlight.command_color = config.getValue("shell.highlight", "command_color", "bold_blue");
    highlight.path_color = config.getValue("shell.highlight", "path_color", "yellow");
    highlight.flag_color = config.getValue("shell.highlight", "flag_color", "cyan");
    highlight.string_color = config.getValue("shell.highlight", "string_color", "green");
    highlight.number_color = config.getValue("shell.highlight", "number_color", "magenta");
    
    // 加载补全设置
    completion.enabled = config.getBool("shell.completion", "enabled", true);
    completion.case_sensitive = config.getBool("shell.completion", "case_sensitive", false);
    completion.max_suggestions = config.getInt("shell.completion", "max_suggestions", 20);
    completion.show_descriptions = config.getBool("shell.completion", "show_descriptions", true);
    
    // 加载历史设置
    history.enabled = config.getBool("shell.history", "enabled", true);
    history.max_entries = config.getInt("shell.history", "max_entries", 1000);
    history.ignore_duplicates = config.getBool("shell.history", "ignore_duplicates", true);
    history.ignore_space_prefix = config.getBool("shell.history", "ignore_space_prefix", true);
    history.history_file = config.getValue("shell.history", "history_file", "~/.nex_history");
}

void ShellConfig::saveToConfig() {
    auto& config = ConfigManager::getInstance();
    
    // 保存提示符设置
    config.setBool("shell.prompt", "show_time", prompt.show_time);
    config.setBool("shell.prompt", "show_git_info", prompt.show_git_info);
    config.setBool("shell.prompt", "show_system_status", prompt.show_system_status);
    config.setBool("shell.prompt", "use_colors", prompt.use_colors);
    config.setValue("shell.prompt", "format", prompt.format);
    config.setValue("shell.prompt", "time_format", prompt.time_format);
    
    // 保存语法高亮设置
    config.setBool("shell.highlight", "enabled", highlight.enabled);
    config.setValue("shell.highlight", "command_color", highlight.command_color);
    config.setValue("shell.highlight", "path_color", highlight.path_color);
    config.setValue("shell.highlight", "flag_color", highlight.flag_color);
    config.setValue("shell.highlight", "string_color", highlight.string_color);
    config.setValue("shell.highlight", "number_color", highlight.number_color);
    
    // 保存补全设置
    config.setBool("shell.completion", "enabled", completion.enabled);
    config.setBool("shell.completion", "case_sensitive", completion.case_sensitive);
    config.setInt("shell.completion", "max_suggestions", completion.max_suggestions);
    config.setBool("shell.completion", "show_descriptions", completion.show_descriptions);
    
    // 保存历史设置
    config.setBool("shell.history", "enabled", history.enabled);
    config.setInt("shell.history", "max_entries", history.max_entries);
    config.setBool("shell.history", "ignore_duplicates", history.ignore_duplicates);
    config.setBool("shell.history", "ignore_space_prefix", history.ignore_space_prefix);
    config.setValue("shell.history", "history_file", history.history_file);
    
    config.saveConfig();
}

// SystemConfig实现
SystemConfig& SystemConfig::getInstance() {
    static SystemConfig instance;
    return instance;
}

void SystemConfig::loadFromConfig() {
    auto& config = ConfigManager::getInstance();
    
    // 加载监控设置
    monitoring.enabled = config.getBool("system.monitoring", "enabled", false);
    monitoring.update_interval = config.getInt("system.monitoring", "update_interval", 5);
    monitoring.show_cpu = config.getBool("system.monitoring", "show_cpu", true);
    monitoring.show_memory = config.getBool("system.monitoring", "show_memory", true);
    monitoring.show_load = config.getBool("system.monitoring", "show_load", true);
    monitoring.show_processes = config.getBool("system.monitoring", "show_processes", false);
    
    // 加载日志设置
    logging.enabled = config.getBool("system.logging", "enabled", true);
    logging.level = config.getValue("system.logging", "level", "info");
    logging.file = config.getValue("system.logging", "file", "~/.nex.log");
    logging.max_file_size = config.getInt("system.logging", "max_file_size", 10);
    logging.max_backup_files = config.getInt("system.logging", "max_backup_files", 5);
}

void SystemConfig::saveToConfig() {
    auto& config = ConfigManager::getInstance();
    
    // 保存监控设置
    config.setBool("system.monitoring", "enabled", monitoring.enabled);
    config.setInt("system.monitoring", "update_interval", monitoring.update_interval);
    config.setBool("system.monitoring", "show_cpu", monitoring.show_cpu);
    config.setBool("system.monitoring", "show_memory", monitoring.show_memory);
    config.setBool("system.monitoring", "show_load", monitoring.show_load);
    config.setBool("system.monitoring", "show_processes", monitoring.show_processes);
    
    // 保存日志设置
    config.setBool("system.logging", "enabled", logging.enabled);
    config.setValue("system.logging", "level", logging.level);
    config.setValue("system.logging", "file", logging.file);
    config.setInt("system.logging", "max_file_size", logging.max_file_size);
    config.setInt("system.logging", "max_backup_files", logging.max_backup_files);
    
    config.saveConfig();
}

} // namespace nex::config
