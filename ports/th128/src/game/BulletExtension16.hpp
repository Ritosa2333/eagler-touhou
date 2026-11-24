#pragma once
#include "BulletRules.hpp"

namespace th128::bullet {
struct Extension16 {
    bool active=false;
    int previous=-1,time=0;
    float fraction=0,targetSpeed=0,angleOffset=0;
    int period=0,limit=0,count=0,mode=2,angleMode=0;
};
struct Extension16Step {
    Extension16 next;
    float speed=0,angle=0;
    Position velocity;
    bool soundRequired=false,finished=false;
};
std::optional<Extension16> initializeExtension16(const ExtensionRaw& record,float speed,float angle) noexcept;
std::optional<Extension16Step> stepExtension16(const Extension16& state,float speed,float angle,Position velocity,
    Position position,Position player,int sound,const Position* cachedPosition=nullptr) noexcept;
}
