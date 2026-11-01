#include "../harness/test_framework.hpp"
#include "game/StageBattle.hpp"
#include "game/GameInput.hpp"

#include <cmath>
#include <limits>

using namespace th13;

namespace {
struct Harness {
    StageBattle battle;
    GameInput input;
    Harness() { battle.set_player_pos(0, 200); battle.sim.set_time_rate(1.f); }
    void step() { CHECK(battle.step_transition(input)); }
};
}

TEST(no_touch_preserves_original_keyboard_movement) {
    Harness h; h.input.held = 0x80; h.step();
    CHECK_NEAR(h.battle.player_x, 576.f / 128.f, 1e-6);
    CHECK_EQ(h.battle.player_y, 200.f);
}

TEST(touch_limited_uses_fixed_point_and_speed) {
    Harness h; h.battle.touch = {1, 100.f, 300.f}; h.step();
    CHECK_NEAR(h.battle.player_x, 407.f / 128.f, 1e-6);
    CHECK_NEAR(h.battle.player_y, 200.f + 407.f / 128.f, 1e-6);
}

TEST(touch_focus_and_time_rate_limit) {
    Harness h; h.input.held = 0x08; h.battle.sim.set_time_rate(.5f);
    h.battle.touch = {1, 100.f, 200.f}; h.step();
    CHECK_NEAR(h.battle.player_x, 128.f / 128.f, 1e-6);
}

TEST(touch_unlimited_and_extreme_target_clamp) {
    Harness h; h.battle.touch = {2, 1e30f, 1e30f}; h.step();
    CHECK_EQ(h.battle.player_x, 184.f); CHECK_EQ(h.battle.player_y, 432.f);
}

TEST(keyboard_direction_overrides_touch) {
    Harness h; h.input.held = 0x80; h.battle.touch = {2, -100.f, 300.f}; h.step();
    CHECK_NEAR(h.battle.player_x, 576.f / 128.f, 1e-6);
    CHECK_EQ(h.battle.player_y, 200.f);
}

TEST(invalid_touch_target_preserves_keyboard_path) {
    Harness h; h.input.held = 0x80;
    h.battle.touch = {2, std::numeric_limits<float>::quiet_NaN(), 300.f}; h.step();
    CHECK_NEAR(h.battle.player_x, 576.f / 128.f, 1e-6);
}

TEST(touch_is_blocked_during_deathbomb) {
    Harness h; h.battle.player_phase = StageBattle::PlayerPhase::Deathbomb;
    h.battle.player_phase_frame = 0; h.battle.touch = {2, 100.f, 300.f}; h.step();
    CHECK_EQ(h.battle.player_x, 0.f); CHECK_EQ(h.battle.player_y, 200.f);
}

int main() { return th13test::run_all("touch movement"); }
