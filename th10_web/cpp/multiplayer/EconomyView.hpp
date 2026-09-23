#pragma once

#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Multiplayer economy views must not enter an ordinary build.
#endif

#include "../game/Timer.hpp"
#include <type_traits>

namespace th10::multiplayer {

// These value owners, not the view's references, belong in rollback state.
struct TeamEconomy {
    i32 high_score=0,score=0,item_value=0,enemy_activity=0;
    Timer faith_timer{};
    u32 faith_timer_flags=0;
    i32 difficulty=0;
    u32 reserved_038=0;
    i32 stage=0;
    u32 reserved_040=0;
    i32 section=0;
    u32 stage_frames=0,section_frames=0;
    i32 score_units=0,high_score_units=0,rank=0,extend_index=0;
    u32 flags=0;
};

struct PilotEconomy {
    std::int16_t power=0;
    u16 reserved_power=0;
    i32 character=0,shot_type=0,lives=0;
};

// Native economy methods retain their typed field access. A pilot view binds
// personal resources once and shares the same team owner with every other
// view. No current-player switching or shared-state copy/merge is involved.
struct EconomyView {
    TeamEconomy& team;
    PilotEconomy& pilot;
    i32 &high_score,&score;
    std::int16_t& power;
    u16& reserved_power;
    i32 &item_value,&enemy_activity;
    Timer& faith_timer;
    u32& faith_timer_flags;
    i32 &character,&shot_type,&lives,&difficulty;
    u32& reserved_038;
    i32& stage;
    u32& reserved_040;
    i32& section;
    u32 &stage_frames,&section_frames;
    i32 &score_units,&high_score_units,&rank,&extend_index;
    u32& flags;

    EconomyView(TeamEconomy& team, PilotEconomy& pilot) noexcept
      :team(team),pilot(pilot),high_score(team.high_score),score(team.score),
       power(pilot.power),reserved_power(pilot.reserved_power),
       item_value(team.item_value),enemy_activity(team.enemy_activity),
       faith_timer(team.faith_timer),faith_timer_flags(team.faith_timer_flags),
       character(pilot.character),shot_type(pilot.shot_type),lives(pilot.lives),
       difficulty(team.difficulty),reserved_038(team.reserved_038),stage(team.stage),
       reserved_040(team.reserved_040),section(team.section),
       stage_frames(team.stage_frames),section_frames(team.section_frames),
       score_units(team.score_units),high_score_units(team.high_score_units),
       rank(team.rank),extend_index(team.extend_index),flags(team.flags) {}

    EconomyView(const EconomyView&)=delete;
    EconomyView& operator=(const EconomyView&)=delete;
};

static_assert(std::is_trivially_copyable<TeamEconomy>::value);
static_assert(std::is_trivially_copyable<PilotEconomy>::value);
}
