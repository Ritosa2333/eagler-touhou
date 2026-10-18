#include "Player.hpp"
#include "HitboxDisplay.hpp"
namespace th15 {
bool Player::draw(AnmRenderer& renderer){
 if(life.state==2)return true;
 auto& root=visuals.root;root.presentation_motion=true;root.visual.translation=motion.position;
 root.visual.render_flags=(root.visual.render_flags&~0x80000u)|0x40000u;
 if(renderer.draw(root)==-2){error=renderer.error;return false;}
 return true;
}
bool Player::draw_hitbox(AnmRenderer& renderer,i32 layer){
 auto& root=visuals.root;
 if(presentation::always_hitbox&&life.state==1&&!visuals.native_focus_visible()){
  const float scale=motion.behavior_flags&16?float(float(motion.enlargement-1)*2+1):1;
  const auto* child=visuals.focus_template(25);const auto* source=visuals.focus_template(26);
  if(child&&source){auto& markers=visuals.hitbox_display.sample(*child,*source,motion.position,scale,frame.input_age.fractional,root.presentation_generation,visuals.focus_rate());
   // Keep the parent relationship inside this draw transaction as the native
   // two-layer focus effect does. Renderer scratch writes stay in these copies.
   auto copies=markers;copies[0].rotation_parent=copies[0].creation_parent=&copies[1];
   auto& marker=copies[layer==14?1:0];
   if(marker.visual.layer==layer&&renderer.draw(marker)==-2){error=renderer.error;return false;}
  }
 }else visuals.hitbox_display.reset();
 return true;
}
}
