#include <eagler/netplay/AdonisConnection.hpp>
#include <eagler/netplay/SessionChannel.hpp>
#include <array>
#include <deque>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace Netplay;
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"calibration retirement failed at %d: %s\n",__LINE__,#x);std::exit(1);}}while(false)
namespace {
struct Link final:PeerTransport {
 struct Item{std::uint64_t due;std::vector<std::uint8_t> bytes;};
 Link* other=nullptr;std::deque<Item> incoming;std::uint64_t now=0,old=0;unsigned seat=0,dropped=0;bool drop_ack=false;
 bool IsOpen()const override{return true;}bool Failed()const override{return false;}
 std::size_t BufferedAmount()const override{return 0;}
 bool send(const std::uint8_t* bytes,std::size_t size){
  InputPacket input;
  if(drop_ack&&DecodeInputPacket(bytes,size,&input)&&input.sessionId==old&&input.ackFrame!=INVALID_FRAME){++dropped;return true;}
  other->incoming.push_back({now+1000,{bytes,bytes+size}});return true;
 }
 bool SendTo(std::uint8_t peer,const std::uint8_t* p,std::size_t n)override{CHECK(peer!=seat&&peer<2);return send(p,n);}
 bool SendRepairTo(std::uint8_t peer,const std::uint8_t* p,std::size_t n)override{return SendTo(peer,p,n);}
 bool SendControl(const std::uint8_t* p,std::size_t n)override{return send(p,n);}
 bool Poll(std::vector<std::uint8_t>* out)override{
  for(auto it=incoming.begin();it!=incoming.end();++it)if(it->due<=now){*out=std::move(it->bytes);incoming.erase(it);return true;}return false;
 }
};
struct Peer{
 Link wire;AdonisConnection calibration{wire};SessionChannel channel{calibration};SessionGate gate;RollbackCore core;SessionConfig config;
 unsigned generation=0;
 Peer(unsigned seat){wire.seat=seat;config.sessionId=wire.old=0xa000;config.gameId=11;config.gameplayAbi=0xb013;config.seed=314;config.playerCount=2;config.localPlayer=seat;prepare(nullptr);}
 void prepare(const SessionConfig* previous){CHECK(gate.Reset(config));CoreConfig policy;policy.sessionId=config.sessionId;policy.playerCount=2;policy.localPlayer=config.localPlayer;policy.allowPrediction=false;CHECK(core.Reset(policy));calibration.Prepare(config,AdonisMode::Delay,false,0,2,previous);}
 void next(){const auto previous=config;config.sessionId++;config.seed++;++generation;prepare(&previous);}
 void pump(std::uint64_t us){
  wire.now=us;
  if(calibration.Waiting()&&channel.Retiring())CHECK(channel.PumpRetirement(us/1000));
  CHECK(calibration.Pump(us,true));
  if(calibration.NeedsApply()){CHECK(calibration.Startup().Probes()==129);CHECK(calibration.Startup().Selected().prediction==0);SessionChannelConfig policy;policy.adonisPhase=true;policy.adonisPredictionFrames=0;policy.inputResendMs=16;CHECK(channel.BeginSession(config,us/1000,policy));calibration.Applied();}
  if(!calibration.Waiting())CHECK(channel.Pump(gate,core,us/1000,false));
 }
};
void asymmetric_final_ack(){
 Peer left(0),right(1);left.wire.other=&right.wire;right.wire.other=&left.wire;std::array<Peer*,2> peers{&left,&right};
 std::uint64_t time=0;
 for(;time<10'000'000;time+=1000){for(auto* p:peers)p->pump(time);if(left.gate.CanStart()&&right.gate.CanStart())break;}
 CHECK(time<10'000'000);
 left.wire.drop_ack=true;
 for(auto* p:peers){CHECK(p->core.ScheduleLocalInput(0,FrameInput(std::uint16_t(p->config.localPlayer+1))));CHECK(p->channel.LocalCaptured(p->core,0,time/1000));}
 for(unsigned n=0;n<1000;++n){time+=1000;for(auto* p:peers)p->pump(time);
  for(auto* p:peers)if(p->core.LastSimulatedFrame()==INVALID_FRAME){const auto d=p->core.PrepareFrame(0);if(d.canAdvance)CHECK(p->core.MarkSimulated(0,d));}
  if(left.channel.CanRetire(left.core,0))break;
 }
 CHECK(left.channel.CanRetire(left.core,0));CHECK(!right.channel.CanRetire(right.core,0));CHECK(left.wire.dropped);
 CHECK(left.channel.Retire(left.core,0,time/1000));left.next();
 CHECK(left.calibration.Waiting()&&!left.gate.CanStart());CHECK(left.channel.Retiring());
 const auto begin=time;
 for(;time<begin+10'000'000;time+=1000){
  // Keep one participant in the old session for 120ms. It retransmits old
  // input/ACK while its faster peer already negotiates the new generation.
  if(time-begin>=120000)left.wire.drop_ack=false;
  left.pump(time);right.pump(time);
  if(!right.generation){
   CHECK(right.channel.FlushRetirementFence(right.core,0,time/1000));
   if(right.channel.CanRetire(right.core,0)){CHECK(right.channel.Retire(right.core,0,time/1000));right.next();}
  }
  if(right.generation&&left.gate.CanStart()&&right.gate.CanStart())break;
 }
 CHECK(time<begin+10'000'000);CHECK(left.generation==1&&right.generation==1);
 for(auto* p:peers){
  CHECK(p->calibration.Startup().Probes()==129);CHECK(p->config.seed==315);CHECK(p->config.sessionId==0xa001);
  CHECK(p->core.LastSimulatedFrame()==INVALID_FRAME);CHECK(!p->core.HasLocalCapture(0));
  CHECK(p->core.ConfirmedThrough(1-p->config.localPlayer)==INVALID_FRAME);
 }
 for(unsigned n=0;n<10;++n){time+=1000;for(auto* p:peers)p->pump(time);}
 CHECK(!left.channel.Retiring()&&!right.channel.Retiring());
 std::puts("PASS lost asymmetric final ACK survives next-generation actual-channel calibration; new core remains empty");
}
void rejected_early_packets(){
 for(unsigned kind=0;kind<6;++kind){
  Link left,right;left.other=&right;right.other=&left;
  SessionConfig previous;previous.sessionId=11;previous.gameId=11;previous.gameplayAbi=23;previous.seed=91;previous.playerCount=2;previous.localPlayer=0;
  auto current=previous;current.sessionId++;current.seed++;
  AdonisConnection connection(left);connection.Prepare(current,AdonisMode::Delay,false,0,2,kind==5?nullptr:&previous);
  std::vector<std::uint8_t> bytes;
  if(kind<3||kind==5){
   InputPacket packet;packet.sessionId=kind==0?current.sessionId:kind==1?100:previous.sessionId;
   packet.playerCount=kind==2?3:2;packet.senderPlayer=1;packet.sequence=1;packet.latestFrame=packet.ackFrame=0;packet.senderFrame=1;
   CHECK(EncodeInputPacket(packet,&bytes));
  }else if(kind==3){
   SessionPacket packet;packet.sessionId=previous.sessionId;packet.gameId=11;packet.gameplayAbi=24;packet.seed=91;packet.playerCount=2;packet.senderPlayer=1;packet.phase=SessionPhase::Hello;
   CHECK(EncodeSessionPacket(packet,&bytes));
  }else{
   AdonisPhaseSample packet;packet.sessionId=previous.sessionId;packet.frame=16;packet.senderPlayer=0;packet.targetPlayer=1;packet.waitCount=16;
   CHECK(EncodeAdonisPhaseSample(packet,&bytes));
  }
  left.incoming.push_back({0,bytes});CHECK(!connection.Pump(0,true));CHECK(connection.Failed());
 }
 std::puts("PASS current/foreign/malformed-scope early inputs, wrong retired ABI/target and non-opted-in old packets stay rejected");
}
}
int main(){asymmetric_final_ack();rejected_early_packets();}

