#include "game/GameSession.hpp"
#include "game/GameState.hpp"
#include "game/StageFlow.hpp"
#include "wasm/Api.hpp"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>

using namespace th12;
static void require(bool result, const char* reason) {
  if (!result) { std::fprintf(stderr, "%s\n", reason); std::exit(1); }
}
static void loadBirthProbe() {
  std::vector<uint8_t> ecl(220);
  auto word=[&](size_t at,uint32_t n){std::memcpy(ecl.data()+at,&n,4);};
  auto half=[&](size_t at,uint16_t n){std::memcpy(ecl.data()+at,&n,2);};
  word(0,0x54504353);half(4,1);half(16,1);word(36,48);
  std::memcpy(ecl.data()+40,"main",5);word(48,0x484c4345);
  size_t at=64;
  auto instruction=[&](int opcode,uint32_t operand,bool reference) {
    half(at+4,opcode);half(at+6,20);half(at+8,reference?1:0);
    ecl[at+10]=63;ecl[at+11]=1;word(at+16,operand);at+=20;
  };
  instruction(42,uint32_t(-9930),true);instruction(43,0,false);
  instruction(44,asbits(-9991.f),true);instruction(45,asbits(4.f),false);
  instruction(42,uint32_t(-10000),true);instruction(43,8,false);
  instruction(83,1000,false);half(at+4,10);half(at+6,16);ecl[at+10]=63;at+=16;
  ecl.resize(at);
  require(th12_load_ecl(ecl.data(),int(ecl.size()))==1,"Synthetic main birth probe failed to load");
}
static void start(int stage=1,bool practice=false) {
  th12_select_stage(stage);loadBirthProbe();
  require(th12_start_full_game(2,0,stage==7?4:1,1234,practice)==1,"Full game did not start");
}
static void clear() {
  ended=1;th12_tick(0,0);
  require(fullGameFlow.status==FullGameStatus::StageClear,"Stage clear was not observed");
}
static void installClearMessage() {
  const uint8_t msg[]={1,0,0,0,12,0,0,0,0,0,0,0,0,0,21,0,1,0,0,0};
  require(messageFiles[0].load(msg,int(sizeof(msg))),"Synthetic clear message did not load");
  require(messageVM.start(messageFiles[0],0),"Synthetic clear message did not start");
}
int main(int argc,char** argv) {
  if (argc==6) {
    const int stage=std::atoi(argv[1]),d=std::atoi(argv[2]),count=std::atoi(argv[3]);
    th12_select_stage(stage);
    require(th12_start_full_game(0,0,d,1234,0)==1,"Continue sample did not start");
    fullGameFlow.continueCount=count;replayStageGlobals[0]=count;
    resources.scoreStored=1234567;graze=1234;resources.lifeFragments=3;
    resources.bombFragments=2;resources.rank=123;replayStageGlobals[1]=9;
    ufoManager.inventory.append(1);ended=2;th12_tick(0,0);
    require(th12_continue_game()==1,"Continue sample was rejected");
    std::printf("%d %d %d %d %d %d %d %d %d %d %d %d\n",
      fullGameFlow.continueCount,score,power,lives,bombs,resources.pointValueStored,graze,
      resources.lifeFragments,resources.bombFragments,resources.rank,replayStageGlobals[1],
      ufoManager.inventory.count);
    return 0;
  }
  start();
  require(enemies.front()->contexts.front().locals[0]==100,"Full-game initial birth power differs");
  require(fullGameFlow.recordHeaderReady&&fullGameFlow.recordHeader.seed==1234&&
    th12::random.calls==2&&fullGameFlow.recordHeader.resources.power==100,
    "Recorder initial header was sampled after main birth");
  resources.scoreStored=12345678;power=347;lives=5;bombs=4;
  resources.lifeFragments=3;resources.bombFragments=2;resources.pointValueStored=1595310;
  resources.rank=-123;graze=9876;replayStageGlobals={4,7,1,9876};
  fullGameFlow.continueCount=4;playerFocused=true;px=-4321;py=49920;
  playerMotion.bodyAnimation=4;playerMotion.bodyAge=987;playerMotion.previousDx=12;
  shotSchedule.frame=13;shotSchedule.previous=12;shotSchedule.time=13;
  playerStateTicks=700;playerStateTime=700;playerTotalTime=1000;
  fireFrame=1234;phasePlayerCounters={1,2,3};
  ufoManager.inventory.append(1);ufoManager.inventory.append(3);
  ufoManager.inventory.rotation=2;ufoManager.inventory.gateTimer=13;
  const auto original=captureSessionCarry();
  clear();
  require(!(stageState.flags&4),"Clear reward prematurely started the stage fade");
  for(int i=0;i<119;i++)th12_tick(0,0);
  require(fullGameFlow.status==FullGameStatus::StageClear,"Next stage started before120 clear ticks");
  require(std::string(th12_background()).find("\"transition\":null")!=std::string::npos,
          "Clear hold published an early field fade");
  require(th12_start_next_stage()==0,"Premature stage birth succeeded");
  th12_tick(0,0);
  require(fullGameFlow.status==FullGameStatus::StageClear&&fullGameFlow.clearAge==120,
          "Clear reward did not retain the outgoing stage before its fade");
  for(int i=0;i<29;i++)th12_tick(0,0);
  require(fullGameFlow.status==FullGameStatus::StageClear&&(stageState.flags&4),
          "Next stage interrupted the30-tick field fade");
  require(std::string(th12_background()).find("\"transition\":{\"out\":true,\"age\":29}")!=std::string::npos,
          "Outgoing field fade lost its logical snapshot age");
  th12_tick(0,0);
  require(fullGameFlow.status==FullGameStatus::NextStage&&fullGameFlow.nextStage==2,
          "Ordinary clear did not schedule the next stage after120+30 ticks");
  const auto beforeQuery=fullGameFlow.totalFrames;const auto rngBefore=th12::random;
  const std::string query=th12_stage_flow();th12_stage_flow();
  require(query==th12_stage_flow()&&beforeQuery==fullGameFlow.totalFrames&&th12::random.calls==rngBefore.calls,
          "Stage-flow query advanced the simulation");
  require(th12_start_next_stage()==0,"Unloaded next-stage resources were accepted");
  th12_select_stage(2);loadBirthProbe();
  Random birthRandom{original.seed,0};const auto expectedWord=birthRandom.next32();
  require(th12_start_next_stage()==1,"Ready next-stage birth failed");
  require(fullGameFlow.entryFadeAge==0,"Incoming stage skipped its field fade-in");
  require(std::string(th12_background()).find("\"transition\":{\"out\":false,\"age\":0}")!=std::string::npos,
          "Incoming field fade was absent from the renderer snapshot");
  const auto& locals=enemies.front()->contexts.front().locals;
  require(locals[0]==347&&asfloat(locals[1])==float(-4321/128.f)&&locals[2]==expectedWord,
          "Next-stage main birth observed late/unrestored power, position or RNG");
  require(th12::random.seed==birthRandom.seed&&th12::random.calls==2,"Per-stage RNG call counter was not reset before birth");
  require(fullGameFlow.recordHeaderReady&&fullGameFlow.recordHeader.seed==original.seed&&
    fullGameFlow.recordHeader.resources.power==347&&fullGameFlow.recordHeader.x==-4321&&
    fullGameFlow.recordHeader.nativeGlobals[1]==7,
    "Recorder next-stage header lost the immutable pre-main boundary");
  require(score==12345678&&power==347&&lives==5&&bombs==4&&graze==9876&&
    resources.lifeFragments==3&&resources.bombFragments==2&&resources.pointValueStored==1595310&&
    resources.rank==-123&&replayStageGlobals==std::array<int,4>{4,7,1,9876},
    "Cross-stage global resources or extend threshold did not survive");
  require(ufoManager.inventory.colors==original.inventory.colors&&
    ufoManager.inventory.rotation==2&&ufoManager.inventory.gateTimer==13,
    "Cross-stage UFO colors/rotation/gate did not survive");
  require(px==-4321&&py==49920&&playerFocused&&playerState==1&&playerStateTicks==0&&
    playerTotalTime==0&&invuln==120&&fireFrame==0&&phasePlayerCounters==std::array<u32,3>{},
    "Incoming stage inherited old player timers or lost retained position/focus");
  require(playerMotion.count==3&&playerMotion.previousDx==0&&playerMotion.bodyAge==0&&
    playerMotion.resetPending==std::array<bool,4>{true,true,true,true}&&
    playerMotion.options[0].x==px&&playerMotion.options[0].y==py&&
    shotSchedule.frame==-1&&shotSchedule.previous==-2&&shotSchedule.time==-1,
    "Incoming stage retained old option-follow or shot-cycle state");
  for(int i=0;i<30;i++)th12_tick(0,0);
  require(fullGameFlow.entryFadeAge==30&&std::string(th12_background()).find("\"transition\":null")!=std::string::npos,
          "Incoming field fade did not retire after30 logical ticks");
  for(int stage=2;stage<=5;stage++) {
    require(stageNumber==stage,"Full run stage order changed");clear();
    for(int i=0;i<150;i++)th12_tick(0,0);
    th12_select_stage(stage+1);loadBirthProbe();require(th12_start_next_stage()==1,"Story could not enter later stage");
  }
  clear();for(int i=0;i<379;i++)th12_tick(0,0);
  require(fullGameFlow.status==FullGameStatus::StageClear,"Ending started before380 clear ticks");
  th12_tick(0,0);require(fullGameFlow.status==FullGameStatus::Ending,"Six-stage run did not enter ending");
  require(th12_start_next_stage()==0,"Main-game ending incorrectly enteredExtra");
  start(4,true);require(lives==9&&power==200,"Practice native starting resources differ");
  clear();for(int i=0;i<120;i++)th12_tick(0,0);
  require(fullGameFlow.status==FullGameStatus::Result&&!fullGameFlow.nextStage,
          "Practice advanced to a different stage");
  start(6,true);installClearMessage();tickMessage(0,0);
  require(score==600000,"Sixth-stage practice received the main-game final resource bonus");
  th12_start(0,0,1,1234);installClearMessage();tickMessage(0,0);
  require(score==600000+stage_award::final(6,1,power,lives,bombs,resources.pointValueStored)/10,
          "Legacy sixth-stage final resource bonus changed");
  start(7);ended=2;th12_tick(0,0);
  require(!canContinueFullGame()&&th12_continue_game()==0,"Extra incorrectly offersContinue");
  start(2);ended=2;th12_tick(0,0);require(canContinueFullGame(),"Story game over lacksContinue");
  require(th12_continue_game()==1&&stageNumber==2&&power==400&&lives==2&&bombs==2&&
    score==0&&graze==0&&resources.rank==-512&&fullGameFlow.continueCount==1,
    "Continue did not restart the current stage with original resources");
  int32_t header[17]={0,51200,143,2,2,1234,1000000,0,0,0,0,0,0,0,0,0,0};
  require(th12_start_replay(0,0,1,1234,header,17)==1,"Legacy replay failed to start");
  require(fullGameFlow.status==FullGameStatus::Disabled,"Full game leaked into legacy replay");
  th12_set_full_game_mode(0);require(fullGameFlow.replay,"Replay did not enable full-game transitions");
  clear();for(int i=0;i<150;i++)th12_tick(0,0);
  th12_select_stage(3);require(th12_start_next_stage()==0,"Replay incorrectly uses live carry instead of stage header");
  th12_start(0,0,1,1234);require(fullGameFlow.status==FullGameStatus::Disabled,"Full mode leaked into legacy start");
  ended=1;const int oldFrame=frame;th12_tick(0,0);
  require(frame==oldFrame&&ended==1,"Legacy end flag behavior changed");
  std::puts("Atomic main birth, six-stage carry, transitions, practice, Continue and legacy isolation passed.");
}
