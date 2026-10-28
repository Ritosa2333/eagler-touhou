#include "SpellState.hpp"

#include <algorithm>

namespace th13 {

SpellEvent SpellState::begin(i32 id, const std::string& name, i32 duration_frames,
                            i32 stage, i32 difficulty, bool bomb_active) {
    error.clear();
    if (id < 0 || id >= 127 || stage < 1 || stage > 7 || difficulty < 0 ||
        difficulty > 5 || duration_frames < 0) {
        error = "Invalid spell start parameters";
        return {};
    }
    // 00413270: no active-spell rejection. The original replaces metadata and
    // resets flags with &0xffffff67|3, then rewrites bit0x20 and clears bit0x40.
    state_.id = id;
    state_.name = name;
    state_.flags = (state_.flags & 0xffffff67u) | kSpellActive | kSpellCapture;
    state_.flags &= ~(kSpellBombProtection | 0x40u);
    if (bomb_active) state_.flags |= kSpellBombProtection;
    state_.current_bonus = (stage + difficulty) * 1000000;
    state_.initial_bonus = std::min(state_.current_bonus, 99999999);
    state_.duration_frames = duration_frames;
    state_.age.set(0);
    state_.active_frames = 1;
    return {SpellEventKind::Begin, state_, 0};
}

SpellEvent SpellState::end() {
    // 00413a60: inactive end is a no-op, including a repeated ECL 523.
    if (!state_.active()) return {};
    SpellEvent event{SpellEventKind::End, state_,
                     state_.captured() ? state_.current_bonus / 10 : 0};
    // Keep metadata, age and all other flags exactly as the original does.
    state_.flags &= ~(kSpellActive | kSpellBombProtection);
    return event;
}

bool SpellState::step(bool bomb_active, const float* rate) {
    if (!error.empty()) return false;
    if (!state_.active()) return true;
    // 00412d00 / 00412d28..00412d75: >=300 BEFORE Timer::tick. Signed x86
    // division/remainder truncate towards zero. Do not clamp a failed spell's
    // bonus: the original keeps applying decay after it was cleared to zero.
    if (state_.age.current >= 300 && !(state_.flags & kSpellSurvival)) {
        const i32 denominator = state_.duration_frames - 300;
        if (denominator <= 0) {
            error = "Spell bonus decay duration does not exceed 300 frames";
            return false; // original idiv would fault; do not invent a divisor
        }
        const i32 decrement = (state_.initial_bonus - state_.initial_bonus / 4) /
                              denominator;
        const i32 bonus = wrapping_sub(state_.current_bonus, decrement);
        state_.current_bonus = bonus - bonus % 10;
    }
    state_.active_frames = wrapping_add(state_.active_frames, 1);
    state_.age.tick(rate);
    // 00412d00 tail: bomb protection follows the real bomb manager lifetime.
    if (!bomb_active) state_.flags &= ~kSpellBombProtection;
    return true;
}

void SpellState::update_visibility(float player_y) noexcept {
    // 00412d83..00412dfa, AFTER ticking the spell timer. Strict spatial
    // comparisons retain the previous state at exactly y=96 and y=128.
    if (!state_.active() || state_.age.current < 120) return;
    if (!(state_.flags & kSpellDisplayHidden)) {
        if (player_y < 96.0f) state_.flags |= kSpellDisplayHidden;
    } else if (player_y > 128.0f) {
        state_.flags &= ~kSpellDisplayHidden;
    }
}

void SpellState::invalidate_capture(bool bomb_active) {
    if (!state_.active()) return;
    // 00444ca0/00444a10 and all four bomb constructors: grace window <60.
    if (state_.age.current < 60) {
        if (bomb_active) state_.flags |= kSpellBombProtection;
        return;
    }
    state_.current_bonus = 0;
    state_.flags &= ~(kSpellCapture | kSpellBombProtection);
}

void SpellState::player_hit(bool bomb_active) { invalidate_capture(bomb_active); }
void SpellState::bomb_started(bool bomb_active) { invalidate_capture(bomb_active); }
void SpellState::trance_active(bool bomb_active) { invalidate_capture(bomb_active); }

void SpellState::phase_timeout(bool bomb_active) {
    // 004218f0: survival jumps past the entire failure/0x80 block. Ordinary
    // timeout sets 0x80 even when no spell is active, then applies the same gate.
    if (state_.flags & kSpellSurvival) return;
    state_.flags |= kSpellTimedOut;
    invalidate_capture(bomb_active);
}

void SpellState::set_survival() noexcept { state_.flags |= kSpellSurvival; }
void SpellState::hide_background() noexcept { state_.flags |= kSpellHiddenBackground; }

void SpellState::reset() {
    state_ = {};
    error.clear();
}

} // namespace th13
