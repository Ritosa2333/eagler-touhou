#include "AnmCoordinates.hpp"
#include <cmath>
namespace th15 {
bool anm_adjust_position(AnmVm& vm,Vec3& p,u32 depth)noexcept{
    if(depth>=64)return false;const auto& v=vm.visual;const u32 scale=v.render_flags>>20&7;
    if(scale>=1&&scale<=4){if(!vm.environment)return false;const float factor=scale==1||scale==3?vm.environment->resolution_scale:float(vm.environment->resolution_scale*.5f);p={float(p.x*factor),float(p.y*factor),float(p.z*factor)};}
    if(vm.rotation_parent&&!(v.render_flags&0x10000)){const auto& parent=*vm.rotation_parent;if(v.render_flags&0x800000){const float sine=float(std::sin(double(parent.variables.rotation.z))),cosine=float(std::cos(double(parent.variables.rotation.z))),x=p.x;p.x=float(float(x*cosine)-float(p.y*sine));p.y=float(float(p.y*cosine)+float(x*sine));}
        Vec3 origin;if(!anm_position(*vm.rotation_parent,origin,depth+1))return false;p={float(p.x+origin.x),float(p.y+origin.y),float(p.z+origin.z)};
    }else{const u32 offset=v.render_flags>>18&3;if(offset){if(!vm.environment)return false;const auto& values=vm.environment->screen_offsets;const u32 first=offset==1?2:0;p.x=float(float(values[first])+p.x);p.y=float(float(values[first+1])+p.y);}}return true;
}
bool anm_position(AnmVm& vm,Vec3& out,u32 depth)noexcept{const auto& p=vm.variables.position;const auto& v=vm.visual;out={float(float(p.x+v.translation.x)+v.child_anchor.x),float(v.child_anchor.y+float(p.y+v.translation.y)),float(v.child_anchor.z+float(p.z+v.translation.z))};return anm_adjust_position(vm,out,depth);}
bool anm_quad_positions(AnmVm& vm,Vec3 (&out)[4])noexcept{
    auto& v=vm.visual;const u32 mode=v.draw_mode(),ax=v.flags>>21&3,ay=v.flags>>23&3;if(mode>3||ax>2||ay>2)return false;
    constexpr float xs[3][4]={{-.5f,.5f,-.5f,.5f},{0,1,0,1},{-1,0,-1,0}},ys[3][4]={{-.5f,-.5f,.5f,.5f},{0,0,1,1},{-1,-1,0,0}};
    float scale_x=float(v.secondary_scale.x*v.scale.x),scale_y=float(v.secondary_scale.y*v.scale.y);if(vm.creation_parent&&!(v.render_flags&0x10000)){const auto& parent=vm.creation_parent->visual;scale_x=float(float(parent.secondary_scale.x*parent.scale.x)*scale_x);scale_y=float(float(parent.secondary_scale.y*parent.scale.y)*scale_y);}
    const u32 resolution=v.render_flags>>20&7;float factor=1;if(resolution==1||resolution==2){if(!vm.environment)return false;factor=resolution==1?vm.environment->resolution_scale:float(vm.environment->resolution_scale*.5f);}
    float sine=0,cosine=1;if(mode==1){const auto rotation=vm.total_rotation();cosine=float(std::cos(double(rotation.z)));sine=float(std::sin(double(rotation.z)));}
    const auto& p=vm.variables.position;Vec3 center{float(v.child_anchor.x+float(v.translation.x+p.x)),float(v.child_anchor.y+float(p.y+v.translation.y)),float(v.child_anchor.z+float(p.z+v.translation.z))};if(!anm_adjust_position(vm,center))return false;
    const float z=float(float(v.translation.z+p.z)+v.child_anchor.z);for(u32 i=0;i<4;i++){float x=float(float(xs[ax][i]*v.sprite_size.x)-v.size.x),y=float(float(ys[ay][i]*v.sprite_size.y)-v.size.y);if(resolution==1||resolution==2){x=float(x*factor);y=float(y*factor);}x=float(x*scale_x);y=float(y*scale_y);
        if(mode==1)out[i]={float(float(float(x*cosine)-float(y*sine))+center.x),float(float(float(x*sine)+float(y*cosine))+center.y),z};else out[i]={float(x+center.x),float(y+center.y),z};
    }return true;
}
}
