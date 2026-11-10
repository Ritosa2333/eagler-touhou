#pragma once
#include <array>
#include <cmath>
#include <algorithm>
namespace th19 {
// EXReigekiInf: 0x53b920 init, 0x53b3a0 update, pl00.anm ordinal 20.
struct ReimuSkill {
    struct Point {float x,y;};
    int frame=0;
    float x=0,y=0,angle=0;
    bool active()const{return frame<105;}
    float orbitRadius()const{
        // Native mode 1 is t^2; the second 22-frame interpolation starts at 22.
        float t=std::clamp(float(frame<22?frame:frame-22)/22.f,0.f,1.f);
        return frame<22?64*t*t:64*(1-t*t);
    }
    std::array<Point,3> centers()const{
        std::array<Point,3> out{};float r=orbitRadius();
        for(int i=0;i<3;++i){float a=angle+i*6.28318530718f/3;out[i]={x+r*std::cos(a),y+r*std::sin(a)};}
        return out;
    }
    float clearRadius()const{return frame<45?64.f:128.f+float((frame-45)*64/60);}
    bool createsDamage()const{return frame>=45||frame%5==0;}
    int damage()const{return frame<45?4:1;}
    int damageDuration()const{return frame<45?1:24;}
    float damageGrowth()const{return frame<45?0:1;}
    void tick(){++frame;angle=std::remainder(angle+3.141592654f/18,6.28318530718f);}
};
}
