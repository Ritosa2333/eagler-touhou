#include "PlayerLife.hpp"
namespace th15 {
void PlayerLife::invalidate_spell()noexcept{if(!(spell.flags&1))return;if(spell.frame<60){if(session.bomb_state==1)spell.flags|=32;}else{spell.bonus=0;spell.flags&=~34u;}}
bool PlayerLife::hit(){
    if(cheat(PracticeInvincible))return true;
    if(!(motion.behavior_flags&8)){if(!sound||!sound(2)){error="Player hit sound unavailable";return false;}}
    if(!effects.create(29,motion.position)){error=effects.error;return false;}invalidate_spell();age.set(0);state=4;invulnerability.set(6);
    if(!visuals.pose(0)){error="Player hit animation failed";return false;}return true;
}
bool PlayerLife::commit_death(){
    if(practice&&practice->enabled)practice->input.begin_retry(practice->active?practice->run.mode:0);
    effects.tracked(28,motion.position);if(!effects.error.empty()){error=effects.error;return false;}if(!practice||!practice->preserve_life(session.extra_lives))session.extra_lives=wrapping_add(session.extra_lives,-1);
    if(practice&&practice->enabled)++practice->misses;
    if(session.extra_lives>=0&&(!life_hud||!life_hud(session.extra_lives,session.life_pieces))){error="Player life display unavailable";return false;}
    state=2;age.set(0);invulnerability.set(180);if(!visuals.pose(0)){error="Player death animation failed";return false;}
    for(u32 i=0;i<8;i++){motion.options[i].active=0;if(!visuals.option_remove(i)){error="Player death option animation failed";return false;}}motion.option_count=0;invalidate_spell();
    session.enemy_deaths=wrapping_add(session.enemy_deaths,1);session.enemy_chain=0;if(!(session.mode_flags&0x300)&&session.deaths<999999)session.deaths=wrapping_add(session.deaths,1);return true;
}
}
