// Purple TH06 wall hooks (MIT): 0x40D57C and 0x40D900.
// Typed template boundary keeps the original formulas and RNG draw order
// executable in tests without depending on Windows memory addresses.
#pragma once
#include <algorithm>
#include <cmath>
namespace PracticeRuntime::Purple {
template<class Bullets, class Random>
float WallAngle(int section, float bossx, float bossy, float plx, float ply,
                float lastx, float lasty, const Bullets& bullets, Random random) {
    const float angle_pl=std::atan2(ply-bossy,plx-bossx);
    const float decision=random();
    auto eligible=[](const auto& b) {
        return b.state && b.state!=5 && b.sprites.spriteBullet.sprite &&
            b.sprites.spriteBullet.sprite->heightPx<30.0f && b.speed==0.0f;
    };
    if(section==46) {
        if(decision<0.4f) {
            float nearest=99999.0f;
            for(const auto& b:bullets) if(eligible(b))
                nearest=std::min(nearest,std::hypot(b.pos.x-bossx,b.pos.y-bossy));
            if(decision<0.2f)
                return angle_pl-nearest*3.14159f/256.0f-0.5235988f+random()*0.5235988f;
            return angle_pl-nearest*3.14159f/256.0f-1.570796f+random()*1.745329f;
        }
        if(decision<0.9f)
            return 3.14159f-std::hypot(ply-bossy,plx-bossx)*3.14159f/256.0f-0.2617f+random()*0.5235988f;
        return random()*6.28318f-3.1415926f;
    }
    if(decision<0.8f) {
        float nearest=99999.0f,x=0,y=0;
        for(const auto& b:bullets) if(eligible(b)) {
            const float distance=std::hypot(b.pos.x-lastx,b.pos.y-lasty);
            if(nearest>distance) {nearest=distance;x=b.pos.x;y=b.pos.y;}
        }
        return angle_pl-std::hypot(x-bossx,y-bossy)*3.14159f/256.0f+(random()-0.5f)*2.0f*0.34f;
    }
    return random()*6.28318f-3.1415926f;
}
inline float WallSnipe(float original,float bx,float by,float px,float py,int near,int far) {
    const float distance=std::min(std::hypot(px-bx,py-by),400.0f);
    const float random_near=1.0f-near/100.0f,random_far=1.0f-far/100.0f;
    return original*(distance*(random_far-random_near)/400.0f+random_near)+std::atan2(py-by,px-bx);
}
}
