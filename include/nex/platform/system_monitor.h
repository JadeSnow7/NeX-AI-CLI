#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>

namespace nex::platform {

/**
 * @brief 系统资源信息结构体
 */
struct SystemInfo {
    std::string hostname;
    std::string os_name;
    std::string os_version;
    std::string kernel_version;
    std::string architecture;
    uint64_t total_memory;
    uint32_t cpu_cores;
    std::string cpu_model;
};

/**
 * @brief CPU使用率信息
 */
struct CpuUsage {
    double total_usage;
    std::vector<double> per_core_usage;
    double user_time;
    double system_time;
    double idle_time;
    std::chrono::steady_clock::time_point timestamp;
};

/**
 * @brief 内存使用信息
 */
struct MemoryInfo {
    uint64_t total;
    uint64_t available;
    uint64_t used;
    uint64_t free;
    uint64_t cached;
    uint64_t buffers;
    double usage_percentage;
    std::chrono::steady_clock::time_point timestamp;
};

/**
 * @brief 磁盘使用信息
 */
struct DiskInfo {
    std::string device;
    std::string mount_point;
    std::string filesystem;
    uint64_t total_space;
    uint64_t used_space;
    uint64_t available_space;
    double usage_percentage;
};

/**
 * @brief 网络接口信息
 */
struct NetworkInterface {
    std::string name;
    std::string ip_address;
    std::string mac_address;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t packets_sent;
    uint64_t packets_received;
    bool is_up;
};

/**
 * @brief 进程信息
 */
struct ProcessInfo {
    uint32_t pid;
    uint32_t ppid;
    std::string name;
    std::string command;
    std::string state;
    double cpu_usage;
    uint64_t memory_usage;
    std::string user;
    std::chrono::system_clock::time_point start_time;
};

/**
 * @brief 跨平台系统资源监控抽象接口
 */
class ISystemMonitor {
public:
    virtual ~ISystemMonitor() = default;

    // 系统信息获取
    virtual SystemInfo getSystemInfo() = 0;
    
    // CPU监控
    virtual CpuUsage getCpuUsage() = 0;
    virtual std::vector<double> getCpuTemperatures() = 0;
    
    // 内存监控
    virtual MemoryInfo getMemoryInfo() = 0;
    
    // 磁盘监控
    virtual std::vector<DiskInfo> getDiskInfo() = 0;
    
    // 网络监控
    virtual std::vector<NetworkInterface> getNetworkInterfaces() = 0;
    
    // 进程监控
    virtual std::vector<ProcessInfo> getProcessList() = 0;
    virtual std::optional<ProcessInfo> getProcessById(uint32_t pid) = 0;
    virtual std::vector<ProcessInfo> getTopProcessesByCpu(size_t limit = 10) = 0;
    virtual std::vector<ProcessInfo> getTopProcessesByMemory(size_t limit = 10) = 0;
    
    // 系统负载
    virtual std::vector<double> getLoadAverage() = 0;
    
    // 系统启动时间
    virtual std::chrono::system_clock::time_point getBootTime() = 0;
    
    // 性能优化建议
    virtual std::vector<std::string> getOptimizationSuggestions() = 0;
};

/**
 * @brief 创建平台特定的系统监控器
 */
std::unique_ptr<ISystemMonitor> createSystemMonitor();

} // namespace nex::platform
