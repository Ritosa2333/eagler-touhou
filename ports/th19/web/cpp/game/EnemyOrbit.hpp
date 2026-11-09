#pragma once
#include <cmath>
#include <array>
namespace th19 {
// EXE 0x4d4aff..0x4d4be3: subtract the ellipse rotation BEFORE
// projecting the phase, scale local X, then rotate back to field coordinates.
inline std::array<float,2> enemyEllipseOffset(float phase,float rotation,float radius,float ratio){
    const float local=std::remainder(phase-rotation,6.28318530717958647692f);
    const float u=std::cos(local)*radius*ratio,v=std::sin(local)*radius;
    return {u*std::cos(rotation)-v*std::sin(rotation),u*std::sin(rotation)+v*std::cos(rotation)};
}
}
