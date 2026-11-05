#include "StageCamera.hpp"
#include <cmath>
namespace th15 {
Vec3 normalize_scene_vector(Vec3 v)noexcept{
    const float squared=float(float(float(v.x*v.x)+float(v.y*v.y))+float(v.z*v.z));if(!(squared>=0x1p-46f))return {};
    const float inverse=float(1.f/float(std::sqrt(double(squared))));const float refined=float(float(.5f*inverse)*float(3.f-float(float(squared*inverse)*inverse)));return {float(v.x*refined),float(v.y*refined),float(v.z*refined)};
}
namespace {
Vec3 cross(Vec3 a,Vec3 b){return {float(float(a.y*b.z)-float(a.z*b.y)),float(float(a.z*b.x)-float(a.x*b.z)),float(float(a.x*b.y)-float(a.y*b.x))};}
Vec3 view_cross(Vec3 a,Vec3 b){return {float(double(a.y)*b.z-double(a.z)*b.y),float(double(a.z)*b.x-double(a.x)*b.z),float(double(a.x)*b.y-double(a.y)*b.x)};}
Vec3 add(Vec3 a,Vec3 b){return {float(a.x+b.x),float(a.y+b.y),float(a.z+b.z)};}
}
Matrix4 scene_look_at(Vec3 eye,Vec3 target,Vec3 up)noexcept{
    const Vec3 z=normalize_scene_vector({float(target.x-eye.x),float(target.y-eye.y),float(target.z-eye.z)}),x=normalize_scene_vector(view_cross(up,z)),y=view_cross(z,x);const Vec3 axes[]={x,y,z};Matrix4 out;for(u32 c=0;c<3;++c){const auto a=axes[c];out.m[c]=a.x;out.m[4+c]=a.y;out.m[8+c]=a.z;out.m[12+c]=-float((double(a.x)*eye.x+double(a.y)*eye.y)+double(a.z)*eye.z);}out.m[15]=1;return out;
}
Matrix4 scene_perspective(float fov,float aspect,float near_plane,float far_plane)noexcept{
    const float half=float(fov*.5f),c=float(std::cos(double(half))),s=float(std::sin(double(half))),cot=float(c/s);Matrix4 out;out.m[0]=float(cot/aspect);out.m[5]=cot;out.m[10]=float(far_plane/float(far_plane-near_plane));out.m[11]=1;out.m[14]=float(-out.m[10]*near_plane);return out;
}
AnmCamera stage_camera(const StageCamera& s,const GraphicsViewport& viewport)noexcept{
    AnmCamera out;out.eye=add(s.position,s.eye_offset);out.view=scene_look_at(out.eye,add(out.eye,s.direction),s.up);out.projection=scene_perspective(s.fov,float(float(viewport.width)/float(viewport.height)),30,8000);out.reference=normalize_scene_vector(cross(s.direction,s.up));out.fog_near=s.fog.near_distance;out.fog_far=s.fog.far_distance;out.fog_rgb={s.fog.channels[2],s.fog.channels[1],s.fog.channels[0]};out.fog_color=s.fog.color;return out;
}
}
