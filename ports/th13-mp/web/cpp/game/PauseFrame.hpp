// 暂停帧回调（th15 PauseFrame 裁剪：失焦自动暂停保留，恢复/节流延后）。
#pragma once

#include "PauseMenu.hpp"
#include "SessionState.hpp"
#include "FrameScheduler.hpp"

namespace th13 {

struct PauseFrameInput {
    u32 pressed = 0, repeated = 0;
    bool lost_focus = false;
};

class PauseFrame {
    PauseState& state;
    SessionState& progress;
    PauseMenu& menu;
    FrameScheduler& scheduler;
    FrameCallback callback;
    PauseFrameInput input;
    bool fail(const std::string&);

public:
    std::string error;
    PauseFrame(PauseState&, SessionState&, PauseMenu&, FrameScheduler&);
    ~PauseFrame();
    PauseFrame(const PauseFrame&) = delete;
    PauseFrame& operator=(const PauseFrame&) = delete;
    void controls(const PauseFrameInput& value) noexcept { input = value; }
    void enable(bool value) noexcept { callback.enabled = value; }
    bool update(const PauseFrameInput&);
};

}  // namespace th13
