#include "BombController.hpp"
namespace th15 {
bool BombController::allowed()const noexcept{return context.session.bomb_state!=1&&context.hud_available&&!context.hud_collect&&context.enemies.available;}
void BombController::invalidate_spell()noexcept{auto& s=context.spell;if(!(s.flags&1))return;if(s.frame>=60){s.bonus=0;s.flags&=~34u;}else if(context.session.bomb_state==1)s.flags|=32;}
bool BombController::begin(){
    if(context.session.bomb_state!=0)return false;context.session.bomb_state=1;age.set(0);auto& s=context.session;if(!context.life.cheat(PracticeBombs))s.bombs=wrapping_sub(s.bombs,1);if(s.bombs<0)s.bombs=0;else if(s.bombs>8)s.bombs=8;
    if(context.life.practice&&context.life.practice->enabled)++context.life.practice->bombs;
    if(context.hud_available&&!context.world.bomb_hud(s.bombs,s.bomb_pieces)){error="Bomb HUD service failed";return false;}
    effective_against_spell=(context.spell.flags&1)&&context.spell.frame>=60;
    if(!context.world.sound(44,false)){error="Bomb audio service failed";return false;}context.enemies.chain=0;return start();
}
bool BombController::update(float rate){if(context.session.bomb_state==0)return true;frame_rate=rate;context.animations.rate=rate;bool finished=false;if(!frame(finished))return false;if(finished){context.session.bomb_state=0;return true;}age.tick(&rate);return true;}
}
