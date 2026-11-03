// PR159 真实触控策略 + StageBattle 移动专项（合成状态，不读私有资产）。
// 覆盖：disabled 不移动、限速/焦点速度上限、unlimited、边界、键盘优先、
// 死亡/Deathbomb/回放门控、reset/cancel 生命周期语义。TouchController 与
// StageBattle 均使用生产实现。
// TOUCH-10 未在此伪造：th13 当前没有 cheat_movement_used / processing-drop
// 字段，Result/Replay 100% 元数据仍未接线（见最终报告），本测试只验证移动。
#include "../harness/test_framework.hpp"

#include "../../portable/input/TouchController.hpp"
#include "game/GameInput.hpp"
#include "game/StageBattle.hpp"

#include <cmath>

using namespace th13;
using touhou::input::TouchController;
using touhou::input::TouchSample;
using touhou::input::TouchState;

namespace {

TouchState play_state() {
    TouchState s;
    s.context = 1;
    s.instance = 1;
    s.ready = true;
    s.x = 0;
    s.y = 200;
    s.fast = 4.5f;   // speed_orth 576 / 128
    s.slow = 2.0f;   // speed_orth_focus 256 / 128
    s.min_x = -184;
    s.max_x = 184;
    s.min_y = 32;
    s.max_y = 432;
    return s;
}

void drag(TouchController& touch, const TouchState& state, float to_x = .6f, float to_y = .6f) {
    touch.pointer(0, 1, .5f, .5f, 0, state, false);
    touch.pointer(1, 1, to_x, to_y, 10, state, false);
}

struct MovementHarness {
    // 共享 StagePlayerState：0x442a19-0x442a57 的 focus 三重门读
    // player+0x67c（startup 计数），需要从外部置位。
    StagePlayerState state;
    StageBattle battle{nullptr, &state};
    GameInput input;

    MovementHarness() {
        battle.set_player_pos(0, 200);
        battle.sim.set_time_rate(1.0f);
    }

    void step() { CHECK(battle.step_transition(input)); }
};

}  // namespace

TEST(touch_disabled_produces_no_motion_or_actions) {
    TouchController touch;
    touch.enabled = false;
    const auto state = play_state();
    drag(touch, state);
    const TouchSample out = touch.sample(state, 20, false, false);
    CHECK_EQ(out.motion, 0);
    CHECK(!out.keys[90]);  // Z / fire
    CHECK(!out.keys[16]);  // Shift / focus
    CHECK(!touch.active());
}

TEST(touch_keyboard_arrows_take_movement_ownership) {
    TouchController touch;
    const auto state = play_state();
    drag(touch, state);
    CHECK(touch.active());
    const TouchSample out = touch.sample(state, 20, false, true);
    CHECK_EQ(out.motion, 0);
    CHECK(!touch.active());
}

TEST(touch_death_and_replay_context_gate_motion) {
    TouchController touch;
    auto state = play_state();
    drag(touch, state);
    CHECK(touch.sample(state, 20, false, false).motion != 0);

    // Death / respawn: movement output is gated even if the finger remains.
    auto dead = state;
    dead.ready = false;
    const TouchSample dead_out = touch.sample(dead, 30, false, false);
    CHECK_EQ(dead_out.motion, 0);
    CHECK(!dead_out.keys[90]);

    // Replay: context 3 never produces movement/action keys.
    TouchController replay;
    auto play = play_state();
    drag(replay, play);
    CHECK(replay.sample(play, 20, false, false).motion != 0);
    auto replay_state = play;
    replay_state.context = 3;
    const TouchSample replay_out = replay.sample(replay_state, 30, false, false);
    CHECK_EQ(replay_out.motion, 0);
    CHECK(!replay_out.keys[90]);
    CHECK(!replay_out.keys[16]);
    CHECK(!replay_out.keys[37]);
    CHECK(!replay_out.keys[38]);
    CHECK(!replay_out.keys[39]);
    CHECK(!replay_out.keys[40]);
}

