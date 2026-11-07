#include "AnmStrip.hpp"
#include "AnmCoordinates.hpp"
#include <cmath>
namespace th17 {
bool anm_update_strip(AnmVm& vm,u32 mode)noexcept{
    auto& v=vm.visual;const i32 count=vm.variables.integers[0];if(count<=0)return true;
    const float denominator=float(wrapping_add(count,-1)),uv_step=float(float(vm.variables.integers[1])/denominator);float uv_y=0;
    const u32 color=v.flags&0x60000?v.secondary_color:v.color;
    // Both edges share the same angle. Round each radius product separately.
    const float u0=float(v.uv_offset.x+v.uv[0].x),u1=float(v.uv[1].x+v.uv_offset.x);
    if(mode==24||mode==25){auto& vertices=vm.geometry.world_vertices;if(u32(count)>vertices.size()/2)return false;
        const auto* values=vm.variables.floats;float angle=normalize_angle(float(values[3]-float(values[0]*.5f))),step=float(values[0]/denominator),half=float(values[1]*.5f),r1=values[2],r2=values[2];
        if(mode==25){r1=float(r1-half);r2=float(half+r2);half=0;}const float lower=-half;
        for(i32 i=0;i<count;i++){
            const double c=std::cos(double(angle)),s=std::sin(double(angle));
            const float y=float(v.uv_offset.y+uv_y);
            vertices[i*2]={{float(c*double(r1)),half,float(s*double(r1))},color,{u0,y}};
            vertices[i*2+1]={{float(c*double(r2)),lower,float(s*double(r2))},color,{u1,y}};
            uv_y=float(uv_y+uv_step);angle=normalize_angle(float(angle+step));
        }
        return true;
    }
    auto& vertices=vm.geometry.screen_vertices;if(u32(count)>vertices.size()/2)return false;Vec3 center;if(!anm_position(vm,center))return false;
    float angle=mode==9?vm.variables.rotation.z:normalize_angle(float(vm.variables.rotation.z-float(vm.variables.rotation.x*.5f)));if(mode==14)angle=normalize_angle(vm.variables.rotation.z);const float step=float((mode==9?6.283185482025146484375f:vm.variables.rotation.x)/denominator);
    const float half=float(v.scale.x*.5f);float outer=float(v.scale.y+half),inner=float(v.scale.y-half);if(vm.creation_parent&&!(v.render_flags&0x10000)){outer=float(outer*vm.creation_parent->visual.scale.x);inner=float(vm.creation_parent->visual.scale.y*inner);}
    const u32 resolution=v.render_flags>>20&7;if(resolution==1||resolution==2){if(!vm.environment)return false;const float factor=resolution==1?vm.environment->resolution_scale:float(vm.environment->resolution_scale*.5f);inner=float(factor*inner);outer=float(outer*factor);}
    const i32 limit=mode==9?count-1:count;const u32 outer_color=mode==9?v.color:color;
    for(i32 i=0;i<limit;i++){
        const double c=std::cos(double(angle)),s=std::sin(double(angle));
        const float y=float(v.uv_offset.y+uv_y);
        vertices[i*2]={{float(float(c*double(outer))+center.x),float(float(s*double(outer))+center.y),center.z},1,outer_color,{u0,y}};
        vertices[i*2+1]={{float(float(c*double(inner))+center.x),float(float(s*double(inner))+center.y),center.z},1,color,{u1,y}};
        uv_y=float(uv_y+uv_step);angle=normalize_angle(float(angle+step));
    }
    if(mode==9){vertices[(count-1)*2]=vertices[0];vertices[(count-1)*2+1]=vertices[1];vertices[(count-1)*2].uv.y=vertices[(count-1)*2+1].uv.y=float(v.uv_offset.y+uv_y);}return true;
}
}
