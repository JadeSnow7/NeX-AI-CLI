#include "aicli/platform/system_monitor.h"
#include "aicli/platform/platform.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <dirent.h>
#include <thread>

namespace aicli::platform {

class LinuxSystemMonitor : public ISystemMonitor {
private:
    mutable CpuUsage last_cpu_usage_;
    
public:
    SystemInfo getSystemInfo() override {
        SystemInfo info;
        
        // 获取主机名
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            info.hostname = hostname;
        }
        
        // 从/etc/os-release获取系统信息
        std::ifstream os_release("/etc/os-release");
        std::string line;
        while (std::getline(os_release, line)) {
            if (line.starts_with("NAME=")) {
                info.os_name = line.substr(5);
                // 移除引号
                if (info.os_name.front() == '"' && info.os_name.back() == '"') {
                    info.os_name = info.os_name.substr(1, info.os_name.length() - 2);
                }
            } else if (line.starts_with("VERSION=")) {
                info.os_version = line.substr(8);
                if (info.os_version.front() == '"' && info.os_version.back() == '"') {
                    info.os_version = info.os_version.substr(1, info.os_version.length() - 2);
                }
            }
        }
        
        // 获取内核版本
        std::ifstream version("/proc/version");
        if (std::getline(version, line)) {
            std::istringstream iss(line);
            std::string word;
            iss >> word >> word >> word; // 跳过前两个词
            info.kernel_version = word;
        }
        
        // 获取架构信息
        std::ifstream cpuinfo("/proc/cpuinfo");
        while (std::getline(cpuinfo, line)) {
            if (line.starts_with("model name")) {
                size_t pos = line.find(':');
                if (pos != std::string::npos) {
                    info.cpu_model = line.substr(pos + 1);
                    // 去除前导空格
                    info.cpu_model.erase(0, info.cpu_model.find_first_not_of(" \t"));
                    break;
                }
            }
        }
        
        // 获取CPU核心数
        info.cpu_cores = std::thread::hardware_concurrency();
        
        // 获取总内存
        struct sysinfo si;
        if (sysinfo(&si) == 0) {
            info.total_memory = static_cast<uint64_t>(si.totalram) * si.mem_unit;
        }
        
        info.architecture = "x86_64"; // 简化处理，实际应该检测
        
        return info;
    }
    
    CpuUsage getCpuUsage() override {
        CpuUsage usage;
        usage.timestamp = std::chrono::steady_clock::now();
        
        std::ifstream stat("/proc/stat");
        std::string line;
        
        // 读取总CPU使用率
        if (std::getline(stat, line)) {
            std::istringstream iss(line);
            std::string cpu_label;
            uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
            
            iss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
            
            uint64_t total_time = user + nice + system + idle + iowait + irq + softirq + steal;
            uint64_t work_time = user + nice + system + irq + softirq + steal;
            
            usage.total_usage = static_cast<double>(work_time) / total_time * 100.0;
            usage.user_time = static_cast<double>(user + nice) / total_time * 100.0;
            usage.system_time = static_cast<double>(system + irq + softirq) / total_time * 100.0;
            usage.idle_time = static_cast<double>(idle) / total_time * 100.0;
        }
        
        // 读取每个核心的使用率
        while (std::getline(stat, line)) {
            if (line.starts_with("cpu")) {
                std::istringstream iss(line);
                std::string cpu_label;
                uint64_t user, nice, system, idle, iowait, irq, softirq, steal;
                
                iss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
                
                uint64_t total_time = user + nice + system + idle + iowait + irq + softirq + steal;
                uint64_t work_time = user + nice + system + irq + softirq + steal;
                
                double core_usage = static_cast<double>(work_time) / total_time * 100.0;
                usage.per_core_usage.push_back(core_usage);
            } else {
                break;
            }
        }
        
        return usage;
    }
    
    std::vector<double> getCpuTemperatures() override {
        std::vector<double> temperatures;
        
        // 尝试从thermal_zone读取温度
        for (int i = 0; i < 10; ++i) {
            std::string path = "/sys/class/thermal/thermal_zone" + std::to_string(i) + "/temp";
            std::ifstream temp_file(path);
            if (temp_file.is_open()) {
                int temp_millidegrees;
                if (temp_file >> temp_millidegrees) {
                    temperatures.push_back(temp_millidegrees / 1000.0);
                }
            } else {
                break;
            }
        }
        
        return temperatures;
    }
    
