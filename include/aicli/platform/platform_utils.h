#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <memory>

namespace aicli::platform {

/**
 * @brief 文件路径分隔符
 */
#ifdef AICLI_PLATFORM_WINDOWS
    constexpr char PATH_SEPARATOR = '\\';
    constexpr const char* PATH_SEPARATOR_STR = "\\";
#else
    constexpr char PATH_SEPARATOR = '/';
    constexpr const char* PATH_SEPARATOR_STR = "/";
#endif

/**
 * @brief 文件系统操作抽象接口
 */
class IFileSystem {
public:
    virtual ~IFileSystem() = default;
    
    // 路径操作
    virtual std::string joinPath(const std::vector<std::string>& components) = 0;
    virtual std::string getAbsolutePath(const std::string& path) = 0;
    virtual std::string getHomePath() = 0;
    virtual std::string getTempPath() = 0;
    virtual std::string getCurrentWorkingDirectory() = 0;
    
    // 文件/目录检查
    virtual bool exists(const std::string& path) = 0;
    virtual bool isFile(const std::string& path) = 0;
    virtual bool isDirectory(const std::string& path) = 0;
    virtual bool isReadable(const std::string& path) = 0;
    virtual bool isWritable(const std::string& path) = 0;
    virtual bool isExecutable(const std::string& path) = 0;
    
    // 文件操作
    virtual std::string readFile(const std::string& path) = 0;
    virtual bool writeFile(const std::string& path, const std::string& content) = 0;
    virtual bool copyFile(const std::string& source, const std::string& destination) = 0;
    virtual bool removeFile(const std::string& path) = 0;
    
    // 目录操作
    virtual bool createDirectory(const std::string& path, bool recursive = true) = 0;
    virtual bool removeDirectory(const std::string& path, bool recursive = false) = 0;
    virtual std::vector<std::string> listDirectory(const std::string& path) = 0;
    
    // 文件信息
    virtual uint64_t getFileSize(const std::string& path) = 0;
    virtual std::chrono::system_clock::time_point getLastModified(const std::string& path) = 0;
};

/**
 * @brief 进程管理抽象接口
 */
class IProcessManager {
public:
    virtual ~IProcessManager() = default;
    
    // 进程启动
    virtual int executeCommand(const std::string& command, 
                              const std::vector<std::string>& args = {},
                              const std::string& working_dir = "") = 0;
    
    virtual int executeCommandWithOutput(const std::string& command,
                                        const std::vector<std::string>& args,
                                        std::string& output,
                                        std::string& error,
                                        const std::string& working_dir = "") = 0;
    
    // 异步进程管理
    virtual uint32_t startProcess(const std::string& command,
                                 const std::vector<std::string>& args = {},
                                 const std::string& working_dir = "") = 0;
    
    virtual bool isProcessRunning(uint32_t pid) = 0;
    virtual bool killProcess(uint32_t pid, bool force = false) = 0;
    virtual int waitForProcess(uint32_t pid) = 0;
    
    // 当前进程信息
    virtual uint32_t getCurrentProcessId() = 0;
    virtual uint32_t getParentProcessId() = 0;
};

/**
 * @brief 环境变量管理抽象接口
 */
class IEnvironment {
public:
    virtual ~IEnvironment() = default;
    
    virtual std::string getVariable(const std::string& name) = 0;
    virtual bool setVariable(const std::string& name, const std::string& value) = 0;
    virtual bool removeVariable(const std::string& name) = 0;
    virtual std::vector<std::pair<std::string, std::string>> getAllVariables() = 0;
    
    // 路径相关
    virtual std::vector<std::string> getPathDirectories() = 0;
    virtual std::string findExecutable(const std::string& name) = 0;
};

/**
 * @brief 平台特定工具集合
 */
class IPlatformUtils {
public:
    virtual ~IPlatformUtils() = default;
    
    virtual std::unique_ptr<IFileSystem> getFileSystem() = 0;
    virtual std::unique_ptr<IProcessManager> getProcessManager() = 0;
    virtual std::unique_ptr<IEnvironment> getEnvironment() = 0;
    
    // 系统特定功能
    virtual void sleep(std::chrono::milliseconds duration) = 0;
    virtual std::string getUsername() = 0;
    virtual bool isAdministrator() = 0;
    virtual std::string getSystemLocale() = 0;
};

/**
 * @brief 创建平台特定的工具集
 */
std::unique_ptr<IPlatformUtils> createPlatformUtils();

} // namespace aicli::platform
