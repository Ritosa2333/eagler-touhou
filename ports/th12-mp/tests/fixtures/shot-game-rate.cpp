#include "../../src/player.hpp"
th12::ShotSchedule schedule;int emitted=-1;
extern "C"{
void shot_rate_begin(){schedule={};}
void shot_rate_tick(int held,float rate){emitted=schedule.begin(held,true);if(!schedule.emitThisTick)emitted=-1;schedule.finish(held,rate);}
int shot_rate_clock(){return schedule.frame;}
int shot_rate_emitted(){return emitted;}
}
