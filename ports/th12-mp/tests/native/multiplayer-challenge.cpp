#include "game/Multiplayer.hpp"
#include "game/GameState.hpp"
#include "game/GameSession.hpp"
#include "game/PlayerSystem.hpp"
#include "game/StageFlow.hpp"
#include "multiplayer/WorldState.hpp"
#include "multiplayer/StageEntry.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
using namespace th12;namespace mp=th12::multiplayer;
static void check(bool ok,const char* why){if(!ok){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
static void miss(){
  for(unsigned seat=0;seat<mp::options.count;++seat){auto& p=mp::pilots[seat];p.playerState=4;p.playerStateTicks=int(mp::deathbombFrames());p.playerStateTime=float(p.playerStateTicks);}
  const auto activeDrops=[](){return std::count_if(itemManager.entities.begin(),itemManager.entities.end(),[](const auto& item){return item.active();});};
  const auto drops=activeDrops();
  for(unsigned frame=0;frame<108;++frame){mp::tickPlayers();mp::tickRules();}
  check(activeDrops()==drops+7*mp::options.count,"challenge retains native death Power drops");
  for(const auto& item:itemManager.entities)check(item.type!=8,"zero-stock challenge death does not use final-life Full Power drop");
}
int main(){
  for(unsigned count:{2u,3u})for(unsigned group:{0u,3u}){
    selectStage(1);mp::Options o;o.count=count;o.challenge=true;o.selections={int(group),int(group+1),int(group+2)};check(mp::begin(o),"challenge begin");
    for(unsigned seat=0;seat<count;++seat){auto& p=mp::pilots[seat];p.resources.lives=0;p.resources.power=400;p.input.held=2;mp::Scope owner(seat);check(!startPlayerBomb()&&!p.playerBomb.active,"all six Bomb starts blocked");}
    for(unsigned death=1;death<=12;++death){miss();for(unsigned seat=0;seat<count;++seat){const auto& p=mp::pilots[seat];check(p.misses==death&&p.resources.lives==0&&p.playerState==1&&!p.ghost,"zero-stock challenge deaths respawn and count once past nine");}check(!mp::finished&&!mp::wipeFrames,"simultaneous challenge deaths never wipe");}
    mp::WorldState saved;const auto before=mp::canonicalHash();miss();saved.restore();check(mp::canonicalHash()==before&&mp::pilots[0].misses==12,"rollback restores challenge Miss and stock");
    for(unsigned i=0;i<4;++i)mp::collectSharedPiece(4);
    for(unsigned seat=0;seat<count;++seat)check(mp::pilots[seat].resources.lives==1&&mp::pilots[seat].misses==12,"hidden team Extend does not alter Miss");
    for(unsigned i=0;i<4;++i)mp::collectSharedPiece(5);
    for(unsigned seat=0;seat<count;++seat)check(!mp::pilots[seat].resources.bombs&&!mp::pilots[seat].resources.bombFragments,"Bomb fragments stay disabled");
    itemManager.reset();for(unsigned seat=0;seat<count;++seat){auto& p=mp::pilots[seat];p.px=0;p.py=400*128;p.input.held=0;}
    mp::pilots[0].resources.lives=2;
    for(unsigned tap=0;tap<5;++tap){mp::pilots[0].input.held=8;mp::tickRules();mp::pilots[0].input.held=0;mp::tickRules();}
    const auto gift=std::find_if(itemManager.entities.begin(),itemManager.entities.end(),[](const auto& item){return item.active()&&item.gift;});
    check(gift!=itemManager.entities.end()&&gift->type==6&&gift->recipient==1&&mp::pilots[0].resources.lives==1,"challenge life gift consumes hidden donor stock");tickItems(false);
    for(unsigned seat=0;seat<count;++seat)check(mp::pilots[seat].resources.lives==(seat==1?2:1)&&mp::pilots[seat].misses==12,"addressed life gift preserves every Miss counter");
    for(unsigned seat=0;seat<count;++seat)mp::pilots[seat].misses+=seat;
    mp::pilots[0].resources.lives=0;
    fullGameFlow.carry=captureSessionCarry();fullGameFlow.status=FullGameStatus::NextStage;fullGameFlow.nextStage=2;selectStage(2);check(startNextStage(),"challenge actual stage transition");
    for(unsigned seat=0;seat<count;++seat)check(mp::pilots[seat].misses==12+seat&&mp::pilots[seat].resources.lives==(seat==0?0:seat==1?2:1),"stage transition retains each hidden stock and Miss");
    mp::StageEntry entry;check(mp::decodeStageEntry(mp::encodeStageEntry(),entry)&&mp::restoreStageEntry(entry),"challenge chapter restore");
    for(unsigned seat=0;seat<count;++seat)check(mp::pilots[seat].misses==12+seat,"chapter restore retains cumulative Miss");
    mp::pilots[0].misses=0xffffffffu;mp::recordStageEntry(mp::options.seed,nullptr);check(mp::decodeStageEntry(mp::encodeStageEntry(),entry)&&mp::restoreStageEntry(entry)&&mp::pilots[0].misses==0xffffffffu,"chapter codec retains full unsigned Miss range");mp::pilots[0].playerState=4;mp::pilots[0].playerStateTicks=int(mp::deathbombFrames());mp::tickPlayers();check(mp::pilots[0].misses==0xffffffffu,"Miss saturates");
    selectStage(1);check(mp::begin(o)&&mp::pilots[0].misses==0,"restart clears Miss");
  }
  mp::options.challenge=false;selectStage(1);mp::Options o;o.selections={0,1,2};check(mp::begin(o),"ordinary begin");mp::pilots[0].resources.lives=0;mp::pilots[0].playerState=4;mp::pilots[0].playerStateTicks=int(mp::deathbombFrames());for(unsigned i=0;i<55;++i)mp::tickPlayers();check(mp::pilots[0].ghost&&mp::pilots[0].resources.lives<0,"ordinary final death still becomes ghost");
  std::puts("PASS TH12 challenge: six loadouts, no Bomb/deathbomb, >9 zero-stock deaths, hidden stock/Extends, stage entry, rollback, restart, saturation and ordinary control");
}
