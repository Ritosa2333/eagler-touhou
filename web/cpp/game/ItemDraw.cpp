#include "ItemManager.hpp"
namespace th15 {
bool ItemManager::draw(AnmRenderer& renderer){
 for(auto& slot:slots){auto& s=slot.state;if(!s.state||!slot.body||!slot.body->visual.visible()||s.delay>0)continue;
  slot.body->presentation_motion=true;slot.body->visual.translation=s.position;if(slot.arrow){slot.arrow->presentation_motion=true;slot.arrow->visual.translation=s.position;}
  if(slot.body->variables.position.y<-8.f){if(slot.arrow&&slot.arrow->visual.visible()){auto& arrow=*slot.arrow;const float y=float(arrow.variables.position.y+8.f);arrow.variables.position.y=8;const u32 alpha=y>=32.f?255u:u32(u8(truncate_int(float(float(y*.03125f)*255.f))));arrow.visual.color=(arrow.visual.color&0xffffffu)|(alpha<<24);if(renderer.draw(arrow)==-2){error=renderer.error;return false;}}s.appearance=1;}
  else{if(renderer.draw(*slot.body)==-2){error=renderer.error;return false;}s.appearance=0;}
 }return true;
}
}