    MemoryInfo getMemoryInfo() override {
        MemoryInfo info;
        info.timestamp = std::chrono::steady_clock::now();
        
        std::ifstream meminfo("/proc/meminfo");
        std::string line;
        
        while (std::getline(meminfo, line)) {
            std::istringstream iss(line);
            std::string key;
            uint64_t value;
            std::string unit;
            
            iss >> key >> value >> unit;
            
            // 转换为字节
            if (unit == "kB") {
                value *= 1024;
            }
            
            if (key == "MemTotal:") {
                info.total = value;
            } else if (key == "MemAvailable:") {
                info.available = value;
            } else if (key == "MemFree:") {
                info.free = value;
            } else if (key == "Cached:") {
                info.cached = value;
            } else if (key == "Buffers:") {
                info.buffers = value;
            }
        }
        
        info.used = info.total - info.available;
        info.usage_percentage = static_cast<double>(info.used) / info.total * 100.0;
        
        return info;
    }
    
    std::vector<DiskInfo> getDiskInfo() override {
        std::vector<DiskInfo> disks;
        
        std::ifstream mounts("/proc/mounts");
        std::string line;
        
        while (std::getline(mounts, line)) {
            std::istringstream iss(line);
            std::string device, mount_point, filesystem;
            iss >> device >> mount_point >> filesystem;
            
            // 只处理真实的块设备
            if (device.starts_with("/dev/") && 
                mount_point != "/proc" && mount_point != "/sys" && 
                mount_point != "/dev" && !mount_point.starts_with("/proc/") &&
                !mount_point.starts_with("/sys/") && !mount_point.starts_with("/dev/")) {
                
                struct statvfs stats;
                if (statvfs(mount_point.c_str()) == 0) {
                    DiskInfo disk;
                    disk.device = device;
                    disk.mount_point = mount_point;
                    disk.filesystem = filesystem;
                    disk.total_space = static_cast<uint64_t>(stats.f_blocks) * stats.f_frsize;
                    disk.available_space = static_cast<uint64_t>(stats.f_bavail) * stats.f_frsize;
                    disk.used_space = disk.total_space - (static_cast<uint64_t>(stats.f_bfree) * stats.f_frsize);
                    
                    if (disk.total_space > 0) {
                        disk.usage_percentage = static_cast<double>(disk.used_space) / disk.total_space * 100.0;
                        disks.push_back(disk);
                    }
                }
            }
        }
        
        return disks;
    }
    
    std::vector<NetworkInterface> getNetworkInterfaces() override {
        std::vector<NetworkInterface> interfaces;
        
        // 从/proc/net/dev读取网络统计信息
        std::ifstream dev("/proc/net/dev");
        std::string line;
        
        // 跳过头两行
        std::getline(dev, line);
        std::getline(dev, line);
        
        while (std::getline(dev, line)) {
            std::istringstream iss(line);
            std::string interface_name;
            uint64_t rx_bytes, rx_packets, tx_bytes, tx_packets;
            uint64_t dummy; // 用于跳过不需要的字段
            
            iss >> interface_name;
            // 移除冒号
            if (!interface_name.empty() && interface_name.back() == ':') {
                interface_name.pop_back();
            }
            
            iss >> rx_bytes >> rx_packets;
            // 跳过一些字段
            for (int i = 0; i < 6; ++i) iss >> dummy;
            iss >> tx_bytes >> tx_packets;
            
            NetworkInterface iface;
            iface.name = interface_name;
            iface.bytes_received = rx_bytes;
            iface.packets_received = rx_packets;
            iface.bytes_sent = tx_bytes;
            iface.packets_sent = tx_packets;
            iface.is_up = true; // 简化处理
            
            interfaces.push_back(iface);
        }
        
        return interfaces;
    }
    
    std::vector<ProcessInfo> getProcessList() override {
        std::vector<ProcessInfo> processes;
        
        DIR* proc_dir = opendir("/proc");
        if (!proc_dir) return processes;
        
        struct dirent* entry;
        while ((entry = readdir(proc_dir)) != nullptr) {
            // 检查是否为数字目录名（PID）
            if (entry->d_type == DT_DIR) {
                std::string dirname = entry->d_name;
                if (std::all_of(dirname.begin(), dirname.end(), ::isdigit)) {
                    uint32_t pid = std::stoul(dirname);
                    auto proc_info = getProcessById(pid);
                    if (proc_info) {
                        processes.push_back(*proc_info);
                    }
                }
            }
        }
        
        closedir(proc_dir);
        return processes;
    }
    
