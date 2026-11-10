// START-01: 标题/回放启动的 effect.anm 开门转场生命周期。
//
// 原生所有权（th13.exe 反汇编）：
//   0x415650(effect type 0) 在标题启动拍创建 effect.anm script0，随后由
//   0x402ff0 初始化 AnmOverlay（四个 effect 面板绑定 script3..6 + astral
//   面板），句柄存 DAT_004dc86c；紧接着 0x46fc30(handle, 7) 走 label7
//   （0x403630：layer=26、mode=0、面板平移复位）。
//   关卡激活 0x42c550:0x42c86a..0x42c88c 对同一句柄发 label1；0x4035fe
//   把四个面板重绑到 script7..10 并各 tick 一次，形成 native draw0 的
//   4-quad 开门批。根节点不随 TitleScene 析构；四面板结束后由普通 ANM
//   调度自然回收（0x403260 返回 1）。后续关卡向旧句柄发 label1 无作用，
//   不重新创建开局效果。
//
// 本 helper 只封装创建、label7、label1 与清理；不创建 script2 替代根。
#pragma once

#include "AnmManager.hpp"
#include "AnmOverlay.hpp"

#include <string>
#include <utility>

namespace th13 {

class RunTransitionEffect final {
public:
    static constexpr i32 kEffectBank = 8;   // effect.anm
    static constexpr i32 kRootScript = 0;
    static constexpr i32 kLayer = 26;       // 0x403630 label7 写入 root+0x24

    explicit RunTransitionEffect(AnmManager& animations) : animations_(animations) {}
    RunTransitionEffect(const RunTransitionEffect&) = delete;
    RunTransitionEffect& operator=(const RunTransitionEffect&) = delete;

    // 标题启动拍：create script0 + 0x402ff0 overlay init + label7。
    bool create();
    // TitleScene 把 raw handle 交给 run-owned owner（begin_run 转移）。
    bool adopt(u32 handle);
    // 放弃所有权但不退役（仅用于显式转移；正常路径用 adopt/release）。
    u32 detach() noexcept {
        const u32 handle = handle_;
        handle_ = 0;
        return handle;
    }
    // 关卡激活：存活句柄接受 label1；自然结束后的旧句柄保持无作用。
    bool stage_start();
    // 回标题/退出对局：整树退役。
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
    if (!effect)
        return fail("Run transition effect bank unavailable");
    u32 handle = animations_.create(kEffectBank, kRootScript, -1, 0, {});
    if (!handle)
        return fail(animations_.error.empty() ? "Run transition effect create failed"
                                              : animations_.error);
    AnmVm* root = animations_.registry.find(handle);
    if (!root) {
        animations_.retire(handle);
        return fail("Run transition effect root missing after create");
    }
    if (!animations_.spawn_effect(*root, 0)) {
        animations_.retire(handle);
        return fail(animations_.error.empty() ? "Run transition overlay initialization failed"
                                              : animations_.error);
    }
    // 0x403630 label7 在 overlay init 之后把 root layer 写成 26。port 的
    // AnmOverlay::interrupt 只复刻 mode/panel 平移，故 layer 在此显式落值。
    root->visual.layer = kLayer;
    if (!animations_.interrupt(handle, 7, false)) {
        animations_.retire(handle);
        return fail(animations_.error.empty() ? "Run transition label7 failed"
                                              : animations_.error);
    }
    handle_ = handle;
    return true;
}

inline bool RunTransitionEffect::adopt(u32 handle) {
    if (handle == handle_) return true;
    if (!handle) return true;  // no title-owned effect: keep any replay-owned one
    if (handle_ && !release()) return false;
    handle_ = handle;
    return true;
}

inline bool RunTransitionEffect::stage_start() {
    if (!handle_) return true;
    if (!animations_.interrupt(handle_, 1, false))
        return fail(animations_.error.empty() ? "Run transition label1 failed"
                                              : animations_.error);
    return true;
}

inline bool RunTransitionEffect::release() {
    if (!handle_) return true;
    u32 handle = handle_;
    if (!animations_.retire(handle))
        return fail(animations_.error.empty() ? "Run transition retire failed"
                                              : animations_.error);
    handle_ = 0;
    return true;
}

}  // namespace th13
