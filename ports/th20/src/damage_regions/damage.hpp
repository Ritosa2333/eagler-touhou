#pragma once
#include "regions.hpp"
namespace th20::source::damage {
// These calls reach separately owned Bomb, player-shot ANM and Item controllers.
// Production supplies their actual source implementations through environment().
class Environment {
public:
    virtual ~Environment()=default;
    virtual int bomb_damage(game_session::Context&,const sprite::Vec3&,const sprite::Vec2*)=0; //477cf0
    virtual int hit_callback(Region&,const sprite::Vec3&,const sprite::Vec2*,float,float)=0; //570c78 table
    virtual void damage_reward(const sprite::Vec3&,int)=0; //4a9f80->533720
};
Environment& environment();
bool player_frame_changed(const void*) noexcept; //4c0ce0->45d030
int player_damage_cap(const void* player_entity,const void* overlay,const game_session::Player&) noexcept; //4ff4e0
void add_score(game_session::Player&,std::uint64_t) noexcept; //4e14b0
int calculate_damage(HitCtrlInf&,const sprite::Vec3&,const sprite::Vec2*,float angle,float radius,std::uint32_t* hit_flag,sprite::Vec3* hit_position,int preview,std::uint32_t target,Environment&); //4c0480
inline int calculate_damage(HitCtrlInf& owner,const sprite::Vec3& p,const sprite::Vec2* size,float angle,float radius,std::uint32_t* flag,sprite::Vec3* hit,int preview,std::uint32_t target){return calculate_damage(owner,p,size,angle,radius,flag,hit,preview,target,environment());}
}
