#pragma once
#include "Arithmetic.hpp"

namespace th08::background_math {
#ifdef TH_BACKGROUND_MATH_TEST
struct Stats { u64 fast=0,fallback=0; };
inline Stats stats;
inline void accepted(bool fast) { fast?++stats.fast:++stats.fallback; }
#else
inline void accepted(bool) {}
#endif
// A whole expression takes the float path only when every original rounding
// boundary is representable in the normal f32 range. On any failed guard,
// recompute the original Extended expression from its original inputs.
inline Extended dot3(float ax,float ay,float az,float bx,float by,float bz) {
    float x,y,z,xy,result;
    if(single_precision_nearest()&&
       touhou::numeric::scalar_binary(ax,bx,x,2)&&
       touhou::numeric::scalar_binary(ay,by,y,2)&&
       touhou::numeric::scalar_binary(x,y,xy,0)&&
       touhou::numeric::scalar_binary(az,bz,z,2)&&
       touhou::numeric::scalar_binary(xy,z,result,0)){
        accepted(true);return number(result);
    }
    accepted(false);return number(ax)*number(bx)+number(ay)*number(by)+number(az)*number(bz);
}
inline float center(float origin,float instance,float offset,float size) {
    float a,b,half,result;
    if(single_precision_nearest()&&
       touhou::numeric::scalar_binary(origin,instance,a,0)&&
       touhou::numeric::scalar_binary(a,offset,b,1)&&
       touhou::numeric::scalar_binary(size,2,half,3)&&
       touhou::numeric::scalar_binary(b,half,result,0)){
        accepted(true);return result;
    }
    accepted(false);return (number(origin)+number(instance)-number(offset)+number(size)/number(2)).to_float();
}
inline float translated(float local,float instance,float offset) {
    float sum,result;
    if(single_precision_nearest()&&
       touhou::numeric::scalar_binary(local,instance,sum,0)&&
       touhou::numeric::scalar_binary(sum,offset,result,1)){
        accepted(true);return result;
    }
    accepted(false);return (number(local)+number(instance)-number(offset)).to_float();
}
inline float coordinate(float animated,float origin,float instance,float offset) {
    float a,b,result;
    if(single_precision_nearest()&&
       touhou::numeric::scalar_binary(animated,origin,a,0)&&
       touhou::numeric::scalar_binary(a,instance,b,0)&&
       touhou::numeric::scalar_binary(b,offset,result,1)){
        accepted(true);return result;
    }
    accepted(false);return (number(animated)+number(origin)+number(instance)-number(offset)).to_float();
}
inline float scaled_add(float origin,float direction,float distance) {
    float product,result;
    if(single_precision_nearest()&&
       touhou::numeric::scalar_binary(direction,distance,product,2)&&
       touhou::numeric::scalar_binary(product,origin,result,0)){
        accepted(true);return result;
    }
    accepted(false);return (number(direction)*number(distance)+number(origin)).to_float();
}
}
