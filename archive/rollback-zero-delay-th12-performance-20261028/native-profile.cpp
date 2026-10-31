#include "game/GameState.hpp"
#include "game/GameSession.hpp"
#include "game/StageFlow.hpp"
#include "multiplayer/WorldState.hpp"
#include "multiplayer/StageEntry.hpp"
#include "wasm/Api.hpp"
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include "game/Practice.hpp"
#include <chrono>
#include <algorithm>
#include <optional>
using namespace th12;
namespace mp=th12::multiplayer;
namespace fs=std::filesystem;
void require(bool ok,const std::string& why){if(!ok){std::fprintf(stderr,"FAIL: %s\n",why.c_str());std::exit(1);}}
fs::path assets;
auto bytes(const std::string& name){std::ifstream f(assets/name,std::ios::binary);require(bool(f),"missing "+name);return std::vector<uint8_t>(std::istreambuf_iterator<char>(f),{});}
void load(int stage){
  th12_select_stage(stage);
  const auto suffix=std::string(stage<10?"0":"")+std::to_string(stage);
  for(const auto& name:std::vector<std::string>{"default.ecl","stage"+suffix+".ecl"}){auto b=bytes(name);require(th12_load_ecl(b.data(),int(b.size())),name);}
  if(stage==7){auto b=bytes("stage07boss.ecl");require(th12_load_ecl(b.data(),int(b.size())),"Extra ECL");}
  {auto b=bytes("stage"+suffix+".std");require(th12_load_std(b.data(),int(b.size())),"STD");}
  for(int i=0;i<6;++i){
    const auto shot=std::to_string(i/2)+(i%2?"b":"a");
    auto b=bytes("pl0"+shot+".sht");require(th12_load_sht(i,b.data(),int(b.size())),"SHT");
    b=bytes("st"+suffix+"_0"+shot+".msg");require(th12_load_msg(i,b.data(),int(b.size())),"MSG");
  }
  for(const auto& [slot,name]:std::vector<std::pair<int,std::string>>{{0,"bullet"},{1,"enemy"},{2,"stgenm"+suffix},{4,"pl00"},{5,"pl01"},{6,"pl02"},{7,"front"},{8,"text"},{11,"ascii"},{9,"st"+suffix+"logo"},{10,"stage"+suffix}}){
    auto b=bytes(name+".anm");require(th12_load_anm(slot,b.data(),int(b.size())),name);
  }
  const auto boss="stgenm"+suffix+"m.anm";
  if(fs::exists(assets/boss)){auto b=bytes(boss);require(th12_load_anm(3,b.data(),int(b.size())),boss);}else th12_unload_anm(3);
}

using Clock=std::chrono::steady_clock;
double elapsed(Clock::time_point start){return std::chrono::duration<double,std::milli>(Clock::now()-start).count();}
void stats(const char* name,std::vector<double> values){std::sort(values.begin(),values.end());double sum=0;for(auto x:values)sum+=x;std::printf("%s mean=%.4f p95=%.4f max=%.4f ",name,sum/values.size(),values[values.size()*95/100],values.back());}
int main(int argc,char**argv){
 require(argc==2,"asset directory required");assets=argv[1];
 uint32_t exactHashes[4]{};
 for(unsigned count:{2u,3u})for(bool rollback:{false,true}){
  mp::active=false;load(6);practice::setEnabled(true);
  int32_t params[]{1,1,5,45,0,0,0,0,9,0,9,0,400,10000,0,0,0,0,0,0};
  require(practice::configure(params,20)&&practice::prepareReplay(0),"actual stage6 THPrac Boss selection");
  practice::setTools(1|256|4);mp::Options options;options.count=count;options.stage=6;options.difficulty=0;options.seed=0x1234;options.selections={5,5,5};
  require(mp::begin(options),"actual MP world start");
  std::array<std::optional<mp::WorldState>,13> history;
  std::vector<double> tickTimes,checkpointTimes,restoreTimes,drawTimes;size_t maxBullets=0,maxNodes=0;
  for(unsigned frame=0;frame<1200;++frame){
   for(auto& p:mp::pilots){p.resources.power=400;p.resources.bombs=9;p.invuln=100000;p.playerInvulnerabilityTime=100000;}
   auto start=Clock::now();
   if(rollback){auto& checkpoint=history[frame%history.size()];checkpoint.reset();checkpoint.emplace();if(frame>=120)checkpointTimes.push_back(elapsed(start));
    if(frame%120==119){const auto hash=mp::canonicalHash();start=Clock::now();checkpoint->restore();restoreTimes.push_back(elapsed(start));require(hash==mp::canonicalHash(),"checkpoint restore hash");}}
   std::array<mp::Input,3> inputs{};for(unsigned seat=0;seat<count;++seat)inputs[seat].held=1|8;
   start=Clock::now();require(mp::update(inputs),"actual MP tick");if(frame>=120)tickTimes.push_back(elapsed(start));
   start=Clock::now();th12_anm_draw();th12_mesh_draw();th12_draw();if(frame>=120)drawTimes.push_back(elapsed(start));
   maxBullets=std::max(maxBullets,bullets.size());maxNodes=std::max(maxNodes,animationScene.nodes().size());
  }
  if(rollback)require(mp::canonicalHash()==exactHashes[count],"snapshot profiling preserves the exact final MP world");else exactHashes[count]=mp::canonicalHash();
  std::printf("seats=%u rollback=%u bullets=%zu nodes=%zu ",count,unsigned(rollback),maxBullets,maxNodes);stats("tick",tickTimes);stats("drawExport",drawTimes);if(rollback){stats("checkpoint",checkpointTimes);stats("restore",restoreTimes);}std::puts("");
  for(auto& checkpoint:history)checkpoint.reset();mp::WorldState::clearPool();
 }
}
