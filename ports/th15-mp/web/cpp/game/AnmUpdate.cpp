#include "AnmCoordinates.hpp"
#include "AnmStrip.hpp"
#include <algorithm>
namespace th15 {
bool AnmVm::advance(float rate,bool tracked_interpolators){
    if((visual.render_flags&0x1000000)&&(visual.scale_velocity.x!=0||visual.scale_velocity.y!=0||visual.uv_velocity.x!=0||visual.uv_velocity.y!=0))changed_program();
    visual.advance(variables.rotation,rate);if(environment&&(visual.flags&0x8000)){const auto& d=environment->background_delta;visual.translation={float(visual.translation.x+d.x),float(visual.translation.y+d.y),float(d.z+visual.translation.z)};}
    if(visual.render_flags&0x2000){changed_program();Vec3 points[4];if(!anm_quad_positions(*this,points)){error="Animation screen UV geometry unavailable";return false;}for(u32 i=0;i<4;i++){visual.uv[i]={float(points[i].x/640.f),float(points[i].y/480.f)};if(visual.uv[i].x<0)visual.uv[i].x=0;if(visual.uv[i].y<0)visual.uv[i].y=0;}}
    if(!tracked_interpolators||!interpolation_idle){const bool changed=interpolators.advance(variables,visual,rate);interpolation_idle=!changed;if(changed)changed_program();}if(!anm_update_strip(*this)){error="Animation strip geometry or environment unavailable";return false;}return true;
}
}
