#include "../../src/game/EnemyLife.hpp"
int32_t life=0,raw=0,threshold=0;uint32_t flags=0;
extern "C" {
void reset_life(int l,int t,int f){life=l;raw=th12::enemy_life::expanded(l);threshold=t;flags=f;}
void begin_spell(){flags|=1u;raw=th12::enemy_life::expanded(life);}
void end_spell(){flags&=~1u;}
void hurt_life(int damage){th12::enemy_life::hurt(life,raw,threshold,flags,damage);}
int life_value(){return life;}
int raw_value(){return raw;}
int life_flags(){return flags;}
}
