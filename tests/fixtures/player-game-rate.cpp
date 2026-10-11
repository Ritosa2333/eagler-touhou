#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C"{
void player_rate_prepare(int loadout,float rate){
 startGame(loadout/2,loadout%2,3,1234);
 frame=120;gameSpeed=animationScene.runtime.delta=rate;px=111;py=38400;power=400;
 playerState=1;playerStateTicks=120;playerStateTime=playerTotalTime=120;
 invuln=120;playerInvulnerabilityTime=120;playerFocused=false;
 playerMotion.updateOptions(loadouts[loadout],power,px,py,false,true);
}
void player_rate_tick(int held){tickPlayer(held,0);++frame;}
const int* player_rate_snapshot(){static int r[6];r[0]=px;r[1]=py;r[2]=playerStateTicks;r[3]=int(playerTotalTime);r[4]=invuln;r[5]=playerFocused;return r;}
}
