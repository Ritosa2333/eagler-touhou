#include "../../portable/input/TouchController.hpp"
#include "../harness/test_framework.hpp"

using touhou::input::TouchController;
using touhou::input::TouchSample;
using touhou::input::TouchState;

namespace {
TouchState active() {
    TouchState s; s.context = 1; s.instance = 7; s.ready = true;
    s.x = 0; s.y = 200; s.fast = 4.5f; s.slow = 2.f;
    s.min_x = -184; s.max_x = 184; s.min_y = 32; s.max_y = 432; return s;
}
void drag(TouchController& t, const TouchState& s) {
    t.pointer(0, 1, .5f, .5f, 0, s, false);
    t.pointer(1, 1, .6f, .6f, 10, s, false);
}
}

TEST(touch_cancel_preserves_fire_choice) {
    TouchController t; t.controls(true, true, 0, 0, 32767, 0);
    auto s = active();
    CHECK(t.sample(s, 1, false, false).keys[90]);
    t.cancel();
    auto after = t.sample(s, 2, false, false);
    CHECK(after.keys[90]);
    CHECK(!after.keys[16]);
    CHECK(!after.keys[39]);
}

TEST(touch_reset_starts_new_session) {
    TouchController t; t.controls(true, false, 0, 0, 0, 0);
    auto s = active();
    CHECK(t.sample(s, 1, false, false).keys[90]);
    t.reset();
    CHECK(!t.sample(s, 2, false, false).keys[90]);
}

TEST(touch_pause_context_blocks_actions_without_losing_fire) {
    TouchController t; t.controls(true, false, 0, 0, 0, 0);
    auto s = active();
    CHECK(t.sample(s, 1, false, false).keys[90]);
    auto paused = s; paused.context = 0; paused.ready = false;
    auto p = t.sample(paused, 2, false, false);
    CHECK(!p.keys[90]);
    auto resumed = active();
    CHECK(t.sample(resumed, 3, false, false).keys[90]);
    // A retry can re-enter gameplay with a new stage instance; it is not a
    // new browser session and the user's fire choice remains set.
    auto retry = resumed; retry.instance = 8;
    CHECK(t.sample(retry, 4, false, false).keys[90]);
}

TEST(touch_four_finger_pause_survives_gesture_cleanup) {
    TouchController t; auto s = active();
    for (int id = 1; id <= 4; ++id) t.pointer(0, id, .5f, .5f, id, s, false);
    CHECK(t.sample(s, 5, false, false).keys[27]);
    CHECK(!t.active());
}

TEST(touch_menu_tap_confirms_without_auto_fire) {
    TouchController t; t.controls(true, false, 0, 0, 0, 0);
    TouchState menu;
    CHECK(!t.sample(menu, 1, false, false).keys[90]);
    t.pointer(0, 1, .5f, .5f, 2, menu, false);
    t.pointer(2, 1, .5f, .5f, 3, menu, false);
    CHECK(t.sample(menu, 4, false, false).keys[90]);
    CHECK(t.sample(active(), 5, false, false).keys[90]);
}

TEST(touch_replay_context_has_no_gameplay_actions) {
    TouchController t; t.controls(true, true, 0, 0, 32767, 0);
    auto s = active(); auto replay = s; replay.context = 3;
    auto out = t.sample(replay, 1, false, false);
    CHECK(!out.keys[90]); CHECK(!out.keys[16]); CHECK(!out.keys[39]);
}

TEST(touch_active_drag_resumes_after_deathbomb) {
    TouchController t; auto s = active(); drag(t, s);
    CHECK(t.sample(s, 1, false, false).motion != 0);
    auto deathbomb = s; deathbomb.ready = false; deathbomb.retain_motion = true;
    CHECK_EQ(t.sample(deathbomb, 2, false, false).motion, 0);
    CHECK(t.active());
    CHECK(t.sample(s, 3, false, false).motion != 0);
}

TEST(touch_non_death_transition_discards_drag) {
    TouchController t; auto s = active(); drag(t, s);
    CHECK(t.sample(s, 1, false, false).motion != 0);
    auto dead = s; dead.ready = false;
    CHECK_EQ(t.sample(dead, 2, false, false).motion, 0);
    CHECK(!t.active());
    CHECK_EQ(t.sample(s, 3, false, false).motion, 0);
}

int main() { return th13test::run_all("th13 touch controller"); }
