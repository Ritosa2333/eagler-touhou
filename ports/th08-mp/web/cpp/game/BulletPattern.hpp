#pragma once
#include "BulletEmission.hpp"
namespace th08 {
struct BulletPatternResult {float angle=0,speed=0;};
BulletPatternResult bullet_pattern(const BulletEmission&,i32 index,i32 layer,float aim,Rng&)noexcept;
}
