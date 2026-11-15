#pragma once
#include <cmath>
namespace th5w { namespace practice {
// Read-only projection: native cycle counts and 56-fps simulation seconds.
struct PhaseTimeout {
 enum Unit { None, Seconds, Turns, Ticks };
 Unit unit; int value; bool estimated; int cycles=-1, frames=-1;
 PhaseTimeout():unit(None),value(0),estimated(false){}
 PhaseTimeout(Unit u,int n,bool estimate=false):unit(u),value(n>0?n:0),estimated(estimate){}
 static PhaseTimeout WithCycles(PhaseTimeout timer,int remaining){timer.cycles=remaining>0?remaining:0;return timer;}
 PhaseTimeout ForDisplay(bool asTime,bool asFrames=false) const {
  if(asFrames&&frames>=0)return PhaseTimeout(Ticks,frames,estimated);
  return !asTime&&cycles>=0?PhaseTimeout(Turns,cycles):*this;
 }
 static PhaseTimeout Frames(int deadline,int elapsed,bool estimate=false){
  const int remaining=deadline>elapsed?deadline-elapsed:0;
  PhaseTimeout timer(Seconds,(remaining+55)/56,estimate);
  timer.frames=remaining;
  return timer;
 }
 // A loop counter advances after movement. Attack and movement reset their
 // local frame separately, so sum their native durations before rounding.
 template<class Attack,class Move>
 static PhaseTimeout Timeline(int limit,int completed,int mode,int elapsed,Attack attack,Move move,bool estimate=false){
  if(completed>=limit)return WithCycles(Frames(0,0,estimate),0);
  int n=(mode?attack(completed):0)+move(completed)-elapsed;
  if(n<0)n=0;
  for(int cycle=completed+1;cycle<limit;cycle++)n+=attack(cycle)+move(cycle);
  return WithCycles(Frames(n,0,estimate),limit-completed);
 }
 // Same convergence as CBoss::MoveTo, without changing the real position.
 static int MoveAxisFrames(float position,float target,int nearDivisor=4){
  int remaining=int(target-position);
  int frames=1;
  int step=remaining/16;
  if(!step)step=remaining/nearDivisor;
  while(step){
   remaining-=step;
   ++frames;
   step=remaining/16;
   if(!step)step=remaining/nearDivisor;
  }
  return frames;
 }
};
} }
