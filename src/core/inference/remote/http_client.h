#pragma once

#include <string>
#include <map>
#include <functional>

namespace inference {

struct HttpResponse {
    int status_code = 0;
    std::string body;
    std::string error;
    bool ok() const { return status_code >= 200 && status_code < 300; }
};

using StreamChunkCallback = std::function<void(const std::string& chunk)>;

class HttpClient {
public:
    // 同步 POST 请求
    static HttpResponse post(const std::string& url,
                            const std::string& body,
                            const std::map<std::string, std::string>& headers,
                            int timeout_ms = 30000);

    // 流式 POST（SSE）
    static bool post_stream(const std::string& url,
                           const std::string& body,
                           const std::map<std::string, std::string>& headers,
                           const StreamChunkCallback& on_chunk,
                           std::string& err,
                           int timeout_ms = 30000);
};

} // namespace inference

