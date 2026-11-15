#include "game/NativeNormalRecord.hpp"
#include "game/NativeRunReplay.hpp"
#include "game/PauseGameOverHost.hpp"
#include "game/StageControl.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

// Only the World/carry containers are synthetic. No actual World, replay-file,
// clear, MSG/ECL event, ending permit or Main credential is produced.
struct WorldSeam {bool faulted()const{return false;} unsigned frame()const{return 120;}};
static WorldSeam worldValue;
static auto* world=&worldValue;
static th128::stage::Control stageControl;
static unsigned worldGeneration=3;
static int worldStartDifficulty=0,currentStageProgress=0,originalContinueCount=0;
static std::string worldStartedName="st_a1_3.ecl";
static auto normalRecord=std::make_shared<th128::record::NativeNormalRecord>(std::span<const std::uint8_t>{});
static auto normalReplay=std::make_shared<th128::record::NativeRunReplay>();
struct CarrySeam {
 bool valid=true,targetBound=true,preInitBound=true,gameRandomRestored=true;
 bool sourceControlActionRecreate=true,sourceDestroyExisting=true;
 bool debugRunTainted=false,debugInfiniteRespawn=false,consumed=true,acknowledged=true,scalarRestored=true;
 unsigned token=7,sourceGeneration=2,restoredGeneration=3,sourceTransitionSerial=1,scalarRestoredGeneration=3;
 int sourceCurrentId=2,nextId=3,sourceDifficulty=0,difficulty=0,targetDifficulty=0;
 int sourceControlCurrent=7,sourceControlPending=7,sourceControlSessionArgument=0;
 int nextProgress=0,originalContinueCount=0;
 std::string sourceWorldName="st_a1_2.ecl",targetWorldName="st_a1_3.ecl";
 std::shared_ptr<th128::record::NativeNormalRecord> normalRecord;
 std::shared_ptr<th128::record::NativeRunReplay> normalReplay;
} stageCarry;
static int stageIdForEcl(const std::string& name){return name=="st_a1_2.ecl"?2:name=="st_a1_3.ecl"?3:0;}

#include "actual-continue-proof.inc"
#include "actual-carry-predicate.inc"

static std::unique_ptr<th128::pause::Host> originalPauseMenu;
struct UnusedReplaySave {
 bool finished()const{throw std::runtime_error("unused replay seam called");}
 bool gameOver()const{throw std::runtime_error("unused replay seam called");}
};
static std::unique_ptr<UnusedReplaySave> normalReplaySave;
static std::vector<std::uint8_t> normalReplayBytes;
static bool worldTickExecuting=false,debugRunTainted=false,debugInfiniteRespawnRun=false;
struct PlaybackSeam{bool active()const{return false;}} replayPlayback;
static unsigned soundJournalWindowSerial=0;
static std::vector<int> grazeSounds;
static std::string operationError;
static int scoreRaw=250000,motivationRaw=-1,motivationFlag=0;
struct WindowSeam{int perfectCharge4b4d70=0;} playerWindowControl;
static std::array<unsigned,3> playerInput{};
#include "actual-pause-commit.inc"

