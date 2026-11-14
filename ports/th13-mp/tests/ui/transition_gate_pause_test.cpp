// 换关门（0xc）内暂停/覆盖层对 0x442960 隐藏计数的影响。
//
// 原版证据：0x442960 是 player update（0x443240 的移动分支）的一部分，
// 0x442f2b test [player+0x14698],8 → 0x442f34 inc [player+0x1469c]；
// 0x443017 cmp [player+0x1469c],0x1e → 0x44301e jl → 0x44302e 清槽 active
// 并对 option 根置 pending=1。environment.paused / scene_flags&0x70 的帧
// 不执行玩家更新，因此计数不推进、第 30 帧的 interrupt 也不触发。
//
// Application.cpp 的门计数与 interrupt 检查统一调用本文件的被测函数；
// 该文件只做纯函数断言，不构造 ApplicationState（SDL 设备层不在 tests/ui
// 的链接范围）。
#define TH13_APPLICATION_PLATFORMS_ONLY
#include "../harness/test_framework.hpp"

#include "sdl/ApplicationState.hpp"

using namespace th13;
using namespace th13::sdl;

TEST(transition_gate_runs_only_on_native_player_update_frames) {
    CHECK(transition_gate_player_update_runs(false, 0u));
    // 关终位 0x4 不阻止 player update（0x442960 仍在其移动分支内）。
    CHECK(transition_gate_player_update_runs(false, 4u));
    CHECK(transition_gate_player_update_runs(false, 0x4000u));
    // 暂停与 0x70 覆盖层位（0x10/0x20/0x40 任意组合）都不跑玩家更新。
    CHECK(!transition_gate_player_update_runs(true, 0u));
    CHECK(!transition_gate_player_update_runs(true, 4u));
    for (u32 overlay : {0x10u, 0x20u, 0x40u, 0x70u}) {
        CHECK(!transition_gate_player_update_runs(false, overlay));
        CHECK(!transition_gate_player_update_runs(true, overlay));
    }
}

TEST(transition_gate_interrupt_waits_through_pause) {
    // 门首帧计数 0，之后每个原生 player update 帧 +1：第 30 帧（计数 29）
    // 触发 interrupt。
    CHECK(!transition_gate_should_interrupt(28u, false, 0u));
    CHECK(transition_gate_should_interrupt(29u, false, 0u));
    CHECK(transition_gate_should_interrupt(30u, false, 0u));
    // 门内暂停：计数冻结在 29，暂停期间不得触发（原版 player update 未跑）。
    CHECK(!transition_gate_should_interrupt(29u, true, 0u));
    CHECK(!transition_gate_should_interrupt(100u, true, 0u));
    // 覆盖层帧同理。
    CHECK(!transition_gate_should_interrupt(29u, false, 0x20u));
    CHECK(!transition_gate_should_interrupt(29u, false, 0x70u));
    // 暂停恢复后计数从原值继续，不会补触发。
    u32 frames = 0;
    for (u32 frame = 0; frame < 29; ++frame) {
        if (transition_gate_player_update_runs(false, 0u)) ++frames;
    }
    CHECK_EQ(frames, 29u);
    for (u32 paused_frame = 0; paused_frame < 40; ++paused_frame) {
        const bool paused = paused_frame < 30;
        if (transition_gate_player_update_runs(paused, 0u)) ++frames;
        CHECK_EQ(transition_gate_should_interrupt(frames, paused, 0u), !paused);
    }
    CHECK_EQ(frames, 39u);  // 暂停 30 帧不推进，恢复 10 帧后 29+10
}

int main() { return th13test::run_all("transition gate pause"); }
