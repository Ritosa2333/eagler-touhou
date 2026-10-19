#pragma once
#include "ShotResource.hpp"
#include "Timer.hpp"
namespace th08 {
struct PlayerMovementState {
    Vec3 position,velocity,history[16],half_boxes[3],bounds[6];
    Vec2 multiplier{1,1},delta; i32 direction=0;
};
struct PlayerMovementActions {virtual ~PlayerMovementActions()=default;virtual void pose(i32 script)=0;virtual bool movement(const PlayerMovementState&,float,const FrameTiming&,float&,float&){return false;}};
i32 player_direction(u16 buttons)noexcept;
// Movement/bounds/pose/history phases of 0044aec0. The owner handles form
// transitions and option callbacks before/after these phases in native order.
void move_player(PlayerMovementState& state,const ShotProfile& human,const ShotProfile& focused,
                 bool focus,u8 character,u16 buttons,const Vec2& minimum,const Vec2& extent,
                 const FrameTiming& timing,PlayerMovementActions* actions,bool record_history=true);
void record_player_position(PlayerMovementState&)noexcept;
}
