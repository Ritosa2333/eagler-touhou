#pragma once
#if !TH15_MULTIPLAYER
#error Cooperative spectator state is an MP-only owner
#endif
#include "SessionSetup.hpp"
#include "../game/GameBattle.hpp"
#include "../game/GameInput.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>

namespace th15::multiplayer {
// Portable state at a native stage-construction boundary. Restore writes
// named values into freshly constructed owners and preserves their callbacks.
struct SpectatorPilotState {
    PlayerLifeSession resources;
    PlayerMotion motion;
    GameInput keys;
    MultiplayerInput input;
    Timer life_age,invulnerability,input_age,ready_age,shot_age,continuous_age;
    Vec3 last_death_position;
    i32 life_state=0,base_bombs=0,power_level=0,shoot_auxiliary=0,life_piece_tier=0;
    bool shoot_auxiliary_active=false,ghost=false,spell_startup_bomb=false,life_latched=false;
    u32 ghost_clock=0,ghost_random=0,life_hold=0,power_taps=0,power_gap=0;
    i32 ghost_dx=0,ghost_dy=0;
    std::array<float,8> option_angles{};
    std::array<i32,9> laser_power{};
    std::array<i32,3> reward_counters{};
};
struct SpectatorStageState {
    static constexpr u32 Schema=1;
    SessionSetup setup;
    SessionState progress;
    ItemScoreState score;
    EnemyWorldState enemy;
    Rng game_random,visual_random;
    GameInput menu;
    std::array<SpectatorPilotState,3> pilots;
    u32 frame=0,application_frames=0;
    i32 fragments=0;
};
bool EncodeSpectatorStageState(const SpectatorStageState&,std::vector<u8>&);
bool DecodeSpectatorStageState(const std::vector<u8>&,SpectatorStageState&);
void CaptureSpectatorPilot(const MultiplayerPilot&,SpectatorPilotState&);
bool RestoreSpectatorPilot(const SpectatorPilotState&,MultiplayerPilot&);
bool RestoreSpectatorEnemyScalars(const EnemyWorldState&,EnemyWorldState&);
// Input to this predicate is a decoded capsule, before touching native owners.
bool ValidSpectatorStageState(const SpectatorStageState&);
}
