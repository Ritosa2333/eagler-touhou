#pragma once
#include "Gui.hpp"
namespace th10 {
// Views into the original score.dat statistics. The Extra clear count shares
// the unused stage-zero slot; keep the two indexes explicit instead of padding
// a C++ struct across overlapping fields.
struct ScoreStatistics {
    u8* data;
    u8* character_record(const GameEconomy& game) const noexcept;
    void unlock_stage(const GameEconomy& game) const noexcept;
    void count_clear(const GameEconomy& game) const noexcept;
};
struct StageConfiguration {u32 resources[6];const char* messages[2];const char* interface_animations;u32 reserved_024;const char* music;u32 reserved_02c;};
static_assert(sizeof(StageConfiguration)==48);
struct GameProgressionEnvironment {
    GameEconomy* game;
    Gui** gui;
    ScoreStatistics statistics;
    const i32* replay_mode;
    const StageConfiguration* stages;
    const StageConfiguration** current_stage;
    // thprac Advanced Options "Always factor in the All Clear Bonus"
    // (thprac_th10.cpp:564-576). Mirrors THAdvOptWnd::mOptCtx.all_clear_bonus.
    bool all_clear_bonus=false;
    virtual void stage_clear_notification()=0;
    virtual void select_screen(i32 screen)=0;
    virtual void show_results()=0;
    virtual void fade_ending()=0;
};
void advance_stage(GameEconomy& game,const StageConfiguration* stages,const StageConfiguration** current) noexcept;
void complete_stage(GameProgressionEnvironment& environment);
}
