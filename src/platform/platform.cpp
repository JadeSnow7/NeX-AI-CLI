#include "nex/platform/platform.h"
#include <string>

namespace nex::platform {

std::string getPlatformName() {
#ifdef AICLI_PLATFORM_WINDOWS
    return "Windows";
#elif defined(AICLI_PLATFORM_MACOS)
    return "macOS";
#elif defined(AICLI_PLATFORM_LINUX)
    return "Linux";
#elif defined(AICLI_PLATFORM_UNIX)
    return "Unix";
#else
    return "Unknown";
#endif
}

} // namespace nex::platform
