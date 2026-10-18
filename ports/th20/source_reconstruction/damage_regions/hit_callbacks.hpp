#pragma once
#include "damage.hpp"
namespace th20::source::damage {
void* find_player_shot(Region&); //4c0f70->4c0fb0->506360
int default_shot_hit(void*,sprite::Controller&); //504af0
int shot_hit_callback(Region&,const sprite::Vec3&,const sprite::Vec2*,float,float,sprite::Controller&); //4bfcd0
int diminish_shot_hit(Region&,const sprite::Vec3&); //4bfd60
// Genuine unresolved ItemCtrl method. The counter and threshold behavior is
// implemented in accumulate_damage_reward; no missing call is replaced by 0.
namespace unrecovered {void spawn_item_004c45b0(void* item_controller,const sprite::Vec3&,int count,int type);}
void accumulate_damage_reward(void* overlay,const sprite::Vec3&,int damage,int type); //4a9f80/533720
}
