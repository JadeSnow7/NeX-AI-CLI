#include "llama_engine.h"
#include "utils/logging.h"
#include "utils/config.h"
#include "core/sysbox/sysbox.h"

#include <chrono>
#include <thread>
#include <vector>
#include <cstring>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <mutex>
#include <atomic>

#if AICLI_WITH_LLAMA
#include "llama.h"
#endif

namespace inference {

struct LlamaEngine::Impl {
    bool loaded = false;
    std::string model_path;
#if AICLI_WITH_LLAMA
    llama_model* model = nullptr;
    llama_context* ctx = nullptr;
    const llama_vocab* vocab = nullptr;
    int n_ctx = 4096;
    int n_threads = std::max(1u, std::thread::hardware_concurrency());
    std::string arch;
    std::string chat_template;
    struct SessionState {
        int n_past = 0;
        std::vector<llama_token> last_tokens;
    };
    std::unordered_map<std::string, SessionState> sessions;
    std::string active_session; // 当前上下文对应的会话
    std::mutex mu;
    std::atomic<bool> abort_requested{false};

    static bool ggml_abort_trampoline(void* ud) {
        Impl* self = reinterpret_cast<Impl*>(ud);
        return self->abort_requested.load(std::memory_order_relaxed);
    }

    bool reinit_context(std::string& err) {
        if (!model) { err = "model not loaded"; return false; }
        if (ctx) { llama_free(ctx); ctx = nullptr; }
        llama_context_params cparams = llama_context_default_params();
        cparams.n_ctx = n_ctx;
        cparams.abort_callback = (ggml_abort_callback) &Impl::ggml_abort_trampoline;
        cparams.abort_callback_data = this;
        ctx = llama_init_from_model(model, cparams);
        if (!ctx) { err = "failed to recreate context"; return false; }
        vocab = llama_model_get_vocab(model);
        sessions.clear();
        active_session.clear();
        abort_requested.store(false, std::memory_order_relaxed);
        return true;
    }
#endif
};

LlamaEngine::LlamaEngine() : impl_(new Impl()) {}
LlamaEngine::~LlamaEngine() { unload_model(); }

bool LlamaEngine::load_model(const std::string& model_path, std::string& err) {
#if AICLI_WITH_LLAMA
    sysbox::ScopedTimer t("inference", "load_model");
    if (impl_->loaded) {
        unload_model();
    }
    // 限制 llama/ggml 日志噪声，仅输出 warn 以上
    llama_log_set(
        [](ggml_log_level level, const char* text, void* user_data) {
            (void)user_data;
            if (level >= GGML_LOG_LEVEL_WARN) {
                fputs(text, stderr);
            }
        },
        nullptr);
    // 从环境变量读取 n_ctx / n_threads
    if (auto v = config::get_env("AICLI_CTX")) {
        try { impl_->n_ctx = std::stoi(*v); } catch (...) {}
    }
    if (auto v = config::get_env("AICLI_THREADS")) {
        try { impl_->n_threads = std::stoi(*v); } catch (...) {}
    }

    llama_backend_init();

    llama_model_params mparams = llama_model_default_params();
    impl_->model = llama_model_load_from_file(model_path.c_str(), mparams);
    if (!impl_->model) {
        err = "failed to load model";
        llama_backend_free();
        return false;
    }

    llama_context_params cparams = llama_context_default_params();
    cparams.n_ctx = impl_->n_ctx;
    cparams.abort_callback = (ggml_abort_callback) &Impl::ggml_abort_trampoline;
    cparams.abort_callback_data = impl_.get();
    impl_->ctx = llama_init_from_model(impl_->model, cparams);
    if (!impl_->ctx) {
        err = "failed to create context";
        llama_model_free(impl_->model);
        impl_->model = nullptr;
        llama_backend_free();
        return false;
    }

    impl_->vocab = llama_model_get_vocab(impl_->model);

    // 读取元数据：architecture 与 chat_template（若存在）
    impl_->arch.clear();
    impl_->chat_template.clear();
    {
        char val[512];
        int vn = llama_model_meta_val_str(impl_->model, "general.architecture", val, sizeof(val));
        if (vn > 0) impl_->arch.assign(val, val + vn);
    }
    {
        static constexpr int BUF_SZ = 8192;
        std::string buf; buf.resize(BUF_SZ);
        int vn = llama_model_meta_val_str(impl_->model, "tokenizer.chat_template", buf.data(), BUF_SZ);
        if (vn > 0) impl_->chat_template.assign(buf.data(), buf.data() + vn);
    }
    impl_->abort_requested.store(false, std::memory_order_relaxed);
#endif
    impl_->model_path = model_path;
    impl_->loaded = true;
    logging::log(logging::Level::Info, "[llama] model loaded: " + model_path);
    (void)err;
    return true;
}

void LlamaEngine::unload_model() {
#if AICLI_WITH_LLAMA
    { std::lock_guard<std::mutex> lk(impl_->mu); impl_->sessions.clear(); impl_->active_session.clear(); }
    if (impl_->ctx) {
        llama_free(impl_->ctx);
        impl_->ctx = nullptr;
    }
    if (impl_->model) {
        llama_model_free(impl_->model);
        impl_->model = nullptr;
    }
    impl_->vocab = nullptr;
    llama_backend_free();
    impl_->abort_requested.store(false, std::memory_order_relaxed);
#endif
    if (impl_->loaded) {
        logging::log(logging::Level::Info, "[llama] model unloaded");
    }
    impl_->loaded = false;
    impl_->model_path.clear();
}

bool LlamaEngine::is_loaded() const { return impl_->loaded; }

#if AICLI_WITH_LLAMA
static std::string token_to_piece(const llama_vocab* vocab, int token_id) {
    char buf[8192];
    int n = llama_token_to_piece(vocab, token_id, buf, sizeof(buf), /*lstrip*/0, /*special*/true);
    if (n < 0) return std::string();
    return std::string(buf, buf + n);
}

static int sample_top_p_temperature(const float* logits, int n_vocab, float temperature, float top_p, std::mt19937& rng) {
    // 复制并缩放 logits
    std::vector<float> scaled(logits, logits + n_vocab);
    if (temperature > 0.0f) {
        const float invT = 1.0f / temperature;
        for (int i = 0; i < n_vocab; ++i) scaled[i] *= invT;
    }
    float max_logit = *std::max_element(scaled.begin(), scaled.end());
    std::vector<float> probs(n_vocab);
    double sum = 0.0;
    for (int i = 0; i < n_vocab; ++i) {
        float v = scaled[i] - max_logit;
        float ev = std::exp(v);
        probs[i] = ev;
        sum += ev;
    }
    if (sum <= 0.0) {
        int best = 0; float bestv = logits[0];
        for (int i = 1; i < n_vocab; ++i) if (logits[i] > bestv) { bestv = logits[i]; best = i; }
        return best;
    }
    for (int i = 0; i < n_vocab; ++i) probs[i] = (float)(probs[i] / sum);

    std::vector<int> idx(n_vocab);
    for (int i = 0; i < n_vocab; ++i) idx[i] = i;
    std::sort(idx.begin(), idx.end(), [&](int a, int b){ return probs[a] > probs[b]; });

    double acc = 0.0;
    int cutoff = n_vocab;
    for (int i = 0; i < n_vocab; ++i) {
        acc += probs[idx[i]];
        if (acc >= top_p) { cutoff = i + 1; break; }
    }
    if (cutoff < 1) cutoff = 1;

    std::uniform_real_distribution<double> uni(0.0, 1.0);
    double r = uni(rng);
    double c = 0.0;
    for (int i = 0; i < cutoff; ++i) {
        int id = idx[i];
        c += probs[id];
        if (r <= c) return id;
    }
    return idx[0];
}
#endif

bool LlamaEngine::generate(const std::string& prompt, const GenerateOptions& options, const StreamCallback& on_token, std::string& err) {
    if (!impl_->loaded) { err = "model not loaded"; return false; }
#if AICLI_WITH_LLAMA
    auto t0 = std::chrono::steady_clock::now();
    if (!impl_->model || !impl_->ctx || !impl_->vocab) { err = "llama runtime not initialized"; return false; }
    if (!impl_->reinit_context(err)) return false; // ensure pos start from 0
    std::vector<llama_token> tokens; { const bool add_bos = true; const bool parse_special = true; std::vector<llama_token> tmp(1024 + prompt.size()); int n = llama_tokenize(impl_->vocab, prompt.c_str(), (int)prompt.size(), tmp.data(), (int)tmp.size(), add_bos, parse_special); if (n < 0) { err = "tokenize failed"; return false; } tmp.resize(n); tokens = std::move(tmp); }
    int n_past = 0; { llama_batch batch = llama_batch_init((int)tokens.size(), 0, 1); for (int i = 0; i < (int)tokens.size(); ++i) { batch.token[i] = tokens[i]; batch.pos[i] = n_past + i; batch.n_seq_id[i] = 1; batch.seq_id[i][0] = 0; batch.logits[i] = (i == (int)tokens.size() - 1); } batch.n_tokens = (int)tokens.size(); int32_t r = llama_decode(impl_->ctx, batch); llama_batch_free(batch); if (r != 0) { err = "decode failed (prefill)"; return false; } n_past += (int)tokens.size(); }
    const int eos = llama_vocab_eos(impl_->vocab); const int n_vocab = llama_vocab_n_tokens(impl_->vocab); std::mt19937 rng; if (auto sv = config::get_env("AICLI_SEED")) { try { rng.seed((uint32_t)std::stoul(*sv)); } catch (...) { std::random_device rd; rng.seed(rd()); } } else { std::random_device rd; rng.seed(rd()); }
    float temperature = options.temperature; float top_p = options.top_p;
    int gen_tokens = 0;
    for (int i = 0; i < options.max_new_tokens; ++i) {
        if (impl_->abort_requested.load(std::memory_order_relaxed)) { err = "aborted"; return false; }
        const float* logits = llama_get_logits(impl_->ctx); if (!logits) { err = "no logits"; return false; }
        int next_id; if (temperature <= 0.0001f) { int best_id = 0; float best_val = logits[0]; for (int t = 1; t < n_vocab; ++t) if (logits[t] > best_val) { best_val = logits[t]; best_id = t; } next_id = best_id; } else { if (top_p <= 0.0f || top_p > 1.0f) top_p = 0.95f; next_id = sample_top_p_temperature(logits, n_vocab, temperature, top_p, rng); }
        if (next_id == eos) break; std::string piece = token_to_piece(impl_->vocab, next_id); if (!piece.empty()) on_token(piece);
        llama_batch step = llama_batch_init(1, 0, 1); step.token[0] = (llama_token)next_id; step.pos[0] = n_past; step.n_seq_id[0] = 1; step.seq_id[0][0] = 0; step.logits[0] = true; step.n_tokens = 1; int32_t r = llama_decode(impl_->ctx, step); llama_batch_free(step); if (r != 0) { err = "decode failed (loop)"; return false; } n_past += 1; ++gen_tokens; }
    auto t1 = std::chrono::steady_clock::now();
    double ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    double tps = ms > 0 ? (gen_tokens * 1000.0 / ms) : 0.0;
    auto [p50,p95] = sysbox::add_duration_sample("inference.generate.ms", ms);
    sysbox::record_json("metrics","info", std::string("{\"tokens\":") + std::to_string(gen_tokens) + ",\"ms\":" + std::to_string(ms) + ",\"tokens_per_s\":" + std::to_string(tps) + ",\"p50\":" + std::to_string(p50) + ",\"p95\":" + std::to_string(p95) + "}");
#else
    std::string fake = "[llama-stub] 你说：" + prompt + " -> 我理解了。"; for (char c : fake) { on_token(std::string(1, c)); std::this_thread::sleep_for(std::chrono::milliseconds(2)); } (void)options; (void)err;
#endif
    return true;
}

bool LlamaEngine::generate_with_session(const std::string& session_id,
                                        const std::string& prompt,
                                        const GenerateOptions& options,
                                        const StreamCallback& on_token,
                                        std::string& err) {
#if !AICLI_WITH_LLAMA
    // 无依赖回退
    return generate(prompt, options, on_token, err);
#else
    if (!impl_->model || !impl_->ctx || !impl_->vocab) { err = "llama runtime not initialized"; return false; }

    // 会话切换：若切换到新的会话，先重建上下文，再初始化该会话状态
    {
        std::unique_lock<std::mutex> lk(impl_->mu);
        const bool switched = (impl_->active_session != session_id);
        lk.unlock();
        if (switched) {
            if (!impl_->reinit_context(err)) return false;
            std::unique_lock<std::mutex> lk2(impl_->mu);
            impl_->active_session = session_id;
            auto& st_new = impl_->sessions[session_id];
            st_new.n_past = 0;
            st_new.last_tokens.clear();
            lk2.unlock();
        }
    }

    // tokenize prompt
    std::vector<llama_token> tokens;
    {
        const bool add_bos = true; const bool parse_special = true;
        std::vector<llama_token> tmp(1024 + prompt.size());
        int n = llama_tokenize(impl_->vocab, prompt.c_str(), (int)prompt.size(), tmp.data(), (int)tmp.size(), add_bos, parse_special);
        if (n < 0) { err = "tokenize failed"; return false; }
        tmp.resize(n); tokens = std::move(tmp);
    }

    // 计算与上一次的最长公共前缀；若历史变化，重建上下文
    int lcp = 0; size_t prev_hist_size = 0;
    {
        std::unique_lock<std::mutex> lk2(impl_->mu);
        auto it = impl_->sessions.find(session_id);
        if (it == impl_->sessions.end()) { impl_->sessions[session_id] = {}; it = impl_->sessions.find(session_id); }
        auto& st = it->second;
        prev_hist_size = st.last_tokens.size();
        while (lcp < (int)st.last_tokens.size() && lcp < (int)tokens.size() && st.last_tokens[lcp] == tokens[lcp]) { ++lcp; }
        lk2.unlock();
    }
    if ((size_t)lcp < prev_hist_size) {
        if (!impl_->reinit_context(err)) return false;
        std::unique_lock<std::mutex> lk3(impl_->mu);
        auto& st = impl_->sessions[session_id];
        st.n_past = 0; st.last_tokens.clear();
        lk3.unlock();
        lcp = 0;
    }

    // 先补齐差额 tokens
    if (lcp < (int)tokens.size()) {
        llama_batch batch = llama_batch_init((int)(tokens.size() - lcp), 0, 1);
        for (int i = lcp; i < (int)tokens.size(); ++i) {
            int bi = i - lcp;
            batch.token[bi] = tokens[i];
            int base_pos = 0; {
                std::unique_lock<std::mutex> lk4(impl_->mu);
                base_pos = impl_->sessions[session_id].n_past;
            }
            batch.pos[bi] = base_pos + bi;
            batch.n_seq_id[bi] = 1;
            batch.seq_id[bi][0] = 0;
            batch.logits[bi] = (i == (int)tokens.size() - 1);
        }
        batch.n_tokens = (int)(tokens.size() - lcp);
        if (llama_decode(impl_->ctx, batch) != 0) { llama_batch_free(batch); err = "decode failed (prefill session)"; return false; }
        llama_batch_free(batch);
        std::unique_lock<std::mutex> lk5(impl_->mu);
        impl_->sessions[session_id].n_past += (int)(tokens.size() - lcp);
        lk5.unlock();
    }

    {
        std::unique_lock<std::mutex> lk6(impl_->mu);
        impl_->sessions[session_id].last_tokens = tokens;
    }

    const int eos = llama_vocab_eos(impl_->vocab); const int n_vocab = llama_vocab_n_tokens(impl_->vocab);

    std::mt19937 rng; if (auto sv = config::get_env("AICLI_SEED")) { try { rng.seed((uint32_t)std::stoul(*sv)); } catch (...) { std::random_device rd; rng.seed(rd()); } } else { std::random_device rd; rng.seed(rd()); }
    float temperature = options.temperature; float top_p = options.top_p;

    for (int i = 0; i < options.max_new_tokens; ++i) {
        if (impl_->abort_requested.load(std::memory_order_relaxed)) { err = "aborted"; return false; }
        const float* logits = llama_get_logits(impl_->ctx); if (!logits) { err = "no logits"; return false; }
        int next_id; if (temperature <= 0.0001f) { int best_id = 0; float best_val = logits[0]; for (int t = 1; t < n_vocab; ++t) if (logits[t] > best_val) { best_val = logits[t]; best_id = t; } next_id = best_id; }
        else { if (top_p <= 0.0f || top_p > 1.0f) top_p = 0.95f; next_id = sample_top_p_temperature(logits, n_vocab, temperature, top_p, rng); }
        if (next_id == eos) break; std::string piece = token_to_piece(impl_->vocab, next_id); if (!piece.empty()) on_token(piece);
        llama_batch step = llama_batch_init(1, 0, 1); step.token[0] = (llama_token)next_id; {
            std::unique_lock<std::mutex> lk7(impl_->mu); step.pos[0] = impl_->sessions[session_id].n_past; }
        step.n_seq_id[0] = 1; step.seq_id[0][0] = 0; step.logits[0] = true; step.n_tokens = 1;
        if (llama_decode(impl_->ctx, step) != 0) { llama_batch_free(step); err = "decode failed (loop session)"; return false; }
        llama_batch_free(step);
        std::unique_lock<std::mutex> lk8(impl_->mu); impl_->sessions[session_id].n_past += 1; impl_->sessions[session_id].last_tokens.push_back((llama_token)next_id); lk8.unlock();
    }
    return true;
#endif
}

void LlamaEngine::reset_session(const std::string& session_id) {
#if AICLI_WITH_LLAMA
    std::lock_guard<std::mutex> lk(impl_->mu);
    impl_->sessions.erase(session_id);
    if (impl_->active_session == session_id) impl_->active_session.clear();
#endif
}

void LlamaEngine::request_abort() {
#if AICLI_WITH_LLAMA
    impl_->abort_requested.store(true, std::memory_order_relaxed);
#endif
}

std::unique_ptr<Engine> create_local_engine() { return std::unique_ptr<Engine>(new LlamaEngine()); }

} // namespace inference
