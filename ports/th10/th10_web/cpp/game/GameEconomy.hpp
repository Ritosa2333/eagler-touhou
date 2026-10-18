#pragma once
#include "Timer.hpp"
namespace th10 {
struct PracticeState;
struct EconomyEnvironment {
#ifdef TH_ENABLE_THPRAC
    const PracticeState* practice=nullptr;
#endif
    virtual void show_notification(i32 script)=0;
    virtual void play_global_sound(i32 sound)=0;
    virtual void update_lives(i32 lives)=0;
};
struct GameEconomy {
    i32 high_score;
    i32 score;
    std::int16_t power;
    u16 reserved_power;
    i32 item_value,enemy_activity;
    Timer faith_timer;
    u32 faith_timer_flags;
    i32 character,shot_type;
    i32 lives,difficulty;
    u32 reserved_038;
    i32 stage;
    u32 reserved_040;
    i32 section;
    u32 stage_frames,section_frames;
    i32 score_units,high_score_units;
    i32 rank;
    i32 extend_index;
    u32 flags;
    void add_score(i32 points) noexcept;
    void add_item_value(i32 points) noexcept;
    void add_rank(i32 delta) noexcept;
    bool add_power(std::int16_t delta,EconomyEnvironment& environment);
    void add_lives(i32 delta,EconomyEnvironment& environment);
    void extend_faith_timer(i32 frames,const float* default_rate) noexcept;
    void select_section(i32 next) noexcept;
};
static_assert(offsetof(GameEconomy,faith_timer)==0x14);
static_assert(offsetof(GameEconomy,rank)==0x58);
static_assert(offsetof(GameEconomy,flags)==0x60);
}
