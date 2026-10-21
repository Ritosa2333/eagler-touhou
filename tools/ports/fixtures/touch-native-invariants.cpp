#define TH_ENABLE_MULTIPLAYER_GAMEPLAY 1

#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
#include "AnalogMovement.cpp"
#endif
#if defined(TOUCH_NATIVE_WITH_TH10_MP_CONSUMER)
#include "InputLanes.cpp"
#include "GameInput.cpp"
#include "NetplayProtocol.cpp"
#endif

#include "TouchController.hpp"
#include "MotionTrack.hpp"

#include <cstdlib>

#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
namespace th08 {
// ResolveAnalogMovement's ordinary finite-float path consults the numeric
// mode. These values select the same fast path configured by the Runtime;
// no movement or limiting behavior is supplied by the fixture. The fallback
// traps ensure tested values stay in the production ExactFloat fast path.
Precision arithmetic_precision() noexcept { return Precision::Single; }
Rounding arithmetic_rounding() noexcept { return Rounding::NearestEven; }
Extended Extended::from_float(float) noexcept { std::abort(); }
float Extended::to_float() const noexcept { std::abort(); }
Extended operator+(Extended, Extended) noexcept { std::abort(); }
Extended operator-(Extended, Extended) noexcept { std::abort(); }
Extended operator*(Extended, Extended) noexcept { std::abort(); }
}
#endif

#include <cmath>
#include <cstdio>
#include <initializer_list>
#include <type_traits>
#include <utility>

