#include "Player.hpp"
namespace th15 {
bool Player::draw(AnmRenderer& renderer){
 if(life.state==2)return true;
 auto& root=visuals.root;root.presentation_motion=true;root.visual.translation=motion.position;
 root.visual.render_flags=(root.visual.render_flags&~0x80000u)|0x40000u;
 if(renderer.draw(root)==-2){error=renderer.error;return false;}return true;
}
}
