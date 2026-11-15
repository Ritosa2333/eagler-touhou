#pragma once
#include "Types.hpp"
namespace th11 {
struct PlayerFrameState;struct PlayerSpellState;struct PlayerOption;struct PlayerMotionState;struct GameEconomy;struct PlayerFrameWorld;
struct Th16PlayerHitServices {
 virtual ~Th16PlayerHitServices()=default;
 virtual bool sound(i32)=0;
 virtual bool effect(Vec3,i32)=0;
 virtual bool reset_body()=0;
};
bool player_th16_hit(PlayerFrameState&,PlayerSpellState&,Vec3,u32,bool,const float*,Th16PlayerHitServices&);
struct Th16PlayerDeathServices {
 virtual ~Th16PlayerDeathServices()=default;
 virtual bool effect(Vec3,i32)=0;
 virtual bool bombs(i32,i32)=0;
 virtual bool lives(i32,i32)=0;
 virtual bool reset_body()=0;
 virtual bool interrupt(u32,i16)=0;
 virtual bool enemy_death()=0;
};
bool player_th16_die(PlayerFrameState&,PlayerSpellState&,GameEconomy&,PlayerOption*,i32&,Vec3,bool,const float*,Th16PlayerDeathServices&);
bool player_th16_death_drops(GameEconomy&,Vec3,PlayerFrameWorld&);
bool player_th16_respawn_frame(PlayerFrameState&,PlayerMotionState&,PlayerOption*,PlayerFrameWorld&);
}
