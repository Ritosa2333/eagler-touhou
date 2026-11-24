#define TH13_APPLICATION_PLATFORMS_ONLY
#include "../harness/test_framework.hpp"

#include "sdl/ApplicationState.hpp"

using namespace th13;
using namespace th13::sdl;

TEST(transition_gate_runs_only_on_native_player_update_frames) {
    CHECK(transition_gate_player_update_runs(false, 0u));
    CHECK(transition_gate_player_update_runs(false, 4u));
    CHECK(transition_gate_player_update_runs(false, 0x4000u));
    CHECK(!transition_gate_player_update_runs(true, 0u));
    CHECK(!transition_gate_player_update_runs(true, 4u));
    for (u32 overlay : {0x10u, 0x20u, 0x40u, 0x70u}) {
        CHECK(!transition_gate_player_update_runs(false, overlay));
        CHECK(!transition_gate_player_update_runs(true, overlay));
    }
}

TEST(transition_gate_interrupt_waits_through_pause) {
    CHECK(!transition_gate_should_interrupt(28u, false, 0u));
    CHECK(transition_gate_should_interrupt(29u, false, 0u));
    CHECK(transition_gate_should_interrupt(30u, false, 0u));
    CHECK(!transition_gate_should_interrupt(29u, true, 0u));
    CHECK(!transition_gate_should_interrupt(100u, true, 0u));
    CHECK(!transition_gate_should_interrupt(29u, false, 0x20u));
    CHECK(!transition_gate_should_interrupt(29u, false, 0x70u));
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
    CHECK_EQ(frames, 39u);
}

int main() { return th13test::run_all("transition gate pause"); }
