add_rules("mode.debug", "mode.release")

-- 设置C++标准
set_languages("c++20")

-- 跨平台编译选项
if is_plat("windows") then
    add_cxxflags("/std:c++20", "/W4")
    add_defines("WIN32_LEAN_AND_MEAN", "NOMINMAX")
    add_syslinks("kernel32", "user32", "advapi32", "shell32", "ole32")
    if is_mode("debug") then
        add_cxxflags("/Od", "/Zi")
        add_defines("DEBUG", "_DEBUG")
    else
        add_cxxflags("/O2", "/DNDEBUG")
    end
else
    add_cxxflags("-Wall", "-Wextra", "-pedantic")
    if is_mode("debug") then
        add_cxxflags("-g", "-O0")
        add_defines("DEBUG")
    else
        add_cxxflags("-O3", "-DNDEBUG")
    end
    
    -- POSIX兼容性
    add_defines("_POSIX_C_SOURCE=200809L")
    if is_plat("linux") then
        add_defines("_GNU_SOURCE")
        add_syslinks("pthread", "dl", "m", "rt", "readline")
    elseif is_plat("macosx") then
        add_frameworks("CoreFoundation", "IOKit", "SystemConfiguration")
        add_syslinks("pthread", "dl", "m", "readline")
    end
end

-- 包含路径
add_includedirs("include")

-- 外部依赖包
add_requires("nlohmann_json", "cli11", "spdlog", "fmt", "catch2")

-- 主程序目标
target("nex")
    set_kind("binary")
    add_files("src/main.cpp")
    add_files("src/platform/platform.cpp")
    add_files("src/shell/*.cpp")
    -- 根据平台添加特定源文件
    if is_plat("linux") then
        add_files("src/platform/linux/*.cpp")
    elseif is_plat("windows") then
        add_files("src/platform/windows/*.cpp")
    elseif is_plat("macosx") then
        add_files("src/platform/macos/*.cpp")
    end
    -- 暂时注释掉依赖包，先测试基本构建
    -- add_packages("nlohmann_json", "cli11", "spdlog", "fmt")
    add_headerfiles("include/nex/**/*.h")

-- 测试目标
target("nex_test")
    set_kind("binary")
    add_files("tests/**/*.cpp", "src/**/*.cpp")
    remove_files("src/main.cpp")
    add_packages("nlohmann_json", "cli11", "spdlog", "fmt", "catch2")
    add_headerfiles("include/nex/**/*.h")
    set_default(false)

--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

