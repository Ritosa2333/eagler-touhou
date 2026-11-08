#include "PauseFrame.hpp"

namespace th13 {

PauseFrame::PauseFrame(PauseState& s, SessionState& p, PauseMenu& m, FrameScheduler& f)
    : state(s), progress(p), menu(m), scheduler(f) {
    callback.owner = this;
    callback.enabled = false;
    callback.run = [](void* p) {
        auto& frame = *static_cast<PauseFrame*>(p);
        return frame.update(frame.input) ? i32(FrameAction::Continue) : i32(FrameAction::Error);
    };
    // 暂停菜单在场景更新之后驱动（标题主回调 6、场景回调 20 之后）。
    if (scheduler.add(callback, FramePass::Update, 11) < 0) fail("Pause callback registration failed");
}

PauseFrame::~PauseFrame() { scheduler.remove(callback); }

bool PauseFrame::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Pause frame unavailable" : why;
    return false;
}

bool PauseFrame::update(const PauseFrameInput& value) {
    if (!error.empty() || !menu.error.empty()) return false;
    if (state.screen == PauseScreen::Inactive) return true;
    if (!menu.update(value.pressed, value.repeated)) return fail(menu.error);
    state.age.tick(&progress.rate);  // 入场延迟/收场延迟共用暂停时钟
    return true;
}

}  // namespace th13
