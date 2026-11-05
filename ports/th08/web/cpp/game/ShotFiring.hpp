#pragma once
#include "Timer.hpp"
namespace th08 {
struct ShotFiringInputs {i32 stage_frame=0,bomb=0,bomb_type=0;u16 buttons=0;u8 character=0,gui_blocked=0;i8 player_state=0;u8 reserved[3]{};};
struct ShotFiringActions {virtual ~ShotFiringActions()=default;virtual void fire(i32 frame)=0;};
void trigger_shooting(Timer& timer)noexcept;
void update_shooting(Timer& timer,const ShotFiringInputs& input,const FrameTiming& timing,ShotFiringActions& actions);
}
