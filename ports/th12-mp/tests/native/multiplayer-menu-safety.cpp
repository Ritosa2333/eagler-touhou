#include "game/GameState.hpp"
#include "game/GameSession.hpp"
#include "game/StageFlow.hpp"
#include "multiplayer/InputLanes.hpp"
#include "multiplayer/WorldState.hpp"
#include <cstdio>
#include <cstdlib>
using namespace th12;
namespace mp=th12::multiplayer;
void check(bool ok,const char* message){if(!ok){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
void start(unsigned count,int stage){
 selectStage(stage);mp::Options o;o.count=count;o.stage=stage;o.difficulty=3;o.selections={5,5,5};
 check(mp::begin(o),"menu fixture starts");
}
int main(){
 for(unsigned count:{2u,3u})for(int stage:{1,3,6})for(unsigned source=0;source<count;++source){
  start(count,stage);std::array<mp::Input,3> input{};const unsigned other=(source+1)%count;
  input[source].held=mp::InputLanes::Restart;
  check(mp::update(input)&&!mp::paused&&!mp::finished,"gameplay R cannot restart a live run");
  input[source].held=33;check(mp::update(input),"hold gameplay shoot and direction");
  input[other].pause=true;check(mp::update(input),"pause edge");input[other].pause=false;
  for(unsigned i=0;i<27;++i)check(mp::update(input)&&mp::paused&&!mp::menuConfirm&&mp::menuSelection==0&&!mp::finished,"held gameplay input cannot navigate pause");
  mp::WorldState checkpoint;const auto hash=mp::canonicalHash();
  input={};check(mp::update(input)&&(mp::menuReadySeats&(1u<<source)),"seat release arms its own menu input");
  checkpoint.restore();check(!(mp::menuReadySeats&(1u<<source))&&mp::canonicalHash()==hash,"rollback restores every seat's input owner");
  input={};check(mp::update(input),"release controls");
  input[source].held=16;check(mp::update(input)&&mp::menuSelection==3,"fresh direction selects retry");
  input={};check(mp::update(input),"release direction");
  input[source].held=1;check(mp::update(input)&&mp::menuConfirm&&mp::menuSelection==1,"retry confirmation defaults to No");
  for(unsigned i=0;i<33;++i)check(mp::update(input)&&!mp::finished&&mp::menuSelection==1,"held confirmation cannot confirm the new modal");
  input={};check(mp::update(input),"release confirmation");
  input[source].held=32;check(mp::update(input)&&mp::menuSelection==0,"fresh direction selects Yes");
  input={};check(mp::update(input),"release Yes direction");
  input[source].held=1;check(mp::update(input)&&mp::finished&&mp::restartRequested,"explicit menu retry remains synchronized");

  start(count,stage);input={};input[other].held=1;check(mp::update(input),"teammate holds Shoot");
  input[source].pause=true;check(mp::update(input),"guest opens pause");input[source].pause=false;
  check(mp::update(input),"guest releases pause while teammate keeps Shoot");
  input[source].held=1;check(mp::update(input)&&!mp::paused&&!mp::finished,"guest confirm resumes despite teammate's held Shoot");
  input[source].held=mp::InputLanes::Restart;check(mp::update(input)&&!mp::finished,"R after Resume cannot restart gameplay");
  input[source].held=0;input[source].pause=true;check(mp::update(input),"pause again");input[source].pause=false;
  check(mp::update(input),"release pause");
  input[source].held=mp::InputLanes::Restart;
  check(mp::update(input)&&mp::paused&&mp::finished&&mp::restartRequested&&mp::menuAction==3&&mp::menuSelection==0,"fresh R restarts only through paused ownership");

  Netplay::FrameInput wire;
  check(mp::InputLanes::Capture(mp::InputLanes::Confirm|mp::InputLanes::Restart,false,0,0,0,wire)&&(wire.buttons&mp::InputLanes::Restart),"wire records the effective R edge");
  std::array<Netplay::FrameInput,3> frames{};frames[source]=Netplay::FrameInput(mp::InputLanes::Restart|64);
  std::array<mp::Input,3> decoded{};
  check(mp::InputLanes::Decode(frames.data(),count,decoded)&&decoded[source].held==(mp::InputLanes::Restart|64),"Replay and remote input preserve R and movement");
 }
 std::puts("TH12 menu ownership: 2P/3P, stages 1/3/6, all seats, held-key isolation, rollback and synchronized restart passed");
}
