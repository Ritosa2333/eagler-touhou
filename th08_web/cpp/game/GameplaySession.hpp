#pragma once
#include "GameGauge.hpp"
#include "PracticeConfig.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/PlayerResources.hpp"
#include "../multiplayer/SessionSetup.hpp"
#include "../multiplayer/NetplayRuntime.hpp"
#endif
namespace th08 {
// Persistent game data shared by title, stage, results and replay owners.
struct GameplaySession {
    PracticeState practice;
    Rng random;GameGlobals numbers;GameConfiguration config,display_config;HighScore history;
    SpellRecord records[spell_count],previous_records[spell_count];ClearRecord clears[13];PracticeRecord practices[12];PlayRecord statistics;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    multiplayer::SessionSetup multiplayer_session;
    multiplayer::NetplayRuntime netplay;
    Netplay::FrameDecision network_frame;
    bool network_frame_open=false,network_waiting=false;
    bool multiplayer_cheat_movement_used=false;
    PilotResources pilot_resources[3]{};
    PlayerResourceView pilot_views[3]{{numbers,pilot_resources[0]},
                                    {numbers,pilot_resources[1]},
                                    {numbers,pilot_resources[2]}};
    PlayerValues pilot_values[3]{{pilot_views[0],history},{pilot_views[1],history},
                                {pilot_views[2],history}};
    GaugeThresholds guest_thresholds[2];
    GameGauge guest_gauges[2]{{pilot_resources[1].gauge,pilot_resources[1].gauge_copy,guest_thresholds[0]},
                             {pilot_resources[2].gauge,pilot_resources[2].gauge_copy,guest_thresholds[1]}};
    u32 player_count=2,local_player=0;
    u8 player_characters[3]{};
#endif
    GameRank rank;GaugeThresholds thresholds;i32 stall_frames=0,stage_copy=0;u16 replay_seed=0;
    GameValues values{numbers,config,display_config,history,random};
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    GameGauge gauge{pilot_resources[0].gauge,pilot_resources[0].gauge_copy,thresholds};
#else
    GameGauge gauge{numbers,thresholds};
#endif
    LastWords last_words;LastName last_name;u32 total_clock=0;
};
struct GameplayLoad {
    i32 stage=0,difficulty=0,character=0,spell=-1;u32 flags=0;
    bool initial=true,keep_resources=false,release_resources=true;
    i32 supervisor_state=2;
};
}
