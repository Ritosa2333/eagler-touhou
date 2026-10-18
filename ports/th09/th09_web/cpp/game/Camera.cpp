#include "Camera.hpp"
#include <cmath>
namespace th09 {
void Camera::screen(const Viewport& v){
    const float x=float(v.width)*.5f,y=float(v.height)*.5f,fov=0x1.41b2f8p-2f;
    const float distance=float(double(y)/std::tan(double(fov*.5f)));
    view=GraphicsMath::look_at({x,y,distance},{x,y,0},{0,-1,0});projection=GraphicsMath::perspective(fov,float(v.width)/float(v.height),1,10000);
}
void Camera::scene(const Viewport& v,const BackgroundCamera& c){
    const Vec3 eye{c.position.x+c.offset.x,c.position.y+c.offset.y,c.position.z+c.offset.z},target{c.position.x+c.direction.x,c.position.y+c.direction.y,c.position.z+c.direction.z};
    view=GraphicsMath::look_at(eye,target,c.up);projection=GraphicsMath::perspective(c.field_of_view,float(v.width)/float(v.height),30,1800);right=GraphicsMath::normalize(GraphicsMath::cross(c.direction,c.up));
}
}
