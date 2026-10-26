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
 selectStage(stage);mp::Options options;options.count=count;options.stage=stage;options.difficulty=3;options.selections={5,5,5};
 check(mp::begin(options),"menu safety setup");
}
int main(){
 for(unsigned count:{2u,3u})for(int stage:{1,3,6})for(unsigned source=0;source<count;++source){
  start(count,stage);std::array<mp::Input,3> input{};
  const unsigned other=(source+1)%count;
  input[source].held=mp::InputLanes::Restart;
  check(mp::update(input)&&!mp::paused&&!mp::restartRequested&&!mp::finished,"R ignored during gameplay");
  input[source].held=32;check(mp::update(input),"gameplay direction");
  input[other].pause=true;check(mp::update(input),"pause edge");input[other].pause=false;
  for(unsigned i=0;i<27;++i)check(mp::update(input),"held gameplay direction in pause");
  input[source].held|=mp::InputLanes::Confirm;check(mp::update(input),"continuing gameplay confirm");
  input[source].held=32;for(unsigned i=0;i<3;++i)check(mp::update(input),"held direction crossing confirmation");
  input[source].held|=mp::InputLanes::Confirm;check(mp::update(input),"second confirm without neutral input");
  if(mp::restartRequested)std::fprintf(stderr,"REPRO: no R input, seats=%u stage=%d source=%u, restart=%d finished=%d\n",count,stage,source,int(mp::restartRequested),int(mp::finished));
  check(mp::paused&&!mp::menuConfirm&&mp::menuSelection==0&&!mp::restartRequested&&!mp::finished,
    "gameplay direction/confirm leaked across pause and restarted the run without R");
  mp::WorldState pauseCheckpoint;const auto pauseHash=mp::canonicalHash();
  input={};check(mp::update(input)&&mp::menuInputReady,"neutral controls arm pause menu");
  pauseCheckpoint.restore();check(!mp::menuInputReady&&mp::canonicalHash()==pauseHash,"rollback restores unarmed pause gate");
  input={};input[source].held=mp::InputLanes::Restart;
  check(mp::update(input)&&!mp::restartRequested&&!mp::finished,"R ignored inside pause menu");
  input={};check(mp::update(input),"release R");
  input[source].held=16;check(mp::update(input)&&mp::menuSelection==3,"intentional retry navigation");
  input={};check(mp::update(input),"release retry navigation");
  input[source].held=mp::InputLanes::Confirm;
  check(mp::update(input)&&mp::menuConfirm&&mp::menuSelection==1&&!mp::menuInputReady,"retry defaults to No with a fresh release gate");
  mp::WorldState confirmationCheckpoint;const auto confirmationHash=mp::canonicalHash();
  input[source].held|=32;
  for(unsigned i=0;i<33;++i)check(mp::update(input)&&mp::menuSelection==1&&!mp::finished,"held controls cannot confirm retry");
  input={};check(mp::update(input)&&mp::menuInputReady,"release arms confirmation");
  confirmationCheckpoint.restore();
  check(!mp::menuInputReady&&mp::canonicalHash()==confirmationHash,"rollback restores confirmation release gate");
  input={};input[source].held=mp::InputLanes::Restart;
  check(mp::update(input)&&mp::menuConfirm&&mp::menuSelection==1&&!mp::finished,"R ignored inside confirmation");
  input={};check(mp::update(input),"release confirmation after restore");
  input[source].held=32;check(mp::update(input)&&mp::menuSelection==0,"fresh navigation selects Yes");
  for(unsigned i=0;i<33;++i)check(mp::update(input)&&mp::menuSelection==0&&!mp::finished,"confirmation requires fresh direction edges");
  input={};check(mp::update(input),"release Yes navigation");
  input[source].held=1;
  check(mp::update(input)&&mp::paused&&mp::finished&&mp::restartRequested&&mp::menuConfirm&&mp::menuAction==3&&mp::menuSelection==0,
    "explicit menu Yes still allows synchronized retry");
  Netplay::FrameInput wire;
  check(mp::InputLanes::Capture(mp::InputLanes::Confirm|mp::InputLanes::Restart,false,0,0,0,wire)&&!(wire.buttons&mp::InputLanes::Restart),"wire capture removes R");
  std::array<Netplay::FrameInput,3> frames{};frames[source]=Netplay::FrameInput(mp::InputLanes::Restart|64);
  std::array<mp::Input,3> decoded{};
  check(mp::InputLanes::Decode(frames.data(),count,decoded)&&decoded[source].held==64,"wire decode ignores R while preserving movement");
 }
 std::puts("TH12 2P/3P pause input release safety passed for stages 1, 3 and 6, every seat");
}
