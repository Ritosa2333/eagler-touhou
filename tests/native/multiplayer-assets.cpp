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
std::array<mp::Input,3> input(unsigned frame){
  std::array<mp::Input,3> out{};
  for(unsigned s=0;s<mp::options.count;++s)out[s].held=1u|8u|((frame/80+s)%2?64u:128u)|((frame%240==s*40)?2u:0u);
  return out;
}
void advance(unsigned start,unsigned end){for(unsigned f=start;f<end;++f)require(mp::update(input(f)),"retail world tick "+std::to_string(f));}
int main(int argc,char**argv){
  require(argc==2,"pass extracted TH12 assets directory");assets=argv[1];
  for(int stage:{1,7})for(int first=0;first<6;++first){
    load(stage);mp::Options o;o.stage=stage;o.difficulty=stage==7?4:1;o.seed=12345;o.count=first%2?2:3;o.selections={first,(first+2)%6,(first+4)%6};
    require(mp::begin(o),"retail start");const auto initial=mp::canonicalHash();mp::StageEntry entry;
    require(mp::decodeStageEntry(mp::encodeStageEntry(),entry)&&mp::restoreStageEntry(entry),"retail checkpoint decode/restore");
    require(mp::canonicalHash()==initial,"retail birth checkpoint hash");
    for(auto& p:mp::pilots){p.resources.power=400;p.resources.bombs=9;p.invuln=100000;p.playerInvulnerabilityTime=100000;}
    advance(0,600);mp::WorldState checkpoint;const auto before=mp::canonicalHash();
    advance(600,780);const auto after=mp::canonicalHash();const auto rng=th12::random;
    checkpoint.restore();require(mp::canonicalHash()==before,"retail animation/world restore");
    advance(600,780);require(mp::canonicalHash()==after&&th12::random.seed==rng.seed&&th12::random.calls==rng.calls,"retail rollback RNG/hash");
    require(th12_draw()>0&&th12_anm_draw()>0,"retail draw records populated");
    std::printf("retail stage=%d seats=%u first=%d frames=780 hash=%08x ANM=%d\n",stage,o.count,first,after,th12_anm_draw());
  }
  {
    load(7);mp::Options setup;setup.count=3;setup.stage=7;setup.difficulty=4;setup.selections={0,3,5};
    require(mp::begin(setup),"checkpoint reuse start");
    for(auto& p:mp::pilots){p.resources.power=400;p.invuln=100000;p.playerInvulnerabilityTime=100000;}
    advance(0,120);const auto hash=mp::canonicalHash();const auto rng=th12::random;
    auto owner=std::make_unique<mp::WorldState>();const auto retained=*owner;owner.reset();
    mp::WorldState::clearPool();
    for(unsigned f=120;f<200;++f){
      require(mp::update(input(f)),"checkpoint reuse actual world tick");
      const auto current=mp::canonicalHash();mp::WorldState scratch;
      require(mp::update(input(f+1)),"checkpoint reuse divergent future");
      scratch.restore();require(mp::canonicalHash()==current,"reused checkpoint restores changing animation graph and inputs");
    }
    retained.restore();require(mp::canonicalHash()==hash&&th12::random.seed==rng.seed&&th12::random.calls==rng.calls,
      "retained shared checkpoint survives repeated buffer reuse and cache clearing");
    mp::WorldState::clearPool();
    retained.restore();require(mp::canonicalHash()==hash,"clearing unused buffers cannot invalidate a live checkpoint");
  }
  load(1);mp::Options o;o.count=3;o.selections={0,3,5};require(mp::begin(o),"shared UFO start");
  for(unsigned s=0;s<3;++s){
    auto& p=mp::pilots[s];p.playerState=1;p.py=200*128;
    auto* token=itemManager.spawn(10,{p.px/128.f,p.py/128.f});require(token!=nullptr,"shared token spawn");token->velocity={};
    tickItems(false);require(ufoManager.inventory.count==int(s+1)&&ufoManager.inventory.colors[s]==1,"different seats collect into one UFO inventory");
  }
  tickUfo();require(ufoManager.enemyId&&ufoManager.kind==1,"shared red UFO summoned from three seats");
  const auto sharedUfo=ufoManager.enemyId;mp::WorldState ufoCheckpoint;
  advance(0,120);const auto ufoHash=mp::canonicalHash();const auto ufoRng=th12::random;
  ufoCheckpoint.restore();require(ufoManager.enemyId==sharedUfo,"shared UFO association restored");
  advance(0,120);require(mp::canonicalHash()==ufoHash&&th12::random.seed==ufoRng.seed&&th12::random.calls==ufoRng.calls,"shared UFO rollback RNG/hash");
  load(1);require(mp::begin(o),"cross-stage start");
  for(int stage=2;stage<=6;++stage){
    mp::pilots[0].misses=7;mp::pilots[1].resources.power=278;ufoManager.inventory.clearSlots();ufoManager.inventory.append(3);
    for(unsigned seat=0;seat<o.count;++seat){auto& p=mp::pilots[seat];p.px=-12000+int(seat)*7000;p.py=17000+int(seat)*6000;p.playerFocused=seat!=1;}
    fullGameFlow.carry=captureSessionCarry();fullGameFlow.status=FullGameStatus::NextStage;fullGameFlow.nextStage=stage;
    load(stage);require(startNextStage(),"retail next-stage start");require(mp::pilots[0].misses==7&&mp::pilots[1].resources.power==278,"all seats carry resources");
    for(unsigned seat=0;seat<o.count;++seat){const auto& p=mp::pilots[seat];require(p.px==-12000+int(seat)*7000&&p.py==17000+int(seat)*6000&&p.playerFocused==(seat!=1),"each seat retains position/focus across stages");}
    const auto hash=mp::canonicalHash();mp::StageEntry entry;require(mp::decodeStageEntry(mp::encodeStageEntry(),entry)&&mp::restoreStageEntry(entry),"retail cross-stage restore");
    require(mp::canonicalHash()==hash&&ufoManager.inventory.count==1,"retail cross-stage RNG/UFO hash");
    advance(0,120);
  }
  std::puts("TH12 multiplayer retail assets: six loadouts, Normal/Extra, rollback and all stage transitions passed");
}
