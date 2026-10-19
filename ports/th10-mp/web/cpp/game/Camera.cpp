#include "Camera.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
Extended unsigned_number(u32 bits){auto value=Extended::from_int(static_cast<i32>(bits));if(bits&0x80000000)value=value+number(4294967296.0f);return value;}
Vec3 add(const Vec3& a,const Vec3& b){return {Scalar::add(a.x,b.x),Scalar::add(a.y,b.y),Scalar::add(a.z,b.z)};}
}
void CameraEnvironment::flush(){if(auto* manager=*animation_manager){AnmRenderer renderer{*manager,*render_environment};renderer.flush();}}
void CameraEnvironment::copy_offset(const Camera& camera){if(auto* manager=*animation_manager)manager->draw_offset=camera.draw_offset;}
// 0x421480. Screen-space sprites still use perspective projection; its eye
// distance cancels the FOV at z=0. Aspect uses stored floats, the half extents
// retain the unsigned conversion's extended precision until vector storage.
void Camera::configure_flat(CameraEnvironment& environment){
    environment.flush();
    const auto width=unsigned_number(viewport.width),height=unsigned_number(viewport.height);
    const auto half_width=width*number(.5f),half_height=height*number(.5f);
    const auto aspect=(number(width.to_float())/number(height.to_float())).to_float();
    const Vec3 target{half_width.to_float(),half_height.to_float(),0};
    const Vec3 eye{half_width.to_float(),half_height.to_float(),(half_height/tangent(Extended::from_double(0.15707963705062866))).to_float()};
    const Vec3 down{0,-1,0};
    environment.look_at(view,eye,target,down);
    environment.perspective(projection,0x1.41b2f8p-2f,aspect,1,10000);
    environment.render_environment->set_transform(Matrices::View,view);environment.render_environment->set_transform(Matrices::Projection,projection);
    environment.copy_offset(*this);
}
// 0x4215a0. Read camera fields after each platform call where the original does.
void Camera::configure_world(CameraEnvironment& environment){
    environment.flush();const auto target=add(target_offset,position),eye=add(eye_offset,position);
    environment.look_at(view,eye,target,up);
    environment.perspective(projection,field_of_view,(unsigned_number(viewport.width)/unsigned_number(viewport.height)).to_float(),30,1800);
    environment.render_environment->set_transform(Matrices::View,view);environment.render_environment->set_transform(Matrices::Projection,projection);
    right={(number(up.z)*number(target_offset.y)-number(up.y)*number(target_offset.z)).to_float(),(number(up.x)*number(target_offset.z)-number(up.z)*number(target_offset.x)).to_float(),(number(up.y)*number(target_offset.x)-number(up.x)*number(target_offset.y)).to_float()};
    environment.normalize(right);environment.copy_offset(*this);
}
// 0x4216f0 preserves matrices, interpolation-related fields and fog state.
void Camera::initialize(Camera& world,Camera& ui) noexcept {
    ui.position={0,0,1000};ui.target_offset={0,0,0};ui.up={0,1,0};ui.eye_offset={0,0,0};
    world.position={0,0,1000};world.target_offset={0,0,0};world.up={0,1,0};
    ui.field_of_view=0x1.0c1524p-1f;ui.viewport={0,0,640,480,0,1};ui.screen_space=1;
    world.field_of_view=0x1.0c1524p-1f;world.viewport={32,16,384,448,0,1};world.screen_space=0;world.eye_offset={0,0,0};
}
}
