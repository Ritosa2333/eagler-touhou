#pragma once
#include "enemy_state.hpp"
namespace th20::source::gameplay {
void combine_enemy_movements(EnemyState&,float clock_scale); //4a7da0
class EnemyMovementServices {
public:
    virtual ~EnemyMovementServices()=default;
    virtual const float* timer_rate()=0;
    virtual float clock_scale()=0;
    virtual sprite::Animation* animation(std::uint32_t)=0;
    virtual sprite::Vec3 viewport_offset()=0;                //viewport0.final_vector,5c50fc
    virtual sprite::AnimationFile& animation_file(EnemyState&,unsigned slot)=0; //4aae10
    virtual void delete_animation(std::uint32_t&)=0;         //44fcd0
    virtual std::uint32_t spawn_animation(sprite::AnimationFile&,int script,const sprite::Vec3&,int layer)=0; //4aba30
    virtual float animation_height(sprite::Animation&)=0;   //4459a0
    virtual float animation_width(sprite::Animation&)=0;    //4459d0
};
int update_enemy_movement(EnemyState&,EnemyMovementServices&); //4a7710
EnemyMovementServices& enemy_movement_services();
}
