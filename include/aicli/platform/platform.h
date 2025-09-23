#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace aicli::platform {

/**
 * @brief 平台检测宏定义
 */
#ifdef _WIN32
    #define AICLI_PLATFORM_WINDOWS
    #ifdef _WIN64
        #define AICLI_PLATFORM_WINDOWS_64
    #else
        #define AICLI_PLATFORM_WINDOWS_32
    #endif
#elif defined(__APPLE__)
    #define AICLI_PLATFORM_MACOS
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define AICLI_PLATFORM_MACOS_DESKTOP
    #endif
#elif defined(__linux__)
    #define AICLI_PLATFORM_LINUX
#elif defined(__unix__)
    #define AICLI_PLATFORM_UNIX
#else
    #define AICLI_PLATFORM_UNKNOWN
#endif

/**
 * @brief 平台类型枚举
 */
enum class PlatformType {
    Unknown,
    Linux,
    Windows,
    MacOS,
    Unix
};

/**
 * @brief 获取当前平台类型
 */
constexpr PlatformType getCurrentPlatform() {
#ifdef AICLI_PLATFORM_WINDOWS
    return PlatformType::Windows;
#elif defined(AICLI_PLATFORM_MACOS)
    return PlatformType::MacOS;
#elif defined(AICLI_PLATFORM_LINUX)
    return PlatformType::Linux;
#elif defined(AICLI_PLATFORM_UNIX)
    return PlatformType::Unix;
#else
    return PlatformType::Unknown;
#endif
}

/**
 * @brief 获取平台名称字符串
 */
std::string getPlatformName();

/**
 * @brief 判断是否为POSIX兼容平台
 */
constexpr bool isPosixCompatible() {
    return getCurrentPlatform() != PlatformType::Windows;
}

} // namespace aicli::platform
