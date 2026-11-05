#include "LineIntersection.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
bool line_intersection(const Vec2& first,float first_angle,const Vec2& second,float second_angle,Vec2& result)noexcept{
    const float a=normalize_angle(first_angle),b=normalize_angle(second_angle),delta=float(a-b);
    const float difference=normalize_angle(delta>3.1415927410125732f?float(a-float(b+6.2831854820251465f)):float(b-a)>3.1415927410125732f?float(a-float(b-6.2831854820251465f)):delta);
    if(std::abs(difference)<.001f){result=first;return false;}
    float parallel=normalize_angle(difference);if(float(parallel-3.1415927410125732f)>3.1415927410125732f)parallel=float(parallel-9.42477798461914f);else if(float(3.1415927410125732f-parallel)>3.1415927410125732f)parallel=float(parallel-(-3.1415927410125732f));else parallel=float(parallel-3.1415927410125732f);parallel=normalize_angle(parallel);
    if(std::abs(parallel)<.001f){result=first;return false;}
    const float ax=float(std::cos(double(first_angle))*10.),ay=float(std::sin(double(first_angle))*10.),bx=float(std::cos(double(second_angle))*10.),by=float(std::sin(double(second_angle))*10.);
    const bool av=std::abs(ax)<.01f,bv=std::abs(bx)<.01f;const float slope=av?0:float(ay/ax),other=bv?0:float(by/bx),constant=av?first.x:float(first.y-float(slope*first.x)),other_constant=bv?second.x:float(second.y-float(other*second.x));
    if(av){if(bv){if(std::abs(float(first.x-second.x))>=.001f)return false;result=first;}else result={first.x,float(float(other*first.x)+other_constant)};}
    else if(bv)result={second.x,float(float(slope*second.x)+constant)};
    else{const float numerator=float(other_constant-constant),denominator=float(slope-other);result={float(numerator/denominator),float(float(float(numerator*slope)/denominator)+constant)};}
    return true;
}
}
