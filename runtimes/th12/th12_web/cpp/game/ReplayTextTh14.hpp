#pragma once
#include "AsciiText.hpp"
#include <cstdio>

namespace th11 {
// Retail455380 queues the recorded rate, separately from the live FPS label.
inline bool th14_replay_fps_request(bool controller,u32 mode,u8 fps,AsciiRequest& out,u32 controller_flags=0){
    if(!controller||(controller_flags&4)||mode!=1)return false;
    char text[16];std::snprintf(text,sizeof text,"%3u",unsigned(fps));
    AsciiStyle style;
    style.color=fps<30?0xff5050ffu:fps<50?0xffa0a0ffu:0xffffffffu;
    out={text,{383,450,0},style};return true;
}
}
