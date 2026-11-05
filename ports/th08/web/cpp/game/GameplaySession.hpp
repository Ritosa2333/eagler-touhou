#pragma once
#include "GameGauge.hpp"
#include "PracticeConfig.hpp"
namespace th08 {
// Persistent game data shared by title, stage, results and replay owners.
struct GameplaySession {
    PracticeState practice;
    Rng random;GameGlobals numbers;GameConfiguration config,display_config;HighScore history;
    SpellRecord records[spell_count],previous_records[spell_count];ClearRecord clears[13];PracticeRecord practices[12];PlayRecord statistics;
    GameRank rank;GaugeThresholds thresholds;i32 stall_frames=0,stage_copy=0;u16 replay_seed=0;
    GameValues values{numbers,config,display_config,history,random};GameGauge gauge{numbers,thresholds};
    LastWords last_words;LastName last_name;u32 total_clock=0;
};
struct GameplayLoad {
    i32 stage=0,difficulty=0,character=0,spell=-1;u32 flags=0;
    bool initial=true,keep_resources=false,release_resources=true;
    i32 supervisor_state=2;
};
}
