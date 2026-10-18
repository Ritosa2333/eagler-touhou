#pragma once
#include "bomb.hpp"
namespace th20::source::bomb::character_environment {
void* player_entity();                                       //460830(0), Context+4
sprite::Vec3 player_position();                              //460850, PlayerEntity+614
float player_horizontal_motion();                            //479020, PlayerEntity+20c4
void set_invulnerability(std::int32_t);                       //479060, timer+2050
void set_movement_scale(float);                              //479080, scalar+20ec
void set_bomb_player_flag(bool);                             //4790a0/4790c0, bit2+14
sprite::AnimationFile& player_animation();                    //421810, PlayerEntity+1c
sprite::Animation& animation_or_fallback(std::uint32_t&);      //44cf10
void interrupt(std::uint32_t handle);                         //479040 ->44ef90 ->44ee90(1)
sprite::Animation* animation_child(sprite::Animation&,std::int32_t script,std::int32_t ordinal); //44c6b0
sprite::Vec3 animation_world_position(sprite::Animation&);    //478f00/478f70
}
namespace th20::source::bomb::unrecovered {
// Actual external collision and projectile controllers. These declarations
// intentionally remain unresolved until their owning engine source is restored.
void cancel_rectangle_0047cc90(void*,const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t);
void cancel_rectangle_004c9db0(void*,const sprite::Vec3&,const sprite::Vec3&,float,std::int32_t,std::int32_t);
void cancel_circle_0047cf60(void*,const sprite::Vec3&,float,std::int32_t,std::int32_t,std::int32_t);
void cancel_circle_004caad0(void*,const sprite::Vec3&,float,std::int32_t,std::int32_t);
}
