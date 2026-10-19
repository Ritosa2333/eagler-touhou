#include "AnmGeometry.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
void AnmTrail::initialize(Rng& random){points={};colors={};angle=float(random.signed_unit()*3.1415927410125732f);age=Timer{0,0,0,0,0};age.set(1);for(u32 i=0;i<64;i++){u32 color=0xff40ff00;if(i<8)color=(color&~255u)|u8(255-i*32);if(i>31)color=(color&0xffffffu)|(u32(u8(255-(i-32)*16))<<24);colors[i]=color;}}
void AnmTrail::initialize_orange(const Vec3& position,Rng& random){initialize(random);points[0]={float(position.x+320.f),float(position.y+16.f)};for(u32 i=0;i<64;i++){u32 color=0xff0080ff;if(i<8)color=(color&~0xff0000u)|(u32(u8(255-i*32))<<16);if(i>=32)color=(color&0xffffffu)|(u32(u8(255-(i-32)*16))<<24);colors[i]=color;}}
i32 AnmTrail::update(Rng& random,float rate){
    if(age.current>63)return 1;if(age.current!=age.previous){const i32 count=age.current;if(count<1){return -1;}for(i32 i=0;i<count;i++){auto& color=colors[u32(i)];const u32 alpha=color>>24;color=(color&0xffffff)|((alpha<16?0:alpha-16)<<24);}
        const float speed=float(float(random.unit()*5.f)+4.f);auto& p=points[u32(count)];p={float(std::cos(double(angle))*double(speed)),float(std::sin(double(angle))*double(speed))};p.x=float(points[u32(count)-1].x+p.x);p.y=float(points[u32(count)-1].y+p.y);angle=normalize_angle(float(float(float(random.signed_unit()*3.1415927410125732f)/5.f)+angle));}
    age.tick(&rate);return 0;
}
}
