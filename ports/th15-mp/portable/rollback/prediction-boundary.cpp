#include "../../web/cpp/multiplayer/PredictionBoundary.hpp"
#include <eagler/netplay/ConfirmedInputClock.hpp>
#include <cstdio>
#include <cstdlib>
static void check(bool value,const char* reason){if(!value){std::fprintf(stderr,"%s\n",reason);std::exit(1);}}
int main(){
 using th15::multiplayer::PredictionBoundary;using Netplay::ConfirmedInputClock;
 {
  PredictionBoundary boundary;ConfirmedInputClock clock;
  unsigned simulation=100,capture=simulation;std::uint64_t now=0;
  boundary.defer(simulation);clock.BeginPhase(now);
  // A dialogue/factory fence is waiting on exact remote input. Physical input
  // can queue at most eight slots while the game waits at the same frame.
  for(unsigned tick=0;tick<20;++tick){
   now+=ConfirmedInputClock::IntervalUs;
   if(clock.CanCapture(capture,simulation,boundary.waiting(),now)){++capture;clock.Captured(now);}
  }
  check(capture-simulation==ConfirmedInputClock::MaxLead,"boundary capture exceeded its bounded UI queue");
  boundary.completed(simulation-1);check(boundary.blocks(simulation),"a different frame released the boundary");
  boundary.completed(simulation++);check(!boundary.waiting(),"confirmed boundary kept combat in the UI capture phase");
  const auto tail=capture;
  while(simulation<tail){now+=ConfirmedInputClock::IntervalUs;check(!clock.CanCapture(capture,simulation,boundary.waiting(),now),"combat continued sampling ahead of its immutable queued tail");++simulation;}
  check(clock.CanCapture(capture,simulation,false,now),"combat failed to resume fresh physical input");
  boundary.defer(simulation);boundary.clear();check(!boundary.waiting(),"correction/reset retained a stale boundary");
 }
 std::puts("PASS confirmed boundary release, bounded UI tail and fresh combat capture");
}