namespace {
using touhou::input::TouchController;
using touhou::input::TouchSample;
using touhou::input::TouchState;
using touhou::input::MotionTrack;

[[noreturn]] void fail(const char* invariant, float actual = 0, float expected = 0) {
    std::fprintf(stderr, "TOUCH_NATIVE_FAIL:%s actual=%.9g expected=%.9g\n", invariant, actual, expected);
    std::exit(1);
}

void near(float actual, float expected, float epsilon, const char* invariant) {
    if (!std::isfinite(actual) || std::abs(actual - expected) > epsilon)
        fail(invariant, actual, expected);
}

void require(bool condition, const char* invariant) {
    if (!condition) fail(invariant);
}

TouchState gameplay_state() {
    TouchState state;
    state.context = 1;
    state.instance = 17;
    state.ready = true;
    state.x = 320;
    state.y = 240;
    state.fast = 4;
    state.slow = 2;
    state.min_x = -1000;
    state.min_y = -1000;
    state.max_x = 1000;
    state.max_y = 1000;
    return state;
}

void controller_reference_values() {
    const auto state = gameplay_state();
    for (const int percent : {100, 200, 300}) {
        TouchController touch;
        touch.sensitivity = static_cast<float>(percent) / 100.0f;
        touch.pointer(0, 41, .25f, .25f, 100, state, false);
        touch.pointer(1, 41, .2625f, .2625f, 101, state, false);
        const TouchSample sample = touch.sample(state, 102, false, false);
        require(sample.motion == 1, "sensitivity_drag_produces_motion");

        // th06's accepted reference maps this in-bounds finger delta to
        // (8,6), then (16,12), then (24,18) at 100/200/300 percent.
        const float scale = static_cast<float>(percent) / 100.0f;
        const float expected_dx = 8.0f * scale;
        const float expected_dy = 6.0f * scale;
        near(sample.x, state.x + expected_dx, .0002f, "sensitivity_target_x");
        near(sample.y, state.y + expected_dy, .0002f, "sensitivity_target_y");

        const TouchSample repeated = touch.sample(state, 103, false, false);
        near(repeated.x, sample.x, 0, "target_persists_until_new_input");
        near(repeated.y, sample.y, 0, "target_persists_until_new_input");

        float dx = sample.x - state.x, dy = sample.y - state.y;

#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
        bool begin = false;
        require(touch.take_player_delta(state, dx, dy, begin) && begin, "first_delta_is_gesture_begin");
        near(dx, expected_dx, .0002f, "sensitivity_delta_x");
        near(dy, expected_dy, .0002f, "sensitivity_delta_y");
        bool second_begin = true;
        require(touch.take_player_delta(state, dx, dy, second_begin), "second_delta_is_available");
        near(dx, 0, 0, "delta_is_not_replayed");
        near(dy, 0, 0, "delta_is_not_replayed");
        require(!second_begin, "gesture_begin_is_one_shot");

        // th06's fixed reference cap for the same arbitrary 4:3 direction is
        // (3.2,2.4) at speed 4 for every sensitivity. The cap changes applied
        // movement; it does not overwrite the fresh target remainder.
        th08::multiplayer::AnalogInput input{
            expected_dx, expected_dy,
            std::uint8_t(Netplay::AnalogMode::DirectTouchBegin), 0, 1, 0};
        th08::multiplayer::TouchRemainder remainder{};
        th08::PlayerMovementState player;
        player.position.x = state.x;
        player.position.y = state.y;
        player.multiplier = {1, 1};
        const th08::Vec2 minimum{-1000, -1000};
        const th08::Vec2 extent{2000, 2000};
        const th08::FrameTiming timing{1, false};
        float applied_x = 0, applied_y = 0;
        require(th08::multiplayer::ResolveAnalogMovement(input, remainder, player,
            minimum, extent, 4, timing, applied_x, applied_y), "actual_consumer_accepts_touch");
        near(applied_x, 3.2f, .0002f, "logic_step_speed_x");
        near(applied_y, 2.4f, .0002f, "logic_step_speed_y");
        near(std::hypot(applied_x, applied_y), 4, .0002f, "logic_step_speed_magnitude");
        near(applied_x / applied_y, 4.0f / 3.0f, .0002f, "arbitrary_angle_preserved");
        near(remainder.x, expected_dx, .0002f, "capped_target_remainder_x");
        near(remainder.y, expected_dy, .0002f, "capped_target_remainder_y");
        require(remainder.active == 1, "capped_target_remainder_active");
#endif
    }
}

void subpixel_follows_without_rounding() {
    const auto state = gameplay_state();
    TouchController touch;
    touch.pointer(0, 52, .3f, .4f, 200, state, false);
    touch.pointer(1, 52, .3005f, .4005f, 201, state, false);
    const auto sample = touch.sample(state, 202, false, false);
    require(sample.motion == 1, "subpixel_drag_produces_motion");
    float dx = sample.x - state.x, dy = sample.y - state.y;
#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
    bool begin = false;
    require(touch.take_player_delta(state, dx, dy, begin) && begin, "subpixel_delta_available");
#endif
    near(dx, .32f, .0001f, "subpixel_target_x");
    near(dy, .24f, .0001f, "subpixel_target_y");
    near(sample.x - state.x, .32f, .0001f, "subpixel_sample_x");
    near(sample.y - state.y, .24f, .0001f, "subpixel_sample_y");

    float limited_x = dx, limited_y = dy;
    touhou::input::limit_vector(limited_x, limited_y, 4);
    require(limited_x == dx && limited_y == dy, "subpixel_limit_is_bitwise_unchanged");

    float diagonal_x = 8, diagonal_y = 6;
    touhou::input::limit_vector(diagonal_x, diagonal_y, 4);
    near(diagonal_x, 3.2f, .0002f, "motion_limit_helper");
    near(diagonal_y, 2.4f, .0002f, "motion_limit_helper");

#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
    th08::multiplayer::AnalogInput input{
        dx, dy, std::uint8_t(Netplay::AnalogMode::DirectTouchDelta), 0, 1, 0};
    th08::multiplayer::TouchRemainder remainder{};
    th08::PlayerMovementState player;
    player.position.x = state.x;
    player.position.y = state.y;
    player.multiplier = {1, 1};
    const th08::Vec2 minimum{-1000, -1000};
    const th08::Vec2 extent{2000, 2000};
    const th08::FrameTiming timing{1, false};
    float applied_x = 0, applied_y = 0;
    require(th08::multiplayer::ResolveAnalogMovement(input, remainder, player,
        minimum, extent, 4, timing, applied_x, applied_y), "actual_consumer_accepts_subpixel");
    require(applied_x == dx && applied_y == dy, "actual_consumer_preserves_subpixel");
    near(remainder.x, dx, 0, "subpixel_remainder_x");
    near(remainder.y, dy, 0, "subpixel_remainder_y");
#endif
}

void release_paths_clear_drag() {
    const auto state = gameplay_state();
    const auto release_path = [&state](const char* invariant) {
        TouchController touch;
        touch.pointer(0, 63, .2f, .2f, 300, state, false);
        touch.pointer(1, 63, .3f, .25f, 301, state, false);
        require(touch.active(), "drag_active_before_release");
        // Both browser end events normalize to this production Controller API.
        touch.pointer(2, 63, .3f, .25f, 302, state, false);
        const auto sample = touch.sample(state, 303, false, false);
        if(touch.active()||sample.motion!=0)fail(invariant);
    };
    release_path("pointer_up_releases_drag");
    release_path("pointer_cancel_releases_drag");
}

void joystick_modes_use_the_native_logical_input_contract() {
    const auto state = gameplay_state();
    TouchController digital;
    digital.mode = 2;
    digital.controls(false, false, 0, 0, 0.8f * 32767.0f, -0.8f * 32767.0f);
    auto sample = digital.sample(state, 400, false, false);
    require(sample.keys[39] && sample.keys[38], "digital_joystick_right_up");
    require(!sample.keys[37] && !sample.keys[40] && !sample.motion, "digital_joystick_has_no_opposite_or_analog_motion");

    digital.controls(false, false, 0, 0, 0.2f * 32767.0f, -0.2f * 32767.0f);
    sample = digital.sample(state, 401, false, false);
    require(!sample.keys[37] && !sample.keys[38] && !sample.keys[39] && !sample.keys[40],
        "digital_joystick_dead_zone");

    TouchController free;
    free.mode = 3;
    free.controls(false, false, 0, 0, 0.6f * 32767.0f, -0.8f * 32767.0f);
    sample = free.sample(state, 402, false, false);
    require(sample.motion == 1, "free_joystick_is_analog_motion");
    near(sample.x, state.x + 2.4f, .0002f, "free_joystick_x");
    near(sample.y, state.y - 3.2f, .0002f, "free_joystick_y");

    free.controls(false, true, 0, 0, 0.6f * 32767.0f, -0.8f * 32767.0f);
    sample = free.sample(state, 403, false, false);
    require(sample.keys[16], "free_joystick_focus_state");
    near(sample.x, state.x + 1.2f, .0002f, "free_joystick_focus_x");
    near(sample.y, state.y - 1.6f, .0002f, "free_joystick_focus_y");
}

void touch_focus_actions_and_bomb_do_not_retire_drag_ownership() {
    const auto state = gameplay_state();
    TouchController touch;
    touch.pointer(0, 71, .25f, .25f, 500, state, false);
    // Focus scales future pointer deltas; it does not rewrite an existing target.
    touch.controls(true, true, 1, 1, 0, 0);
    touch.pointer(1, 71, .2625f, .2625f, 501, state, false);

    auto sample = touch.sample(state, 502, false, false);
    require(sample.motion == 1 && touch.active(), "bomb_and_focus_keep_drag_owner");
    require(sample.keys[90] && sample.keys[16] && sample.keys[88] && sample.keys[27],
        "fire_focus_bomb_pause_use_logical_buttons");
    near(sample.x, state.x + 4, .0002f, "focused_drag_target_x");
    near(sample.y, state.y + 3, .0002f, "focused_drag_target_y");

    for (int tick = 0; tick < 2; ++tick) {
        sample = touch.sample(state, 503 + tick, false, false);
        require(sample.keys[88] && sample.keys[27] && touch.active(), "serial_action_pulses_keep_drag_owner");
    }
    sample = touch.sample(state, 505, false, false);
    require(!sample.keys[88] && !sample.keys[27] && touch.active(), "serial_actions_expire_without_cancelling_drag");

    touch.unlimited = true;
    sample = touch.sample(state, 506, false, false);
    require(sample.motion == 2 && touch.active(), "unlimited_direct_mode_is_distinct");
}

void bomb_pulse_remains_available_while_movement_is_blocked() {
    auto state = gameplay_state();
    TouchController touch;
    touch.pointer(0, 72, .25f, .25f, 510, state, false);
    touch.pointer(1, 72, .2625f, .2625f, 511, state, false);
    state.ready = false;
    touch.controls(false, false, 1, 0, 0, 0);
    const auto sample = touch.sample(state, 512, false, false);
    require(sample.motion == 0, "blocked_bomb_suppresses_movement");
    require(sample.keys[88], "bomb_pulse_during_blocked_movement");
}

void double_tap_bomb_is_an_action_from_a_secondary_finger() {
    const auto state = gameplay_state();
    TouchController touch;
    touch.double_tap = true;
    touch.pointer(0, 81, .25f, .25f, 600, state, false);
    touch.pointer(1, 81, .2625f, .2625f, 601, state, false);
    require(touch.sample(state, 602, false, false).motion == 1, "double_tap_fixture_has_moving_primary");

    touch.pointer(0, 82, .4f, .4f, 700, state, false);
    touch.pointer(2, 82, .4f, .4f, 710, state, false);
    touch.pointer(0, 82, .4f, .4f, 800, state, false);
    const auto sample = touch.sample(state, 801, false, false);
    require(sample.keys[88] && sample.motion == 1 && touch.active(), "double_tap_bomb_keeps_primary_movement");
}

void menu_and_dialogue_gestures_resolve_to_logical_keys() {
    TouchState menu = gameplay_state();
    menu.context = 0;
    TouchController tap;
    tap.pointer(0, 91, .5f, .5f, 900, menu, false);
    tap.pointer(2, 91, .5f, .5f, 910, menu, false);
    require(tap.sample(menu, 911, false, false).keys[90], "menu_short_tap_confirms");

    TouchController back;
    back.pointer(0, 92, .5f, .5f, 920, menu, false);
    back.pointer(0, 93, .5f, .5f, 921, menu, false);
    back.pointer(2, 92, .5f, .5f, 922, menu, false);
    require(back.sample(menu, 923, false, false).keys[27], "menu_two_finger_tap_cancels");

    TouchState dialogue = gameplay_state();
    dialogue.context = 2;
    TouchController confirm;
    confirm.pointer(0, 95, .5f, .5f, 1000, dialogue, false);
    confirm.pointer(2, 95, .5f, .5f, 1200, dialogue, false);
    require(confirm.sample(dialogue, 1201, false, false).keys[90], "dialogue_short_tap_confirms");

    TouchController skip;
    skip.pointer(0, 96, .5f, .5f, 1300, dialogue, false);
    auto sample = skip.sample(dialogue, 1800, false, false);
    require(sample.keys[17] && !sample.keys[90], "dialogue_long_hold_uses_skip");
    skip.pointer(2, 96, .5f, .5f, 1801, dialogue, false);
    sample = skip.sample(dialogue, 1802, false, false);
    require(!sample.keys[17], "dialogue_release_clears_skip");
}

void modal_confirm_requires_a_new_touch_after_gameplay_fire() {
    auto gameplay = gameplay_state();
    TouchController touch;
    touch.controls(true, false, 0, 0, 0, 0);
    require(touch.sample(gameplay, 1810, false, false).keys[90], "gameplay_fire_is_held_before_modal");

    auto dialogue = gameplay;
    dialogue.context = 2;
    require(!touch.sample(dialogue, 1811, false, false).keys[90], "modal_does_not_inherit_gameplay_fire");
    touch.pointer(0, 97, .5f, .5f, 1812, dialogue, false);
    touch.pointer(2, 97, .5f, .5f, 1912, dialogue, false);
    require(touch.sample(dialogue, 1913, false, false).keys[90], "modal_confirm_fresh_sample_edge");
}

template <typename T, typename = void>
struct has_choice_state : std::false_type {};

template <typename T>
struct has_choice_state<T, std::void_t<decltype(std::declval<T&>().choice)>> : std::true_type {};

template <typename State>
void choice_swipe_is_one_shot_when_production_state_supports_it(State choice) {
    if constexpr (has_choice_state<State>::value) {
        choice.context = 2;
        choice.choice = true;
        TouchController navigate;
        navigate.pointer(0, 94, .5f, .5f, 930, choice, false);
        navigate.pointer(1, 94, .56f, .5f, 931, choice, false);
        auto sample = navigate.sample(choice, 932, false, false);
        require(sample.keys[39] && !sample.keys[37] && !sample.keys[38] && !sample.keys[40],
            "choice_swipe_one_shot");
        sample = navigate.sample(choice, 933, false, false);
        require(!sample.keys[39] && !sample.keys[37] && !sample.keys[38] && !sample.keys[40],
            "choice_swipe_direction_is_one_shot");
        navigate.pointer(2, 94, .56f, .5f, 934, choice, false);
        std::puts("touch native behavior: choice-swipe-one-shot");
    } else {
        std::puts("touch native behavior unavailable: choice-swipe-one-shot (production TouchState has no choice state)");
    }
}

template <typename T, typename = void>
struct has_cheat_movement_marker : std::false_type {};

template <typename T>
struct has_cheat_movement_marker<T, std::void_t<
    decltype(std::declval<T&>().cheat_movement_used),
    decltype(std::declval<T&>().mark_cheat_movement(0.0f, 0.0f))>> : std::true_type {};

template <typename Track>
void verify_unlimited_processing_drop_marker() {
    if constexpr (has_cheat_movement_marker<Track>::value) {
        Track motion;
        motion.target(1, 4, 0);
        motion.mark_cheat_movement(4, 0);
        require(!motion.cheat_movement_used, "limited_movement_does_not_mark_unlimited_use");

        motion.target(2, 0, 0);
        motion.mark_cheat_movement(0, 0);
        require(!motion.cheat_movement_used, "zero_unlimited_movement_does_not_mark_use");

        motion.target(2, .25f, 0);
        motion.mark_cheat_movement(.25f, 0);
        require(motion.cheat_movement_used, "nonzero_unlimited_movement_marks_use");
        motion.clear();
        require(!motion.cheat_movement_used, "new_run_clears_unlimited_use_marker");
        std::puts("touch native: unlimited processing-drop marker API executed");
    } else {
        std::puts("touch native: unlimited processing-drop marker API unavailable in production MotionTrack");
    }
}

void unlimited_processing_drop_marker_is_consumption_driven() {
    verify_unlimited_processing_drop_marker<MotionTrack>();
}

#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
void focused_original_speed_is_retained() {
    th08::multiplayer::AnalogInput input{
        8, 6, std::uint8_t(Netplay::AnalogMode::DirectTouch), 0, 1, 0};
    th08::multiplayer::TouchRemainder remainder{};
    th08::PlayerMovementState player;
    player.position = {};
    player.multiplier = {1, 1};
    const th08::Vec2 minimum{-1000, -1000};
    const th08::Vec2 extent{2000, 2000};
    const th08::FrameTiming timing{1, false};
    float x = 0, y = 0;
    require(th08::multiplayer::ResolveAnalogMovement(input, remainder, player,
        minimum, extent, 2, timing, x, y), "focused_consumer_accepts_touch");
    near(x, 1.6f, .0002f, "focused_speed_x");
    near(y, 1.2f, .0002f, "focused_speed_y");
    near(std::hypot(x, y), 2, .0002f, "focused_speed_magnitude");
}
#endif

#if defined(TOUCH_NATIVE_WITH_TH10_MP_CONSUMER)
void th10_mp_fixed_point_touch_quantization_stays_inside_speed() {
    using Netplay::AnalogMode;
    using th10::multiplayer::InputLanes::ResolveMovement;
    Netplay::FrameInput diagonal{};
    diagonal.analogMode = AnalogMode::DirectTouch;
    diagonal.x = 293;
    diagonal.y = 273;
    int x = 0, y = 0;
    require(ResolveMovement(diagonal, 400, 0, 0, 1, x, y), "th10_mp_direct_touch_resolves");
    // Independent fixed reference: (293,273) exceeds the radius; (292,273)
    // is the closest integer point inside the 400-unit speed disc.
    require(x == 292 && y == 273, "th10_mp_quantized_speed_cap");
    require(std::hypot(static_cast<float>(x), static_cast<float>(y)) <= 400,
        "th10_mp_quantized_speed_cap");

    Netplay::FrameInput micro{};
    micro.analogMode = AnalogMode::DirectTouchDelta;
    micro.x = 30;
    micro.y = 40;
    require(ResolveMovement(micro, 400, 0, 0, 1, x, y), "th10_mp_micro_touch_resolves");
    require(x == 30 && y == 40, "th10_mp_micro_touch_not_normalized");

    Netplay::FrameInput joystick{};
    joystick.analogMode = AnalogMode::Joystick;
    joystick.x = joystick.y = 1;
    require(ResolveMovement(joystick, 400, 0, 0, 1, x, y), "th10_mp_joystick_resolves");
    require(x == 282 && y == 283, "th10_mp_joystick_quantized_cap");
    require(std::hypot(static_cast<float>(x), static_cast<float>(y)) <= 400,
        "th10_mp_joystick_quantized_cap");

    diagonal.unlimited = true;
    require(ResolveMovement(diagonal, 400, 0, 0, 1, x, y), "th10_mp_unlimited_touch_resolves");
    require(x == 293 && y == 273, "th10_mp_unlimited_rounding_preserved");
}
#endif
}

