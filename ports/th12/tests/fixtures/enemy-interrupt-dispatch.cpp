#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C"{
void interrupt_probe_prepare(int damage,int pending){
 startGame(2,1,3,1234);spellState.flags=pending?35:0;spellState.age=100;spellState.ageTime=100;
 playerStateTicks=0;playerPreviousStateTicks=-1;
 if(damage){auto* source=playerDamageSources.circle(0,128,500,0,500,damage);source->previousTicks=source->ticks;}
}
void interrupt_probe_tick(){tickEnemies();}
const char* interrupt_probe_snapshot(){return captureState().c_str();}
}
