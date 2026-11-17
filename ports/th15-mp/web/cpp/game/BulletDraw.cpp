#include "BulletScene.hpp"
namespace th15 {
bool BulletVisual::draw(AnmRenderer& renderer,const MultiplayerGrazeVisual* graze){
 if(overlay.visual.visible()){overlay.variables.position=state.motion.position;prepare(overlay);if(renderer.draw(overlay)==-2){error=renderer.error;return false;}}
 body.visual.translation=state.motion.position;
 prepare(body);int result=0;
 // Multiplayer body drawing changes a temporary graze view. With no parent
 // rotation or owned panels it has no durable VM writes to replay.
 if(graze&&renderer.logical_frame()&&!body.rotation_parent&&!body.geometry.overlay){state.visual_flags=body.visual.flags;return true;}
 if(!graze)result=renderer.draw(body);
 else if(body.geometry.overlay){auto copy=body;copy.visual.flags=(copy.visual.flags&~0x60000u)|graze->flags;copy.visual.secondary_color=graze->color;copy.variables.position=graze->jitter;result=renderer.draw(copy);}
 else if(body.geometry.empty()&&!body.rotation_parent&&!body.creation_parent&&body.visual.draw_mode()<=3){
  const auto quad=body.visual.quad;const auto inherited=body.visual.inherited_color,flags=body.visual.flags,color=body.visual.secondary_color;const auto position=body.variables.position;
  body.visual.flags=(flags&~0x60000u)|graze->flags;body.visual.secondary_color=graze->color;body.variables.position=graze->jitter;
  result=renderer.draw(body);body.visual.flags=flags;body.visual.secondary_color=color;body.variables.position=position;body.visual.quad=quad;body.visual.inherited_color=inherited;
 }
 else{
  // Graze tint is a view of the same VM. Restore every Draw-written value
  // after submission instead of deep-copying the complete VM for each bullet.
  const auto visual=body.visual;const auto variables=body.variables;
  body.visual.flags=(body.visual.flags&~0x60000u)|graze->flags;body.visual.secondary_color=graze->color;body.variables.position=graze->jitter;
  result=renderer.draw(body);body.visual=visual;body.variables=variables;
 }
 if(result==-2){error=renderer.error;return false;}state.visual_flags=body.visual.flags;return true;
}
bool BulletScene::draw(AnmRenderer& renderer){
 const bool logical=renderer.logical_frame();
 for(const auto& group:manager.draw_groups)for(u32 index:group){auto* vm=owned_visual(index);if(!vm){error="Bullet draw slot unavailable";return false;}const auto* graze=viewpoint_graze(index);if(logical&&graze&&vm->logical_draw())continue;if(!vm->draw(renderer,graze)){error=vm->error;return false;}}return true;
}
}
