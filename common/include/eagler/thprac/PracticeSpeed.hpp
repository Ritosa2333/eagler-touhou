#pragma once
#include <algorithm>
namespace eagler::thprac {
// Purple logical simulation period; not presentation refresh.
struct PracticeSpeed {
 int fps_status=1,fps=60,fps_replay_slow=15,fps_replay_fast=60,fps_debug_acc=0;
 double interval(bool replay,bool fast,bool slow,bool debug)const noexcept{
  const int rate=fps_debug_acc&&debug?(replay?9999:fps_replay_fast>1200?9999:fps_replay_fast):replay?(fast?(fps_replay_fast>1200?9999:fps_replay_fast):slow?fps_replay_slow:fps):fps;
  return 1./std::max(1,rate);
 }
};
}
