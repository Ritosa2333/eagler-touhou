#include "game/Multiplayer.hpp"
#include "game/GameState.hpp"
#include "game/GameSession.hpp"
#include "game/StageFlow.hpp"
#include "game/PresentationMotion.hpp"
#include "game/PresentationTracking.hpp"
#include "multiplayer/WorldState.hpp"
#include "multiplayer/StageEntry.hpp"
#include "multiplayer/InputLanes.hpp"
#include "multiplayer/ReplayArchive.hpp"
#include "multiplayer/Session.hpp"
#include "wasm/Api.hpp"
#include <cstdio>
#include <cstdlib>
using namespace th12;
namespace mp=th12::multiplayer;
void check(bool ok,const char* name){if(!ok){std::fprintf(stderr,"FAIL: %s\n",name);std::exit(1);}}
void begin(unsigned count=3){selectStage(1);mp::Options o;o.count=count;o.selections={0,3,5};check(mp::begin(o),"begin");}
void rules(unsigned held){mp::pilots[0].input.held=held;mp::tickRules();}
void homingItems(unsigned count) {
  for(int type:{9,16,17,18}) {
    begin(count);itemManager.reset();
    for(unsigned s=0;s<count;++s){auto& p=mp::pilots[s];p.px=int(s)*80*128;p.py=340*128;p.playerState=1;p.resources.power=s?100:400;}
    item_system::SpawnRequest request;request.type=type;request.position={0,100};request.speed=0;
    request.bucketA=4;request.bucketB=3;request.multiplier=2;
    auto* item=itemManager.spawn(request);check(item!=nullptr,"spawn automatic item");
    const unsigned expected=type==16||type==18?1:0;
    const auto calls=th12::random.calls;tickItems(false);
    check(item->recipient==int(expected),"automatic drops bind to a real recipient below the collection line");
    if(type>=16)check(item->state==item_system::State::Aggregate&&item->aggregateSpeed==0,"UFO reward retains its original initial delay");
    bool collected=false;
    for(unsigned f=0;f<240&&item->active();++f){
      tickItems(false);
      for(const auto& e:itemManager.events)if(e.kind==item_system::EventKind::CollectOrdinary||e.kind==item_system::EventKind::CollectAggregate){
        check(e.entity.collector==int(expected),"automatic item reaches its assigned collector");collected=true;
      }
      check(std::abs(item->position.x)<=192&&item->position.y<450,"automatic drop stays inside the game instead of seeking an offscreen sentinel");
    }
    check(collected&&!item->active(),"automatic item is actually collected");
    if(type==16||type==18)check(mp::pilots[1].resources.power==100+4*int(count)&&mp::pilots[0].resources.power==400,"UFO power aggregate awards the needy player only");
    check(th12::random.calls==calls,"recipient selection does not consume gameplay RNG");
  }
  begin(count);itemManager.reset();
  for(unsigned s=0;s<count;++s){auto& p=mp::pilots[s];p.px=int(s)*80*128;p.py=340*128;p.playerState=1;p.resources.power=100;}
  auto* item=itemManager.spawn(17,{0,100});tickItems(false);
  check(item&&item->recipient==0,"point aggregate initially targets the nearest player");
  {mp::Scope seat(0);mp::becameGhost();}
  tickItems(false);
  check(item->recipient==1&&item->state==item_system::State::Aggregate,"automatic aggregate retargets a surviving player without becoming a falling item");
  for(unsigned s=0;s<count;++s)mp::pilots[s].playerState=2;
  const auto position=item->position;
  for(unsigned f=0;f<45;++f)tickItems(false);
  check(item->active()&&item->recipient==-1&&item->position.x==position.x&&item->position.y==position.y,
    "homing reward waits in place while every pilot respawns");
  mp::pilots[1].playerState=1;tickItems(false);
  check(item->recipient==1,"waiting reward reacquires a real recipient after respawn");
}
void sharedStock(unsigned count) {
  begin(count);
  for(unsigned s=0;s<count;++s) {
    auto& p=mp::pilots[s];p.px=int(s)*100*128;p.py=300*128;p.playerState=1;
    check(p.resources.bombs==2,"TH12 retains original two initial bombs");
  }
  const auto collect=[&](int type,int seat=0,bool gift=false) {
    itemManager.reset();item_system::SpawnRequest request;
    request.type=type;request.position={float(seat*100),300};request.speed=0;
    auto* item=itemManager.spawn(request);check(item!=nullptr,"spawn shared stock pickup");
    item->gift=gift;if(gift)item->recipient=seat;
    tickItems(false);check(!item->active(),"stock pickup is collected through the world");
  };
  for(int fragment:{2,3,4,0}) {
    collect(4,1);
    for(unsigned s=0;s<count;++s)check(mp::pilots[s].resources.lifeFragments==fragment&&
      mp::pilots[s].resources.lives==(fragment?2:3),"one life fragment advances every seat by the original amount");
  }
  for(int fragment:{2,4,0}) {
    collect(5);
    for(unsigned s=0;s<count;++s)check(mp::pilots[s].resources.bombFragments==fragment&&
      mp::pilots[s].resources.bombs==(fragment?2:3),"one bomb fragment advances every seat by the original amount");
  }
  collect(6,1);collect(7);
  for(unsigned s=0;s<count;++s)check(mp::pilots[s].resources.lives==4&&mp::pilots[s].resources.bombs==4,
    "one natural life and bomb award one stock to all seats");
  collect(6,1,true);
  for(unsigned s=0;s<count;++s)check(mp::pilots[s].resources.lives==(s==1?5:4),"donated life remains exclusive to the recipient");
  mp::pilots[1].resources.lives=8;mp::pilots[1].resources.bombs=9;
  collect(6);collect(7);
  for(unsigned s=0;s<count;++s)check(mp::pilots[s].resources.lives==(s==1?8:5)&&
    mp::pilots[s].resources.bombs==(s==1?9:5),"each seat independently retains the original stock caps");
  itemManager.reset();Enemy enemy;enemy.drop=6;enemy.dropCounts[6]=1;
  const auto calls=th12::random.calls;dropItems(enemy);
  int lives=0,bombs=0;
  for(const auto& item:itemManager.entities)if(item.active()){lives+=item.type==6;bombs+=item.type==7;}
  check(lives==1&&bombs==1&&itemManager.activeCount()==2,"natural whole-stock drops never multiply with room seats");
  check(th12::random.calls-calls==6,"single stock drops preserve original RNG consumption");
  for(int stock:{0,1,3}) {
    auto& p=mp::pilots[0];p.playerState=2;p.playerStateTicks=30;p.playerStateTime=30;
    p.resources.lives=1;p.resources.bombs=stock;p.resources.bombFragments=2;
    tickPlayer(0,0);
    check(p.resources.bombs==std::max(2,stock)&&p.resources.bombFragments==2,"miss restores original two bombs without losing fragments or surplus");
  }
  mp::pilots[0].resources.bombs=0;mp::revive(0,true);
  check(mp::pilots[0].resources.bombs==2,"rescue restores original bomb stock");
  mp::pilots[0].resources.bombs=4;mp::revive(0,true);
  check(mp::pilots[0].resources.bombs==4,"rescue retains earned surplus bombs");
}
void powerItems(unsigned count) {
  begin(count);
  for (unsigned s=0;s<count;++s) {
    auto& p=mp::pilots[s];p.px=int(s)*100*128;p.py=300*128;p.playerState=1;
  }
  auto& resources=mp::pilots[0].resources;
  const auto collect=[&](int type,int power,int bucket=0) {
    itemManager.reset();resources.power=power;
    for(unsigned s=1;s<count;++s)mp::pilots[s].resources.power=400;
    item_system::SpawnRequest request;request.type=type;request.position={0,300};request.speed=0;
    request.bucketA=bucket;
    auto* item=itemManager.spawn(request);check(item!=nullptr,"spawn power test item");
    item->recipient=0;
    tickItems(false);
    check(!item->active(),"world actually collects power item");
  };
  collect(1,100);check(resources.power==100+int(count),"small P awards 0.02/0.03");
  const int rank=resources.rank;
  collect(1,199);check(resources.power==199+int(count)&&resources.rank==rank+12,"small P crosses power tier once");
  collect(1,399);check(resources.power==400,"small P clamps at max power");
  const int score=resources.scoreStored;
  collect(1,400);check(resources.power==400&&resources.scoreStored==score+1,"full power retains original small P score");
  collect(3,100);check(resources.power==200,"large P still awards one tier");
  collect(8,100);check(resources.power==400,"full power item retains original award");
  collect(16,100,4);check(resources.power==100+4*int(count),"UFO aggregate values each small P consistently");
  mp::pilots[count-1].ghost=true;
  collect(1,100);check(resources.power==100+int(count),"small P value uses room seats even with a ghost");
  itemManager.reset();
  Enemy enemy;enemy.x=12;enemy.y=120;enemy.drop=1;enemy.dropCounts[0]=2;enemy.dropCounts[2]=2;
  const auto calls=th12::random.calls;
  dropItems(enemy);
  int small=0,large=0;
  for(const auto& item:itemManager.entities)if(item.active()){small+=item.type==1;large+=item.type==3;}
  check(small==3&&large==2&&itemManager.activeCount()==5,"enemy P drop counts do not multiply by room seats");
  // One initial angle and two floats per counted drop; each float draws two words.
  check(th12::random.calls-calls==18,"enemy drops preserve original RNG draw count");
}
void exclusiveBombs(unsigned count) {
  begin(count);
  auto enemy=std::make_unique<Enemy>();enemy->active=true;enemies.push_back(std::move(enemy));
  for(unsigned s=0;s<count;++s){auto& p=mp::pilots[s];p.playerState=1;p.playerStateTicks=100;p.playerStateTime=100;}
  // Seat order is shared by every peer, including simultaneous/deathbomb inputs.
  for(unsigned s=0;s<count;++s){mp::Scope owner(s);tickPlayer(2,2);}
  check(mp::pilots[0].playerBomb.active&&mp::pilots[0].resources.bombs==1,"first simultaneous Bomb starts and spends one stock");
  for(unsigned s=1;s<count;++s)check(!mp::pilots[s].playerBomb.active&&mp::pilots[s].resources.bombs==2,"overlapping Bombs preserve other players' stock");
  mp::WorldState checkpoint;
  {mp::Scope owner(1);mp::pilot().playerState=4;mp::pilot().playerStateTicks=0;mp::pilot().playerStateTime=0;tickPlayer(2,2);}
  check(!mp::pilots[1].playerBomb.active&&mp::pilots[1].resources.bombs==2,"deathbomb does not bypass the active team Bomb");
  mp::pilots[0].playerBomb.reset();
  {mp::Scope owner(1);mp::pilot().playerState=1;tickPlayer(2,2);}
  check(mp::pilots[1].playerBomb.active&&mp::pilots[1].resources.bombs==1,"next player can Bomb after the previous Bomb ends");
  checkpoint.restore();
  {mp::Scope owner(1);tickPlayer(2,2);}
  check(mp::pilots[0].playerBomb.active&&!mp::pilots[1].playerBomb.active&&mp::pilots[1].resources.bombs==2,"rollback restores the team Bomb exclusion");
}
int main(){
  for(unsigned count:{2u,3u})for(unsigned local=0;local<count;++local){
    begin(count);mp::options.local=local;phasePlayerCounters[0]=7;
    check(th12_player_instance()==0,"local touch identity starts independently of shared phase counters");
    auto& other=mp::pilots[(local+1)%count];other.playerState=4;other.playerStateTicks=int(mp::deathbombFrames());mp::tickPlayers();
    check(other.misses==1,"other pilot completes native death window");
    check(th12_player_instance()==0,"another pilot's death does not invalidate local touch");
    phasePlayerCounters[0]=0;
    check(th12_player_instance()==0,"ECL spell boundary reset does not invalidate local touch");
    mp::WorldState checkpoint;auto& own=mp::pilots[local];own.playerState=4;own.playerStateTicks=int(mp::deathbombFrames());mp::tickPlayers();
    check(th12_player_instance()==1,"local death advances only local touch identity");
    checkpoint.restore();check(th12_player_instance()==0,"rollback restores local touch identity");
  }
  exclusiveBombs(2);exclusiveBombs(3);
  {
    begin();
    auto enemy=std::make_unique<Enemy>();enemy->active=true;enemy->id=1234;enemies.push_back(std::move(enemy));
    presentation::ownerRevisions[1234]=7;
    for(unsigned id=1;id<10000;++id)presentation::ownerRevisions[0x10000000u+id]=id;
    auto* item=itemManager.spawn(1,{0,100});check(item!=nullptr,"revision fixture item");
    presentation::ownerRevisions[0x20000000u+item->id]=8;
    const auto before=mp::canonicalHash(),calls=th12::random.calls;
    mp::WorldState retained;
    collectPresentationRevisions();
    check(presentation::ownerRevisions.size()==2&&presentation::revision(1234)==7&&presentation::revision(0x20000000u+item->id)==8,"revision collection retains live owners and removes retired shots");
    check(mp::canonicalHash()==before&&th12::random.calls==calls,"revision collection does not change gameplay or RNG");
    retained.restore();check(presentation::ownerRevisions.size()==10001,"retained rollback checkpoint owns its complete revision table");
    collectPresentationRevisions();check(presentation::ownerRevisions.size()==2,"restored revisions can be collected again");
  }
  {
    mp::SessionSetup setup;setup.version=mp::SessionSetup::Version;setup.session_id=123;setup.automatic=false;setup.build={1,2,3,4};
    for(unsigned mode:{0u,2u}){
      setup.mode=mode;check(setup.valid(),"native setup restores rollback modes");
      const auto words=mp::EncodeSessionSetup(setup);mp::SessionSetup decoded;
      check(mp::DecodeSessionSetup(decoded,words.data(),words.size())&&decoded.mode==mode,"native wire accepts restored rollback modes");
    }
    setup.mode=1;
    auto local=std::make_unique<mp::NetplayRuntime>(),remote=std::make_unique<mp::NetplayRuntime>();
    check(local->Reset(setup),"delay local setup");setup.local_player=1;check(remote->Reset(setup),"delay peer setup");
    check(local->ApplySession(remote->Hello())==Netplay::SessionPacketResult::Accepted&&
          remote->ApplySession(local->Hello())==Netplay::SessionPacketResult::Accepted,"delay Hello exchange");
    local->MarkLocalReady();remote->MarkLocalReady();
    check(local->ApplySession(remote->Ready())==Netplay::SessionPacketResult::Accepted,"delay Ready exchange");
    check(local->PredictionLimit()==0&&local->Capture(0,{}),"delay captures actual local input with zero prediction");
    check(!local->Prepare(0).canAdvance&&local->NextFrame()==0,"missing remote input stalls frame zero");
    Netplay::FrameInput actual;actual.buttons=64;
    check(local->SubmitRemote(1,0,actual)==Netplay::RemoteInputResult::Accepted,"accept actual remote input");
    auto decision=local->Prepare(0);
    check(decision.canAdvance&&!decision.predictedMask&&decision.inputs[1]==actual&&local->MarkSimulated(0,decision),"advance only using exact peer input");
    check(local->Capture(1,{}),"capture second delay frame");
    check(!local->Prepare(1).canAdvance&&local->NextFrame()==1,"previous peer input is never predicted forward");
    local->SetCapturePhase(false,Netplay::ConfirmedInputClock::IntervalUs);
    check(local->NeedsCapture()&&local->Capture(2,{}),"combat keeps sampling real input at 60 Hz while a packet is in flight");
    check(!local->NeedsCapture(),"one wall-clock slot cannot sample a second future frame");
    check(!local->Prepare(1).canAdvance,"sampling ahead never permits predicted simulation");
    actual.buttons=128;
    check(local->SubmitRemote(1,1,actual)==Netplay::RemoteInputResult::Accepted,"accept changed remote input");
    decision=local->Prepare(1);
    check(decision.canAdvance&&!decision.predictedMask&&decision.inputs[1]==actual&&local->MarkSimulated(1,decision),"second frame uses newly received exact input");
  }
  {
    const auto run=[](bool late){
      selectStage(1);mp::SessionSetup setup;setup.version=mp::SessionSetup::Version;
      setup.mode=0;setup.automatic=false;setup.session_id=789;setup.build={1,2,3,4};
      const auto words=mp::EncodeSessionSetup(setup);
      auto session=std::make_unique<mp::Session>();auto peer=std::make_unique<mp::NetplayRuntime>();
      check(session->configure(words.data(),words.size()),"rollback session setup");
      setup.local_player=1;check(peer->Reset(setup),"rollback peer setup");
      check(session->network.ApplySession(peer->Hello())==Netplay::SessionPacketResult::Accepted&&
        peer->ApplySession(session->network.Hello())==Netplay::SessionPacketResult::Accepted,"rollback Hello exchange");
      session->network.MarkLocalReady();peer->MarkLocalReady();
      check(session->network.ApplySession(peer->Ready())==Netplay::SessionPacketResult::Accepted&&session->start(),"rollback session starts");
      check(session->network.SubmitRemote(1,0,{})==Netplay::RemoteInputResult::Accepted&&session->step(0,false,0,0,0,0),"rollback exact frame zero");
      Netplay::FrameInput right;right.buttons=128;
      if(!late)check(session->network.SubmitRemote(1,1,right)==Netplay::RemoteInputResult::Accepted,"on-time actual direction");
      check(session->step(0,false,0,0,0,16.667)&&session->network.NextFrame()==2,"rollback advances before a remote frame arrives");
      if(late)check(session->network.SubmitRemote(1,1,right)==Netplay::RemoteInputResult::RollbackRequired,"late changed direction requests a correction");
      check(session->network.SubmitRemote(1,2,{})==Netplay::RemoteInputResult::Accepted,"next actual input after the correction");
      check(session->step(0,false,0,0,0,33.334),"rollback correction preserves the current callback's fresh tick");
      if(late)check(session->restores()==1&&session->resimulated()>0,"rollback restores the actual world and replays input");
      // Compare at the same confirmed frame, before any future prediction.
      const auto hash=mp::canonicalHash();const auto rng=th12::random;
      check(session->checkpoints()==unsigned(late),"only a speculative frame allocates a rollback checkpoint");
      check(session->network.NextFrame()==3,"correction catches the old frontier and advances the new frame in the same callback");
      session->stop();return std::make_pair(hash,rng);
    };
    const auto corrected=run(true),exact=run(false);
    check(corrected.first==exact.first&&corrected.second.seed==exact.second.seed&&corrected.second.calls==exact.second.calls,
      "late-input correction matches the exact-input world and RNG");
  }
  {
    selectStage(1);
    mp::SessionSetup setup;setup.session_id=456;setup.automatic=false;setup.build={1,2,3,4};
    const auto words=mp::EncodeSessionSetup(setup);
    auto session=std::make_unique<mp::Session>();auto peer=std::make_unique<mp::NetplayRuntime>();
    check(session->configure(words.data(),words.size()),"streaming exact session setup");
    setup.local_player=1;check(peer->Reset(setup),"streaming exact peer setup");
    check(session->network.ApplySession(peer->Hello())==Netplay::SessionPacketResult::Accepted&&
          peer->ApplySession(session->network.Hello())==Netplay::SessionPacketResult::Accepted,"streaming Hello exchange");
    session->network.MarkLocalReady();peer->MarkLocalReady();
    check(session->network.ApplySession(peer->Ready())==Netplay::SessionPacketResult::Accepted&&session->start(),"streaming session starts");
    for(unsigned n=0;n<3;++n)check(session->step(64,false,0,0,0,n*16.667),"capture on the physical clock while remote is late");
    check(session->network.CaptureFrame()==3&&session->network.NextFrame()==0,"waiting records real samples but never simulates missing input");
    Netplay::FrameInput actual;actual.buttons=128;
    for(unsigned n=0;n<3;++n)check(session->network.SubmitRemote(1,n,actual)==Netplay::RemoteInputResult::Accepted,"receive delayed actual frame burst");
    check(session->step(64,false,0,0,0,50.001)&&session->network.NextFrame()==3,"one callback drains the confirmed backlog");
    check(session->network.NextFrame()<=session->network.CaptureFrame()&&session->network.PredictionLimit()==0,"catchup stays within real sampled time and has no prediction");
    session->stop();
  }
  {
    const auto run=[](bool late){
      selectStage(1);mp::SessionSetup setup;setup.version=mp::SessionSetup::Version;
      setup.mode=0;setup.automatic=false;setup.session_id=790;setup.build={1,2,3,4};
      const auto words=mp::EncodeSessionSetup(setup);
      auto session=std::make_unique<mp::Session>();auto peer=std::make_unique<mp::NetplayRuntime>();
      check(session->configure(words.data(),words.size()),"full-window rollback setup");
      setup.local_player=1;check(peer->Reset(setup),"full-window peer setup");
      check(session->network.ApplySession(peer->Hello())==Netplay::SessionPacketResult::Accepted&&
        peer->ApplySession(session->network.Hello())==Netplay::SessionPacketResult::Accepted,"full-window Hello exchange");
      session->network.MarkLocalReady();peer->MarkLocalReady();
      check(session->network.ApplySession(peer->Ready())==Netplay::SessionPacketResult::Accepted&&session->start(),"full-window session starts");
      const auto input=[](unsigned f){Netplay::FrameInput value;value.buttons=f?(f%2?128:64):0;return value;};
      for(unsigned f=0;f<13;++f){
        if(!late||!f)check(session->network.SubmitRemote(1,f,input(f))==Netplay::RemoteInputResult::Accepted,"full-window on-time input");
        check(session->step(0,false,0,0,0,f*16.667)&&session->network.NextFrame()==f+1,"every physical callback advances one fresh frame");
      }
      for(unsigned f=late?1:13;f<=13;++f){
        const auto result=session->network.SubmitRemote(1,f,input(f));
        check(result==Netplay::RemoteInputResult::Accepted||result==Netplay::RemoteInputResult::PredictionCorrect||result==Netplay::RemoteInputResult::RollbackRequired,"full-window actual input burst");
      }
      check(session->step(0,false,0,0,0,13*16.667)&&session->network.NextFrame()==14,"twelve replayed frames and one fresh tick fit the same callback");
      if(late)check(session->restores()==1&&session->resimulated()==12,"the complete rollback window is genuinely replayed");
      const auto result=mp::canonicalHash();session->stop();return result;
    };
    check(run(false)==run(true),"full-window correction preserves the on-time world without dropping its fresh tick");
  }
  powerItems(2);powerItems(3);sharedStock(2);sharedStock(3);homingItems(2);homingItems(3);
  for(unsigned count:{2u,3u}) {
    begin(count);
    Enemy ufo;ufo.id=102;ufo.isUfo=true;
    Enemy regular;regular.id=103;
    int dealt=0;for(int i=0;i<12;++i)dealt+=mp::bossDamage(ufo,1);
    check(dealt==int(count==2?9:8),"UFO uses the same fractional multiplayer damage as bosses");
    check(mp::bossDamage(regular,120)==120,"ordinary enemies retain original damage");
    Enemy boss;boss.id=104;boss.boss=true;
    mp::pilots[count-1].resources.lives=-1;
    const int viable=count==3?90:120;
    check(mp::bossDamage(ufo,120)==viable&&mp::bossDamage(boss,120)==viable,
      "Boss and UFO immediately use the surviving-pilot coefficient during the last death animation");
    mp::pilots[count-1].resources.lives=1;
    for(unsigned seat=1;seat<count;++seat)mp::pilots[seat].ghost=true;
    check(mp::bossDamage(ufo,120)==120,"UFO returns to original health with one operable pilot");
  }
  mp::Options o;check(o.valid(),"options");o.count=1;check(!o.valid(),"reject single player");
  begin();check(mp::pilots[0].character==0&&mp::pilots[1].character==1&&mp::pilots[2].character==2,"six loadouts");
  check(mp::pilots[0].px<mp::pilots[1].px&&mp::pilots[1].px<mp::pilots[2].px,"separate spawn positions");
  mp::pilots[0].resources.power=400;mp::pilots[1].resources.power=100;mp::pilots[2].resources.power=300;
  mp::pilots[0].px=0;mp::pilots[1].px=1280;mp::pilots[2].px=2560;
  for(auto& p:mp::pilots){p.py=64*128;p.playerState=1;p.invuln=0;}
  item_system::Entity item;item.type=1;item.position={0,64};item.state=item_system::State::Falling;
  item_system::PlayerView view;mp::itemPlayer(item,view);
  check(item.recipient==1,"full-power collector redirects to nearest needy seat");
  for(int i=0;i<5;++i){rules(1);rules(0);}
  check(mp::pilots[0].resources.power==300,"five taps donate exactly one power tier");
  auto gift=std::find_if(itemManager.entities.begin(),itemManager.entities.end(),[](const auto& e){return e.gift;});
  check(gift!=itemManager.entities.end()&&gift->type==3&&gift->recipient==1,"gift targets lowest relative power");
  {mp::Scope seat(1);mp::pilots[1].resources.lives=-1;mp::becameGhost();}
  for(int i=0;i<4;++i){
    rules(8);rules(8);rules(0);
    check(mp::pilots[1].ghost&&mp::pilots[0].resources.lives==2,
          "four Shift taps and held frames cannot donate a life");
  }
  rules(8);
  check(!mp::pilots[1].ghost&&mp::pilots[0].resources.lives==1,"rescue consumes donor life");
  rules(8);rules(0);
  check(mp::pilots[0].resources.lives==1,"holding fifth Shift tap cannot donate again");
  check(mp::deathbombFrames()==16,"double deathbomb window");
  Enemy boss;boss.id=101;boss.boss=true;int damage=0;for(int i=0;i<12;++i)damage+=mp::bossDamage(boss,1);
  check(damage==8,"three-player boss damage uses fractional remainder");
  ufoManager.inventory.append(1);ufoManager.inventory.append(2);
  {mp::Scope seat(2);mp::becameGhost();}
  check(ufoManager.inventory.count==2&&ufoManager.inventory.colors[0]==1&&ufoManager.inventory.colors[1]==2,"death preserves shared UFO inventory");
  mp::collectSharedPiece(4);mp::collectSharedPiece(4);mp::collectSharedPiece(4);mp::collectSharedPiece(4);
  check(mp::lifeFragments==0&&mp::pilots[0].resources.lives==2&&mp::pilots[1].resources.lives==1,"shared life fragments");
  mp::WorldState saved;const auto before=mp::canonicalHash();std::array<mp::Input,3> input{};
  input[0].held=64;input[1].held=128;input[2].held=8;
  for(int i=0;i<120;++i)check(mp::update(input),"world tick");
  const auto after=mp::canonicalHash();const auto seed=th12::random.seed;const auto calls=th12::random.calls;
  saved.restore();check(mp::canonicalHash()==before,"world snapshot restores all seats and inventory");
  for(int i=0;i<120;++i)check(mp::update(input),"resimulation");
  check(mp::canonicalHash()==after&&th12::random.seed==seed&&th12::random.calls==calls,"resimulation RNG/hash");
  const auto seatHash=mp::canonicalHash();mp::options.local=2;check(mp::canonicalHash()==seatHash,"hash independent of local seat");
  input={};input[0].pause=true;check(mp::update(input)&&mp::paused,"shared pause");
  input={};check(mp::update(input),"release pause");input[0].held=32;check(mp::update(input)&&mp::menuSelection==1,"pause navigation");
  input={};check(mp::update(input),"release navigation");input[0].held=mp::InputLanes::Confirm;
  check(mp::update(input)&&mp::menuConfirm&&mp::menuSelection==1&&!mp::finished,"original Yes/No defaults to No");
  mp::WorldState menuCheckpoint;const auto menuHash=mp::canonicalHash();
  input={};check(mp::update(input),"release confirm");input[0].held=mp::InputLanes::Confirm;
  check(mp::update(input)&&!mp::menuConfirm&&!mp::finished,"No returns to pause without retiring a peer");
  menuCheckpoint.restore();check(mp::canonicalHash()==menuHash&&mp::menuConfirm,"confirmation rollback state");
  input={};input[0].held=2;check(mp::update(input)&&!mp::menuConfirm,"Bomb cancels confirmation");
  input={};check(mp::update(input),"release cancel");input[0].pause=true;check(mp::update(input)&&!mp::paused,"Escape resumes shared world");
  input={};input[0].held=mp::InputLanes::Restart;check(mp::update(input)&&!mp::paused&&!mp::restartRequested&&!mp::finished,"R cannot restart multiplayer");
  begin();auto initial=mp::canonicalHash();mp::StageEntry entry;
  check(mp::decodeStageEntry(mp::encodeStageEntry(),entry),"stage entry schema");
  check(mp::restoreStageEntry(entry)&&mp::canonicalHash()==initial,"initial checkpoint reconstruction");
  ufoManager.inventory.append(3);mp::pilots[1].resources.power=278;
  fullGameFlow.carry=captureSessionCarry();fullGameFlow.status=FullGameStatus::NextStage;fullGameFlow.nextStage=2;
  selectStage(2);check(startNextStage(),"stage transition");
  initial=mp::canonicalHash();check(mp::decodeStageEntry(mp::encodeStageEntry(),entry)&&mp::restoreStageEntry(entry),"next-stage schema");
  check(mp::canonicalHash()==initial&&mp::pilots[1].resources.power==278&&ufoManager.inventory.count==1,"cross-stage resources and RNG reconstruction");
  Netplay::FrameInput wire;check(mp::InputLanes::Capture(1,true,2,-184,432,wire),"touch capture");
  std::array<Netplay::FrameInput,3> frames{wire,{}, {}};std::array<mp::Input,3> decoded;
  check(mp::InputLanes::Decode(frames.data(),3,decoded)&&decoded[0].pause&&decoded[0].x==-23552&&decoded[0].y==55296,"touch input roundtrip");
  check(mp::InputLanes::Capture(mp::InputLanes::Confirm|mp::InputLanes::Restart,false,0,0,0,wire),"menu wire lanes");
  frames[0]=wire;check(mp::InputLanes::Decode(frames.data(),3,decoded)&&decoded[0].held==(mp::InputLanes::Confirm|mp::InputLanes::Restart)&&(wire.buttons&mp::InputLanes::Restart),"Enter and Retry roundtrip preserve native menu ownership");
  mp::SessionSetup setup;setup.session_id=123;setup.automatic=false;setup.selections={0,3,5};setup.player_count=3;setup.build={1,2,3,4};
  mp::ReplayArchive replay;check(replay.Begin(setup,12345)&&replay.Append(0,1,frames.data(),3),"confirmed Replay append");
  std::vector<uint8_t> bytes;check(replay.Encode(bytes,"PLAYER",123,false),"Replay encode");mp::ReplayArchive loaded;
  check(loaded.Load(bytes.data(),bytes.size())&&loaded.FrameCount()==1&&(*loaded.FrameAt(0))[0]==wire,"Replay input roundtrip");
  bytes.back()^=1;check(!mp::ReplayArchive::Validate(bytes.data(),bytes.size()),"Replay corruption rejected");
  std::puts("TH12 multiplayer: resources, UFO inventory, transfers, rescue, boss damage, rollback/RNG, stage checkpoints, input and Replay passed");
}
