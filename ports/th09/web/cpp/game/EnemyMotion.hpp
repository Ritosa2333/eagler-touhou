#pragma once
#include "Types.hpp"
#include "Timer.hpp"
namespace th09 {
struct EclVm;struct EclInstruction;
struct EnemyMotion {
    Timer time{0,0,0};i32 duration=0;float orbit_growth=0,bounds[4]{};
    Vec3 hitbox,low_damage_hitbox;float player_protect_squared=0;
    Vec3 capture_velocity,previous_position;Timer capture_time{0,0,0};
    bool command(EclVm&,const EclInstruction&);
    void clamp(EclVm&) noexcept;
    void update_velocity(EclVm&,const FrameTiming&,float frame_step) noexcept;
    void integrate_position(EclVm&,const FrameTiming&)noexcept;
};
}
