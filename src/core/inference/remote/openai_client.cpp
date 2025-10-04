#include "openai_client.h"
#include "http_client.h"
#include "core/sysbox/sysbox.h"

#include <sstream>

namespace inference {

OpenAIClient::OpenAIClient(const std::string& api_key, const std::string& base_url, const std::string& model)
    : api_key_(api_key), base_url_(base_url), model_(model) {}

bool OpenAIClient::load_model(const std::string&, std::string&) { return true; }
void OpenAIClient::unload_model() {}
bool OpenAIClient::is_loaded() const { return loaded_; }

bool OpenAIClient::generate(const std::string& prompt,
                           const GenerateOptions& options,
                           const StreamCallback& on_token,
                           std::string& err) {
    sysbox::ScopedTimer t("openai", "generate");
    
    // 构造请求 JSON
    std::ostringstream body;
    body << "{\"model\":\"" << model_ << "\",";
    body << "\"messages\":[{\"role\":\"user\",\"content\":\"";
    // 简易转义
    for (char c : prompt) {
        if (c == '"') body << "\\\"";
        else if (c == '\n') body << "\\n";
        else if (c == '\\') body << "\\\\";
        else body << c;
    }
    body << "\"}],";
    body << "\"max_tokens\":" << options.max_new_tokens << ",";
    body << "\"temperature\":" << options.temperature << ",";
    body << "\"top_p\":" << options.top_p << ",";
    body << "\"stream\":true}";

    std::map<std::string, std::string> headers;
    headers["Authorization"] = "Bearer " + api_key_;
    headers["Content-Type"] = "application/json";

    std::string url = base_url_ + "/chat/completions";
    
    // 流式调用
    bool ok = HttpClient::post_stream(url, body.str(), headers, [&](const std::string& chunk){
        // 解析 SSE：data: {...}\n
        if (chunk.rfind("data: ", 0) == 0) {
            std::string line = chunk.substr(6);
            if (line.rfind("[DONE]", 0) == 0) return;
            // 简化解析 delta.content
            auto pos = line.find("\"content\":\"");
            if (pos != std::string::npos) {
                auto start = pos + 11;
                auto end = line.find('"', start);
                if (end != std::string::npos) {
                    std::string text = line.substr(start, end - start);
                    // 反转义
                    std::string unesc;
                    for (size_t i = 0; i < text.size(); ++i) {
                        if (text[i] == '\\' && i+1 < text.size()) {
                            if (text[i+1] == 'n') { unesc += '\n'; ++i; continue; }
                            if (text[i+1] == '"') { unesc += '"'; ++i; continue; }
                            if (text[i+1] == '\\') { unesc += '\\'; ++i; continue; }
                        }
                        unesc += text[i];
                    }
                    if (!unesc.empty()) on_token(unesc);
                }
            }
        }
    }, err);
    
    return ok;
}

std::unique_ptr<Engine> create_openai_engine(const std::string& api_key, const std::string& base_url, const std::string& model) {
    return std::make_unique<OpenAIClient>(api_key, base_url, model);
}

} // namespace inference

