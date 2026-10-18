#include "../../th11_web/cpp/multiplayer/NetplayRuntime.hpp"
#include "../../th11_web/cpp/multiplayer/InputLanes.hpp"
#include "../../th11_web/cpp/multiplayer/ReplayArchive.hpp"
#include "../../th11_web/cpp/game/GameInput.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace th11::multiplayer;
namespace {
unsigned checks=0,groups=0;
#define VERIFY(x) do{++checks;if(!(x)){std::fprintf(stderr,"rollback network group %u check %u line %d: %s\n",groups,checks,__LINE__,#x);std::exit(1);}}while(false)
using Result=Netplay::RemoteInputResult;
SessionSetup config(unsigned count,unsigned seat,unsigned mode=0,unsigned delay=0){
 SessionSetup s;s.version=SessionSetup::Version;s.mode=mode;s.automatic=false;
 s.session_id=0x1122334455667788ull;s.player_count=count;s.local_player=seat;s.seed=417;
 s.requested_delay=s.input_delay=delay;s.selections={0,4,count==3?5u:0u};
 s.build={0x12345678,0x90abcdef,0x87654321,0xfedcba09};return s;
}
Netplay::FrameInput value(unsigned held=9,int motion=0,float x=0,float y=0,bool pause=false){
 Netplay::FrameInput input;VERIFY(InputLanes::Capture(held,pause,motion,x,y,input));return input;
}
bool delivered(Result r){return r==Result::Accepted||r==Result::Duplicate||r==Result::PredictionCorrect||r==Result::RollbackRequired;}
void put(std::vector<unsigned char>& bytes,unsigned offset,unsigned word){for(unsigned i=0;i<4;++i)bytes[offset+i]=static_cast<unsigned char>(word>>(i*8));}
void checksum(std::vector<unsigned char>& bytes){unsigned h=2166136261u;for(unsigned i=0;i<bytes.size();++i)if(i<36||i>=40){h^=bytes[i];h*=16777619u;}put(bytes,36,h);}
struct Fixture {
 std::array<NetplayRuntime,3> peers;unsigned count;
 explicit Fixture(unsigned n=2,unsigned mode=0,unsigned delay=0):count(n){
  for(unsigned s=0;s<count;++s)VERIFY(peers[s].Reset(config(count,s,mode,delay)));
  for(unsigned i=0;i<count;++i)for(unsigned j=0;j<count;++j)if(i!=j)VERIFY(peers[i].ApplySession(peers[j].Hello())==Netplay::SessionPacketResult::Accepted);
  for(unsigned i=0;i<count;++i){VERIFY(peers[i].CanSendReady());peers[i].MarkLocalReady();}
  for(unsigned i=0;i<count;++i)for(unsigned j=0;j<count;++j)if(i!=j)VERIFY(peers[i].ApplySession(peers[j].Ready())==Netplay::SessionPacketResult::Accepted);
  for(unsigned i=0;i<count;++i)VERIFY(peers[i].CanStart());
 }
 NetplayRuntime& r(){return peers[0];}
 void capture(const Netplay::FrameInput& local={}){
  VERIFY(r().NeedsCapture());VERIFY(r().CaptureFrame()==r().NextFrame());
  VERIFY(r().Capture(r().CaptureFrame(),local));VERIFY(!r().NeedsCapture());
 }
 void actual(unsigned frame,const Netplay::FrameInput& input){
  for(unsigned s=1;s<count;++s)VERIFY(delivered(r().SubmitRemote(s,frame,input)));
 }
 Netplay::FrameDecision step(bool predicted){
  auto d=r().Prepare(r().NextFrame());VERIFY(d.canAdvance);VERIFY(bool(d.predictedMask)==predicted);
  VERIFY(r().MarkSimulated(r().NextFrame(),d));return d;
 }
 void prime(const Netplay::FrameInput& input=value()){
  capture();actual(0,input);step(false);VERIFY(r().SetCommitLimit(0));VERIFY(r().SetWorldReady(true));
 }
};
void admission(){
 ++groups;auto old=config(3,2,1,2);old.version=SessionSetup::LegacyVersion;
 auto current=old;current.version=SessionSetup::Version;VERIFY(old.valid()&&current.valid());
 VERIFY(GameplayAbi(old)==GameplayAbi(current));
 for(unsigned mode:{0u,1u,2u}){
  auto s=config(3,2,mode,mode?2:0);auto words=EncodeSessionSetup(s);SessionSetup read;
  VERIFY(words.size()==22&&words[0]==6&&words[14]==mode);
  VERIFY(DecodeSessionSetup(read,words.data(),words.size()));VERIFY(EncodeSessionSetup(read)==words);
  auto legacy=words;legacy[0]=5;VERIFY(DecodeSessionSetup(read,legacy.data(),legacy.size())==(mode==1));
  auto wrong=s;wrong.prediction_limit=11;VERIFY(!wrong.valid());wrong=s;wrong.measured_prediction=3;VERIFY(!wrong.valid());
 }
 auto direct=config(2,0),hybrid=config(2,0,2),delay=config(2,0,1);
 VERIFY(GameplayAbi(direct)!=GameplayAbi(hybrid)&&GameplayAbi(direct)!=GameplayAbi(delay));
 auto wrong=direct;wrong.automatic=true;VERIFY(!wrong.valid());wrong=direct;wrong.requested_delay=1;VERIFY(!wrong.valid());
 wrong=direct;wrong.input_delay=1;VERIFY(!wrong.valid());
 Fixture f;auto& r=f.r();VERIFY(r.AllowsRollback()&&r.PredictionLimit()==12&&!r.WorldReady());
 f.capture();VERIFY(!r.Prepare(0).canAdvance);VERIFY(!r.SetCommitLimit(0));f.actual(0,value());
 VERIFY(r.Confirmed()==0&&r.LastFrame()==Netplay::INVALID_FRAME&&!r.SetCommitLimit(0));
 f.step(false);std::array<Netplay::FrameInput,3> exact{};VERIFY(!r.ConfirmedInputs(0,exact)); // no output grant yet
 VERIFY(r.SetCommitLimit(0));VERIFY(!r.SetCommitLimit(1));VERIFY(r.SetWorldReady(true));
 VERIFY(r.SetWorldReady(false));f.capture();VERIFY(!r.Prepare(1).canAdvance);
 VERIFY(r.SetWorldReady(true));VERIFY(r.Prepare(1).canAdvance);
}
void held_and_depth(unsigned count,int motion){
 ++groups;Fixture f(count);auto& r=f.r();
 const auto source=value(0x2003ff,motion,motion==3?.5f:123.25f,motion==3?-.25f:234.5f,true);
 f.prime(source);th11::GameInput game;game.update(source.buttons);
 for(unsigned frame=1;frame<=12;++frame){
  f.capture();auto d=f.step(true);
  VERIFY(d.predictedMask==((1u<<count)-2));VERIFY(d.inputs[1].buttons==(frame<=3?0xf9:9));
  VERIFY(d.inputs[1].analogMode==source.analogMode&&d.inputs[1].x==source.x&&d.inputs[1].y==source.y);
  VERIFY(d.inputs[1].touchUsed==source.touchUsed&&d.inputs[1].unlimited==source.unlimited&&!d.inputs[1].touchBomb);
  game.update(d.inputs[1].buttons);VERIFY((game.pressed&9)==0&&((game.held&9)==9));
  VERIFY(r.PredictionFrontier()==frame+1);VERIFY(r.MissingRemoteInputs()==frame);
 }
 VERIFY(!r.SetWorldReady(false));f.capture();VERIFY(!r.Prepare(13).canAdvance);
 f.actual(13,value());VERIFY(!r.Prepare(13).canAdvance); // later exact slot cannot hide twelve missing worlds
 VERIFY(r.CaptureFrame()==14&&r.NextFrame()==13&&!r.NeedsCapture());
}
void stable_and_edges(){
 ++groups;Fixture f;auto& r=f.r();const auto held=value();f.prime(held);th11::GameInput game;game.update(9);
 for(unsigned frame=1;frame<=8;++frame){
  f.capture();auto d=f.step(true);VERIFY(d.inputs[1]==held);game.update(d.inputs[1].buttons);VERIFY((game.pressed&9)==0);
  VERIFY(r.SubmitRemote(1,frame,held)==Result::PredictionCorrect);VERIFY(r.RollbackFrame()==Netplay::INVALID_FRAME);
  VERIFY(r.SetCommitLimit(frame));
 }
 f.capture();f.step(true);VERIFY(r.SubmitRemote(1,9,value(0))==Result::RollbackRequired);VERIFY(r.RollbackFrame()==9);
 VERIFY(r.BeginCorrection(9));auto release=f.step(false);game.update(release.inputs[1].buttons);VERIFY((game.released&9)==9);
 VERIFY(r.EndCorrection());VERIFY(r.SetCommitLimit(9));
 f.capture();auto neutral=f.step(true);VERIFY(neutral.inputs[1].buttons==0);
 VERIFY(r.SubmitRemote(1,10,held)==Result::RollbackRequired);VERIFY(r.BeginCorrection(10));
 auto start=f.step(false);game.update(start.inputs[1].buttons);VERIFY((game.pressed&9)==9);
 VERIFY(r.EndCorrection());VERIFY(r.SetCommitLimit(10));
}
void correction_frontiers(unsigned count){
 ++groups;Fixture f(count);auto& r=f.r();f.prime();
 for(unsigned frame=1;frame<7;++frame){f.capture();f.step(true);}
 f.capture(value(0x80,2,150.5f,123.25f)); // capture/send the live frontier before restoring
 VERIFY(r.CaptureFrame()==8&&r.NextFrame()==7);
 VERIFY(r.SubmitRemote(1,3,value(1))==Result::RollbackRequired);
 std::array<Netplay::FrameInput,3> exact{};
 VERIFY(!r.ConfirmedInputs(0,exact));VERIFY(!r.SetCommitLimit(0));
 VERIFY(r.BeginCorrection(3));VERIFY(r.Correcting()&&r.NextFrame()==3&&r.LastFrame()==2);
 VERIFY(r.CorrectionEnd()==7&&r.PredictionFrontier()==7&&r.MissingRemoteInputs()==6);
 VERIFY(!r.NeedsCapture()&&!r.EndCorrection());f.step(count==3);
 VERIFY(r.SubmitRemote(1,1,value(8))==Result::RollbackRequired);VERIFY(r.RestartCorrection(1));
 VERIFY(r.NextFrame()==1&&r.CorrectionEnd()==7&&r.PredictionFrontier()==7);
 for(unsigned frame=1;frame<7;++frame)for(unsigned seat=1;seat<count;++seat){
  const auto actual=value(seat==1&&frame==1?8:seat==1&&frame==3?1:9);
  VERIFY(delivered(r.SubmitRemote(seat,frame,actual)));
 }
 VERIFY(r.Confirmed()==6&&!r.SetCommitLimit(0));
 for(unsigned frame=1;frame<7;++frame){
  VERIFY(!r.NeedsCapture());auto d=f.step(false);VERIFY(!r.ConfirmedInputs(frame,exact));
  VERIFY(r.Correcting());VERIFY(r.CaptureFrame()==8);
 }
 VERIFY(r.EndCorrection());VERIFY(!r.Correcting()&&r.NextFrame()==7&&!r.NeedsCapture());
 VERIFY(!r.ConfirmedInputs(6,exact));VERIFY(r.SetCommitLimit(6));VERIFY(r.CommittedThrough()==6);
 VERIFY(r.ConfirmedInputs(6,exact));VERIFY(!r.SetCommitLimit(5));VERIFY(!r.BeginCorrection(1));
 f.actual(7,value());auto d=f.step(false);VERIFY(d.inputs[0]==value(0x80,2,150.5f,123.25f));
 VERIFY(r.NeedsCapture()&&r.CaptureFrame()==8);VERIFY(r.SetCommitLimit(7));
 VERIFY(r.Finish());VERIFY(r.BeginNextRun(418));VERIFY(r.NextFrame()==0&&r.CaptureFrame()==0);
 VERIFY(r.CommittedThrough()==Netplay::INVALID_FRAME&&!r.WorldReady()&&r.Mode()==Netplay::AdonisMode::Rollback);
}
void confirmed_history_prefix(unsigned count){
 ++groups;Fixture f(count);auto& r=f.r();f.prime();
 for(unsigned frame=1;frame<=5;++frame){f.capture(value(frame&1));f.step(true);}
 f.capture(value(0x80,2,17.5f,250.25f)); // physical frame six stays captured exactly once
 for(unsigned frame=1;frame<=3;++frame)f.actual(frame,value(1,2,float(frame),300));
 f.actual(5,value(8)); // exact fifth slot is NOT a reconciled prefix while four is missing
 VERIFY(r.BeginCorrection(1));VERIFY(r.CanReplayConfirmed());
 VERIFY(!r.SetCommitLimit(0)); // legacy full-frontier grant stays strict
 VERIFY(r.CommitReconciledPrefix(0));
 std::array<Netplay::FrameInput,3> exact;
 for(unsigned frame=1;frame<=3;++frame){
  VERIFY(r.CanReplayConfirmed()&&!r.NeedsCapture());
  VERIFY(!r.CommitReconciledPrefix(frame)); // actual input is not a simulated/sealed frame
  const auto d=f.step(false);
  VERIFY(r.CommitReconciledPrefix(frame)&&r.CommittedThrough()==frame);
  VERIFY(r.CommittedPrefixInputs(frame,exact)&&exact==d.inputs);
  VERIFY(!r.ConfirmedInputs(frame,exact)&&r.Correcting());
  VERIFY(!r.CommitReconciledPrefix(frame-1));
  VERIFY(r.CaptureFrame()==7&&r.PredictionFrontier()==6);
 }
 VERIFY(!r.CanReplayConfirmed());f.step(true);
 VERIFY(!r.CanReplayConfirmed()&&!r.CommitReconciledPrefix(4));
 VERIFY(!r.CommittedPrefixInputs(4,exact));
 f.actual(4,value(8,2,-50,200));VERIFY(r.RollbackFrame()==4);
 VERIFY(!r.CommittedPrefixInputs(3,exact)); // pending correction is still a publication fence
 VERIFY(r.RestartCorrection(4));VERIFY(!r.RestartCorrection(2));
 VERIFY(r.CommittedThrough()==3&&r.CanReplayConfirmed());
 for(unsigned frame=4;frame<=5;++frame){f.step(false);VERIFY(r.CommitReconciledPrefix(frame));}
 VERIFY(!r.CanReplayConfirmed()&&r.EndCorrection());
 VERIFY(r.NextFrame()==6&&r.CaptureFrame()==7&&!r.NeedsCapture());
 f.actual(6,value());const auto d=f.step(false);
 VERIFY(d.inputs[0]==value(0x80,2,17.5f,250.25f));VERIFY(r.SetCommitLimit(6));
 VERIFY(r.ConfirmedInputs(6,exact)&&r.NeedsCapture());
 VERIFY(r.SubmitRemote(1,2,value(0))==Result::ConflictingConfirmedInput);
}
void lifecycle_trim(){
 ++groups;Fixture f;auto& r=f.r();f.prime();
 for(unsigned frame=1;frame<=4;++frame){f.capture(value(frame&1));f.step(true);}
 f.capture(value(0x80,1,12.5f,234.75f));
 VERIFY(r.SubmitRemote(1,2,value(9,0,0,0,true))==Result::RollbackRequired);VERIFY(r.BeginCorrection(2));
 VERIFY(!r.EndCorrection(true)); // frame one is still predicted
 VERIFY(r.SubmitRemote(1,1,value())==Result::PredictionCorrect);
 VERIFY(r.EndCorrection(true));VERIFY(r.NextFrame()==2&&r.LastFrame()==1&&r.CaptureFrame()==6);
 VERIFY(r.SetCommitLimit(1));VERIFY(r.SetWorldReady(false));
 auto boundary=f.step(false);VERIFY(boundary.inputs[1].buttons&InputLanes::Pause);VERIFY(r.SetCommitLimit(2));
 for(unsigned frame=3;frame<=5;++frame){
  VERIFY(!r.NeedsCapture());VERIFY(!r.Prepare(frame).canAdvance);f.actual(frame,value());auto d=f.step(false);
  if(frame==5)VERIFY(d.inputs[0]==value(0x80,1,12.5f,234.75f));
  VERIFY(r.SetCommitLimit(frame));
 }
 VERIFY(r.NeedsCapture()&&r.CaptureFrame()==6);VERIFY(r.SetWorldReady(true));
}
void confirmed_ui_pipeline(){
 ++groups;
 for(unsigned seats:{2u,3u}){
  Fixture f(seats);f.prime();auto& r=f.r();VERIFY(r.SetWorldReady(false));
  constexpr auto interval=Netplay::ConfirmedInputClock::IntervalUs;
  for(unsigned frame=1;frame<=Netplay::ConfirmedInputClock::MaxLead;++frame){
   r.SetCapturePhase(true,100'000+(frame-1)*interval);
   VERIFY(r.NeedsCapture()&&r.CaptureFrame()==frame);
   VERIFY(r.Capture(frame,value(frame<4?1:0)));
   VERIFY(!r.NeedsCapture()); // repeated callback cannot sample another slot
   VERIFY(r.NextFrame()==1&&!r.Prepare(1).canAdvance);
  }
  r.SetCapturePhase(true,1'000'000);VERIFY(!r.NeedsCapture()); // bounded during blackout
  r.SetCapturePhase(false,1'000'000); // resumed combat consumes the immutable UI tail
  for(unsigned frame=1;frame<=Netplay::ConfirmedInputClock::MaxLead;++frame){
   VERIFY(!r.NeedsCapture());f.actual(frame,value());
   const auto d=f.step(false);VERIFY(d.inputs[0]==value(frame<4?1:0));VERIFY(r.SetCommitLimit(frame));
  }
  VERIFY(r.NeedsCapture()&&r.CaptureFrame()==r.NextFrame());
  VERIFY(r.Finish());VERIFY(r.BeginNextRun(419));VERIFY(r.CaptureFrame()==0);
 }
}
void readonly_and_hybrid(const char* direct_path,const char* hybrid_path){
 ++groups;
 for(unsigned mode:{0u,2u}){
  auto s=config(3,2,mode,mode==2?2:0);s.measured_prediction=mode==2?2:0;
  ReplayArchive archive;VERIFY(archive.Begin(s,1723456789));
  std::array<Netplay::FrameInput,3> inputs{value(1),value(8,2,-18.75f,200.25f),value(9,3,.5f,-.25f)};
  for(unsigned frame=0;frame<4;++frame)VERIFY(archive.Append(frame,1,inputs.data(),3));
  std::vector<unsigned char> bytes;VERIFY(archive.Encode(bytes,"RBTEST",123456,false));
  ReplayArchive parsed;VERIFY(parsed.Load(bytes.data(),bytes.size()));VERIFY(parsed.Setup().mode==mode&&parsed.Setup().version==6);
  if(!mode){
   auto corrupt=bytes;auto invalid=s;invalid.input_delay=1;VERIFY(!invalid.valid());
   put(corrupt,52,1);put(corrupt,16,GameplayAbi(invalid));checksum(corrupt);
   VERIFY(!ReplayArchive::Validate(corrupt.data(),corrupt.size())); // valid checksum/ABI cannot authorize direct D1
  }
  NetplayRuntime viewer;VERIFY(viewer.BeginPlayback(parsed.Setup()));VERIFY(!viewer.AllowsRollback()&&!viewer.PredictionLimit());
  VERIFY(!viewer.SetWorldReady(true)&&!viewer.NeedsCapture());VERIFY(!viewer.Prepare(0).canAdvance);
  for(unsigned frame=0;frame<4;++frame){
   VERIFY(viewer.FeedPlayback(frame,parsed.FrameAt(frame)->data(),3));auto d=viewer.Prepare(frame);
   VERIFY(d.canAdvance&&!d.predictedMask&&d.inputs==inputs);VERIFY(viewer.MarkSimulated(frame,d));
   std::array<Netplay::FrameInput,3> exact;VERIFY(viewer.ConfirmedInputs(frame,exact)&&exact==inputs);
  }
  VERIFY(viewer.Finish());const auto path=mode?hybrid_path:direct_path;
  if(path){auto* file=std::fopen(path,"wb");VERIFY(file);VERIFY(std::fwrite(bytes.data(),1,bytes.size(),file)==bytes.size());VERIFY(std::fclose(file)==0);}
 }
 Fixture hybrid(2,2,2);auto& r=hybrid.r();VERIFY(r.AllowsRollback()&&r.PredictionLimit()==12);
 for(unsigned frame=0;frame<2;++frame){hybrid.capture(value());hybrid.actual(frame,{});hybrid.step(false);VERIFY(r.SetCommitLimit(frame));}
 VERIFY(r.SetWorldReady(true));hybrid.capture(value());auto predicted=hybrid.step(true);VERIFY(predicted.inputs[0]==value());
 Fixture delay(2,1,0);auto& exact=delay.r();VERIFY(!exact.AllowsRollback()&&!exact.PredictionLimit());
 delay.capture();VERIFY(!exact.Prepare(0).canAdvance);delay.actual(0,value());delay.step(false);VERIFY(exact.CommittedThrough()==0);
}
}
void rollback_tests(const char* direct_path,const char* hybrid_path){
 admission();held_and_depth(2,2);held_and_depth(3,3);stable_and_edges();
 correction_frontiers(2);correction_frontiers(3);confirmed_history_prefix(2);confirmed_history_prefix(3);lifecycle_trim();confirmed_ui_pipeline();readonly_and_hybrid(direct_path,hybrid_path);
 std::printf("PASS TH11 rollback network: %u groups, %u checks; real gate/core/InputLanes/GameInput; no World or browser performance claim\n",groups,checks);
}