TEST(touch_deathbomb_preserves_drag_only_during_deathbomb) {
    TouchController touch;
    auto play = play_state();
    drag(touch, play);
    CHECK(touch.sample(play, 20, false, false).motion != 0);

    // Deathbomb: movement is blocked but the held drag stays owned.
    auto deathbomb = play;
    deathbomb.ready = false;
    deathbomb.retain_motion = true;
    touch.pointer(1, 1, .7f, .7f, 30, deathbomb, false);
    const TouchSample blocked = touch.sample(deathbomb, 40, false, false);
    CHECK_EQ(blocked.motion, 0);
    CHECK(touch.active());

    // Recovery to Active resumes the same drag.
    auto active = play;
    touch.pointer(1, 1, .8f, .8f, 50, active, false);
    const TouchSample resumed = touch.sample(active, 60, false, false);
    CHECK(resumed.motion != 0);
    CHECK(touch.active());

    // Dying/Entering（裁决 2026-11-02）：retain_motion=false 时移动被压制
    //（motion=0）但拖拽所有权按上游 held-touch 契约保留（TouchController.hpp
    // pointer() 分支注释：『只压制位移，不丢弃运动手指』，c5bf4712d 对齐），
    // ready 后同指继续接管（sample() motion_blocked 解除）。旧断言『丢弃且
    // 不得复活』与实现契约矛盾（4fc0f57ce 自相矛盾提交），按实现裁决更新。
    TouchController dying;
    auto p = play_state();
    drag(dying, p);
    CHECK(dying.sample(p, 20, false, false).motion != 0);
    auto dead = p;
    dead.ready = false;
    dead.retain_motion = false;
    CHECK_EQ(dying.sample(dead, 30, false, false).motion, 0);
    CHECK(dying.active());
    // ready 后同指继续接管（sample() 解除 motion_blocked，恢复位移）。
    CHECK(dying.sample(p, 40, false, false).motion != 0);
    CHECK(dying.active());
}

TEST(touch_cancel_and_reset_have_distinct_control_semantics) {
    TouchController touch;
    touch.mode = 2;  // digital joystick: stick also emits an arrow key
    touch.controls(true, true, 0, 0, 32767, 0);
    const auto state = play_state();
    const TouchSample held = touch.sample(state, 10, false, false);
    CHECK(held.keys[90]);  // fire
    CHECK(held.keys[16]);  // focus
    CHECK(held.keys[39]);  // stick right

    // cancel() 裁决（2026-11-02，TouchController.hpp:31-34 注释）：cancel 丢弃
    // 瞬态手势与控制所有权（focus/stick 清零），fire 是文档级开关存续。
    touch.cancel();
    const TouchSample after_cancel = touch.sample(state, 20, false, false);
    CHECK(after_cancel.keys[90]);
    CHECK(!after_cancel.keys[16]);
    CHECK(!after_cancel.keys[39]);

    // reset() clears all transient controls as well.
    touch.reset();
    const TouchSample after_reset = touch.sample(state, 30, false, false);
    CHECK(!after_reset.keys[90]);
    CHECK(!after_reset.keys[16]);
    CHECK(!after_reset.keys[37]);
    CHECK(!after_reset.keys[38]);
    CHECK(!after_reset.keys[39]);
    CHECK(!after_reset.keys[40]);
}

TEST(touch_unlimited_reports_unlimited_motion) {
    TouchController touch;
    touch.unlimited = true;
    const auto state = play_state();
    drag(touch, state);
    const TouchSample out = touch.sample(state, 20, false, false);
    CHECK_EQ(out.motion, 2);
}

TEST(stagebattle_disabled_touch_does_not_move) {
    MovementHarness h;
    h.battle.touch = {0, 100.f, 300.f};
    h.step();
    CHECK_EQ(h.battle.player_x, 0.f);
    CHECK_EQ(h.battle.player_y, 200.f);
}

