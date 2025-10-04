#include "http_client.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <array>

namespace inference {

// 简化实现：使用 curl 命令行（生产环境建议用 libcurl）
HttpResponse HttpClient::post(const std::string& url,
                              const std::string& body,
                              const std::map<std::string, std::string>& headers,
                              int timeout_ms) {
    (void)timeout_ms; // 简化：未传递超时
    std::string cmd = "curl -s -X POST -w '\\n%{http_code}' ";
    for (auto& [k, v] : headers) {
        cmd += "-H \"" + k + ": " + v + "\" ";
    }
    cmd += "-d @- ";
    cmd += "'" + url + "'";
    
    FILE* pipe = popen(cmd.c_str(), "w");
    if (!pipe) return {0, "", "popen failed"};
    fwrite(body.data(), 1, body.size(), pipe);
    pclose(pipe);
    
    // 简化：再次读取（实际应管道双向）
    cmd = "curl -s -X POST -w '\\n%{http_code}' ";
    for (auto& [k, v] : headers) {
        cmd += "-H \"" + k + ": " + v + "\" ";
    }
    cmd += "--data-binary @- ";
    cmd += "'" + url + "' <<'EOF'\n" + body + "\nEOF";
    
    FILE* rd = popen(cmd.c_str(), "r");
    if (!rd) return {0, "", "popen read failed"};
    std::string result;
    char buf[4096];
    while (fgets(buf, sizeof(buf), rd)) result += buf;
    pclose(rd);
    
    // 解析最后一行为状态码
    auto pos = result.rfind('\n');
    if (pos != std::string::npos && pos > 0) {
        std::string code_str = result.substr(pos + 1);
        std::string resp_body = result.substr(0, pos);
        try {
            int code = std::stoi(code_str);
            return {code, resp_body, ""};
        } catch (...) {}
    }
    return {0, result, "parse failed"};
}

bool HttpClient::post_stream(const std::string& url,
                             const std::string& body,
                             const std::map<std::string, std::string>& headers,
                             const StreamChunkCallback& on_chunk,
                             std::string& err,
                             int timeout_ms) {
    (void)timeout_ms;
    std::string cmd = "curl -s -N -X POST ";
    for (auto& [k, v] : headers) {
        cmd += "-H \"" + k + ": " + v + "\" ";
    }
    cmd += "--data-binary @- ";
    cmd += "'" + url + "' <<'EOF'\n" + body + "\nEOF";
    
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) { err = "popen failed"; return false; }
    char buf[4096];
    while (fgets(buf, sizeof(buf), pipe)) {
        on_chunk(buf);
    }
    int rc = pclose(pipe);
    if (rc != 0) { err = "curl failed"; return false; }
    return true;
}

} // namespace inference

