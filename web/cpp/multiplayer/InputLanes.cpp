#include "InputLanes.hpp"
#include <cmath>
namespace th11::multiplayer {
bool InputLanes::Capture(std::uint32_t held,bool pause,int motion,float x,float y,Netplay::FrameInput& out){
    if(motion<0||motion>3||!std::isfinite(x)||!std::isfinite(y))return false;
    Netplay::FrameInput next(std::uint16_t(held&0x3ffu)|(held&0x200000u?Restart:0)|(pause?Pause:0));
    if(motion==3){if(x<-1||x>1||y<-1||y>1)return false;next.analogMode=Netplay::AnalogMode::Joystick;next.x=x;next.y=y;}
    else if(motion){if(x<-184||x>184||y<32||y>432)return false;next.analogMode=Netplay::AnalogMode::DirectTouch;next.x=x;next.y=y;next.unlimited=motion==2;}
    next.touchUsed=motion!=0;
    if(!Netplay::IsValidFrameInput(next))return false;out=next;return true;
}
bool InputLanes::Decode(const Netplay::FrameInput* in,unsigned count,std::array<MultiplayerInput,3>& out){
    if(!in||count<2||count>3)return false;
    std::array<MultiplayerInput,3> next{};
    for(unsigned s=0;s<count;++s){const auto& f=in[s];if(!Netplay::IsValidFrameInput(f))return false;
        if(f.buttons&~(0x3ffu|Pause|Restart))return false;
        auto& n=next[s];n.held=(f.buttons&0x3ffu)|(f.buttons&Restart?0x200000u:0);n.pause=(f.buttons&Pause)!=0;
        if(f.analogMode==Netplay::AnalogMode::DirectTouch){if(f.x<-184||f.x>184||f.y<32||f.y>432)return false;n.touch_mode=f.unlimited?2:1;}
        else if(f.analogMode==Netplay::AnalogMode::Joystick)n.touch_mode=3;
        else if(f.analogMode!=Netplay::AnalogMode::None)return false;
        n.touch_x=f.x;n.touch_y=f.y;
    }
    out=next;return true;
}
}