TEST(stagebattle_rate_limited_touch_caps_speed) {
    MovementHarness h;
    h.battle.touch = {1, 100.f, 300.f};
    h.step();
    // 目标差 (12800,12800) fixed，限速 576 fixed；等分后 407 fixed/轴。
    CHECK_NEAR(h.battle.player_x, 407.f / 128.f, 1e-6);
    CHECK_NEAR(h.battle.player_y, 200.f + 407.f / 128.f, 1e-6);
    CHECK(std::hypot(h.battle.player_x, h.battle.player_y - 200.f) <= 576.f / 128.f + 1e-6);
}

TEST(stagebattle_focus_touch_caps_at_focus_speed) {
    MovementHarness h;
    h.input.held = 0x08;  // Shift / focus
    // 0x442a19-0x442a57 focus 三重门：ctl+0xb8（存活敌机链计数）非 0 且
    // player+0x67c>=4 同时成立才保留 Shift 位。补 inert 敌机（threads 为空，
    // tick_enemy 跳过解释器；位置远离玩家）并置 startup 计数。
    auto focus_anchor = std::make_unique<StageEnemy>();
    focus_anchor->serial = 9999; focus_anchor->script_id = 9999;
    focus_anchor->position[0] = 1000.f; focus_anchor->position[1] = 1000.f;
    h.battle.sim.enemies.push_back(std::move(focus_anchor));
    h.state.startup_clock.set(25);
    h.battle.touch = {1, 100.f, 300.f};
    h.step();
    CHECK_NEAR(h.battle.player_x, 181.f / 128.f, 1e-6);
    CHECK_NEAR(h.battle.player_y, 200.f + 181.f / 128.f, 1e-6);
    CHECK(std::hypot(h.battle.player_x, h.battle.player_y - 200.f) <= 256.f / 128.f + 1e-6);
}

TEST(stagebattle_unlimited_touch_reaches_target) {
    MovementHarness h;
    h.battle.touch = {2, 100.f, 300.f};
    h.step();
    CHECK_EQ(h.battle.player_x, 100.f);
    CHECK_EQ(h.battle.player_y, 300.f);
}

TEST(stagebattle_unlimited_touch_clamps_to_playfield_bounds) {
    MovementHarness h;
    h.battle.touch = {2, 999.f, 999.f};
    h.step();
    CHECK_EQ(h.battle.player_x, 184.f);
    CHECK_EQ(h.battle.player_y, 432.f);
}

TEST(stagebattle_keyboard_arrows_override_touch_target) {
    MovementHarness h;
    h.input.held = 0x80;  // right arrow
    h.battle.touch = {1, 100.f, 300.f};
    h.step();
    CHECK_EQ(h.battle.player_y, 200.f);
    CHECK_NEAR(h.battle.player_x, 576.f / 128.f, 1e-6);
}

TEST(stagebattle_death_and_deathbomb_gate_touch_motion) {
    {
        MovementHarness h;
        h.battle.player_dead = true;
        h.battle.touch = {2, 100.f, 300.f};
        h.step();
        CHECK_EQ(h.battle.player_x, 0.f);
        CHECK_EQ(h.battle.player_y, 200.f);
    }
    {
        MovementHarness h;
        h.battle.player_dead = false;
        h.battle.player_phase = StageBattle::PlayerPhase::Deathbomb;
        h.battle.player_phase_frame = 0;
        h.battle.touch = {2, 100.f, 300.f};
        h.step();
        CHECK_EQ(h.battle.player_x, 0.f);
        CHECK_EQ(h.battle.player_y, 200.f);
    }
}

TEST(stagebattle_replay_touch_sample_is_gated) {
    TouchController touch;
    auto state = play_state();
    drag(touch, state);
    auto replay = state;
    replay.context = 3;
    const TouchSample out = touch.sample(replay, 20, false, false);
    CHECK_EQ(out.motion, 0);

    MovementHarness h;
    h.battle.touch = {out.motion, out.x, out.y};
    h.step();
    CHECK_EQ(h.battle.player_x, 0.f);
    CHECK_EQ(h.battle.player_y, 200.f);
}

int main() { return th13test::run_all("Touch input"); }
