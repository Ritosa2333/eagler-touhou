#include "PlayerBombObjects.hpp"
namespace th08 {
void player_invincibility_effect(PlayerLifeState& life,const Vec3& position,PlayerBombStartActions& actions){
    if(life.invincible_effect)life.invincible_effect->active=0;
    auto* e=actions.fixed_effect(23,position,0,0xffffffff);life.invincible_effect=e;if(!e)return;
    e->interpCurrentTimers[AnmInterp_Scale].set(0);e->interpEndTimers[AnmInterp_Scale]=life.timer;e->interpModes[AnmInterp_Scale]=0;
    e->scaleInitial=e->scale;e->scaleFinal={.0625f,.0625f};e->intVar0=life.timer.current;e->angleVel.z=Scalar::mul(e->angleVel.z,-1);
    e->color1.r=255;e->color1.g=64;e->color1.b=64;
}
void begin_player_bomb(PlayerBombObjects& objects,PlayerBombState& bomb,PlayerLifeState& life,const Vec3& position,i32 sprite,const char* name,i32 duration,i32 invincibility,i32 variant,PlayerBombStartActions& actions){
    actions.spell_overlay(sprite,name,variant);bomb.duration=duration;life.timer.set(invincibility);life.state=3;player_invincibility_effect(life,position,actions);
    for(auto& object:objects.objects)object.state=0;actions.home_items();objects.origin=position;
}
u32 player_bomb_color(u32 color,const Timer& timer,i32 duration)noexcept{
    i32 factor=0;
    if(timer.current<60)factor=timer.current;
    else if(timer.current>=wrapping_sub(duration,60))factor=wrapping_sub(duration,timer.current);
    else return (color&0xffffff)|0x80000000;
    u32 result=0x80000000;
    for(u32 shift=0;shift<24;shift+=8){const i32 delta=128-i32((color>>shift)&255),channel=wrapping_sub(128,signed_bits(u32(delta)*u32(factor))/60);result|=u32(u8(channel))<<shift;}
    return result;
}
}
