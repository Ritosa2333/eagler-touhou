#pragma once
#include "Session.hpp"
#include "../game/ReplayState.hpp"
#include "../game/GameInput.hpp"
#include <vector>

namespace th17::sdl {struct ApplicationState;}
namespace th17::multiplayer {
// Captured at a newly constructed stage, before its first input or ECL tick.
struct StageEntry {
    // The relay uses schema 3 to retain the input tape across stages.
    static constexpr u32 transport_schema=3;
    // Payload version 4 adds the challenge rule. Older checkpoints use ordinary rules.
    static constexpr u32 schema=4;
    static constexpr bool supports(u32 version){return version>=1&&version<=schema;}
    SessionSetup setup;
    ReplayStageState stage;
    std::array<PlayerLifeSession,3> pilots{};
    std::array<PlayerFixedPosition,3> positions{};
    std::array<i32,3> focus{};
    std::array<i32,3> piece_tiers{};
    Rng game,visual,creation_game,creation_visual;
    GameInput menu,input;
    u32 frame=0,app_frame=0,scene_flags=0;
    u8 ghost_mask=0;
    bool deferred=false;
};
bool capture_stage_entry(sdl::ApplicationState&,const SessionSetup&,const GameInput&,u32,std::vector<u8>&);
bool restore_stage_entry(sdl::ApplicationState&,const SessionSetup&,GameInput&,u32,const std::vector<u8>&);
bool decode_stage_entry(const std::vector<u8>&,StageEntry&);
}
