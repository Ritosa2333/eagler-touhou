// Shared effective spell state (DAT_004c2178), independent of presentation and
// persistence. Begin/end return value snapshots at the actual ECL instruction,
// including when both instructions execute in the same thread step.
#pragma once

#include "Timer.hpp"

#include <string>

namespace th13 {

inline constexpr u32 kSpellActive = 0x01;
inline constexpr u32 kSpellCapture = 0x02;
inline constexpr u32 kSpellDisplayHidden = 0x04; // top-of-playfield hysteresis
inline constexpr u32 kSpellSurvival = 0x08; // ECL 542: no decay; timeout can capture
inline constexpr u32 kSpellHiddenBackground = 0x10; // ECL 543
inline constexpr u32 kSpellBombProtection = 0x20;
inline constexpr u32 kSpellTimedOut = 0x80; // drives end sound / practice routing

struct SpellSnapshot {
    i32 id = -1;
    std::string name;
    u32 flags = 0;
    i32 current_bonus = 0; // manager+0x80: displayed points, award divides by 10
    i32 initial_bonus = 0; // manager+0x84: fixed decay numerator, capped at 99999999
    i32 duration_frames = 0; // manager+0x88, ECL frames rather than seconds
    Timer age;              // manager+0x24 timer; age.current = manager+0x28
    i32 active_frames = 0;  // manager+0x90, starts at 1, increments once per update

    bool active() const noexcept { return (flags & kSpellActive) != 0; }
    bool captured() const noexcept { return (flags & kSpellCapture) != 0; }
};

enum class SpellEventKind : u8 { None, Begin, End };

struct SpellEvent {
    SpellEventKind kind = SpellEventKind::None;
    SpellSnapshot state; // immutable value of the spell at the instruction
    i32 score_award = 0; // internal score units, displayed points / 10
    bool emitted() const noexcept { return kind != SpellEventKind::None; }
    bool captured() const noexcept { return emitted() && state.captured(); }
};

class SpellState {
public:
    const SpellSnapshot& state() const noexcept { return state_; }
    SpellEvent begin(i32 id, const std::string& name, i32 duration_frames,
                     i32 stage, i32 difficulty, bool bomb_active = false);
    SpellEvent end();
    // Original spell update callback priority 26, after enemy ECL. Checks the
    // current age for decay, then ticks it, then clears expired bomb protection.
    bool step(bool bomb_active, const float* rate = nullptr);
    void update_visibility(float player_y) noexcept; // call after step, update26
    void player_hit(bool bomb_active);
    void bomb_started(bool bomb_active = true);
    void phase_timeout(bool bomb_active);
    void set_survival() noexcept;
    void hide_background() noexcept;
    // Original 00413080 draw callback mutates capture state during Trance.
    // Caller invokes only while Trance is active; it shares the hit/bomb gate.
    void trance_active(bool bomb_active);
    void reset(); // unload/retry: discard, never award a capture
    std::string error;

private:
    SpellSnapshot state_;
    void invalidate_capture(bool bomb_active);
};

} // namespace th13