int main() {
    controller_reference_values();
    subpixel_follows_without_rounding();
    release_paths_clear_drag();
    joystick_modes_use_the_native_logical_input_contract();
    std::puts("touch native behavior: joystick-digital-and-free");
    touch_focus_actions_and_bomb_do_not_retire_drag_ownership();
    std::puts("touch native behavior: focus-actions-and-drag-ownership");
    bomb_pulse_remains_available_while_movement_is_blocked();
    std::puts("touch native behavior: bomb-pulse-during-blocked-movement");
    double_tap_bomb_is_an_action_from_a_secondary_finger();
    std::puts("touch native behavior: secondary-double-tap-bomb");
    menu_and_dialogue_gestures_resolve_to_logical_keys();
    std::puts("touch native behavior: menu-confirm-cancel-and-dialogue");
    modal_confirm_requires_a_new_touch_after_gameplay_fire();
    std::puts("touch native behavior: modal-confirm-fresh-sample-edge");
    choice_swipe_is_one_shot_when_production_state_supports_it(gameplay_state());
    unlimited_processing_drop_marker_is_consumption_driven();
    std::puts("touch native behavior: unlimited-mode-tag");
    std::puts("touch native behavior: pointer-up-and-cancel-release");
#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
    focused_original_speed_is_retained();
#endif
#if defined(TOUCH_NATIVE_WITH_TH10_MP_CONSUMER)
    th10_mp_fixed_point_touch_quantization_stays_inside_speed();
#endif
#if defined(TOUCH_NATIVE_WITH_TH08_MP_CONSUMER)
    std::puts("touch native: production TouchController + MotionTrack + th08-mp ResolveAnalogMovement verified");
#elif defined(TOUCH_NATIVE_WITH_TH10_MP_CONSUMER)
    std::puts("touch native: production TouchController + MotionTrack + th10-mp InputLanes::ResolveMovement verified");
#else
    std::puts("touch native: production TouchController + MotionTrack verified");
#endif
}
