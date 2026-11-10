#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include "NativeRandom.hpp"
namespace th19 {
// 0x51c8c0: deliberately independent of ordinary cancellation locks/width.
inline bool tsukasaReflects(int state,unsigned kind,float bx,float by,float x,float y,float radius){
    if(state!=1||kind>=4||radius<0)return false;
    float dx=bx-x,dy=by-y,r=radius+1;return dx*dx+dy*dy<=r*r;
}
// 0x402540 / 0x401960: Park-Miller state, shared replay seed not yet wired.
struct ReflectionRng {
    uint32_t state=1;
    uint32_t next(){state=uint32_t(uint64_t(state)*48271%2147483647);return state;}
    float unit(){return float(next())/float(2147483646u);}
    float signedUnit(){return float(next())/(float(2147483647u)/2-1)-1;}
};
// Descriptor 6 at 0x5c6650+6*0x158, first visual column per color.
inline int reflectedSprite(int color){return color>=0&&color<16?96+color:-1;}
// 0x5450e0: the ordinary counter queue starts with descriptor 1; later
// battle-time bands mix descriptor 4. This is unrelated to the sender character.
inline int counterBulletType(int frame,uint32_t draw){
    return frame<3600?1:frame<7200?(draw%5>3?4:1):frame<10800?(draw%5>2?4:1):(draw%6>2?4:1);
}
// 0x51ca8d..0x51cb6c: death circles keep the incoming point-bullet
// descriptor, with a 10/20/30 percent descriptor-4 substitution as time grows.
inline int chainBulletType(int incoming,int frame,uint32_t draw){
    if(frame<3600)return incoming;
    const unsigned threshold=frame<7200?9:frame<10800?8:7;
    return draw%10<threshold?incoming:4;
}
// 0x51c250 / 0x403967 mode 8. Inputs are native screen coordinates.
// RNG draws are supplied explicitly: do not substitute an invented RNG here.
struct ReflectedBulletFlight {
    struct Point {float x=0,y=0;};
    Point from,to,start_tangent,end_tangent;
    int frame=0,duration=60,type=6,color=0;
    float base_speed=0;
    static int flightDuration(unsigned random16){return (random16&65535)%100+60;}
    static float baseSpeed(float incoming){return std::min(6.f,incoming+.5f);}
    // 0x51c9fe/0x51ca67: X uses a signed draw, Y a nonnegative unit draw.
    static Point destination(float source_x,float signed_draw,float unit_draw){return {source_x+16*signed_draw,32*unit_draw};}
    Point position()const{
        float t=std::clamp(float(frame)/std::max(1,duration),0.f,1.f),u=1-t;
        float a=u*u*(2*t+1),b=t*t*(3-2*t),c=u*u*t,d=(t-1)*t*t;
        return {a*from.x+b*to.x+c*start_tangent.x+d*end_tangent.x,
                a*from.y+b*to.y+c*start_tangent.y+d*end_tangent.y};
    }
    bool arrived()const{return frame>=duration;}
    static float arrivalAngle(float x,float unit_draw){
        constexpr float pi=3.1415927410125732f;
        if(x<=-64)return pi*.5f-unit_draw*pi/6;
        if(x>=64)return pi*.5f+unit_draw*pi/6;
        return pi*.5f+(2*unit_draw-1)*pi/6;
    }
    // 0x51bed1 normalizes a signed draw directly in the middle of the field.
    // The edges use 0x404c90's unsigned draw and point inwards.
    static float nativeArrivalAngle(float x,uint32_t draw){
        constexpr float halfTurn=3.1415927410125732f;
        constexpr float forward=halfTurn/2.f, spread=halfTurn/6.f;
        constexpr float range=NativeRandom::range;
        if(std::abs(x)<64.f)return forward+(float(draw)/(range/2.f-1.f)-1.f)*spread;
        const float offset=(float(draw)/(range-1.f))*spread;
        return x<0.f?forward-offset:forward+offset;
    }
    // 0x51bff1: random draw is nonnegative, NOT signed jitter.
    float arrivalSpeed(float unit_draw,int rank,int rank_cap)const{
        return base_speed+.2f*unit_draw+float(std::min(std::max(rank,0),rank_cap))/200.f*2.f;
    }
};
}
