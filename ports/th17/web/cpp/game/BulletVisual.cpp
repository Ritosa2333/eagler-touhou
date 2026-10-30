#include "BulletVisual.hpp"
namespace th17 {
bool BulletVisual::clear_children(){
    if(body.object_host&&!body.object_host->release_tree(body)){error="Bullet animation child cleanup failed";return false;}
    if(manager&&overlay_handle&&!manager->retire(overlay_handle)){error=manager->error;return false;}return true;
}
void BulletVisual::push()noexcept{body.visual.flags=state.visual_flags;body.variables.position=state.visual_jitter;body.visual.secondary_color=state.graze_color;body.pending_interrupt=state.hit_interrupt;}
void BulletVisual::pull()noexcept{state.visual_flags=body.visual.flags;state.visual_jitter=body.variables.position;state.graze_color=body.visual.secondary_color;state.hit_interrupt=body.pending_interrupt;state.spawn_animation_finished=body.variables.integers[0]!=0;}
bool BulletVisual::bind(i32 script,i32 overlay_script){
    error.clear();if(!resource||script<0||!body.bind(*resource,u32(script))){error="Bullet animation resource unavailable";return false;}
    body.sprite_source=&source;if(body.tick(random,rate)<0){error=body.error;return false;}
    if((body.visual.render_flags&0xc000)==0x8000)body.visual.render_flags=(body.visual.render_flags&~0x8000u)|0x4000;
    body.visual.render_flags=(body.visual.render_flags&~0x80000u)|0x40000;
    if(overlay_script){if(!manager){error="Bullet overlay animation manager unavailable";return false;}const i32 bank=manager->resource_id(resource);manager->rate=rate;overlay_handle=manager->create(bank,overlay_script,-1,0,state.motion.position);if(!overlay_handle){error=manager->error;return false;}}
    state.overlay_active=overlay_handle!=0;pull();return true;
}
bool BulletVisual::interrupt(i32 id)noexcept{body.pending_interrupt=id;state.hit_interrupt=id;return true;}
bool BulletVisual::interrupt_overlay(i32 id)noexcept{if(!manager||!overlay_handle)return true;return manager->interrupt(overlay_handle,id);}
i32 BulletVisual::tick(bool overlay_animation){
    // TH17 overlay handles belong to AnmManager and advance in its own pass.
    if(overlay_animation)return 0;push();const i32 result=body.tick(random,rate);pull();if(result<0)error=body.error;return result;
}
}
