#pragma once
#include <cmath>
namespace th19 {
// ECL412, EXE 0x501013..0x5012ce. Inputs are the native signed-angle
// draw and (only in the middle band) the following 16-bit RNG draw.
inline float nativeBossMoveAngle(float x,float y,float playerX,float bx,float by,
                                float bw,float bh,float randomAngle,unsigned random16){
    constexpr float pi=3.14159274101257324219f;
    float angle;
    if(x<bx-bw*.25f)angle=randomAngle/3.f;
    else if(x>bx+bw*.25f)angle=std::remainder(randomAngle/3.f+pi,2*pi);
    else{
        angle=randomAngle/4.f;
        if(playerX>x?random16%3==0:random16%3!=0)angle+=pi;
    }
    if(y<by-bh*.25f)angle=std::abs(angle);
    else if(y>by+bh*.25f)angle=-std::abs(angle);
    if(std::abs(angle+pi*.5f)<.05f)angle=-pi*.5f+(angle<-pi*.5f?-.05f:.05f);
    else if(std::abs(angle-pi*.5f)<.05f)angle=pi*.5f+(angle<pi*.5f?-.05f:.05f);
    return angle;
}
}
