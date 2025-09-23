#pragma once

#include <string>
#include <memory>
#include <functional>
#include <any>
#include <map>
#include <vector>
#include <chrono>

namespace nex::core {

/**
 * @brief 应用程序状态枚举
 */
enum class AppState {
    Initializing,
    Running,
    Paused,
    Stopping,
    Stopped,
    Error
};

/**
 * @brief 应用程序配置管理器
 */
class IConfigManager {
public:
    virtual ~IConfigManager() = default;
    
    virtual bool loadConfig(const std::string& config_path) = 0;
    virtual bool saveConfig(const std::string& config_path = "") = 0;
    
    template<typename T>
    T getValue(const std::string& key, const T& default_value = T{}) const {
        auto value = getValueImpl(key);
        if (value.has_value()) {
            try {
                return std::any_cast<T>(value);
            } catch (const std::bad_any_cast&) {
                return default_value;
            }
        }
        return default_value;
    }
    
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        setValueImpl(key, std::any(value));
    }
    
    virtual std::vector<std::string> getKeys() const = 0;
    virtual bool hasKey(const std::string& key) const = 0;
    virtual void removeKey(const std::string& key) = 0;
    
protected:
    virtual std::any getValueImpl(const std::string& key) const = 0;
    virtual void setValueImpl(const std::string& key, const std::any& value) = 0;
};

/**
 * @brief 日志级别
 */
enum class LogLevel {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

/**
 * @brief 日志管理器接口
 */
class ILogger {
public:
    virtual ~ILogger() = default;
    
    virtual void log(LogLevel level, const std::string& message) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // 便捷方法
    void trace(const std::string& message) { log(LogLevel::Trace, message); }
    void debug(const std::string& message) { log(LogLevel::Debug, message); }
    void info(const std::string& message) { log(LogLevel::Info, message); }
    void warning(const std::string& message) { log(LogLevel::Warning, message); }
    void error(const std::string& message) { log(LogLevel::Error, message); }
    void critical(const std::string& message) { log(LogLevel::Critical, message); }
};

/**
 * @brief 事件类型
 */
enum class EventType {
    SystemResourceChange,
    ConfigChange,
    AppStateChange,
    UserCommand,
    AIAnalysisComplete,
    Custom
};

/**
 * @brief 事件数据
 */
struct Event {
    EventType type;
    std::string name;
    std::any data;
    std::chrono::system_clock::time_point timestamp;
    
    Event(EventType t, const std::string& n, std::any d = {})
        : type(t), name(n), data(std::move(d)), 
          timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @brief 事件处理器
 */
using EventHandler = std::function<void(const Event&)>;

/**
 * @brief 事件管理器接口
 */
class IEventManager {
public:
    virtual ~IEventManager() = default;
    
    virtual void subscribe(EventType type, const EventHandler& handler) = 0;
    virtual void subscribe(const std::string& event_name, const EventHandler& handler) = 0;
    virtual void unsubscribe(EventType type) = 0;
    virtual void unsubscribe(const std::string& event_name) = 0;
    
    virtual void publish(const Event& event) = 0;
    virtual void publish(EventType type, const std::string& name, std::any data = {}) = 0;
    
    virtual size_t getSubscriberCount(EventType type) const = 0;
    virtual size_t getSubscriberCount(const std::string& event_name) const = 0;
};

/**
 * @brief 应用程序上下文
 */
class ApplicationContext {
private:
    std::shared_ptr<IConfigManager> config_manager_;
    std::shared_ptr<ILogger> logger_;
    std::shared_ptr<IEventManager> event_manager_;
    AppState state_;
    
public:
    ApplicationContext() : state_(AppState::Initializing) {}
    
    // 设置器
    void setConfigManager(std::shared_ptr<IConfigManager> manager) {
        config_manager_ = std::move(manager);
    }
    
    void setLogger(std::shared_ptr<ILogger> logger) {
        logger_ = std::move(logger);
    }
    
    void setEventManager(std::shared_ptr<IEventManager> manager) {
        event_manager_ = std::move(manager);
    }
    
    // 获取器
    std::shared_ptr<IConfigManager> getConfigManager() const { return config_manager_; }
    std::shared_ptr<ILogger> getLogger() const { return logger_; }
    std::shared_ptr<IEventManager> getEventManager() const { return event_manager_; }
    
    // 状态管理
    AppState getState() const { return state_; }
    void setState(AppState state) {
        if (state_ != state) {
            auto old_state = state_;
            state_ = state;
            if (event_manager_) {
                event_manager_->publish(EventType::AppStateChange, "state_changed", 
                                      std::make_pair(old_state, state));
            }
        }
    }
};

} // namespace nex::core
