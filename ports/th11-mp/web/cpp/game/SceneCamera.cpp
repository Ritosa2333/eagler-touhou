#include "SceneCamera.hpp"
#include <cmath>
namespace th11 {
void SceneCamera::perspective(){
    const auto add=[](Vec3 a,Vec3 b){return Vec3{float(double(a.x)+b.x),float(double(a.y)+b.y),float(double(a.z)+b.z)};};
    view=GraphicsMath::look_at(add(position,eye_offset),add(position,direction),up);
    projection=GraphicsMath::perspective(fov,float(double(viewport.width)/viewport.height),30,1800);
    right=GraphicsMath::normalize(GraphicsMath::cross(direction,up));
}
void SceneCamera::screen(){
    const float width=float(viewport.width),height=float(viewport.height),x=float(double(width)*.5),y=float(double(height)*.5);
    const float tangent=float(std::tan(double(0.15707963705062866f))),distance=float(double(y)/tangent);
    view=GraphicsMath::look_at({x,y,distance},{x,y,0},{0,-1,0});
    projection=GraphicsMath::perspective(0.3141592741012573f,float(double(width)/height),1,10000);
}

SceneCamera blend_camera(const SceneCamera& a,const SceneCamera& b,float alpha){
    const auto mix=[alpha](float previous,float current){return previous+(current-previous)*alpha;};
    const auto mix3=[&](const Vec3& previous,const Vec3& current){return Vec3{mix(previous.x,current.x),mix(previous.y,current.y),mix(previous.z,current.z)};};
    SceneCamera out=b;
    out.position=mix3(a.position,b.position);out.direction=mix3(a.direction,b.direction);
    out.up=mix3(a.up,b.up);out.right=mix3(a.right,b.right);out.eye_offset=mix3(a.eye_offset,b.eye_offset);
    out.animation_delta=mix3(a.animation_delta,b.animation_delta);
    out.offset={mix(a.offset.x,b.offset.x),mix(a.offset.y,b.offset.y)};
    out.fov=mix(a.fov,b.fov);
    out.fog.near_distance=mix(a.fog.near_distance,b.fog.near_distance);
    out.fog.far_distance=mix(a.fog.far_distance,b.fog.far_distance);
    for(u32 i=0;i<4;++i)out.fog.channels[i]=mix(a.fog.channels[i],b.fog.channels[i]);
    return out;
}

bool camera_discontinuous(const SceneCamera& a,const SceneCamera& b,float position_limit,float direction_dot){
    const float dx=b.position.x-a.position.x,dy=b.position.y-a.position.y,dz=b.position.z-a.position.z;
    if(dx*dx+dy*dy+dz*dz>position_limit*position_limit)return true;
    const auto dot=[](const Vec3& x,const Vec3& y){return float(double(x.x)*y.x+double(x.y)*y.y+double(x.z)*y.z);};
    const float lengths=float(std::sqrt(double(dot(a.direction,a.direction))*double(dot(b.direction,b.direction))));
    if(lengths>1e-6f&&dot(a.direction,b.direction)/lengths<direction_dot)return true;
    const float ups=float(std::sqrt(double(dot(a.up,a.up))*double(dot(b.up,b.up))));
    return ups>1e-6f&&dot(a.up,b.up)/ups<direction_dot;
}
}
