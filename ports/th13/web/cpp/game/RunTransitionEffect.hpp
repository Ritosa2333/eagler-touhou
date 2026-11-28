#pragma once

#include "AnmManager.hpp"
#include "AnmOverlay.hpp"

#include <string>
#include <utility>

namespace th13 {

class RunTransitionEffect final {
public:
    static constexpr i32 kEffectBank = 8;
    static constexpr i32 kRootScript = 0;
    static constexpr i32 kLayer = 26;

    explicit RunTransitionEffect(AnmManager& animations) : animations_(animations) {}
    RunTransitionEffect(const RunTransitionEffect&) = delete;
    RunTransitionEffect& operator=(const RunTransitionEffect&) = delete;

    bool create();
    bool adopt(u32 handle);
    u32 detach() noexcept {
        const u32 handle = handle_;
        handle_ = 0;
        return handle;
    }
    bool stage_start();
    bool release();

    u32 handle() const noexcept { return handle_; }
    bool active() const noexcept { return animations_.registry.find(handle_) != nullptr; }
    std::string error;

private:
    bool fail(const std::string& why) {
        if (error.empty()) error = why.empty() ? "Run transition effect failed" : why;
        return false;
    }

    AnmManager& animations_;
    u32 handle_ = 0;
};

inline bool RunTransitionEffect::create() {
    error.clear();
    if (handle_ && !release()) return false;
    AnmResource* effect = animations_.resource(kEffectBank);
    if (!effect) return fail("Run transition effect bank unavailable");
    u32 handle = animations_.create(kEffectBank, kRootScript, -1, 0, {});
    if (!handle) return fail(animations_.error.empty() ? "Run transition effect create failed" : animations_.error);
    AnmVm* root = animations_.registry.find(handle);
    if (!root) {
        animations_.retire(handle);
        return fail("Run transition effect root missing after create");
    }
    if (!animations_.spawn_effect(*root, 0)) {
        animations_.retire(handle);
        return fail(animations_.error.empty() ? "Run transition overlay initialization failed" : animations_.error);
    }
    root->visual.layer = kLayer;
    if (!animations_.interrupt(handle, 7, false)) {
        animations_.retire(handle);
        return fail(animations_.error.empty() ? "Run transition label7 failed" : animations_.error);
    }
    handle_ = handle;
    return true;
}

inline bool RunTransitionEffect::adopt(u32 handle) {
    if (handle == handle_) return true;
    if (!handle) return true;
    if (handle_ && !release()) return false;
    handle_ = handle;
    return true;
}

inline bool RunTransitionEffect::stage_start() {
    if (!handle_) return true;
    if (!animations_.interrupt(handle_, 1, false))
        return fail(animations_.error.empty() ? "Run transition label1 failed" : animations_.error);
    return true;
}

inline bool RunTransitionEffect::release() {
    if (!handle_) return true;
    u32 handle = handle_;
    if (!animations_.retire(handle))
        return fail(animations_.error.empty() ? "Run transition retire failed" : animations_.error);
    handle_ = 0;
    return true;
}

}
