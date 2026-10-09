#pragma once
#include <algorithm>
#include <cmath>
namespace th15 {
// Portable period owner corresponding to purple's native FPS pointer and
// replay-speed callbacks. This is simulation speed, not presentation refresh.
struct PracticeSpeed {
 int fps_status=1,fps=60,fps_replay_slow=15,fps_replay_fast=60,fps_debug_acc=0;
 double interval(bool replay,bool fast,bool slow,bool debug)const noexcept{
  const int rate=fps_debug_acc&&debug?(replay?9999:fps_replay_fast>1200?9999:fps_replay_fast):replay?(fast?(fps_replay_fast>1200?9999:fps_replay_fast):slow?fps_replay_slow:fps):fps;
  return 1./std::max(1,rate);
 }
};
struct PracticeCadence {
 double debt=0,period=1./60.;
 void reset(){debt=0;}
 unsigned advance(double seconds){debt=std::min(.1,debt+std::clamp(seconds,0.,.1));const auto ticks=std::min(std::abs(period-1./60.)<1e-12?4u:1024u,unsigned(std::floor((debt+1e-9)/period)));debt=std::max(0.,debt-ticks*period);return ticks;}
};
}
