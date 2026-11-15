#pragma once
#include <algorithm>
#include <cmath>
namespace th19 {
// EXLaserInf 0x53d280 / 0x53d150: coordinates and tangents are screen pixels.
struct MarisaSkill {
    int frame=0,duration=60;
    float from_x=0,from_y=0,to_x=0,to_y=0,tx=0,ty=0;
    struct Point {float x,y;};
    Point position()const {
        float t=std::clamp(float(frame)/duration,0.f,1.f),u=1-t;
        float a=u*u*(2*t+1),b=(3-2*t)*t*t,c=u*u*t+(t-1)*t*t;
        return {a*from_x+b*to_x+c*tx,a*from_y+b*to_y+c*ty};
    }
    bool arrived()const{return frame>=duration;}
    static int flightDuration(unsigned random16){return (random16&65535)%100+60;}
};
}
