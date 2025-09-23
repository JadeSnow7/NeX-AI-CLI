cmake_minimum_required(VERSION 3.20)
project(NeX VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
if(MSVC)
    add_compile_options(/W4 /std:c++20)
    add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi)
        add_compile_definitions(DEBUG _DEBUG)
    else()
        add_compile_options(/O2)
        add_compile_definitions(NDEBUG)
    endif()
else()
    add_compile_options(-Wall -Wextra -pedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
        add_compile_definitions(DEBUG)
    else()
        add_compile_options(-O3)
        add_compile_definitions(NDEBUG)
    endif()
    
    # POSIX兼容性
    add_compile_definitions(_POSIX_C_SOURCE=200809L)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_compile_definitions(_GNU_SOURCE)
    endif()
endif()

# 包含目录
# TODO: 添加 llama.cpp 依赖查找
# find_package(llama QUIET)
# if(llama_FOUND)
#     message("-- llama.cpp: Found")
#     set(AI_ENABLED TRUE)
# else()
#     message("-- llama.cpp: Not found, AI features will be disabled")
#     set(AI_ENABLED FALSE)
# endif()

# 启用AI功能（支持在线和本地模型）
set(AI_ENABLED TRUE)
message("-- AI Features: Enabled (with online and local model support)")

# 包含目录
include_directories(${CMAKE_SOURCE_DIR}/include)

# 平台特定的库
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM_LIBS pthread dl m rt readline)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM_LIBS kernel32 user32 advapi32 shell32 ole32)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM_LIBS pthread dl m readline)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(SYSTEMCONFIGURATION_LIBRARY SystemConfiguration)
    list(APPEND PLATFORM_LIBS ${COREFOUNDATION_LIBRARY} ${IOKIT_LIBRARY} ${SYSTEMCONFIGURATION_LIBRARY})
endif()

# 查找依赖包（可选）
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(READLINE readline)
    if(READLINE_FOUND)
        include_directories(${READLINE_INCLUDE_DIRS})
        link_directories(${READLINE_LIBRARY_DIRS})
    endif()
endif()

# AI功能依赖
if(AI_ENABLED)
    # 添加编译定义
    add_compile_definitions(AI_ENABLED)
    
    # 查找 libcurl
    find_package(CURL QUIET)
    if(CURL_FOUND)
        message("-- libcurl: Found")
        include_directories(${CURL_INCLUDE_DIRS})
        set(AI_LIBS ${CURL_LIBRARIES})
    else()
        message("-- libcurl: Not found, trying to find manually")
        find_library(CURL_LIBRARY curl)
        if(CURL_LIBRARY)
            set(AI_LIBS ${CURL_LIBRARY})
            message("-- libcurl: Found manually")
        else()
            message(WARNING "-- libcurl: Not found, AI online features may not work")
            set(AI_LIBS)
        endif()
    endif()
    
    # 查找 jsoncpp
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(JSONCPP jsoncpp)
        if(JSONCPP_FOUND)
            message("-- jsoncpp: Found")
            include_directories(${JSONCPP_INCLUDE_DIRS})
            link_directories(${JSONCPP_LIBRARY_DIRS})
            list(APPEND AI_LIBS ${JSONCPP_LIBRARIES})
        else()
            message("-- jsoncpp: Not found via pkg-config, trying manual search")
            find_library(JSONCPP_LIBRARY jsoncpp)
            if(JSONCPP_LIBRARY)
                list(APPEND AI_LIBS ${JSONCPP_LIBRARY})
                message("-- jsoncpp: Found manually")
            else()
                message(WARNING "-- jsoncpp: Not found, using header-only implementation")
            endif()
        endif()
    endif()
else()
    set(AI_LIBS)
endif()

# 源文件定义
set(CORE_SOURCES
    src/main.cpp
)

set(SHELL_SOURCES
    # src/shell/shell.cpp  # 暂时注释，等待重新实现
    src/shell/prompt_utils.cpp
)

set(CONFIG_SOURCES
    src/config/config.cpp
)

# AI 源文件
if(AI_ENABLED)
    set(AI_SOURCES
        src/ai/ai_core.cpp
    )
else()
    set(AI_SOURCES)
endif()

# 所有源文件
set(SOURCES
    ${CORE_SOURCES}
    ${SHELL_SOURCES}
    ${CONFIG_SOURCES}
    ${AI_SOURCES}
)

# 头文件
set(NEX_HEADERS
    include/nex/shell/shell.h
    include/nex/shell/prompt_utils.h
    include/nex/platform/platform.h
    include/nex/core/core.h
    include/nex/config/config.h
)

# AI 头文件
if(AI_ENABLED)
    list(APPEND NEX_HEADERS
        include/nex/ai/ai_core.h
        include/nex/ai/ai_engine.h
    )
endif()

# 平台特定源文件（目前这些文件还不存在，先注释掉）
# if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#     list(APPEND NEX_SOURCES src/platform/linux/linux_platform.cpp)
# elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows") 
#     list(APPEND NEX_SOURCES src/platform/windows/windows_platform.cpp)
# elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
#     list(APPEND NEX_SOURCES src/platform/macos/macos_platform.cpp)
# endif()

# 主程序目标
add_executable(nex ${SOURCES})

# 链接库
target_link_libraries(nex ${PLATFORM_LIBS})

# 添加AI依赖库
if(AI_ENABLED AND AI_LIBS)
    target_link_libraries(nex ${AI_LIBS})
    message("-- AI Libraries: ${AI_LIBS}")
endif()

# 安装配置
install(TARGETS nex
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# 测试配置
enable_testing()

# 创建测试目标（如果有测试文件）
if(EXISTS "${CMAKE_SOURCE_DIR}/tests")
    file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
    if(TEST_SOURCES)
        # 排除main.cpp避免冲突
        list(REMOVE_ITEM NEX_SOURCES src/main.cpp)
        
        add_executable(nex_tests ${TEST_SOURCES} ${NEX_SOURCES})
        target_link_libraries(nex_tests ${PLATFORM_LIBS})
        
        # 查找Catch2测试框架
        find_package(Catch2 QUIET)
        if(Catch2_FOUND)
            target_link_libraries(nex_tests Catch2::Catch2)
        endif()
        
        add_test(NAME nex_unit_tests COMMAND nex_tests)
    endif()
endif()

# 文档生成（可选）
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    if(EXISTS ${DOXYGEN_IN})
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM
        )
    endif()
endif()

# 打包配置
set(CPACK_PACKAGE_NAME "NeX")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NeX: AI-Powered Command Line Interface")
set(CPACK_PACKAGE_VENDOR "NeX Development Team")
set(CPACK_PACKAGE_CONTACT "nex@example.com")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libreadline8")
    set(CPACK_RPM_PACKAGE_REQUIRES "readline")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(CPACK_GENERATOR "DragNDrop;TGZ")
endif()

include(CPack)

# 显示构建信息
message(STATUS "")
message(STATUS "NeX: AI-CLI Build Configuration")
message(STATUS "================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Platform Libraries: ${PLATFORM_LIBS}")
if(READLINE_FOUND)
    message(STATUS "Readline: Found (${READLINE_VERSION})")
else()
    message(STATUS "Readline: System default")
endif()
message(STATUS "")
