#pragma once
#include "CombatRuntime.hpp"
#include "InputController.hpp"

namespace th08 {
enum class PlayerTeam : u8 {ReimuYukari,MarisaAlice,SakuyaRemilia,YoumuYuyuko};
struct PlayerRuntimeState {PlayerTeam team=PlayerTeam::ReimuYukari;Vec3 position{128,400,0};u8 focused=0,alive=1;u16 lives=2,bombs=3;u32 power=0;u32 invincible=0;u32 shot_cooldown=0;u32 deaths=0,bombs_used=0;};
class PlayerRuntime {
    CombatRuntime& combat;PlayerRuntimeState state{};InputFrame input{};
    float normal_speed=4.0f,focus_speed=2.0f;
    void shoot() noexcept;
public:
    explicit PlayerRuntime(CombatRuntime& c):combat(c){sync_player();}
    void reset(PlayerTeam team=PlayerTeam::ReimuYukari) noexcept;
    void set_team(PlayerTeam team) noexcept{state.team=team;}
    void set_power(u32 power) noexcept{state.power=power>128?128:power;}
    void damage() noexcept;
    void step(u16 buttons,float rate=1) noexcept;
    void bomb() noexcept;
    const PlayerRuntimeState& status()const noexcept{return state;}
    PlayerRuntimeState& status() noexcept{return state;}
private:
    void sync_player() noexcept;
};
}
