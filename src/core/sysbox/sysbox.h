#pragma once

#include <string>
#include <chrono>
#include <utility>

namespace sysbox {

struct Event {
    std::string component;
    std::string level;  // info|warn|error
    std::string message;
};

void record(const Event& ev);

// 直接记录 JSON 行（json 应为对象文本，不含换行）
void record_json(const std::string& component, const std::string& level, const std::string& json);

class ScopedTimer {
public:
    ScopedTimer(const std::string& component, const std::string& label);
    ~ScopedTimer();
private:
    std::string component_;
    std::string label_;
    std::chrono::steady_clock::time_point start_;
};

// 指标聚合：加入样本（毫秒）并返回 p50/p95（毫秒）
std::pair<double,double> add_duration_sample(const std::string& name, double ms);

} // namespace sysbox
