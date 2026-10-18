#include "ShotSchedule.hpp"
namespace th11 {
i32 ShotSchedule::begin(i32 state,i32 warp,u32 held,const float* rate)noexcept{
    if(state!=1)return -1;
    if(timer.current<0){if(warp>=99||!(held&1))return -1;timer.set(0,rate);}
    return timer.current!=timer.previous?timer.current:-1;
}
void ShotSchedule::finish(u32 held,const float* rate)noexcept{
    if(timer.current>=14){if(held&1)timer.advance(-14);else timer.set(-1,rate);}
    else timer.tick();
}
i32 ShotSchedule::update(i32 state,i32 warp,u32 held,const float* rate)noexcept{
    const i32 emitted=begin(state,warp,held,rate);
    if(state==1&&timer.current>=0)finish(held,rate);
    return emitted;
}
}
