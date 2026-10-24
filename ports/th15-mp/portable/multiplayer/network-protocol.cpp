#include "../../web/cpp/multiplayer/NetplayRuntime.hpp"
#include "../../web/cpp/multiplayer/InputLanes.hpp"
#include "../../web/cpp/multiplayer/ReplayArchive.hpp"
#include <array>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
using namespace th15::multiplayer;
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"network check failed at %d: %s\n",__LINE__,#x);std::exit(1);}}while(false)
namespace {
SessionSetup setup(unsigned count,unsigned seat,unsigned delay){
 SessionSetup s;s.session_id=0x1122334455667788ull;s.player_count=count;s.local_player=seat;s.seed=417;s.difficulty=1;
 s.automatic=false;s.requested_delay=s.input_delay=delay;s.selections={0,1,count==3?3u:0u};s.chapter_threshold=delay==9?5000000:delay==2?500000:0;s.build={0x12345678,0x90abcdef,0x87654321,0xfedcba09};return s;
}
Netplay::FrameInput input(unsigned frame,unsigned seat){
 Netplay::FrameInput out;const unsigned held=((frame+seat)%7==0?2:1)|((frame/6+seat)%2?0x10:0x20)|((frame/11)%2?4:0);
 const int mode=(frame%3)==0?2:(frame%3)==1?1:0;const float x=mode==4?float(int(seat)-1)*.5f:mode?float(frame%300)-150:0;
 const float y=mode==4?.25f:mode?float(64+frame%320):0;
 CHECK(InputLanes::Capture(held,frame==105,mode,x,y,out));return out;
}
void link(std::array<NetplayRuntime,3>& players,unsigned count){
 for(unsigned i=0;i<count;++i)for(unsigned j=0;j<count;++j)if(i!=j)CHECK(players[i].ApplySession(players[j].Hello())==Netplay::SessionPacketResult::Accepted);
 for(unsigned i=0;i<count;++i){CHECK(players[i].CanSendReady());players[i].MarkLocalReady();}
 for(unsigned i=0;i<count;++i)for(unsigned j=0;j<count;++j)if(i!=j)CHECK(players[i].ApplySession(players[j].Ready())==Netplay::SessionPacketResult::Accepted);
 for(unsigned i=0;i<count;++i)CHECK(players[i].CanStart());
}
void checksum(std::vector<unsigned char>& bytes){
 unsigned h=2166136261u;for(unsigned i=0;i<bytes.size();++i)if(i<36||i>=40){h^=bytes[i];h*=16777619u;}
 for(unsigned i=0;i<4;++i)bytes[36+i]=static_cast<unsigned char>(h>>(i*8));
}
void legacy_replay_admission(){
 const std::array<unsigned,4> baseline{0x69cf52f0,0x644bdac4,0x784d5168,0xcab6de55};
 const auto current=setup(2,0,0).build;
 for(unsigned count:{2u,3u})for(unsigned delay:{0u,2u,9u})for(bool known:{true,false}){
  auto config=setup(count,count-1,delay);config.build=baseline;if(!known)config.build[3]^=1;
  Netplay::InputReplay old;Netplay::InputReplayConfig metadata;
  metadata.gameId=15;metadata.gameplayAbi=ReplayAbiBeforeStageFlowFix(config);
  metadata.playerCount=count;metadata.recordedPlayer=config.local_player;
  metadata.description.resize(ReplayArchive::DescriptionBytes);
  const auto put=[&](unsigned at,unsigned value){for(unsigned i=0;i<4;++i)metadata.description[at+i]=static_cast<unsigned char>(value>>(i*8));};
  put(0,ReplayArchive::Magic);put(4,ReplayArchive::Version);put(8,20);put(12,delay);
  const auto words=EncodeSessionSetup(config);for(unsigned i=0;i<words.size();++i)put(40+i*4,words[i]);
  CHECK(old.Begin(metadata));
  for(unsigned frame=0;frame<180;++frame){std::array<Netplay::FrameInput,3> inputs;for(unsigned seat=0;seat<count;++seat)inputs[seat]=input(frame,seat);CHECK(old.Append(frame,frame<90?1:2,inputs.data(),count));}
  std::vector<unsigned char> bytes;CHECK(old.Encode(&bytes));ReplayArchive replay;
  CHECK(replay.Load(bytes.data(),bytes.size()));CHECK(replay.LegacyStageFlow());
  CHECK(replay.CompatibleBuild(current)==known);CHECK(!replay.CompatibleBuild({}));
  CHECK(replay.CompatibleBuild(config.build)==known);
  CHECK(replay.Info().chapterCount==2&&replay.FrameCount()==180);
  for(unsigned frame=0;frame<180;++frame)CHECK(*replay.FrameAt(frame)==*old.FrameAt(frame));
  CHECK(replay.Encode(bytes,"LEGACY",0,false));ReplayArchive restored;
  CHECK(restored.Load(bytes.data(),bytes.size())&&restored.LegacyStageFlow());
  CHECK(restored.CompatibleBuild(current)==known);
  replay.Clear();CHECK(!replay.LegacyStageFlow()&&!replay.CompatibleBuild(current));
 }
}
std::vector<unsigned char> run(unsigned count,unsigned delay,bool challenge=false){
 std::array<NetplayRuntime,3> players;std::array<ReplayArchive,3> recordings;
 for(unsigned s=0;s<count;++s){auto config=setup(count,s,delay);config.challenge=challenge;CHECK(players[s].Reset(config));CHECK(recordings[s].Begin(config,1723456789));}
 link(players,count);
 for(unsigned dst=0;dst<count;++dst)for(unsigned src=0;src<count;++src)if(src!=dst)for(unsigned f=0;f<delay;++f)
  CHECK(players[dst].SubmitRemote(src,f,{})==Netplay::RemoteInputResult::Accepted);
 Netplay::FrameInput withheld;
 for(unsigned f=0;f<180;++f){
  for(unsigned s=0;s<count;++s){CHECK(players[s].NeedsCapture());CHECK(players[s].Capture(f,input(f,s)));CHECK(!players[s].NeedsCapture());}
  for(unsigned dst=0;dst<count;++dst)for(unsigned src=0;src<count;++src)if(src!=dst){
   const auto value=input(f,src);if(dst==0&&src==count-1&&f+delay==80){withheld=value;continue;}
   CHECK(players[dst].SubmitRemote(src,f+delay,value)==Netplay::RemoteInputResult::Accepted);
  }
  if(f==80){CHECK(!players[0].Prepare(f).canAdvance);CHECK(!players[0].NeedsCapture());CHECK(players[0].NextFrame()==80);
   CHECK(players[0].SubmitRemote(count-1,80,withheld)==Netplay::RemoteInputResult::Accepted);}
  for(unsigned s=0;s<count;++s){
   const auto decision=players[s].Prepare(f);CHECK(decision.canAdvance);CHECK(!decision.predictedMask);
   for(unsigned lane=0;lane<count;++lane)CHECK(decision.inputs[lane]==(f<delay?Netplay::FrameInput{}:input(f-delay,lane)));
   CHECK(players[s].MarkSimulated(f,decision));CHECK(recordings[s].Append(f,f<90?1:2,decision.inputs.data(),count));
   std::array<Netplay::FrameInput,3> exact;CHECK(players[s].ConfirmedInputs(f,exact));CHECK(exact==decision.inputs);
  }
 }
 std::vector<unsigned char> bytes;
 for(unsigned s=0;s<count;++s){
  CHECK(players[s].Finish());CHECK(players[s].Retired());CHECK(recordings[s].Encode(bytes,s==2?"P3TEST":"PLAYER",999999999,true));
  ReplayArchive replay;CHECK(replay.Load(bytes.data(),bytes.size()));CHECK(replay.FrameCount()==180);CHECK(replay.Info().config.recordedPlayer==s);
  CHECK(replay.Setup().input_delay==delay);CHECK(replay.Setup().challenge==challenge);CHECK(replay.Score()==999999999);CHECK(replay.Completed());
  CHECK(!replay.LegacyStageFlow()&&replay.CompatibleBuild(replay.Setup().build));
  auto other_build=replay.Setup().build;other_build[0]^=1;CHECK(!replay.CompatibleBuild(other_build));
  NetplayRuntime viewer;CHECK(viewer.BeginPlayback(replay.Setup()));CHECK(viewer.Setup().challenge==challenge);CHECK(!viewer.NeedsCapture());
  for(unsigned f=0;f<180;++f){CHECK(viewer.FeedPlayback(f,replay.FrameAt(f)->data(),count));const auto d=viewer.Prepare(f);CHECK(d.canAdvance&&!d.predictedMask);
   CHECK(d.inputs==*recordings[s].FrameAt(f));CHECK(viewer.MarkSimulated(f,d));}
  CHECK(viewer.Finish());CHECK(!viewer.BeginNextRun(418));
  CHECK(players[s].BeginNextRun(418));CHECK(players[s].Generation()==1);CHECK(players[s].NextFrame()==0);CHECK(!players[s].CanStart());
  auto corrupt=bytes;corrupt.back()^=1;CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size()));
  for(unsigned old=1;old<SessionSetup::GameplayVersion;++old){corrupt=bytes;corrupt[Netplay::InputReplay::HeaderBytes+8]=old;checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size()));} // previous economies
  corrupt=bytes;corrupt[12]=10;checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size()));
  corrupt=bytes;corrupt[21]=static_cast<unsigned char>((s+1)%count);checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size()));
  corrupt=bytes;corrupt[136]=2;checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size())); // measured mode word
  corrupt=bytes;corrupt[164]=2;checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size())); // invalid challenge flag
  CHECK(!ReplayArchive::Validate(bytes.data(),bytes.size()-1));
 }
 link(players,count);
 for(unsigned s=0;s<count;++s){CHECK(players[s].Setup().seed==418);CHECK(players[s].Setup().challenge==challenge);}
 return bytes;
}
}
extern "C" {
__attribute__((export_name("network_allocate"))) void* network_allocate(unsigned bytes){return std::malloc(bytes);}
__attribute__((export_name("network_free"))) void network_free(void* pointer){std::free(pointer);}
__attribute__((export_name("network_validate"))) int network_validate(const unsigned char* bytes,unsigned size){return ReplayArchive::Validate(bytes,size);}
}
int main(int argc,char** argv){
 legacy_replay_admission();
 for(unsigned count:{2u,3u}){
  std::array<NetplayRuntime,3> players;
  for(unsigned seat=0;seat<count;++seat)CHECK(players[seat].Reset(setup(count,seat,0)));
  link(players,count);auto& r=players[0];
  for(unsigned frame=0;frame<Netplay::ConfirmedInputClock::MaxLead;++frame){
   r.SetCapturePhase(true,100'000+frame*Netplay::ConfirmedInputClock::IntervalUs);
   CHECK(r.NeedsCapture()&&r.CaptureFrame()==frame);CHECK(r.Capture(frame,input(frame,0)));
   CHECK(!r.NeedsCapture()&&r.NextFrame()==0&&!r.Prepare(0).canAdvance);
  }
  r.SetCapturePhase(true,1'000'000);CHECK(!r.NeedsCapture());
  // A confirmed menu may queue inputs, combat must drain that tail rather
  // than preserving a second permanent delay after returning from the menu.
  r.SetCapturePhase(false,1'000'000);
  for(unsigned frame=0;frame<Netplay::ConfirmedInputClock::MaxLead;++frame){
   CHECK(!r.NeedsCapture());
   for(unsigned seat=1;seat<count;++seat)CHECK(r.SubmitRemote(seat,frame,input(frame,seat))==Netplay::RemoteInputResult::Accepted);
   const auto d=r.Prepare(frame);CHECK(d.canAdvance&&!d.predictedMask&&d.inputs[0]==input(frame,0));CHECK(r.MarkSimulated(frame,d));
  }
  CHECK(r.NeedsCapture()&&r.CaptureFrame()==r.NextFrame());
  CHECK(r.Finish()&&r.BeginNextRun(419));CHECK(r.CaptureFrame()==0);
 }
 for(unsigned count:{2u,3u}){auto s=setup(count,0,0);auto words=EncodeSessionSetup(s);CHECK(words[22]==(count==2?1250000u:1500000u));SessionSetup decoded;CHECK(DecodeSessionSetup(decoded,words.data(),words.size()));s.chapter_threshold=5000000;CHECK(GameplayAbi(s)!=GameplayAbi(decoded));NetplayRuntime a,b;CHECK(a.Reset(s));decoded.local_player=1;CHECK(b.Reset(decoded));CHECK(a.ApplySession(b.Hello())==Netplay::SessionPacketResult::ContractMismatch);
 for(unsigned value:{0u,250000u,500001u,5250000u}){words[22]=value;CHECK(!DecodeSessionSetup(decoded,words.data(),words.size()));}}
 auto config=setup(3,2,2);auto words=EncodeSessionSetup(config);SessionSetup decoded;CHECK(DecodeSessionSetup(decoded,words.data(),words.size()));
 CHECK(GameplayAbi(config)==GameplayAbi(setup(3,0,2)));CHECK(!decoded.challenge);words[14]=2;CHECK(!DecodeSessionSetup(decoded,words.data(),words.size()));words[14]=1;words[21]=1;CHECK(DecodeSessionSetup(decoded,words.data(),words.size())&&decoded.challenge);CHECK(EncodeSessionSetup(decoded)==words);CHECK(GameplayAbi(config)!=GameplayAbi(decoded));words[21]=2;CHECK(!DecodeSessionSetup(decoded,words.data(),words.size()));
 auto changed=config;changed.local_player=0;changed.build[0]^=1;NetplayRuntime left,right;CHECK(left.Reset(config));CHECK(right.Reset(changed));CHECK(left.ApplySession(right.Hello())==Netplay::SessionPacketResult::ContractMismatch);
 changed=config;changed.local_player=0;changed.challenge=true;CHECK(left.Reset(config));CHECK(right.Reset(changed));CHECK(left.ApplySession(right.Hello())==Netplay::SessionPacketResult::ContractMismatch);
 Netplay::FrameInput touch;CHECK(InputLanes::Capture(0x200001,true,2,-184,432,touch));std::array<Netplay::FrameInput,3> wire{touch,touch,touch};std::array<th15::MultiplayerInput,3> native;
 CHECK(InputLanes::Decode(wire.data(),3,native));CHECK(native[2].held==0x200001&&native[2].pause&&native[2].touch_mode==2&&native[2].touch_y==432);
 CHECK(!InputLanes::Capture(0,false,3,1.1f,0,touch));CHECK(!InputLanes::Capture(0,false,1,0,433,touch));
 CHECK(!InputLanes::Capture(0,false,0,std::numeric_limits<float>::quiet_NaN(),0,touch));
 std::vector<unsigned char> fixture,challenge_fixture;for(bool challenge:{false,true})for(unsigned count:{2u,3u})for(unsigned delay:{0u,2u,9u}){auto bytes=run(count,delay,challenge);if(count==3&&delay==2)(challenge?challenge_fixture:fixture)=std::move(bytes);}
 if(argc>1){auto* file=std::fopen(argv[1],"wb");CHECK(file);CHECK(std::fwrite(fixture.data(),1,fixture.size(),file)==fixture.size());CHECK(std::fclose(file)==0);}
 if(argc>2){auto* file=std::fopen(argv[2],"wb");CHECK(file);CHECK(std::fwrite(challenge_fixture.data(),1,challenge_fixture.size(),file)==challenge_fixture.size());CHECK(std::fclose(file)==0);}
 std::puts("PASS TH15 exact 2P/3P, D=0/2/9, reordered input hole, frame capture ownership, generation fence, all-seat Replay/recorded P3, no second delay, malformed Replay rejection");
}