static void openContinue(){
 originalPauseMenu->open(true,originalContinueCount,true,world->frame(),false);
 for(unsigned i=0;i<10;++i)originalPauseMenu->tick({0,0});
 assert(originalPauseMenu->state().ready);
 if(originalContinueCount<5){
  assert(originalPauseMenu->state().selection==0&&!originalPauseMenu->state().disabled[0]);
  originalPauseMenu->tick({0,1});
  for(unsigned i=0;i<20&&originalPauseMenu->state().action!=th128::pause::Action::Continue;++i)originalPauseMenu->tick({0,0});
  assert(originalPauseMenu->state().action==th128::pause::Action::Continue);
 }
 motivationRaw=-1;
}
int main(int argc,char** argv){
 try{
  if(argc!=2)throw std::runtime_error("pinned front required");
  std::ifstream input(argv[1],std::ios::binary);const std::vector<std::uint8_t> front{std::istreambuf_iterator<char>(input),std::istreambuf_iterator<char>()};
  assert(front.size()==1993048u);stageControl.initialize();
  assert(stageControl.constructFreshSession7([](int n){return n==0;}).status==th128::stage::CommitStatus::Applied);
  assert(stageControl.request(12).pending==12);assert(stageControl.consume({true,0}).status==th128::stage::CommitStatus::Applied);
  assert(stageControl.sessionConstructed()&&stageControl.sessionFlag()==0);
  stageCarry.normalRecord=normalRecord;stageCarry.normalReplay=normalReplay;
  originalPauseMenu=std::make_unique<th128::pause::Host>();originalPauseMenu->load(front);
  assert(ordinaryCarrySessionPresent(true));unsigned accepted=1,rejected=0;
  for(unsigned count=1;count<=5;++count){
   openContinue();const auto serial=originalPauseMenu->state().serial;
   if(count==1){
    soundJournalWindowSerial=1;assert(th128_world_pause_commit(2)==0);soundJournalWindowSerial=0;
    assert(originalContinueCount==0&&ordinaryCarryContinue.pauseSerial==0);++rejected;
    motivationRaw=1;assert(th128_world_pause_commit(2)==0);motivationRaw=-1;
    assert(originalContinueCount==0&&ordinaryCarryContinue.pauseSerial==0);++rejected;
   }
   assert(th128_world_pause_commit(2)==1);
   assert(!originalPauseMenu->state().active&&originalContinueCount==int(count));
   assert(stageCarry.originalContinueCount==0&&ordinaryCarryContinue.currentCount==int(count));
   assert(ordinaryCarryContinue.pauseSerial==serial&&stageControl.sessionFlag()==0&&scoreRaw==0&&motivationRaw==20000);
   assert(ordinaryCarrySessionPresent(true));++accepted;
   assert(th128_world_pause_commit(2)==0&&ordinaryCarryContinue.currentCount==int(count));++rejected;
   originalContinueCount=int(count+1);assert(!ordinaryCarrySessionPresent(true));originalContinueCount=int(count);++rejected;
   const auto proof=ordinaryCarryContinue;
   ordinaryCarryContinue={};assert(!ordinaryCarrySessionPresent(true));ordinaryCarryContinue=proof;++rejected;
   const auto token=stageCarry.token;stageCarry.token=token+1;assert(!ordinaryCarrySessionPresent(true));stageCarry.token=token;++rejected;
   const auto generation=ordinaryCarryContinue.generation;ordinaryCarryContinue.generation=generation+1;assert(!ordinaryCarrySessionPresent(true));ordinaryCarryContinue.generation=generation;++rejected;
   auto record=normalRecord;normalRecord=std::make_shared<th128::record::NativeNormalRecord>(std::span<const std::uint8_t>{});
   stageCarry.normalRecord=normalRecord;assert(!ordinaryCarrySessionPresent(true));normalRecord=record;stageCarry.normalRecord=record;++rejected;
   auto replay=normalReplay;normalReplay=std::make_shared<th128::record::NativeRunReplay>();
   stageCarry.normalReplay=normalReplay;assert(!ordinaryCarrySessionPresent(true));normalReplay=replay;stageCarry.normalReplay=replay;++rejected;
  }
  openContinue();assert(originalPauseMenu->state().disabled[0]&&originalPauseMenu->state().selection==1);
  const auto last=ordinaryCarryContinue.pauseSerial;assert(th128_world_pause_commit(2)==0);
  assert(originalContinueCount==5&&ordinaryCarryContinue.pauseSerial==last);++rejected;
  // A real menu commit cannot legitimize an unproven pre-existing count.
  originalPauseMenu=std::make_unique<th128::pause::Host>();originalPauseMenu->load(front);
  ordinaryCarryContinue={};originalContinueCount=1;assert(!ordinaryCarrySessionPresent(true));
  openContinue();assert(th128_world_pause_commit(2)==1);assert(originalContinueCount==2&&!ordinaryCarrySessionPresent(true));
  assert(ordinaryCarryContinue.pauseSerial==0);++rejected;
  std::cout<<"{\"passed\":true,\"accepted\":"<<accepted<<",\"rejected\":"<<rejected<<",\"realWorldExportsPauseCommit\":true,\"realControl12\":true,\"realPauseHost\":true,\"syntheticWorldCarry\":true,\"sealedCarryUnchanged\":true,\"WorldStarts\":0,\"permits\":0}\n";
  return 0;
 }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
