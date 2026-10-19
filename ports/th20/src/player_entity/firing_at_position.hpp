#pragma once
#include "firing.hpp"
namespace th20::source::player_entity {
int fire_shots_at_position(ShotController&,int pattern,int frame,int secondary_frame,const sprite::Vec3&,FiringServices&); //505e40
inline int fire_shots_at_position(ShotController& owner,int pattern,int frame,int secondary,const sprite::Vec3& p){return fire_shots_at_position(owner,pattern,frame,secondary,p,firing_services());}
}
