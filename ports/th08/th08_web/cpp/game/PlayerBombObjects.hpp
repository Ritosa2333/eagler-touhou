#pragma once
#include "PlayerBomb.hpp"
#include "DamageRegions.hpp"
namespace th08 {
// Shared storage used by the original seventeen bomb callbacks. The eight
// ANM objects and region links are real native objects, not PE addresses.
struct PlayerBombObject {
    i32 state=0,frame=0;float speed=0,distance=0,angle=0;Vec3 position,history[32],velocity,acceleration;
    AnmVm animation[8];EffectState* effect=nullptr;Timer timer;DamageRegion *damage=nullptr,*cancel=nullptr;
};
static_assert(sizeof(PlayerBombObject)==0x16f0&&offsetof(PlayerBombObject,animation)==0x1b8&&offsetof(PlayerBombObject,damage)==0x16e8);
struct PlayerBombObjects {PlayerBombObject objects[128];Vec3 origin;};
struct PlayerBombStartActions {
    virtual ~PlayerBombStartActions()=default;
    virtual void spell_overlay(i32 sprite,const char* name,i32 variant)=0;
    virtual EffectState* fixed_effect(i32 kind,const Vec3& position,i32 slot,u32 color)=0;
    virtual void home_items()=0;
};
void player_invincibility_effect(PlayerLifeState&,const Vec3&,PlayerBombStartActions&);
void begin_player_bomb(PlayerBombObjects&,PlayerBombState&,PlayerLifeState&,const Vec3&,i32 sprite,const char* name,i32 duration,i32 invincibility,i32 variant,PlayerBombStartActions&);
u32 player_bomb_color(u32 color,const Timer&,i32 duration)noexcept;
}
