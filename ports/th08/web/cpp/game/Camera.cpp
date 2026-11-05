#include "Camera.hpp"
#include "GraphicsMath.hpp"
#include "GameMath.hpp"
namespace th08 {
void Camera::screen(Matrix4& view,Matrix4& projection,const Viewport& viewport) {
    const float x=(Extended::from_int64(viewport.width)/number(2)).to_float();
    const float y=(Extended::from_int64(viewport.height)/number(2)).to_float();
    const float aspect=(Extended::from_int64(viewport.width)/Extended::from_int64(viewport.height)).to_float();
    constexpr float fov=.3141592741012573f;
    const auto tan=tangent(Scalar::div(fov,2));
    const float distance=(number(y)/tan).to_float();
    GraphicsMath::look_at(view,{x,y,distance},{x,y,0},{0,-1,0});
    GraphicsMath::perspective(projection,fov,aspect,1,10000);
}
void Camera::scene(Matrix4& view,Matrix4& projection,const Viewport& viewport,SceneCamera& data) {
    const auto add=[](const Vec3& a,const Vec3& b) { return Vec3{Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)}; };
    GraphicsMath::look_at(view,add(data.eye_offset,data.position),add(data.target_offset,data.position),data.up);
    const float aspect=(Extended::from_int64(viewport.width)/Extended::from_int64(viewport.height)).to_float();
    GraphicsMath::perspective(projection,data.field_of_view,aspect,30,1800);
    const auto& a=data.target_offset; const auto& b=data.up;
    data.right={(number(a.y)*number(b.z)-number(a.z)*number(b.y)).to_float(),
                (number(a.z)*number(b.x)-number(a.x)*number(b.z)).to_float(),
                (number(a.x)*number(b.y)-number(a.y)*number(b.x)).to_float()};
    GraphicsMath::normalize(data.right,data.right);
}
}
