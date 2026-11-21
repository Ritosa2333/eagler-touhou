#include "game/NativeRunReplay.hpp"
#include "game/NativeNormalRecord.hpp"
#include "game/NativeReplayStageBinding.hpp"
#include "game/NativeReplaySlotPreview.hpp"
#include "game/OwnedReplayPlaybackInput.hpp"
#include "game/PlayerShotBirthRules.hpp"
#include "game/PerfectHud.hpp"
#include "game/GrazeRules.hpp"
#include "engine/Random.hpp"
#include "runtime/Memory.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
// Only World/carry, input configuration and terminal-binding containers are seams.
// No World/ECL/MSG/nativeclear/normalpermit is created by this component.
struct WorldSeam{bool failed=false;unsigned frameValue=0;th128::Random random{34772,0};bool faulted()const{return failed;}unsigned frame()const{return frameValue;}th128::Random gameRandomState()const{return random;}} worldValue;
static auto* world=&worldValue;
static bool worldTickExecuting=false,playerMotion=true,playerFireConfigured=true,playerConfigured=true,debugRunTainted=false;
static unsigned worldGeneration=2;
static int worldStartDifficulty=0,shotDifficulty=0,originalContinueCount=0,scoreRaw=0,pointValueRaw=100000,motivationRaw=20000,motivationFlag=0;
static float cumulativeFrozenArea=0;
static std::string worldStartedName="st_a1_1.ecl",operationError;
static th128::graze::Globals grazeGlobals{2529,10000};
static std::shared_ptr<th128::record::NativeRunReplay> normalReplay;
static std::shared_ptr<th128::record::NativeNormalRecord> normalRecord;
static th128::record::OwnedReplayPlaybackInput replayPlayback;
static auto perfectHud=std::make_unique<th128::hud::PerfectHud>();
static double normalReplayOrigin=0,normalReplayClock=0;static bool normalReplayClockPending=false;
struct WindowSeam{int perfectCharge4b4d70=0;} playerWindowControl;
struct PositionSeam{int horizontal=0,vertical=51200;} playerRaw;
struct CarrySeam{std::shared_ptr<th128::record::NativeRunReplay> normalReplay;bool valid=false,acknowledged=false,consumed=false,gameRandomRestored=false,scalarRestored=false;unsigned restoredGeneration=0,scalarRestoredGeneration=0;int nextId=0,targetDifficulty=0;std::string targetWorldName;} stageCarry;
static void bindOrdinaryTerminalAfterReplayBegin(){}
#include "actual-stage-table.inc"
#include "actual-stage-id.inc"
#include "actual-replay-begin.inc"
#include "actual-playback-restore.inc"
#include "actual-playback-load.inc"
static unsigned passed=0,failed=0;
static void check(bool ok,const char* label){std::cout<<(ok?"PASS ":"FAIL ")<<label<<'\n';if(ok)++passed;else++failed;}
static void need(bool ok,const char* label){if(!ok)throw std::runtime_error(std::string(label)+": "+operationError);}
static std::vector<std::uint8_t> file(const char* name){std::ifstream f(name,std::ios::binary);return{std::istreambuf_iterator<char>(f),std::istreambuf_iterator<char>()};}
static std::uint32_t word(const std::vector<std::uint8_t>& b,unsigned at){return th128::record::NativeReplaySlotPreview::word(b,at);}
static void put(std::vector<std::uint8_t>& b,unsigned at,std::uint32_t v){for(unsigned n=0;n<4;++n)b[at+n]=std::uint8_t(v>>(n*8));}
int main(int argc,char** argv){try{
 if(argc!=3)throw std::runtime_error("front and original header required");
 const auto front=file(argv[1]),header=file(argv[2]);need(front.size()==1993048&&header.size()==144,"fixed inputs");
 need(perfectHud->load(front,operationError)&&perfectHud->boot(0,operationError),"realPerfectHud");
 // Exact production ordinary begin consumes the live counter, no new ABI.
 need(th128_world_replay_begin(34772,0)==1,"actual ordinary writer begin");
 need(normalReplay->append({0,0,0},false,60,1./60),"synthetic component input append");
 auto written=normalReplay->snapshot(1,0,1,0,{});
 check(word(written,148+0x28)==2529,"writer preserves live graze at byte0x28");
 check(grazeGlobals.grazeCount==2529&&grazeGlobals.powerRaw==10000&&world->random.calls==0,"writer does not mutate graze power RNG");
 normalReplay.reset();worldStartedName="st_a1_2.ecl";worldStartDifficulty=shotDifficulty=3;world->random={41297,0};
 playerRaw={std::bit_cast<int>(word(header,24)),std::bit_cast<int>(word(header,28))};
 cumulativeFrozenArea=std::bit_cast<float>(word(header,136));grazeGlobals={0,10000};
 need(th128_world_replay_playback_restore(header.data(),unsigned(header.size()),3)==1,"restore exact originalheader");
 check(grazeGlobals.grazeCount==2529,"restore original header graze2529");
 check(grazeGlobals.powerRaw==10000&&world->random.seed==41297&&world->random.calls==0&&!normalRecord&&!normalReplay,"restore preserves power RNG no record");
 auto packet=header;put(packet,4,1);put(packet,8,6);packet.resize(150);
 grazeGlobals.grazeCount=2530;
 check(th128_world_replay_playback_load(packet.data(),unsigned(packet.size()),3)==0,"bind rejects different live graze");
 replayPlayback.cancel();
 grazeGlobals.grazeCount=2529;
 need(th128_world_replay_playback_load(packet.data(),unsigned(packet.size()),3)==1,"bind exact graze");
 check(replayPlayback.active()&&replayPlayback.consumed()==0,"binding consumes no input");
 replayPlayback.cancel();
 for(const auto value:{0u,99999999u,1000000000u}){auto b=header;put(b,40,value);grazeGlobals.grazeCount=7;
  const auto result=th128_world_replay_playback_restore(b.data(),unsigned(b.size()),3);
  check(result==1&&grazeGlobals.grazeCount==int(value),"supported graze range restored exactly");
 }
 for(const auto value:{0xffffffffu,1000000001u}){auto b=header;put(b,40,value);grazeGlobals.grazeCount=9;scoreRaw=71;motivationRaw=99;playerWindowControl.perfectCharge4b4d70=0;
  const auto result=th128_world_replay_playback_restore(b.data(),unsigned(b.size()),3);
  check(result==0&&grazeGlobals.grazeCount==9&&scoreRaw==71&&motivationRaw==99&&playerWindowControl.perfectCharge4b4d70==0,"invalid graze rejects before scalar writes");
 }
 for(const auto value:{-1,1000000001}){worldStartedName="st_a1_1.ecl";worldStartDifficulty=shotDifficulty=0;world->random={34772,0};cumulativeFrozenArea=0;normalReplay.reset();grazeGlobals.grazeCount=value;
  check(th128_world_replay_begin(34772,0)==0,"writer rejects unsupported live graze");
 }
 std::cout<<"{\"passed\":"<<(failed?"false":"true")<<",\"checks\":"<<passed+failed<<",\"acceptedAssertions\":"<<passed<<",\"failedAssertions\":"<<failed<<",\"actualProducerRestoreBind\":true,\"actualPerfectHud\":true,\"syntheticWorldCarryInput\":true,\"WorldStarts\":0,\"normalPermits\":0}\n";
 return failed?1:0;
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
