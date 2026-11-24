#pragma once

#include "Timer.hpp"

#include <string>

namespace th13 {

inline constexpr u32 kSpellActive = 0x01;
inline constexpr u32 kSpellCapture = 0x02;
inline constexpr u32 kSpellDisplayHidden = 0x04;
inline constexpr u32 kSpellSurvival = 0x08;
inline constexpr u32 kSpellHiddenBackground = 0x10;
inline constexpr u32 kSpellBombProtection = 0x20;
inline constexpr u32 kSpellTimedOut = 0x80;

struct SpellSnapshot {
    i32 id = -1;
    std::string name;
    u32 flags = 0;
    i32 current_bonus = 0;
    i32 initial_bonus = 0;
    i32 duration_frames = 0;
    Timer age;
    i32 active_frames = 0;

    bool active() const noexcept { return (flags & kSpellActive) != 0; }
    bool captured() const noexcept { return (flags & kSpellCapture) != 0; }
};

enum class SpellEventKind : u8 { None, Begin, End };

struct SpellEvent {
    SpellEventKind kind = SpellEventKind::None;
    SpellSnapshot state;
    i32 score_award = 0;
    bool emitted() const noexcept { return kind != SpellEventKind::None; }
    bool captured() const noexcept { return emitted() && state.captured(); }
};

class SpellState {
public:
    const SpellSnapshot& state() const noexcept { return state_; }
    SpellEvent begin(i32 id, const std::string& name, i32 duration_frames, i32 stage, i32 difficulty,
                     bool bomb_active = false);
    SpellEvent end();
    bool step(bool bomb_active, const float* rate = nullptr);
    void update_visibility(float player_y) noexcept;
    void player_hit(bool bomb_active);
    void bomb_started(bool bomb_active = true);
    void phase_timeout(bool bomb_active);
    void set_survival() noexcept;
    void hide_background() noexcept;
    void trance_active(bool bomb_active);
    void reset();
    void restore(const SpellSnapshot& snapshot) noexcept { state_ = snapshot; }
    std::string error;

private:
    SpellSnapshot state_;
    void invalidate_capture(bool bomb_active);
};

}
