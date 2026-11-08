#include "game/ReplayFastForward.hpp"
#include "game/FrameScheduler.hpp"
#include <cassert>
#include <cstdio>
#include <initializer_list>
using namespace th13;

int main() {
    for (u32 key : {0x200u, 1u, 0x201u}) {
        for (u32 frame = 0; frame < 24; ++frame) {
            assert(replay_fast_forward_restart(key, frame, true, false, 0, false) ==
                   (frame % 8 != 0));
            assert(!replay_fast_forward_restart(key, frame, false, false, 0, false));
            assert(!replay_fast_forward_restart(key, frame, true, true, 0, false));
            assert(!replay_fast_forward_restart(key, frame, true, false, 4, false));
            assert(!replay_fast_forward_restart(key, frame, true, false, 0, true));
        }
    }
    assert(!replay_fast_forward_restart(0, 1, true, false, 0, false));
    assert(!replay_fast_forward_restart(8, 1, true, false, 0, false));
    struct State { u32 consumed = 0, late = 0, held = 0x200; bool restart = false; } s;
    FrameScheduler scheduler;
    FrameCallback input{}, gate{}, late{};
    for (auto* cb : {&input, &gate, &late}) { cb->owner = &s; cb->enabled = true; }
    input.run = [](void* p) -> i32 { ++static_cast<State*>(p)->consumed; return 1; };
    gate.run = [](void* p) -> i32 {
        auto& s = *static_cast<State*>(p);
        s.restart = replay_fast_forward_restart(s.held, s.consumed, true, false, 0, false);
        return s.restart ? 3 : 1;
    };
    late.run = [](void* p) -> i32 { ++static_cast<State*>(p)->late; return 1; };
    scheduler.add(input, FramePass::Update, 20);
    scheduler.add(gate, FramePass::Update, 30);
    scheduler.add(late, FramePass::Update, 34);
    do { assert(scheduler.update() >= 0); } while (s.restart);
    assert(s.consumed == 8 && s.late == 1);
    s.held = 0;
    assert(scheduler.update() >= 0);
    assert(s.consumed == 9 && s.late == 2);
    s.held = 1;
    do { assert(scheduler.update() >= 0); } while (s.restart);
    assert(s.consumed == 16 && s.late == 3);
    std::puts("replay_fast_forward_test: PASS (Ctrl/Z, 8-frame phase, frozen/EOF gates, late ANM)");
}
