# 插件与扩展

## 工具插件

### 注册自定义工具

在代码中（如 `src/custom_tools.cpp`）：

```cpp
#include "core/tools/tools.h"

static tools::ToolResult my_tool(const std::string& args_json) {
    // 解析 args_json，执行逻辑
    return {true, "{\"result\":\"ok\"}", ""};
}

void register_custom_tools() {
    tools::Registry::instance().register_tool(
        {"my_tool", "自定义工具描述", my_tool}
    );
}
```

在 `main.cpp` 调用：
```cpp
#include "custom_tools.h"
// ...
register_custom_tools();
```

### 工具 Schema

参考 `assets/schema/tool.schema.json`，描述工具的参数结构：

```json
{
  "name": "my_tool",
  "description": "...",
  "parameters": {
    "type": "object",
    "properties": {
      "arg1": {"type": "string"}
    },
    "required": ["arg1"]
  }
}
```

## Provider 插件

### 添加新的云端 Provider

1. 在 `src/core/inference/remote/` 创建 `my_provider_client.{h,cpp}`
2. 实现 HTTP 调用、流式解析与重试
3. 在 `config/aicli.yaml` 注册：

```yaml
remote:
  providers:
    my_provider:
      enabled: true
      base_url: https://api.example.com
      model: my-model
      timeout_ms: 30000
```

## 路由策略插件

扩展 `src/core/router/policy.h`：

```cpp
class MyPolicy : public Policy {
public:
    Decision decide(const Request& req) override {
        // 自定义分流逻辑
        return Decision::Local;
    }
};
```

## 观测插件

实现自定义 `SysBoxObserver`（计划中）：

```cpp
class MyObserver : public sysbox::Observer {
public:
    void on_event(const sysbox::Event& e) override {
        // 推送到外部监控系统
    }
};
```

## 嵌入模型插件

为 RAG 提供自定义 embedder（计划中）：

```cpp
class MyEmbedder : public rag::Embedder {
public:
    std::vector<float> embed(const std::string& text) override;
};
```



