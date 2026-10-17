#include "../../th08_web/cpp/multiplayer/NetplayRuntime.hpp"
#include <array>
#include <cassert>
#include <cstdio>
using th08::multiplayer::NetplayRuntime;
using th08::multiplayer::SessionSetup;
int main(){
 for(unsigned count:{2u,3u}){
  std::array<NetplayRuntime,3> players;
  SessionSetup setup;setup.configured=true;setup.version=7;setup.session_id=1234;setup.seed=417;
  setup.player_count=count;setup.build[0]=1;setup.characters[1]=1;if(count==3)setup.characters[2]=2;
  for(unsigned seat=0;seat<count;++seat){setup.local_player=seat;assert(players[seat].Reset(setup));}
  for(unsigned to=0;to<count;++to)for(unsigned from=0;from<count;++from)if(to!=from)
   assert(players[to].ApplySession(players[from].SessionPacket(Netplay::SessionPhase::Hello))==Netplay::SessionPacketResult::Accepted);
  for(unsigned seat=0;seat<count;++seat)assert(players[seat].MarkReady());
  for(unsigned to=0;to<count;++to)for(unsigned from=0;from<count;++from)if(to!=from)
   assert(players[to].ApplySession(players[from].SessionPacket(Netplay::SessionPhase::Ready))==Netplay::SessionPacketResult::Accepted);
  auto& r=players[0];assert(r.CanStart());
  for(unsigned frame=0;frame<Netplay::ConfirmedInputClock::MaxLead;++frame){
   r.SetCapturePhase(true,100'000+frame*Netplay::ConfirmedInputClock::IntervalUs);
   assert(r.CanCapture()&&r.NextCaptureFrame()==frame);
   Netplay::FrameInput input;input.buttons=frame<3?1:0;assert(r.CaptureLocal(frame,input));
   assert(!r.CanCapture()&&r.NextFrame()==0&&!r.Prepare(0).canAdvance);
  }
  r.SetCapturePhase(true,1'000'000);assert(!r.CanCapture());
  r.SetCapturePhase(false,1'000'000);
  for(unsigned frame=0;frame<Netplay::ConfirmedInputClock::MaxLead;++frame){
   assert(!r.CanCapture());
   for(unsigned seat=1;seat<count;++seat)assert(r.SubmitRemote(seat,frame,{})==Netplay::RemoteInputResult::Accepted);
   const auto d=r.Prepare(frame);assert(d.canAdvance&&!d.predictedMask);
   assert(d.inputs[0].buttons==(frame<3?1:0));assert(r.MarkSimulated(frame,d));
  }
  assert(r.CanCapture()&&r.NextCaptureFrame()==r.NextFrame());
  assert(r.CanRetire()&&r.Retire());SessionSetup next;assert(r.BeginNextRun(next,419));
  assert(r.NextCaptureFrame()==0&&r.NextFrame()==0);
 }
 std::puts("PASS TH08 2P/3P confirmed UI pipeline, immutable release, bounded blackout, combat drain and generation reset");
}
