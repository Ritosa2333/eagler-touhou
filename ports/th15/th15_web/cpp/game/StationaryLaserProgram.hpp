#pragma once
#include "LaserVisual.hpp"
#include "BulletShooter.hpp"
namespace th15 {
class StationaryLaserProgram {
public:
    u32 flags=0;i32 index=0;Timer freeze{0,0,0,0,0};std::string error;
    bool update(const std::array<BulletTransform,18>&,LaserVisual&,i32& state,i32& protection,float rate);
};
}
