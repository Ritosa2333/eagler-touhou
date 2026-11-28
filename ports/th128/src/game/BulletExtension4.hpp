#pragma once
#include "BulletRules.hpp"
namespace th128::bullet {
struct Extension4 {
    bool active=false;
    int previous=-1,time=0,period=0;
    float fraction=0,magnitude=0,direction=0;
    Position vector;
};
struct Extension4Step {Extension4 next;float speed=0,angle=0;Position velocity;bool finished=false;};
std::optional<Extension4> initializeExtension4(const ExtensionRaw& record,float angle,Position position,Position player) noexcept;
std::optional<Extension4Step> stepExtension4(const Extension4& state,float speed,float angle,Position velocity) noexcept;
}
