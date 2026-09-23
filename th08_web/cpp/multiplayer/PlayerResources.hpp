#pragma once

#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer player resources must not enter an ordinary build.
#endif
#include "../game/GameValues.hpp"
#include <type_traits>

namespace th08 {

// Per-seat counters that must not share storage with the native run globals.
// Keep this object at a stable address for as long as its PlayerResourceView
// is alive. Rollback restores it in place with restore().
struct PilotResources {
    float lives=0,bombs=0,power=0;
    i16 gauge=0,gauge_copy=0;
    float deaths=0,deaths_stage=0,bombs_used=0,bombs_used_stage=0;

    void reset() noexcept { *this=PilotResources{}; }
    void restore(const PilotResources& checkpoint) noexcept { *this=checkpoint; }
};
static_assert(std::is_trivially_copyable_v<PilotResources>);

// A non-owning, permanent binding from one pilot's view of TH08 resources to
// the shared native GameGlobals owner and that pilot's personal resource bank.
// The shared GameGlobals layout is deliberately untouched; integrity/RNG
// bookkeeping is not exposed through this view.
class PlayerResourceView final {
public:
    GameGlobals& shared;
    PilotResources& pilot;

    u32& display_score;
    i32& graze_stage;
    u32& score;
    i32& graze;
    i32& score_increment;
    u32& high_score;
    u8& high_score_retries;
    i32& captured_spells;
    i16& gauge_copy;
    i16& gauge;
    i32& point_value;
    i8& clock_time;
    u8& retries;
    i32& points_stage;
    i32& points;
    u32& point_extends;
    i32& next_point_extend;
    i32& time_orbs;
    i32& last_spell_requirement;
    i32& total_time_orbs;
    float& deaths;
    float& deaths_stage;
    float& lives;
    float& bombs;
    float& bombs_used;
    float& bombs_used_stage;
    float& power;

    PlayerResourceView(GameGlobals& shared, PilotResources& pilot) noexcept;
    PlayerResourceView(const PlayerResourceView&)=delete;
    PlayerResourceView& operator=(const PlayerResourceView&)=delete;
    PlayerResourceView(PlayerResourceView&&)=delete;
    PlayerResourceView& operator=(PlayerResourceView&&)=delete;
};

// Multiplayer arithmetic counterpart to GameValues. It preserves TH08's
// gameplay conversions and wrapping rules while deliberately omitting the
// native raw-byte integrity checksum and its shared-RNG mutations.
class PlayerValues final {
public:
    PlayerValues(PlayerResourceView& resources,HighScore& high_score) noexcept
        :resources(resources),high_score(high_score){}

    void set_lives(i32 value);
    void set_bombs(i32 value);
    void set_power(i32 value);
    void set_deaths_stage(i32 value);
    void set_bombs_stage(i32 value);
    // These bool results preserve gameplay call sites; MP has no native
    // tamper gate, so a completed arithmetic update returns true.
    bool add_lives(i32 value);
    bool add_bombs(i32 value);
    bool add_power(i32 value);
    bool add_deaths(i32 value);
    bool count_bombs(i32 value);
    void add_time_orbs(i32 value);
    void add_score(i32 value) { resources.score+=u32(value/10); }
    void add_clock(i8 amount) { resources.clock_time=i8(u8(resources.clock_time)+u8(amount)); }

private:
    PlayerResourceView& resources;
    HighScore& high_score;
};

} // namespace th08
