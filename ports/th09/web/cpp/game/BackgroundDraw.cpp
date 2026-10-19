#include "BackgroundDraw.hpp"
#include <algorithm>
namespace th09 {
u32 BackgroundDrawServices::tinted(u32 color,bool alpha)const noexcept{
    if(!tint_enabled)return color;u32 result=alpha?0:color&0xff000000;
    for(u32 shift=0;shift<(alpha?32u:24u);shift+=8)result|=std::min((u32(u8(color>>shift))*u8(tint>>shift))>>7,255u)<<shift;
    return result;
}
void BackgroundDraw::base(Background& bg,BackgroundDrawState& state,BackgroundDrawServices& s){
    if(s.fog_supported)s.fog(false);s.flush();s.select_field(s.side);
    if(state.clear_pending){s.clear(true,false,0xff000000);state.clear_pending=false;}
    if(bg.tint>>24){s.tint_enabled=true;s.tint=bg.tint;}
    if((bg.clear_color>>24)==255)s.clear(true,true,s.tinted(bg.fog.color,true));
    else{if(bg.clear_color)s.rectangle(32,16,416,464,bg.fog.color);s.clear(false,true,bg.clear_color);}
    if(bg.boss_state<2){
        if(bg.overlays[0].activeSpriteIndex>0){bg.overlays[0].pos.x=s.geometry.to_screen({-144,0,0}).x;bg.overlays[0].pos.z=.99f;s.draw_animation(bg.overlays[0]);}
        if(bg.overlays[1].activeSpriteIndex>0)s.draw_animation(bg.overlays[1]);
    }
    s.depth_compare(Compare::LessEqual);s.fog_color(s.tinted(bg.fog.color,false));s.fog_range(bg.fog.near_plane,bg.fog.far_plane);if(s.fog_supported)s.fog(true);
    if(bg.boss_state<2){s.models(bg,0);s.models(bg,1);}
    if(!state.preserve_tint){s.tint=0x80808080;s.tint_enabled=false;}
}
void BackgroundDraw::overlay(Background& bg,BackgroundDrawState& state,BackgroundDrawServices& s){
    s.select_field(s.side);
    if(bg.boss_state<2){s.models(bg,2);s.models(bg,3);if(s.fog_supported)s.fog(false);}
    if(bg.tint>>24){s.tint_enabled=true;s.tint=bg.tint;}bg.tint=0x00808080;
    s.flush();s.depth_compare(Compare::Always);if(s.fog_supported)s.fog(false);
    if(bg.boss_state>0){for(i32 i=0;i<bg.boss_count&&i<32;++i){auto& vm=bg.boss_animations[i];vm.pos=s.geometry.to_screen(vm.pos2);vm.pos.z=vm.pos2.z;s.draw_animation(vm);}s.custom_boss(bg);}
    s.texture_mode(false);s.screen_camera();s.fog_range(1000,2000);
    if(!state.preserve_tint){s.tint=0x80808080;s.tint_enabled=false;}state.preserve_tint=false;state.capture_positions=false;
}
}
