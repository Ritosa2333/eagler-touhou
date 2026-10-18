#include "BulletVisual.hpp"
namespace th15 {
bool BulletVisual::clear_children(){if((body.object_host&&!body.object_host->release_tree(body))||(overlay.object_host&&!overlay.object_host->release_tree(overlay))){error="Bullet embedded animation cleanup failed";return false;}return true;}
namespace {
bool initialize(AnmVm& vm,AnmResource& resource,i32 script,Rng& random,float rate,const AnmSpriteSource* source){
    if(script<0||!vm.bind(resource,u32(script)))return false;
    vm.sprite_source=source;
    if(vm.tick(random,rate)<0)return false;
    if((vm.visual.render_flags&0xc000)==0x8000)vm.visual.render_flags=(vm.visual.render_flags&~0x8000u)|0x4000;
    vm.visual.render_flags=(vm.visual.render_flags&~0x80000u)|0x40000;return true;
}
}
void BulletVisual::push()noexcept{body.visual.flags=state.visual_flags;body.variables.position=state.visual_jitter;body.visual.secondary_color=state.graze_color;body.pending_interrupt=state.hit_interrupt;}
void BulletVisual::pull()noexcept{state.visual_flags=body.visual.flags;state.visual_jitter=body.variables.position;state.graze_color=body.visual.secondary_color;state.hit_interrupt=body.pending_interrupt;state.spawn_animation_finished=body.variables.integers[0]!=0;}
bool BulletVisual::bind(i32 script,i32 overlay_script){
    error.clear();if(!resource){error="Bullet animation resource not loaded";return false;}
    if(!initialize(body,*resource,script,random,rate,&source)){error=body.error.empty()?"Invalid bullet body animation":body.error;return false;}
    const i32 layer=overlay.visual.layer;const Vec3 translation=overlay.visual.translation;overlay=AnmVm{};overlay.visual.layer=layer;overlay.visual.translation=translation;overlay.environment=body.environment;overlay.object_host=body.object_host;
    if(overlay_script&&!initialize(overlay,*resource,overlay_script,random,rate,nullptr)){error=overlay.error.empty()?"Invalid bullet overlay animation":overlay.error;return false;}
    if(!overlay_script)overlay.initialize_unbound();
    overlay.visual.flags&=overlay_script?~0u:~1u;state.overlay_active=overlay_script!=0;pull();return true;
}
bool BulletVisual::interrupt(i32 id)noexcept{body.pending_interrupt=id;state.hit_interrupt=id;return true;}
bool BulletVisual::interrupt_overlay(i32 id)noexcept{overlay.pending_interrupt=id;return true;}
i32 BulletVisual::tick(bool overlay_animation){
    AnmVm& vm=overlay_animation?overlay:body;if(!overlay_animation)push();
    const int result=vm.tick(random,rate);if(!overlay_animation)pull();
    if(result<0)error=vm.error;return result;
}
}
