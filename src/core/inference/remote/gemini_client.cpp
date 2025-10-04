#include "gemini_client.h"
#include "http_client.h"
#include "core/sysbox/sysbox.h"

#include <sstream>

namespace inference {

GeminiClient::GeminiClient(const std::string& api_key, const std::string& base_url, const std::string& model)
    : api_key_(api_key), base_url_(base_url), model_(model) {}

bool GeminiClient::load_model(const std::string&, std::string&) { return true; }
void GeminiClient::unload_model() {}
bool GeminiClient::is_loaded() const { return loaded_; }

bool GeminiClient::generate(const std::string& prompt,
                           const GenerateOptions& options,
                           const StreamCallback& on_token,
                           std::string& err) {
    sysbox::ScopedTimer t("gemini", "generate");
    
    // 构造 Gemini API 请求 JSON
    // POST /v1beta/models/{model}:streamGenerateContent?key={api_key}
    std::ostringstream body;
    body << "{\"contents\":[{\"parts\":[{\"text\":\"";
    for (char c : prompt) {
        if (c == '"') body << "\\\"";
        else if (c == '\n') body << "\\n";
        else if (c == '\\') body << "\\\\";
        else body << c;
    }
    body << "\"}]}],";
    body << "\"generationConfig\":{";
    body << "\"maxOutputTokens\":" << options.max_new_tokens << ",";
    body << "\"temperature\":" << options.temperature << ",";
    body << "\"topP\":" << options.top_p;
    body << "}}";

    std::map<std::string, std::string> headers;
    headers["Content-Type"] = "application/json";

    std::string url = base_url_ + "/models/" + model_ + ":streamGenerateContent?key=" + api_key_;
    
    // 流式调用
    bool ok = HttpClient::post_stream(url, body.str(), headers, [&](const std::string& chunk){
        // Gemini SSE 格式：data: {...}\n
        if (chunk.rfind("data: ", 0) == 0) {
            std::string line = chunk.substr(6);
            // 解析 candidates[0].content.parts[0].text
            auto pos = line.find("\"text\":\"");
            if (pos != std::string::npos) {
                auto start = pos + 8;
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

std::unique_ptr<Engine> create_gemini_engine(const std::string& api_key, const std::string& base_url, const std::string& model) {
    return std::make_unique<GeminiClient>(api_key, base_url, model);
}

} // namespace inference

