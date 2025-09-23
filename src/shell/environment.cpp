#include "nex/shell/shell.h"
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <climits>

extern char **environ;

namespace nex::shell {

Environment& Environment::getInstance() {
    static Environment instance;
    static bool initialized = false;
    if (!initialized) {
        instance.loadSystemEnvironment();
        initialized = true;
    }
    return instance;
}

std::string Environment::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    
    const char* value = std::getenv(name.c_str());
    if (value) {
        variables_[name] = value;
        return value;
    }
    
    return "";
}

void Environment::setVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
    setenv(name.c_str(), value.c_str(), 1);
}

void Environment::unsetVariable(const std::string& name) {
    variables_.erase(name);
    unsetenv(name.c_str());
}

bool Environment::hasVariable(const std::string& name) const {
    return !getVariable(name).empty();
}

std::vector<std::string> Environment::getPath() const {
    std::vector<std::string> paths;
    std::string path_var = getVariable("PATH");
    
    if (path_var.empty()) {
        return paths;
    }
    
    size_t start = 0;
    size_t end = path_var.find(':');
    
    while (end != std::string::npos) {
        std::string path = path_var.substr(start, end - start);
        if (!path.empty()) {
            paths.push_back(path);
        }
        start = end + 1;
        end = path_var.find(':', start);
    }
    
    std::string last_path = path_var.substr(start);
    if (!last_path.empty()) {
        paths.push_back(last_path);
    }
    
    return paths;
}

std::string Environment::findExecutable(const std::string& name) const {
    // 如果包含路径分隔符，直接检查
    if (name.find('/') != std::string::npos) {
        struct stat st;
        if (stat(name.c_str(), &st) == 0 && (st.st_mode & S_IXUSR)) {
            return name;
        }
        return "";
    }
    
    // 在PATH中查找
    auto paths = getPath();
    for (const auto& path : paths) {
        std::string full_path = path + "/" + name;
        struct stat st;
        if (stat(full_path.c_str(), &st) == 0 && (st.st_mode & S_IXUSR)) {
            return full_path;
        }
    }
    
    return "";
}

std::string Environment::getCurrentDirectory() const {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd);
    }
    return "";
}

bool Environment::changeDirectory(const std::string& path) {
    std::string target_path = path;
    
    // 处理特殊路径
    if (target_path.empty() || target_path == "~") {
        target_path = getVariable("HOME");
    } else if (target_path == "-") {
        target_path = getVariable("OLDPWD");
        if (target_path.empty()) {
            return false;
        }
    }
    
    // 保存当前目录
    std::string old_pwd = getCurrentDirectory();
    
    if (chdir(target_path.c_str()) == 0) {
        setVariable("OLDPWD", old_pwd);
        setVariable("PWD", getCurrentDirectory());
        return true;
    }
    
    return false;
}

std::map<std::string, std::string> Environment::getAllVariables() const {
    std::map<std::string, std::string> all_vars = variables_;
    
    // 添加系统环境变量
    for (char **env = environ; *env != nullptr; ++env) {
        std::string env_str(*env);
        size_t eq_pos = env_str.find('=');
        if (eq_pos != std::string::npos) {
            std::string name = env_str.substr(0, eq_pos);
            std::string value = env_str.substr(eq_pos + 1);
            if (all_vars.find(name) == all_vars.end()) {
                all_vars[name] = value;
            }
        }
    }
    
    return all_vars;
}

void Environment::loadSystemEnvironment() {
    // 加载一些基本的环境变量
    if (!hasVariable("HOME")) {
        const char* home = std::getenv("HOME");
        if (home) {
            setVariable("HOME", home);
        }
    }
    
    if (!hasVariable("PATH")) {
        const char* path = std::getenv("PATH");
        if (path) {
            setVariable("PATH", path);
        } else {
            // 设置默认PATH
            setVariable("PATH", "/usr/local/bin:/usr/bin:/bin");
        }
    }
    
    if (!hasVariable("PWD")) {
        setVariable("PWD", getCurrentDirectory());
    }
    
    if (!hasVariable("USER")) {
        const char* user = std::getenv("USER");
        if (user) {
            setVariable("USER", user);
        }
    }
}

} // namespace nex::shell