    std::optional<ProcessInfo> getProcessById(uint32_t pid) override {
        std::string stat_path = "/proc/" + std::to_string(pid) + "/stat";
        std::ifstream stat_file(stat_path);
        
        if (!stat_file.is_open()) {
            return std::nullopt;
        }
        
        ProcessInfo info;
        info.pid = pid;
        
        std::string line;
        if (std::getline(stat_file, line)) {
            std::istringstream iss(line);
            std::string comm;
            char state;
            
            iss >> info.pid >> comm >> state >> info.ppid;
            
            // 移除括号
            if (comm.length() >= 2 && comm.front() == '(' && comm.back() == ')') {
                info.name = comm.substr(1, comm.length() - 2);
            }
            
            info.state = std::string(1, state);
        }
        
        // 读取命令行
        std::string cmdline_path = "/proc/" + std::to_string(pid) + "/cmdline";
        std::ifstream cmdline_file(cmdline_path);
        if (cmdline_file.is_open()) {
            std::getline(cmdline_file, info.command);
            // 替换null字符为空格
            std::replace(info.command.begin(), info.command.end(), '\0', ' ');
        }
        
        return info;
    }
    
    std::vector<ProcessInfo> getTopProcessesByCpu(size_t limit) override {
        auto processes = getProcessList();
        
        std::sort(processes.begin(), processes.end(), 
                 [](const ProcessInfo& a, const ProcessInfo& b) {
                     return a.cpu_usage > b.cpu_usage;
                 });
        
        if (processes.size() > limit) {
            processes.resize(limit);
        }
        
        return processes;
    }
    
    std::vector<ProcessInfo> getTopProcessesByMemory(size_t limit) override {
        auto processes = getProcessList();
        
        std::sort(processes.begin(), processes.end(),
                 [](const ProcessInfo& a, const ProcessInfo& b) {
                     return a.memory_usage > b.memory_usage;
                 });
        
        if (processes.size() > limit) {
            processes.resize(limit);
        }
        
        return processes;
    }
    
    std::vector<double> getLoadAverage() override {
        std::vector<double> load_avg(3, 0.0);
        
        std::ifstream loadavg("/proc/loadavg");
        if (loadavg.is_open()) {
            loadavg >> load_avg[0] >> load_avg[1] >> load_avg[2];
        }
        
        return load_avg;
    }
    
    std::chrono::system_clock::time_point getBootTime() override {
        std::ifstream stat("/proc/stat");
        std::string line;
        
        while (std::getline(stat, line)) {
            if (line.starts_with("btime ")) {
                uint64_t boot_time;
                std::istringstream iss(line);
                std::string label;
                iss >> label >> boot_time;
                
                return std::chrono::system_clock::from_time_t(boot_time);
            }
        }
        
        return std::chrono::system_clock::now();
    }
    
    std::vector<std::string> getOptimizationSuggestions() override {
        std::vector<std::string> suggestions;
        
        auto memory = getMemoryInfo();
        auto cpu = getCpuUsage();
        auto disks = getDiskInfo();
        
        // 内存建议
        if (memory.usage_percentage > 90) {
            suggestions.push_back("内存使用率过高，建议关闭不必要的程序或增加内存");
        } else if (memory.usage_percentage > 80) {
            suggestions.push_back("内存使用率较高，建议监控内存使用情况");
        }
        
        // CPU建议
        if (cpu.total_usage > 90) {
            suggestions.push_back("CPU使用率过高，建议检查占用CPU的进程");
        }
        
        // 磁盘建议
        for (const auto& disk : disks) {
            if (disk.usage_percentage > 95) {
                suggestions.push_back("磁盘 " + disk.mount_point + " 空间不足，建议清理磁盘空间");
            } else if (disk.usage_percentage > 85) {
                suggestions.push_back("磁盘 " + disk.mount_point + " 空间较少，建议关注磁盘使用情况");
            }
        }
        
        if (suggestions.empty()) {
            suggestions.push_back("系统运行良好，无需特别优化");
        }
        
        return suggestions;
    }
};

std::unique_ptr<ISystemMonitor> createSystemMonitor() {
    return std::make_unique<LinuxSystemMonitor>();
}

} // namespace aicli::platform
