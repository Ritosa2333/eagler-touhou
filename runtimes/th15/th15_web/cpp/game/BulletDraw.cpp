#include "BulletScene.hpp"
namespace th15 {
bool BulletVisual::draw(AnmRenderer& renderer){
 const auto prepare=[&](AnmVm& vm){vm.presentation_motion=true;if(vm.visual.render_flags&0x80){vm.variables.rotation.z=normalize_angle(float(state.motion.angle+1.5707963705062866f));vm.visual.flags|=4;}if(state.flags&0x40){vm.visual.flags|=8;vm.visual.secondary_scale={state.scale,state.scale};}};
 if(overlay.visual.visible()){overlay.variables.position=state.motion.position;prepare(overlay);if(renderer.draw(overlay)==-2){error=renderer.error;return false;}}
 body.visual.translation=state.motion.position;
 prepare(body);if(renderer.draw(body)==-2){error=renderer.error;return false;}state.visual_flags=body.visual.flags;return true;
}
bool BulletScene::draw(AnmRenderer& renderer){
 for(const auto& group:manager.draw_groups)for(u32 index:group){auto* vm=visual(index);if(!vm){error="Bullet draw slot unavailable";return false;}if(!vm->draw(renderer)){error=vm->error;return false;}}return true;
}
}
