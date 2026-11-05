#include "object_draw.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../ecl_vm/math.hpp"
#include "../platform_window/directx_math.hpp"
#include <cstring>
#include <system_error>
#ifdef TH_SDL3
#include "platform/GraphicsMath.hpp"
#endif
namespace th20::source::background {
namespace n=th20::recovered;namespace s=sprite;namespace dx=platform_window::directx;
namespace {
float sub(float a,float b){return _mm_cvtss_f32(_mm_sub_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float div(float a,float b){return _mm_cvtss_f32(_mm_div_ss(_mm_set_ss(a),_mm_set_ss(b)));}
float unsigned_float(std::uint32_t value){auto v=_mm_cvtsi32_sd(_mm_setzero_pd(),static_cast<std::int32_t>(value));if(value&0x80000000u)v=_mm_add_sd(v,_mm_set_sd(4294967296.0));return _mm_cvtss_f32(_mm_cvtsd_ss(_mm_setzero_ps(),v));}
#ifndef TH_SDL3
struct ProjectionLibrary {
    HMODULE module=LoadLibraryW(L"d3dx9_43.dll");
    using Translation=D3DMATRIX*(WINAPI*)(D3DMATRIX*,float,float,float);
    using ProjectArray=s::Vec3*(WINAPI*)(s::Vec3*,UINT,const s::Vec3*,UINT,const D3DVIEWPORT9*,const D3DMATRIX*,const D3DMATRIX*,const D3DMATRIX*,UINT);
    Translation translation;ProjectArray project;
    ProjectionLibrary(){if(!module)throw std::system_error(GetLastError(),std::system_category(),"D3DX9 SDK");translation=reinterpret_cast<Translation>(GetProcAddress(module,"D3DXMatrixTranslation"));project=reinterpret_cast<ProjectArray>(GetProcAddress(module,"D3DXVec3ProjectArray"));if(!translation||!project)throw std::system_error(ERROR_PROC_NOT_FOUND,std::system_category(),"D3DX projection exports");}
    ~ProjectionLibrary(){if(module)FreeLibrary(module);}
};
ProjectionLibrary& sdk(){static ProjectionLibrary instance;return instance;}
#endif
}
int cull_object(const Object& object,const s::Vec3& instance,float squared_limit,const program_entry::ViewportState& camera){
    const auto* p=object.parameters;const s::Vec3 center{p[5],p[6],p[7]};
    const s::Vec3 delta{sub(n::add32(center.x,instance.x),n::add32(camera.vectors[0][0],camera.vectors[5][0])),sub(n::add32(center.y,instance.y),n::add32(camera.vectors[0][1],camera.vectors[5][1])),sub(n::add32(center.z,instance.z),n::add32(camera.vectors[0][2],camera.vectors[5][2]))};
    const float square=n::add32(n::add32(n::mul32(delta.x,delta.x),n::mul32(delta.y,delta.y)),n::mul32(delta.z,delta.z));if(square>squared_limit)return 1;
    std::uint32_t mode;std::memcpy(&mode,p,4);if(mode)return 0;
    const s::Vec3 half{div(p[8],2.f),div(p[9],2.f),div(p[10],2.f)};s::Vec3 corners[16],projected[16];
    for(unsigned i=0;i<8;++i)corners[i]={(i&4)?sub(center.x,half.x):n::add32(center.x,half.x),(i&2)?sub(center.y,half.y):n::add32(center.y,half.y),(i&1)?sub(center.z,half.z):n::add32(center.z,half.z)};
    corners[8]={center.x,sub(center.y,half.y),sub(center.z,half.z)};corners[9]={center.x,n::add32(center.y,half.y),sub(center.z,half.z)};corners[10]={center.x,sub(center.y,half.y),n::add32(center.z,half.z)};corners[11]={center.x,n::add32(center.y,half.y),n::add32(center.z,half.z)};
    corners[12]={center.x,sub(center.y,half.y),center.z};corners[13]={center.x,n::add32(center.y,half.y),center.z};corners[14]={center.x,sub(center.y,half.y),sub(center.z,div(half.z,2.f))};corners[15]={center.x,n::add32(center.y,half.y),n::add32(div(half.z,2.f),center.z)};
#ifdef TH_SDL3
    float world[16];web::math::translation(world,instance.x,instance.y,instance.z);
    {const web::math::Viewport vp{camera.viewport.X,camera.viewport.Y,camera.viewport.Width,camera.viewport.Height,camera.viewport.MinZ,camera.viewport.MaxZ};
    web::math::project_array(reinterpret_cast<web::math::Vec3*>(projected),12,reinterpret_cast<const web::math::Vec3*>(corners),12,16,vp,&camera.projection._11,&camera.view._11,world);}
#else
    D3DMATRIX world;sdk().translation(&world,instance.x,instance.y,instance.z);sdk().project(projected,12,corners,12,&camera.viewport,&camera.projection,&camera.view,&world,16);
#endif
    const float x=unsigned_float(camera.viewport.X),y=unsigned_float(camera.viewport.Y);float left=n::add32(n::add32(n::add32(640.f,0.f),8.f),x),top=n::add32(n::add32(n::add32(480.f,0.f),8.f),y),right=n::add32(sub(0.f,8.f),x),bottom=n::add32(sub(0.f,8.f),y);
    for(const auto& point:projected){if(point.z<0.f||point.z>1.f)continue;if(point.x<left)left=point.x;if(right<point.x)right=point.x;if(point.y<top)top=point.y;if(bottom<point.y)bottom=point.y;}
    return n::add32(x,0.f)<=right&&left<=n::add32(n::add32(640.f,0.f),x)&&n::add32(y,0.f)<=bottom&&top<=n::add32(n::add32(0.f,480.f),y)?0:1;
}
void update_perspective_camera(program_entry::ViewportState& camera){
    auto& device=s::draw_environment::device();if(auto* c=program_entry::sprite_controller)s::flush_textured_quads(*c,device);
    const dx::Vector3 direction{camera.vectors[3][0],camera.vectors[3][1],camera.vectors[3][2]},eye{n::add32(camera.vectors[5][0],camera.vectors[0][0]),n::add32(camera.vectors[5][1],camera.vectors[0][1]),n::add32(camera.vectors[5][2],camera.vectors[0][2])},target{n::add32(direction.x,eye.x),n::add32(direction.y,eye.y),n::add32(direction.z,eye.z)},up{camera.vectors[2][0],camera.vectors[2][1],camera.vectors[2][2]};
    dx::look_at(camera.view,eye,target,up);dx::perspective(camera.projection,camera.field_of_view,div(unsigned_float(camera.viewport.Width),unsigned_float(camera.viewport.Height)),30.f,8000.f);
#ifdef TH_SDL3
    device.transform(touhou::graphics::MatrixKind::View,&camera.view._11);device.transform(touhou::graphics::MatrixKind::Projection,&camera.projection._11);
#else
    device.SetTransform(D3DTS_VIEW,&camera.view);device.SetTransform(D3DTS_PROJECTION,&camera.projection);
#endif
    s::Vec3 cross{sub(n::mul32(direction.y,up.z),n::mul32(direction.z,up.y)),sub(n::mul32(direction.z,up.x),n::mul32(direction.x,up.z)),sub(n::mul32(direction.x,up.y),n::mul32(direction.y,up.x))};
    const float length=ecl::math::square_root(n::add32(n::add32(n::mul32(cross.x,cross.x),n::mul32(cross.y,cross.y)),n::mul32(cross.z,cross.z)));if(length>=.01f){cross.x=div(cross.x,length);cross.y=div(cross.y,length);cross.z=div(cross.z,length);}std::memcpy(camera.vectors[4],&cross,12);
}
}

