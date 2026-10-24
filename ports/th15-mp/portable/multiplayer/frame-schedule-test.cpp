// Exercise the production clock and real NetplayRuntime capture/correction
// frontiers. Full game-memory, drawing and rAF integration run in the browser.
#include "../../web/cpp/multiplayer/FrameSchedule.hpp"
#include "../../web/cpp/multiplayer/NetplayRuntime.hpp"
#include "../../web/cpp/multiplayer/InputLanes.hpp"
#include "../../web/cpp/multiplayer/PendingSave.hpp"
#include "../../web/cpp/game/PracticeCadence.hpp"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <limits>

namespace {
using th15::multiplayer::FrameSchedule;
using th15::multiplayer::FrameWait;
using th15::multiplayer::NetplayRuntime;
using th15::multiplayer::SessionSetup;
constexpr double step=FrameSchedule::interval;
unsigned checks=0,groups=0;
void check(bool value,const char* why){++checks;if(!value){std::fprintf(stderr,"FAIL schedule group %u check %u: %s\n",groups,checks,why);std::exit(1);}}
void near(double actual,double expected,const char* why){check(std::abs(actual-expected)<1.e-8,why);}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
void consume(FrameSchedule& clock,unsigned count){for(unsigned n=0;n<count;++n)check(clock.complete(),"one new forward owns one offered slot");check(clock.owed()==0,"offered work consumed exactly");}
Netplay::FrameInput local_input(unsigned frame){
 Netplay::FrameInput result;
 check(th15::multiplayer::InputLanes::Capture(1u|((frame&1)?0x40u:0x80u),false,1,float(int(frame%100)-50),320,result),"valid native physical input");
 return result;
}
struct Peers {
 std::array<NetplayRuntime,3> peers;unsigned count,delay;
 Peers(unsigned players,unsigned mode):count(players),delay(mode==2?2u:0u){
  for(unsigned seat=0;seat<count;++seat){SessionSetup setup;setup.version=SessionSetup::Version;
   setup.session_id=0x12345678;setup.seed=417;setup.difficulty=1;setup.player_count=count;setup.local_player=seat;
   setup.mode=mode;setup.automatic=false;setup.input_delay=setup.requested_delay=delay;
   setup.selections={0,1,count==3?3u:0u};setup.build={1,2,3,4};check(peers[seat].Reset(setup),"real runtime configuration");
  }
  for(unsigned to=0;to<count;++to)for(unsigned from=0;from<count;++from)if(to!=from)
   check(peers[to].ApplySession(peers[from].Hello())==Netplay::SessionPacketResult::Accepted,"real Hello gate");
  for(unsigned seat=0;seat<count;++seat){check(peers[seat].CanSendReady(),"ready admission");peers[seat].MarkLocalReady();}
  for(unsigned to=0;to<count;++to)for(unsigned from=0;from<count;++from)if(to!=from)
   check(peers[to].ApplySession(peers[from].Ready())==Netplay::SessionPacketResult::Accepted,"real Ready gate");
  check(runtime().CanStart(),"real runtime started");
 }
 NetplayRuntime& runtime(){return peers[0];}
 void remote(unsigned frame,Netplay::FrameInput input){
  for(unsigned seat=1;seat<count;++seat){const auto result=runtime().SubmitRemote(seat,frame,input);
   check(result==Netplay::RemoteInputResult::Accepted||result==Netplay::RemoteInputResult::PredictionCorrect||result==Netplay::RemoteInputResult::RollbackRequired,"real remote input accepted");
  }
 }
 void capture(){auto& r=runtime();check(r.NeedsCapture(),"new frontier permits capture");check(r.Capture(r.CaptureFrame(),local_input(r.CaptureFrame())),"capture immutable local slot");}
 void simulate(){auto& r=runtime();const auto frame=r.NextFrame();const auto decision=r.Prepare(frame);
  check(decision.canAdvance,"actual core admits simulation");
  check(decision.inputs[0]==(frame<delay?Netplay::FrameInput{}:local_input(frame-delay)),"delayed local lane keeps its original captured bytes");
  check(r.MarkSimulated(frame,decision),"real simulation cursor commits");
 }
};
void ordinary_clock(){
 group("TH15 ordinary 60 Hz cadence equivalence and bounded offers");
 for(unsigned hz:{60u,90u,120u,144u,165u,240u}){
  FrameSchedule fixed;th15::PracticeCadence original;unsigned frames=0;
  for(unsigned callback=0;callback<5*hz;++callback){const auto due=fixed.advance(1./hz);
   check(due==original.advance(1./hz),"completed rollback forwards retain real PracticeCadence timing");
   check(due<=4&&fixed.owed()==due,"four-call offer cap");near(fixed.phase(),original.debt,"fractional phase unchanged");consume(fixed,due);frames+=due;
  }
  check(frames==300,"five nominal seconds own 300 new forwards");
 }
}
void cpu_accounting(){
 group("history, CPU splits and completion handoff retain forward obligations");
 for(unsigned offered=1;offered<=4;++offered)for(unsigned completed=0;completed<=offered;++completed){
  FrameSchedule clock;check(clock.advance((offered+.25)*step)==offered,"known forward offer");
  for(unsigned n=0;n<completed;++n)check(clock.complete(),"actual new forward completes");
  const auto remaining=offered-completed;
  check(clock.owed()==remaining,"history calls leave the new-forward ledger intact");
  clock.wait(FrameWait::Cpu);near(clock.phase(),(remaining+.25)*step,"all unfinished CPU offers returned");
  clock.cpu_yield();near(clock.phase(),(remaining+.25)*step,"tail settlement cannot return twice");
  check(clock.advance(0)==remaining,"no new wall time needed to retry owed forward");consume(clock,remaining);
  near(clock.phase(),.25*step,"CPU retry preserves fraction");check(!clock.complete(),"no phantom completion");
 }
 FrameSchedule clock;th15::PracticeCadence original;unsigned fixed=0,old=0;
 for(unsigned cycle=0;cycle<100;++cycle){check(clock.advance(3*step)==3&&original.advance(3*step)==3,"same delayed offer");
  check(clock.complete(),"one forward after two historical calls");++fixed;++old;clock.cpu_yield();
  const auto retained=clock.advance(0);consume(clock,retained);fixed+=retained;old+=original.advance(0);
 }
 check(fixed==300&&old==100,"history completion does not silently discard 200 due forwards");
}
void actual_correction(unsigned count,unsigned mode){
 group("real NetplayRuntime split/restarted correction preserves capture identity");
 Peers fixture(count,mode);auto& r=fixture.runtime();FrameSchedule clock;
 for(unsigned frame=0;frame<8;++frame){check(clock.advance(step)==1,"forward due before correction");fixture.capture();
  if(frame<3)fixture.remote(frame,Netplay::FrameInput(1));fixture.simulate();consume(clock,1);
  if(frame==0)check(r.SetWorldReady(true),"stable rollback world");
  if(frame<3)check(r.SetCommitLimit(frame),"exact prefix committed");
 }
 check(r.NextFrame()==8&&r.CaptureFrame()==8,"original forward/capture frontier");
 check(clock.advance(1.5*step)==1,"one due forward before historical work");
 fixture.remote(3,Netplay::FrameInput(0x80));check(r.RollbackFrame()==3&&r.BeginCorrection(3),"actual late input begins correction");
 fixture.simulate();fixture.simulate();
 check(r.Correcting()&&r.CaptureFrame()==8&&!r.NeedsCapture(),"history cannot sample physical input");
 check(clock.owed()==1,"two historical frames did not complete forward");clock.wait(FrameWait::Cpu);
 check(clock.advance(.5*step)==2,"remaining CPU time plus newly elapsed half frame");
 fixture.remote(4,Netplay::FrameInput(0x40));check(r.RestartCorrection(4),"second actual mismatch restarts history");
 for(unsigned frame=5;frame<8;++frame)fixture.remote(frame,Netplay::FrameInput(0x40));
 while(r.NextFrame()<r.CorrectionEnd()){check(!r.NeedsCapture()&&r.CaptureFrame()==8,"restarted history retains every physical sample");fixture.simulate();}
 check(r.EndCorrection()&&!r.Correcting(),"real correction completes");
 check(clock.owed()==2&&r.NextFrame()==8&&r.CaptureFrame()==8,"EndCorrection owns no new-forward completion");
 clock.wait(FrameWait::Cpu);check(!clock.retry_pending(),"completion handoff is CPU work, not missing information");
 check(clock.advance(0)==2,"completion handoff returns both due forward slots");
 for(unsigned frame=8;frame<10;++frame){fixture.capture();fixture.remote(frame,Netplay::FrameInput(0x40));fixture.simulate();check(clock.complete(),"only newly simulated forward consumes credit");}
 check(clock.owed()==0&&r.NextFrame()==10&&r.CaptureFrame()==10,"exactly two new captures after recovery");
 check(clock.advance(0)==0,"no additional forward invented after refund");
}
void actual_network_wait(unsigned count,unsigned mode){
 group("actual prediction cap retries one captured frontier without waiting-time burst");
 Peers fixture(count,mode);auto& r=fixture.runtime();FrameSchedule clock;unsigned blocked=0;
 for(;;){const auto frame=r.NextFrame();fixture.capture();
  if(frame==0)fixture.remote(0,Netplay::FrameInput(1));
  if(!r.Prepare(frame).canAdvance){blocked=frame;break;}
  fixture.simulate();if(frame==0){check(r.SetCommitLimit(0),"initial commit");check(r.SetWorldReady(true),"stable world");}
  check(frame<=SessionSetup::MaxRollbackFrames,"prediction remains bounded");
 }
 check(blocked==SessionSetup::MaxRollbackFrames+1&&r.CaptureFrame()==blocked+1,"real twelve-frame prediction cap");
 check(clock.advance(3.75*step)==3,"three due offers before genuine information wait");clock.wait(FrameWait::Network);
 for(double elapsed:{0.,.001,.5,10.,100.}){
  check(clock.advance(elapsed)==1,"information wait retains one retry");
  check(!r.NeedsCapture()&&r.NextFrame()==blocked&&r.CaptureFrame()==blocked+1,"waiting never recaptures or grows combat lead");
  check(!r.Prepare(blocked).canAdvance,"actual inputs still absent");clock.wait(FrameWait::Network);clock.cpu_yield();near(clock.phase(),.75*step,"no waiting time converted to CPU debt");
 }
 for(unsigned frame=1;frame<=blocked;++frame)fixture.remote(frame,Netplay::FrameInput(1));
 check(r.RollbackFrame()==Netplay::INVALID_FRAME,"matching arrivals need no restoration");
 check(clock.advance(100)==1,"arrival does not backfill lost network time");fixture.simulate();consume(clock,1);
 check(r.CaptureFrame()==blocked+1&&r.NeedsCapture(),"the previously captured physical slot was reused exactly once");
 check(clock.advance(0)==0,"wait completion has no burst of synthesized frames");
 check(clock.advance(.25*step)==1,"normal 60 Hz fraction resumes");fixture.capture();fixture.remote(blocked+1,Netplay::FrameInput(1));fixture.simulate();consume(clock,1);
}
void menu_capture(){
 group("real confirmed-menu capture clock remains paced during a pending retry");
 Peers fixture(3,2);auto& r=fixture.runtime();FrameSchedule clock;
 constexpr auto interval=Netplay::ConfirmedInputClock::IntervalUs;
 r.SetCapturePhase(true,100000);fixture.capture();check(clock.advance(step)==1,"first menu offer");clock.network_wait();
 for(unsigned n=1;n<16;++n){const auto now=100000+(n*interval)/2;r.SetCapturePhase(true,now);check(clock.advance(.5*step)==1,"pending menu retry at 120 Hz");
  const bool expected=n%2==0&&r.CaptureFrame()<Netplay::ConfirmedInputClock::MaxLead;
  check(r.NeedsCapture()==expected,"capture permission still follows actual 60 Hz menu clock");if(expected)fixture.capture();clock.network_wait();
 }
 check(r.NextFrame()==0&&r.CaptureFrame()==Netplay::ConfirmedInputClock::MaxLead,"menu lead remains capped at eight real samples");
 r.SetCapturePhase(false,1000000);
 for(unsigned frame=0;frame<Netplay::ConfirmedInputClock::MaxLead;++frame){check(clock.advance(frame?step:100)==1,"one due frame drains existing menu tail");
  check(!r.NeedsCapture(),"combat does not add samples ahead of the queued menu tail");fixture.remote(frame,Netplay::FrameInput(1));fixture.simulate();consume(clock,1);
 }
 check(r.CaptureFrame()==r.NextFrame()&&r.NeedsCapture(),"menu resume has no permanent second input delay");
}
void save_handoff(){
 group("actual PendingSave completion returns one forward obligation");
 th15::multiplayer::PendingSave save;FrameSchedule clock;const auto token=save.Request();check(token!=0,"real save requested");
 check(clock.advance(2.5*step)==2,"due offers before save confirmation fence");clock.wait(FrameWait::Network);
 unsigned writes=0;
 check(save.Complete(true,false,false,1,1,[&]{++writes;return true;})==th15::multiplayer::PendingSave::Pending,"unconfirmed save remains pending");
 check(clock.advance(30)==1,"save fence discards waiting wall time");
 check(save.Complete(true,false,true,0,0,[&]{++writes;return true;})==th15::multiplayer::PendingSave::Completed,"actual save request completes");
 check(!save.Waiting()&&writes==1,"native completion releases save fence");clock.wait(FrameWait::Cpu);
 check(clock.advance(0)==1,"save completion itself consumes no forward slot");consume(clock,1);near(clock.phase(),.5*step,"save preserves original fractional phase");
}
void boundaries(){
 group("CPU debt, invalid time and generation resets stay bounded");
 FrameSchedule clock;
 for(unsigned n=0;n<50;++n){check(clock.advance(100)==4,"large CPU stalls retain four-call cap");check(clock.complete(),"one forward progresses");clock.cpu_yield();check(clock.phase()<=.1+1.e-9,"CPU debt never exceeds100 ms");}
 clock.reset();check(clock.advance(100)==4,"six retained frames offer four");clock.cpu_yield();near(clock.phase(),.1,"bounded retained debt");
 check(clock.advance(0)==4,"first bounded drain");consume(clock,4);check(clock.advance(0)==2,"remaining bounded drain");consume(clock,2);check(clock.advance(0)==0,"drain cannot invent more time");
 for(double bad:{-1.,std::numeric_limits<double>::infinity(),-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN()}){check(clock.advance(bad)==0,"invalid elapsed has no credit");near(clock.phase(),0,"invalid time cannot poison clock");}
 check(clock.advance(2.5*step)==2,"pending generation offer");clock.reset();clock.cpu_yield();check(clock.advance(0)==0&&!clock.retry_pending(),"reset clears CPU debt");
 check(clock.advance(1.5*step)==1,"pending generation network wait");clock.network_wait();clock.reset();check(clock.advance(0)==0&&!clock.retry_pending(),"reset clears old network retry");
 check(clock.advance(2.5*step)==2&&clock.complete(),"unfinished offer setup");check(clock.advance(.5*step)==2,"forgotten CPU offer is defensively returned once");consume(clock,2);
}
}
int main(){
 ordinary_clock();cpu_accounting();
 for(unsigned count:{2u,3u})for(unsigned mode:{0u,2u}){actual_correction(count,mode);actual_network_wait(count,mode);}
 menu_capture();save_handoff();boundaries();
 std::printf("PASS TH15 frame schedule: %u groups, %u checks\n",groups,checks);
}
