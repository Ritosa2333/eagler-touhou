#include "../harness/test_framework.hpp"
#include "game/SpellState.hpp"

#include <vector>

using namespace th13;
namespace {
void advance(SpellState& spell, i32 frames, bool bomb_active = false) {
    for (i32 i = 0; i < frames; ++i) CHECK(spell.step(bomb_active));
}
}

TEST(spell_begin_end_same_ecl_turn_preserves_both_value_events) {
    SpellState spell;
    std::vector<SpellEvent> events;
    events.push_back(spell.begin(2, "synthetic", 3000, 1, 2));
    events.push_back(spell.end());
    CHECK_EQ(events.size(), size_t(2));
    CHECK_EQ(i32(events[0].kind), i32(SpellEventKind::Begin));
    CHECK_EQ(i32(events[1].kind), i32(SpellEventKind::End));
    CHECK_EQ(events[0].state.id, 2);
    CHECK(events[0].state.name == "synthetic");
    CHECK_EQ(events[0].state.flags, 3u);
    CHECK_EQ(events[0].state.age.current, 0);
    CHECK_EQ(events[0].state.age.previous, -1);
    CHECK_EQ(events[0].state.active_frames, 1);
    CHECK(events[1].captured());
    CHECK_EQ(events[1].score_award, 300000);
    CHECK(!spell.state().active());
    CHECK_EQ(spell.state().id, 2); // end does not erase original metadata
    CHECK(!spell.end().emitted());
    spell.begin(3, "replacement", 3000, 1, 3);
    CHECK_EQ(events[0].state.id, 2); // returned snapshots never alias live state
}

TEST(spell_hit_and_bomb_have_exact_59_60_frame_capture_boundary) {
    for (i32 kind = 0; kind < 3; ++kind) {
        SpellState spell;
        spell.begin(0, "grace", 3000, 1, 0);
        advance(spell, 59);
        if (kind == 0) spell.player_hit(false);
        if (kind == 1) spell.bomb_started();
        if (kind == 2) spell.trance_active(false);
        CHECK(spell.state().captured());
        CHECK_EQ(spell.state().current_bonus, 1000000);
        CHECK_EQ(bool(spell.state().flags & kSpellBombProtection), kind == 1);
        CHECK(spell.step(false));
        CHECK_EQ(spell.state().age.current, 60);
        if (kind == 0) spell.player_hit(false);
        if (kind == 1) spell.bomb_started();
        if (kind == 2) spell.trance_active(false);
        CHECK(!spell.state().captured());
        CHECK_EQ(spell.state().current_bonus, 0);
        CHECK(!(spell.state().flags & kSpellBombProtection));
        CHECK_EQ(spell.end().score_award, 0);
    }
}

TEST(spell_bomb_carry_in_protection_tracks_bomb_lifetime_without_breaking_capture) {
    SpellState spell;
    spell.begin(0, "carry", 3000, 1, 0, true);
    CHECK(spell.state().flags & kSpellBombProtection);
    advance(spell, 90, true);
    CHECK(spell.state().captured()); // a pre-existing bomb is not a new bomb event
    CHECK(spell.state().flags & kSpellBombProtection);
    CHECK(spell.step(false));
    CHECK(!(spell.state().flags & kSpellBombProtection));
    CHECK(spell.state().captured());
}

TEST(spell_decay_checks_age300_before_tick_and_rounds_each_step_to_tens) {
    SpellState spell;
    spell.begin(0, "decay", 3000, 1, 0);
    advance(spell, 299);
    CHECK_EQ(spell.state().age.current, 299);
    CHECK_EQ(spell.state().current_bonus, 1000000);
    CHECK(spell.step(false));
    CHECK_EQ(spell.state().age.current, 300);
    CHECK_EQ(spell.state().current_bonus, 1000000);
    CHECK(spell.step(false));
    // trunc(750000/2700)=277; trunc((1000000-277)/10)*10=999720.
    CHECK_EQ(spell.state().current_bonus, 999720);
    CHECK_EQ(spell.state().initial_bonus, 1000000);
    CHECK_EQ(spell.state().active_frames, 302);
    CHECK(spell.step(false));
    CHECK_EQ(spell.state().current_bonus, 999440);
    CHECK_EQ(spell.end().score_award, 99944);
}

