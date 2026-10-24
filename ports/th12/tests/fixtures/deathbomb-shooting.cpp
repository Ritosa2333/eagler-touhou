#include "../../src/game/GameSession.hpp"
#include "../../src/game/GameState.hpp"
using namespace th12;
extern "C"{
void deathbomb_shooting_prepare(int loadout,int age){
 startGame(loadout/2,loadout%2,3,4796);
 setInitial(111,38400,400,2,2,0);frame=120;
 invuln=0;playerMissCollision();playerStateTicks=2;playerStateTime=2;
 shotSchedule.frame=age;shotSchedule.time=float(age);shotSchedule.previous=age-1;
}
void deathbomb_shooting_tick(int held){tickPlayer(held,0);++frame;}
void deathbomb_shooting_options(const int* input){
 for(int index=0;index<4;++index){
  playerMotion.options[index].x=input[index*3];playerMotion.options[index].y=input[index*3+1];
  playerMotion.resetPending[index]=input[index*3+2]!=0;
 }
}
const char* deathbomb_shooting_snapshot(){return captureState().c_str();}
}
