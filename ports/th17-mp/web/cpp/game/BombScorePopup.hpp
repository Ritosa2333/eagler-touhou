#pragma once
#include "HudDrawData.hpp"
#include "Timer.hpp"
namespace th17 {
struct BombScorePopup {
    float amount=0,displayed=-1;Timer age{0,0,0,0,0};i32 tier=0;Vec3 position{};
    bool draw(HudDrawServices&,const Vec3& player,float rate,std::string& error);
};
}
