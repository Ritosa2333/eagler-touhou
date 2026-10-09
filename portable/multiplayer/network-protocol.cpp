#include "../../th11_web/cpp/multiplayer/NetplayRuntime.hpp"
#include "../../th11_web/cpp/multiplayer/InputLanes.hpp"
#include "../../th11_web/cpp/multiplayer/ReplayArchive.hpp"
#include <array>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
using namespace th11::multiplayer;
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"network check failed at %d: %s\n",__LINE__,#x);std::exit(1);}}while(false)
namespace {
SessionSetup setup(unsigned count,unsigned seat,unsigned delay){
 SessionSetup s;s.session_id=0x1122334455667788ull;s.player_count=count;s.local_player=seat;s.seed=417;s.difficulty=1;
 s.automatic=false;s.requested_delay=s.input_delay=delay;s.selections={0,4,count==3?5u:0u};s.build={0x12345678,0x90abcdef,0x87654321,0xfedcba09};return s;
}
Netplay::FrameInput input(unsigned frame,unsigned seat){
 Netplay::FrameInput out;const unsigned held=((frame+seat)%7==0?2:1)|((frame/6+seat)%2?0x10:0x20)|((frame/11)%2?4:0);
 const int mode=(frame%3)==0?3:(frame%3)==1?1:0;const float x=mode==3?float(int(seat)-1)*.5f:mode?float(frame%300)-150:0;
 const float y=mode==3?.25f:mode?float(64+frame%320):0;
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
std::vector<unsigned char> run(unsigned count,unsigned delay){
 std::array<NetplayRuntime,3> players;std::array<ReplayArchive,3> recordings;
 for(unsigned s=0;s<count;++s){const auto config=setup(count,s,delay);CHECK(players[s].Reset(config));CHECK(recordings[s].Begin(config,1723456789));}
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
  CHECK(replay.Setup().input_delay==delay);CHECK(replay.Score()==999999999);CHECK(replay.Completed());
  NetplayRuntime viewer;CHECK(viewer.BeginPlayback(replay.Setup()));CHECK(!viewer.NeedsCapture());
  for(unsigned f=0;f<180;++f){CHECK(viewer.FeedPlayback(f,replay.FrameAt(f)->data(),count));const auto d=viewer.Prepare(f);CHECK(d.canAdvance&&!d.predictedMask);
   CHECK(d.inputs==*recordings[s].FrameAt(f));CHECK(viewer.MarkSimulated(f,d));}
  CHECK(viewer.Finish());CHECK(!viewer.BeginNextRun(418));
  CHECK(players[s].BeginNextRun(418));CHECK(players[s].Generation()==1);CHECK(players[s].NextFrame()==0);CHECK(!players[s].CanStart());
  auto corrupt=bytes;corrupt.back()^=1;CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size()));
  corrupt=bytes;corrupt[12]=10;checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size()));
  corrupt=bytes;corrupt[21]=static_cast<unsigned char>((s+1)%count);checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size()));
  corrupt=bytes;corrupt[136]=2;checksum(corrupt);CHECK(!ReplayArchive::Validate(corrupt.data(),corrupt.size())); // measured mode word
  CHECK(!ReplayArchive::Validate(bytes.data(),bytes.size()-1));
 }
 link(players,count);
 for(unsigned s=0;s<count;++s)CHECK(players[s].Setup().seed==418);
 return bytes;
}
}
extern "C" {
__attribute__((export_name("network_allocate"))) void* network_allocate(unsigned bytes){return std::malloc(bytes);}
__attribute__((export_name("network_free"))) void network_free(void* pointer){std::free(pointer);}
__attribute__((export_name("network_validate"))) int network_validate(const unsigned char* bytes,unsigned size){return ReplayArchive::Validate(bytes,size);}
}
int main(int argc,char** argv){
 auto config=setup(3,2,2);auto words=EncodeSessionSetup(config);SessionSetup decoded;CHECK(DecodeSessionSetup(decoded,words.data(),words.size()));
 CHECK(GameplayAbi(config)==GameplayAbi(setup(3,0,2)));words[14]=2;CHECK(!DecodeSessionSetup(decoded,words.data(),words.size()));words[14]=1;words[21]=1;CHECK(!DecodeSessionSetup(decoded,words.data(),words.size()));
 auto changed=config;changed.local_player=0;changed.build[0]^=1;NetplayRuntime left,right;CHECK(left.Reset(config));CHECK(right.Reset(changed));CHECK(left.ApplySession(right.Hello())==Netplay::SessionPacketResult::ContractMismatch);
 Netplay::FrameInput touch;CHECK(InputLanes::Capture(0x200001,true,2,-184,432,touch));std::array<Netplay::FrameInput,3> wire{touch,touch,touch};std::array<th11::MultiplayerInput,3> native;
 CHECK(InputLanes::Decode(wire.data(),3,native));CHECK(native[2].held==0x200001&&native[2].pause&&native[2].touch_mode==2&&native[2].touch_y==432);
 CHECK(!InputLanes::Capture(0,false,3,1.1f,0,touch));CHECK(!InputLanes::Capture(0,false,1,0,433,touch));
 CHECK(!InputLanes::Capture(0,false,0,std::numeric_limits<float>::quiet_NaN(),0,touch));
 std::vector<unsigned char> fixture;for(unsigned count:{2u,3u})for(unsigned delay:{0u,2u,9u}){auto bytes=run(count,delay);if(count==3&&delay==2)fixture=std::move(bytes);}
 if(argc>1){auto* file=std::fopen(argv[1],"wb");CHECK(file);CHECK(std::fwrite(fixture.data(),1,fixture.size(),file)==fixture.size());CHECK(std::fclose(file)==0);}
 std::puts("PASS TH11 exact 2P/3P, D=0/2/9, reordered input hole, frame capture ownership, generation fence, all-seat Replay/recorded P3, no second delay, malformed Replay rejection");
}
