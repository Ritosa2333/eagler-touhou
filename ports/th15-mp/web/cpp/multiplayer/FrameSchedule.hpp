#pragma once
#include "../../../portable/sdl/FrameCadence.hpp"
#include <eagler/netplay/PendingFrameSchedule.hpp>
#include <algorithm>
#include <cmath>

namespace th15::multiplayer {
enum class FrameWait { Network, Cpu };

// Rollback-only forward obligations. Ordinary, mode-1 and replay clocks keep
// PracticeCadence. Pace newly elapsed time once before advance(); historical
// simulation and an unfinished CPU batch never consume a new forward slot.
class FrameSchedule final {
 using Cadence=touhou::sdl::FrameCadence;
 Netplay::PendingFrameSchedule<Cadence> clock_;
 unsigned offered_=0;
public:
 static constexpr double interval=Cadence::interval,max_debt=.1;
 static constexpr unsigned max_ticks=4;
 void reset(){clock_.reset();offered_=0;}
 unsigned owed()const{return offered_;}
 bool retry_pending()const{return clock_.retry_pending();}
 double phase()const{return clock_.debt;}
 unsigned advance(double normalized_seconds){
  if(offered_){if(clock_.retry_pending())network_wait();else cpu_yield();}
  const double elapsed=std::isfinite(normalized_seconds)?std::max(0.,normalized_seconds):0.;
  offered_=clock_.advance(elapsed,true);return offered_;
 }
 bool complete(){
  if(!offered_)return false;
  --offered_;clock_.complete();return true;
 }
 void network_wait(){
  if(!offered_)return;
  // Keep one already-due retry and fractional phase, without accumulating
  // missing-input/save-confirmation wall time into a later capture burst.
  clock_.blocked(true);offered_=0;
 }
 void cpu_yield(){
  if(!offered_)return;
  clock_.complete();clock_.debt=std::min(max_debt,clock_.debt+offered_*interval);offered_=0;
 }
 void wait(FrameWait reason){if(reason==FrameWait::Cpu)cpu_yield();else network_wait();}
};
}
