#pragma once
#include <cmath>
namespace th5w { namespace practice {
// Read-only projection: native cycle counts and 56-fps simulation seconds.
struct PhaseTimeout {
 enum Unit { None, Seconds, Turns };
 Unit unit; int value; bool estimated; int cycles=-1;
 PhaseTimeout():unit(None),value(0),estimated(false){}
 PhaseTimeout(Unit u,int n,bool estimate=false):unit(u),value(n>0?n:0),estimated(estimate){}
 static PhaseTimeout WithCycles(PhaseTimeout timer,int remaining){timer.cycles=remaining>0?remaining:0;return timer;}
 PhaseTimeout ForDisplay(bool asTime) const {return !asTime&&cycles>=0?PhaseTimeout(Turns,cycles):*this;}
 static PhaseTimeout Frames(int deadline,int elapsed,bool estimate=false){int n=deadline-elapsed;return PhaseTimeout(Seconds,n>0?(n+55)/56:0,estimate);}
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
 static int MoveAxisFrames(float position,float target){
  int n=0;
  while(position!=target&&n<256){float delta=target-position;float distance=std::fabs(delta);position+=distance>=16?delta/16:distance>=4?delta/4:delta;++n;}
  return n+1; // MoveTo reports completion on the following call.
 }
};
} }
