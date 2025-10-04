#pragma once

#include <string>
#include <vector>
#include "session.h"

namespace conversation {

struct RenderOptions {
    std::string system_prompt;
    bool use_chatml = true;
};

class TemplateBuilder {
public:
    static std::string render_plain(const std::vector<Message>& history,
                                    const RenderOptions& opts) {
        std::string out;
        if (!opts.system_prompt.empty()) {
            out += opts.system_prompt;
            out += "\n";
        }
        for (const auto& m : history) {
            out += m.role + ": " + m.content + "\n";
        }
        return out;
    }

    static std::string render_chatml(const std::vector<Message>& history,
                                     const RenderOptions& opts) {
        std::string out;
        if (!opts.system_prompt.empty()) {
            out += "<|im_start|>system\n";
            out += opts.system_prompt;
            out += "<|im_end|>\n";
        }
        for (const auto& m : history) {
            out += "<|im_start|>";
            out += m.role;
            out += "\n";
            out += m.content;
            out += "<|im_end|>\n";
        }
        out += "<|im_start|>assistant\n"; // 让模型继续assistant角色
        return out;
    }
};

} // namespace conversation
