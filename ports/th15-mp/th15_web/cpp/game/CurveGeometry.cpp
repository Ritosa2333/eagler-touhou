#include "CurveGeometry.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {
float edge_angle(const CurvePathSample& point,const CurvePathSample* prior,float side)noexcept{
    constexpr float pi=3.1415927410125732421875f,tau=6.283185482025146484375f,half_pi=1.57079637050628662109375f;
    const float current=normalize_angle(side>0?float(point.angle+half_pi):float(point.angle-half_pi));if(!prior)return current;
    const float previous=normalize_angle(side>0?float(prior->angle+half_pi):float(prior->angle-half_pi));float difference=float(previous-current);
    if(difference>pi)difference=float(previous-float(current+tau));else if(float(current-previous)>pi)difference=float(previous-float(current-tau));
    return normalize_angle(float(normalize_angle(float(normalize_angle(difference)*.5f))+current));
}
}
bool curve_strip(const CurvePathSample* points,u32 count,float width,const Vec2& offset,const Vec2& edge_v,std::vector<AnmGeometryVertex>& output){
    if((count&&!points)||count>0x100000)return false;output.resize(std::size_t(count)*2);float u=0;const float half_width=float(width*.5f);
    for(u32 i=0;i<count;i++){for(u32 side=0;side<2;side++){const float angle=edge_angle(points[i],i?&points[i-1]:nullptr,side? -1.f:1.f);const Vec2 delta{float(std::cos(double(angle))*double(half_width)),float(std::sin(double(angle))*double(half_width))};
        output[std::size_t(i)*2+side]={Vec3{float(offset.x+float(points[i].position.x+delta.x)),float(offset.y+float(points[i].position.y+delta.y)),0},1,0xffffffff,{u,side?edge_v.y:edge_v.x}};
    }u=float(float(1.f/float(i32(count)-1))+u);}return true;
}
}
