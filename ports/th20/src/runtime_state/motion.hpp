#pragma once
#include "../sprite_renderer/animation.hpp"
namespace th20::source::state {
// Original shared constructor478530; used by Enemy, Bomb and DamageRegion.
struct Motion {
    sprite::Vec3 position,velocity;
    float field_18,angle_1c,field_20,field_24,angle_28,field_2c,angle_30,field_34;
    sprite::Vec3 vector_38;
    std::uint32_t field_44;
};
static_assert(sizeof(Motion)==0x48);
void update_motion_velocity(Motion&,float clock_scale);       //453e40
void update_motion_position(Motion&,float clock_scale);       //453ac0
void snap_motion_position(Motion&);                          //4543d0, floor coordinates to1/100
void update_motion(Motion&,float clock_scale);                //47a1f0
bool outside_motion_bounds(const Motion&,float x,float y,float width,float height); //47a400
}
