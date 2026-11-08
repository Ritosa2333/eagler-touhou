#include "AnmGeometry.hpp"

namespace th13 {
void AnmRibbon::initialize(GameRandom& random) {
    // 00402a20: 418-byte state, sixteen (3,0,-16) segments, timer=1.
    *this={};
    angle=float(double(random.signed_unit())*double(3.1415927f));
    for(auto& segment:segments)segment={3,0,-16};
    age.set(1);
}

void AnmRibbon::update(GameRandom& random,float rate) {
    // 00402b20 shifts the segment history every fourth timer tick. Its
    // callback runs before the ordinary ANM instructions/UV scroll.
    if(age.current%4==0) {
        for(size_t i=segments.size()-1;i>0;--i)segments[i]=segments[i-1];
        const float x=segments[0].x;
        if(x<3.f) {
            const float value=x>0?random.signed_unit():random.unit();
            segments[0].x=float(double(x)+double(value)*double(0.1f));
        } else segments[0].x=float(double(x)-double(random.unit())*double(0.1f));
    }
    Vec3 point{};float width=0;
    for(size_t i=0;i<segments.size();++i) {
        vertices[i*2]={{float(point.x+width),point.y,point.z},0x80ffffffu,{0,float(i)*0.0625f}};
        vertices[i*2+1]={{float(point.x-width),point.y,point.z},0x80ffffffu,{1,float(i)*0.0625f}};
        if(width<8)width+=2;
        point={float(point.x+segments[i].x),float(point.y+segments[i].y),float(point.z+segments[i].z)};
    }
    // Native vertex alpha pattern, including the deliberately asymmetric
    // intermediate vertices (402e36..402ead); no uniform-opacity substitute.
    for(u32 i:{30u,31u,0u,1u,15u,10u,24u,3u,12u,27u})vertices[i].color=0x00ffffffu;
    vertices[28].color=vertices[29].color=0x50ffffffu;
    vertices[26].color=0xb0ffffffu;
    age.tick(&rate);
}
}
