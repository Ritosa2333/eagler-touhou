#pragma once
#include "GameGauge.hpp"
namespace th08 {
struct PlayerGrazeContext {
    Vec3 position;i32 bomb=0;u32 hud_flags=0;u16 replay_flags=0;u8 character=0,youkai=0,time_spell=0,boss_present=0,reserved[2]{};
};
struct PlayerGrazeActions {
    virtual ~PlayerGrazeActions()=default;
    virtual void effect(i32 kind,const Vec3& position,i32 count,u32 color)=0;
    virtual void sound(i32 index,float x)=0;
    virtual void item(i32 type,const Vec3& position,i32 mode)=0;
};
void graze_player(PlayerGrazeContext&,GameGlobals&,GameGauge&,GameRank&,const Vec3& bullet,bool laser,PlayerGrazeActions&);
}
