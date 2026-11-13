#pragma once
#include "ShtResource.hpp"
#include "Timer.hpp"
namespace th11 {
struct ShotSchedule {
    Timer timer;
    // Reports the frame to emit before advancing the original 15-frame cycle.
    // A stopped player retains the timer but clears its homing lock elsewhere.
    i32 update(i32 player_state,i32 warp_state,u32 held,const float* rate)noexcept;
    i32 begin(i32 player_state,i32 warp_state,u32 held,const float* rate)noexcept;
    void finish(u32 held,const float* rate)noexcept;
};
#if PORT_TITLE == 16
struct Th16ShotRequests {
    virtual ~Th16ShotRequests()=default;
    virtual bool spawn(u32 packed,i32 frame)=0;
};
struct Th16FireFrames {
    virtual ~Th16FireFrames()=default;
    virtual bool fire(i32 frame,i32 long_frame)=0;
};
struct Th16ShotSchedule {
    Timer regular,long_timer;
    u32 locked_id=0;u8 locked=0;
    bool update(i32 state,u32 held,const float* rate,Th16FireFrames&);
};
struct Th16SeasonState;
bool shot_th16_select(const ShtResource&,const ShtResource&,i32 power,i32 step,bool focused,const Th16SeasonState&,i32 frame,i32 long_frame,Th16ShotRequests&);
#endif
}
