#include "BackgroundView.hpp"
#include "BackgroundScript.hpp"
#include "Presentation.hpp"
#include "PresentationVisual.hpp"
#include "GameMath.hpp"
#include <algorithm>
namespace th08 {
namespace {
float background_angle(float before,float current){if(presentation::world_alpha>=1)return current;if(presentation::world_alpha<=0)return before;constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=current-before;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;return add_angle(before+delta*presentation::world_alpha,0);}
u8 background_channel(u8 before,u8 current){return u8(std::clamp(presentation::lerp_world(float(before),float(current)),0.0f,255.0f));}
}
void BackgroundView::snapshot_spell_presentation(){if(!presentation_marker.capture())return;for(u32 i=0;i<presentation_spell_vms.size();++i)presentation_spell_vms[i]=state.spell_vms[i];presentation_spell_valid=true;}
AnmVm BackgroundView::presentation_spell_vm(u32 index)const{
    if(index>=presentation_spell_vms.size())return {};const auto& source=state.spell_vms[index];AnmVm draw=source;if(!presentation::active||!presentation_spell_valid)return draw;const auto& before=presentation_spell_vms[index];
    if(before.anmFile!=source.anmFile||before.scriptIndex!=source.scriptIndex||before.visible!=source.visible||source.currentTimeInScript.current<before.currentTimeInScript.current)return draw;
    const presentation::VisualSample sample(before);
    sample.apply(source,draw,presentation::world_alpha,presentation::VisualSample::Uv,sample.authored_uv_fields(source));
    const float dx=source.pos.x-before.pos.x,dy=source.pos.y-before.pos.y;if(dx*dx+dy*dy<16384.0f)draw.pos={presentation::lerp_world(before.pos.x,source.pos.x),presentation::lerp_world(before.pos.y,source.pos.y),presentation::lerp_world(before.pos.z,source.pos.z)};
    const float ox=source.pos2.x-before.pos2.x,oy=source.pos2.y-before.pos2.y;if(ox*ox+oy*oy<16384.0f)draw.pos2={presentation::lerp_world(before.pos2.x,source.pos2.x),presentation::lerp_world(before.pos2.y,source.pos2.y),presentation::lerp_world(before.pos2.z,source.pos2.z)};
    draw.rotation={background_angle(before.rotation.x,source.rotation.x),background_angle(before.rotation.y,source.rotation.y),background_angle(before.rotation.z,source.rotation.z)};
    if(before.scale.x*source.scale.x>=0&&before.scale.y*source.scale.y>=0)draw.scale={presentation::lerp_world(before.scale.x,source.scale.x),presentation::lerp_world(before.scale.y,source.scale.y)};
    draw.color1.b=background_channel(before.color1.b,source.color1.b);draw.color1.g=background_channel(before.color1.g,source.color1.g);draw.color1.r=background_channel(before.color1.r,source.color1.r);draw.color1.a=background_channel(before.color1.a,source.color1.a);
    draw.color2.b=background_channel(before.color2.b,source.color2.b);draw.color2.g=background_channel(before.color2.g,source.color2.g);draw.color2.r=background_channel(before.color2.r,source.color2.r);draw.color2.a=background_channel(before.color2.a,source.color2.a);return draw;
}
JobResult BackgroundView::high(){
    auto& s=state;auto& r=renderer;
    if(!th08::presentation::render_only){presentation={s.spell_flag,s.tint_color,s.use_tint,s.effect_visible,true};objects.snapshot();}
    else if(presentation.valid){restore={s.spell_flag,s.tint_color,s.use_tint,s.effect_visible,s.effect_flags,{},true};for(u32 i=0;i<32;++i)restore.effect_positions[i]=s.effect_positions[i];s.spell_flag=presentation.spell_flag;s.tint_color=presentation.tint_color;s.use_tint=presentation.use_tint;s.effect_visible=presentation.effect_visible;saved_camera=s.camera;s.camera=script.presentation_camera();camera_override=true;}
    s.effect_flags=0;for(u32 i=0;i<16;++i)s.effect_positions[i]={};
    auto viewport=r.viewport;viewport.x=32;viewport.y=16;viewport.width=384;viewport.height=448;r.viewport=viewport;r.begin_background();
    if(!r.fog_disabled)r.set_fog(false);r.flush();
    if(s.spell_flag){r.set_viewport({32,16,384,448,0,1});r.clear_target(1,0xff000000);s.spell_flag=0;}
    r.set_viewport(viewport);
    if(s.tint_color.a){r.mix_enabled=true;r.mix_color=s.tint_color.d3dColor;}s.tint_color.d3dColor=0x00808080;
    if(s.spell_state<2&&!actions.stage_finished()){for(u32 i=0;i<2;++i)if(s.layers[i].activeSpriteIndex>0)layer(s.layers[i]);if(s.moon_effect)actions.moon(*s.moon_effect);}
    if((s.clear_color&0xff000000)==0xff000000)r.clear_target(3,s.clear_color);
    else{if(s.clear_color)rectangle(s.clear_color);r.clear_target(2,s.clear_color);}
    r.set_depth_func(DepthFunc::LessEqual);u32 fog=s.fog.color.d3dColor;
    if(r.mix_enabled){u32 result=fog&0xff000000;for(u32 shift=0;shift<24;shift+=8)result|=std::min(255u,(((fog>>shift)&255)*((r.mix_color>>shift)&255))>>7)<<shift;fog=result;}
    r.set_fog_color(fog);r.set_fog_range(s.fog.near_plane,s.fog.far_plane);if(!r.fog_disabled)r.set_fog(true);
    if(s.spell_state<2&&!actions.stage_finished()){objects.draw(0);objects.draw(1);}return JobResult::Continue;
}
JobResult BackgroundView::low(){
    auto& s=state;auto& r=renderer;
    if(s.spell_state<2&&!actions.stage_finished()){
        objects.draw(2);objects.draw(3);if(!r.fog_disabled)r.set_fog(false);actions.effects();
        if(s.spell_state==1){const float frames=presentation::active&&s.spell_frames>0?presentation::lerp_world(float(s.spell_frames-1),float(s.spell_frames)):float(s.spell_frames);const i32 alpha=std::clamp(i32(frames*255.0f/60.0f),0,255);r.flush();r.set_depth_func(DepthFunc::Always);if(!r.fog_disabled)r.set_fog_state(false);rectangle(u32(alpha)<<24);}
    }
    r.flush();r.set_depth_func(DepthFunc::Always);if(!r.fog_disabled)r.set_fog(false);
    if(s.spell_state>0){for(i32 i=0;i<s.spell_vm_count&&i<32;++i){if(presentation::render_only){auto vm=presentation_spell_vm(u32(i));layer(vm);}else layer(s.spell_vms[i]);}if(s.callback)s.callback(s,r,callback_context);}
    r.screen_camera();r.set_viewport(r.viewport);r.set_fog_range(1000,2000);
    if(!s.use_tint){r.mix_enabled=false;r.mix_color=0x80808080;}s.use_tint=0;s.effect_visible=0;
    if(restore.active){s.spell_flag=restore.spell_flag;s.tint_color=restore.tint_color;s.use_tint=restore.use_tint;s.effect_visible=restore.effect_visible;s.effect_flags=restore.effect_flags;for(u32 i=0;i<32;++i)s.effect_positions[i]=restore.effect_positions[i];restore.active=false;}if(camera_override){s.camera=saved_camera;camera_override=false;}
    return JobResult::Continue;
}
}
