#pragma once
#include <algorithm>
#include <cmath>

namespace th10::multiplayer {
// Conservative REJECTION ONLY, not a replacement hit/graze test. The retail
// routine rounds left/right/top/graze edges to float but keeps other edges in
// extended precision. With finite operands bounded to +/-65536 every authored
// edge is within 1/128 pixel of its mathematical edge. Our extra pixel makes
// this double-precision hull strictly larger, including negative-size inputs.
// Boundary, exceptional and unbounded inputs ALWAYS use the original routine.
// Neither native hitboxes nor the 24-pixel graze extent are changed.
struct PlayerCollisionBroadphase {
    double left=0,right=0,top=0,bottom=0;
    bool bounded=false;
    static bool finite_domain(float value) noexcept {
        return value>=-65536.0f&&value<=65536.0f;
    }
    PlayerCollisionBroadphase(float x,float y,float width,float height) noexcept {
        bounded=finite_domain(x)&&finite_domain(y)&&finite_domain(width)&&finite_domain(height);
        if(!bounded)return;
        const double dx=std::max(24.0,std::abs(double(width))*.5)+1.0;
        const double dy=std::max(24.0,std::abs(double(height))*.5)+1.0;
        left=double(x)-dx;right=double(x)+dx;top=double(y)-dy;bottom=double(y)+dy;
    }
    bool MayOverlap(float minX,float minY,float maxX,float maxY)const noexcept {
        if(!bounded||!finite_domain(minX)||!finite_domain(minY)||
           !finite_domain(maxX)||!finite_domain(maxY)||minX>maxX||minY>maxY)return true;
        return !(right<double(minX)||bottom<double(minY)||double(maxX)<left||double(maxY)<top);
    }
};
} // namespace th10::multiplayer
