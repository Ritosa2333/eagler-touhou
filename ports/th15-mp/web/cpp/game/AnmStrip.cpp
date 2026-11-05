#include "AnmStrip.hpp"
#include "AnmCoordinates.hpp"
#include <cmath>
namespace th15 {
namespace {Vec2 radial(float angle,float radius){return {float(std::cos(double(angle))*double(radius)),float(std::sin(double(angle))*double(radius))};}}
bool anm_update_strip(AnmVm& vm)noexcept{
    auto& v=vm.visual;const u32 mode=v.draw_mode();if(mode!=9&&mode!=13&&mode!=14&&mode!=24&&mode!=25)return true;const i32 count=vm.variables.integers[0];if(count<=0)return true;
    const float denominator=float(wrapping_add(count,-1)),uv_step=float(float(vm.variables.integers[1])/denominator);float uv_y=0;
    const u32 color=v.flags&0x60000?v.secondary_color:v.color;
    if(mode==24||mode==25){auto& vertices=vm.geometry.world_vertices;if(u32(count)>vertices.size()/2)return false;
        const auto* values=vm.variables.floats;float angle=normalize_angle(float(values[3]-float(values[0]*.5f))),step=float(values[0]/denominator),half=float(values[1]*.5f),r1=values[2],r2=values[2];
        if(mode==25){r1=float(r1-half);r2=float(half+r2);half=0;}const float lower=-half;
        for(i32 i=0;i<count;i++){const auto a=radial(angle,r1),b=radial(angle,r2);vertices[i*2]={{a.x,half,a.y},color,{float(v.uv_offset.x+v.uv[0].x),float(v.uv_offset.y+uv_y)}};vertices[i*2+1]={{b.x,lower,b.y},color,{float(v.uv[1].x+v.uv_offset.x),float(v.uv_offset.y+uv_y)}};uv_y=float(uv_y+uv_step);angle=normalize_angle(float(angle+step));}return true;
    }
    auto& vertices=vm.geometry.screen_vertices;if(u32(count)>vertices.size()/2)return false;Vec3 center;if(!anm_position(vm,center))return false;
    float angle=mode==9?vm.variables.rotation.z:normalize_angle(float(vm.variables.rotation.z-float(vm.variables.rotation.x*.5f)));if(mode==14)angle=normalize_angle(vm.variables.rotation.z);const float step=float((mode==9?6.283185482025146484375f:vm.variables.rotation.x)/denominator);
    const float half=float(v.scale.x*.5f);float outer=float(v.scale.y+half),inner=float(v.scale.y-half);if(vm.creation_parent&&!(v.render_flags&0x10000)){outer=float(outer*vm.creation_parent->visual.scale.x);inner=float(vm.creation_parent->visual.scale.y*inner);}
    const u32 resolution=v.render_flags>>20&7;if(resolution==1||resolution==2){if(!vm.environment)return false;const float factor=resolution==1?vm.environment->resolution_scale:float(vm.environment->resolution_scale*.5f);inner=float(factor*inner);outer=float(outer*factor);}
    const i32 limit=mode==9?count-1:count;for(i32 i=0;i<limit;i++){const auto a=radial(angle,outer),b=radial(angle,inner);vertices[i*2]={ {float(a.x+center.x),float(a.y+center.y),center.z},1,mode==9?v.color:color,{float(v.uv_offset.x+v.uv[0].x),float(v.uv_offset.y+uv_y)}};vertices[i*2+1]={{float(b.x+center.x),float(b.y+center.y),center.z},1,color,{float(v.uv[1].x+v.uv_offset.x),float(v.uv_offset.y+uv_y)}};uv_y=float(uv_y+uv_step);angle=normalize_angle(float(angle+step));}
    if(mode==9){vertices[(count-1)*2]=vertices[0];vertices[(count-1)*2+1]=vertices[1];vertices[(count-1)*2].uv.y=vertices[(count-1)*2+1].uv.y=float(v.uv_offset.y+uv_y);}return true;
}
}
