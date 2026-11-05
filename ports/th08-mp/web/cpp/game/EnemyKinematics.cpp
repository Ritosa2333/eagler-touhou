#include "EclVm.hpp"
namespace th08 {
// Original 0x42c180; comparisons retain the unordered-value branch order.
void EclVm::clamp_position()noexcept{
    if(!(flags&0x80000))return;
    if(!(bounds[0]<=position.x))position.x=bounds[0];else if(bounds[2]<position.x)position.x=bounds[2];
    if(!(bounds[1]<=position.y))position.y=bounds[1];else if(bounds[3]<position.y)position.y=bounds[3];
}
// Original 0x42deb0 measures displacement before integrating this frame.
// The horizontal mirror flag applies to velocity, including scripted motion.
void EclVm::apply_velocity(const FrameTiming& timing)noexcept{
    last_delta={Scalar::sub(position.x,previous_position.x),Scalar::sub(position.y,previous_position.y),Scalar::sub(position.z,previous_position.z)};
    previous_position=position;
    const auto x=number(timing.rate)*number(velocity.x);
    position.x=((flags&0x40000)?number(position.x)-x:x+number(position.x)).to_float();
    position.y=(number(timing.rate)*number(velocity.y)+number(position.y)).to_float();
    position.z=(number(timing.rate)*number(velocity.z)+number(position.z)).to_float();
}
// Position phase in EnemyManager::OnUpdate (0x42c660). Combat, culling,
// animation and enemy lifecycle phases must be performed by the manager.
void EclVm::integrate_position(const FrameTiming& timing)noexcept{
    if(!(flags&0x20000000)){
        clamp_position();apply_velocity(timing);clamp_position();
        if(parent&&(flags&0x200))position_offset=parent->position;
    }
    refresh_position();resolved_position.z=0;
}
}
