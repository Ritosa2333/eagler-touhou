#include "BulletScene.hpp"
namespace th17 {
bool BulletVisual::draw(AnmRenderer& renderer){
    body.visual.translation=state.motion.position;
    if(body.visual.render_flags&0x80){body.variables.rotation.z=normalize_angle(float(state.motion.angle+1.5707963705062866f));body.visual.flags|=4;}
    if(state.flags&0x40){body.visual.flags|=8;body.visual.secondary_scale={state.scale,state.scale};}
    if(renderer.draw(body)==-2){error=renderer.error;return false;}state.visual_flags=body.visual.flags;return true;
}
bool BulletScene::draw(AnmRenderer& renderer){
    if(drawing_disabled)return true;
    for(const auto& group:manager.draw_groups)for(u32 index:group){auto* vm=visual(index);if(!vm||!vm->draw(renderer)){error=vm?vm->error:"Bullet draw slot unavailable";return false;}}return true;
}
}