TEST(spell_failed_bonus_keeps_original_signed_decay_without_award) {
    SpellState spell;
    spell.begin(0, "failed", 3000, 1, 0);
    advance(spell, 300);
    spell.player_hit(false);
    CHECK(spell.step(false));
    CHECK_EQ(spell.state().current_bonus, -270);
    CHECK_EQ(spell.end().score_award, 0);
}

TEST(spell_survival_disables_decay_and_timeout_failure_but_not_bomb_failure) {
    SpellState spell;
    spell.begin(107, "survival", 100, 7, 4);
    spell.set_survival();
    advance(spell, 350);
    CHECK_EQ(spell.state().current_bonus, 11000000);
    spell.phase_timeout(false);
    CHECK(spell.state().captured());
    CHECK(!(spell.state().flags & kSpellTimedOut));
    CHECK_EQ(spell.end().score_award, 1100000);
    spell.begin(107, "survival", 1000, 7, 4);
    spell.set_survival();
    advance(spell, 60);
    spell.bomb_started();
    CHECK(!spell.state().captured());
}

TEST(spell_normal_timeout_marks_routing_and_end_snapshot_even_when_uncaptured) {
    SpellState spell;
    spell.begin(0, "timeout", 3000, 1, 0);
    advance(spell, 60);
    spell.phase_timeout(false);
    const SpellEvent event = spell.end();
    CHECK(event.emitted());
    CHECK(event.state.flags & kSpellTimedOut); // sound0x45 is independent of capture
    CHECK(!event.captured());
    CHECK_EQ(event.score_award, 0);
    CHECK_EQ(spell.state().flags, kSpellTimedOut);
    const auto next = spell.begin(1, "next", 3000, 1, 1);
    CHECK_EQ(next.state.flags, 3u);
    CHECK_EQ(next.state.age.current, 0);
    CHECK_EQ(next.state.active_frames, 1);
}

TEST(spell_timeout_inside_grace_retains_capture_and_optional_bomb_protection) {
    SpellState spell;
    spell.begin(0, "early-timeout", 3000, 1, 0);
    advance(spell, 59);
    spell.phase_timeout(true);
    CHECK(spell.state().captured());
    CHECK(spell.state().flags & kSpellBombProtection);
    CHECK(spell.state().flags & kSpellTimedOut);
    const auto event = spell.end();
    CHECK(event.captured());
    CHECK_EQ(event.score_award, 100000);
}

TEST(spell_timer_fractional_rate_and_invalid_decay_duration_remain_explicit) {
    SpellState spell;
    spell.begin(0, "slow", 3000, 1, 0);
    const float rate = 0.5f;
    CHECK(spell.step(false, &rate));
    CHECK_EQ(spell.state().age.current, 0);
    CHECK_EQ(spell.state().active_frames, 2);
    CHECK(spell.step(false, &rate));
    CHECK_EQ(spell.state().age.current, 1);
    spell.begin(0, "invalid-duration", 300, 1, 0);
    advance(spell, 300);
    CHECK(!spell.step(false));
    CHECK(!spell.error.empty());
    spell.reset();
    CHECK(spell.error.empty());
    CHECK(!spell.state().active());
    CHECK(!spell.end().emitted());
    CHECK(!spell.begin(127, "invalid-id", 3000, 1, 0).emitted());
    CHECK(!spell.error.empty());
}

TEST(spell_display_hysteresis_starts_after_tick120_and_has_strict_spatial_bounds) {
    SpellState spell;
    spell.begin(0, "visibility", 3000, 1, 0);
    advance(spell, 119);
    spell.update_visibility(0);
    CHECK(!(spell.state().flags & kSpellDisplayHidden));
    CHECK(spell.step(false));
    CHECK_EQ(spell.state().age.current, 120);
    spell.update_visibility(96);
    CHECK(!(spell.state().flags & kSpellDisplayHidden));
    spell.update_visibility(95.999f);
    CHECK(spell.state().flags & kSpellDisplayHidden);
    spell.update_visibility(128);
    CHECK(spell.state().flags & kSpellDisplayHidden);
    spell.update_visibility(128.001f);
    CHECK(!(spell.state().flags & kSpellDisplayHidden));
    spell.end();
    spell.update_visibility(0);
    CHECK(!(spell.state().flags & kSpellDisplayHidden));
}

int main() { return th13test::run_all("spell_state_test"); }
