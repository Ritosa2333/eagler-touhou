#include "../Game/Coop/Rollback.h"
#include "../Game/Stage/Stage.h"
#include "../Game/Game.h"
#include <eagler/netplay/NetplayCore.hpp>
#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/RollbackReplayBudget.hpp>
#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <emscripten.h>

extern "C" int th05_tick(int render);
extern "C" unsigned th05_coop_checksum();
namespace th5w { namespace coop {
namespace {
Netplay::RollbackCore core;
Netplay::RollbackJournal journal;
constexpr int MaxPredictionFrames=8;
unsigned nextFrame=0,frontier=0,rollbacks=0,resimulated=0,predicted=0,lastBytes=0,worldRevision=0;
bool active=false,audioOpen=false;
unsigned audioFrame=0;
struct AudioEvent {int kind,index;double value;std::string name;};
std::map<unsigned,std::vector<AudioEvent>> audio;
std::map<unsigned,unsigned> hashes;

int Confirmed(){auto value=core.ConfirmedThroughAllRemotes();return value==Netplay::INVALID_FRAME?-1:int(value);}
CStage* Stage(){auto* game=CGame::s_pCurGame;return game&&game->m_curSchemeID==4?static_cast<CStage*>(game->m_pCurScheme):nullptr;}
struct JournalSink:CaptureSink {
 void Bytes(void* address,unsigned size) override {if(!journal.Touch(address,size))throw std::runtime_error("TH05 rollback capture exceeded its bounds");}
};


void Commit(){
 if(core.HasRollbackRequest()||nextFrame<frontier)return;
 const int through=std::min(Confirmed(),int(nextFrame)-1);
 for(auto it=audio.begin();it!=audio.end()&&int(it->first)<=through;){
  for(const auto& event:it->second){
   if(event.kind==0)web::PlaySound(event.index);
   else if(event.kind==1)web::MusicTrack(event.name.c_str());
   else web::MusicAction(event.index,event.value);
  }
  it=audio.erase(it);
 }
 journal.DiscardBefore(unsigned(through+1));
 while(hashes.size()>8)hashes.erase(hashes.begin());
}

bool Simulate(){
 const auto decision=core.PrepareFrame(nextFrame);if(!decision.canAdvance)return false;
 const bool exact=Confirmed()>=int(nextFrame);
 auto* stage=Stage();bool boundary=!stage||!stage->RollbackPredictable();
 // Menus and scene changes require confirmed input. A Bomb changes journaled
 // gameplay state and can run on the same frame as the local press.
 for(int i=0;i<Run().count;i++)if(decision.inputs[i].buttons&(4096|8192|16384|32768))boundary=true;
 if(boundary&&!exact)return false;
 if(!exact){
  if(!journal.BeginFrame(nextFrame))throw std::runtime_error("TH05 rollback history unavailable");
  // This whole pass only reads world bytes. Batch its large ranges into one
  // native TypedArray copy before any input or gameplay mutation begins.
  if(!journal.BeginReadOnlyCapture())throw std::runtime_error("TH05 rollback capture unavailable");
  JournalSink sink;stage->CaptureRollback(sink);
  if(!journal.EndReadOnlyCapture())throw std::runtime_error("TH05 rollback capture failed");
  if(!journal.EndFrame())throw std::runtime_error("TH05 rollback capture incomplete");
  lastBytes=unsigned(journal.BytesForFrame(nextFrame));
 }
 for(int seat=0;seat<Run().count;seat++){
  const auto& in=decision.inputs[seat];
  const bool absolute=in.analogMode==Netplay::AnalogMode::DirectTouch;
  Run().input[seat]={in.buttons,in.x,in.y,(absolute?2:0)+int(in.unlimited)};
 }
 audioFrame=nextFrame;audio.erase(audioFrame);audioOpen=true;
 const int success=th05_tick(0);audioOpen=false;
 if(!success)throw std::runtime_error("TH05 netplay simulation failed");
 ++worldRevision;
 if(!core.MarkSimulated(nextFrame,decision))throw std::runtime_error("TH05 input decision rejected");
 if(decision.predictedMask)++predicted;
 ++nextFrame;
 if(nextFrame%120==0)hashes[nextFrame]=th05_coop_checksum();
 return true;
}
}

bool QueueSound(int sound){if(!audioOpen)return false;audio[audioFrame].push_back({0,sound,0,{}});return true;}
bool QueueMusic(const char* name){if(!audioOpen)return false;audio[audioFrame].push_back({1,0,0,name});return true;}
bool QueueMusicAction(int action,double value){if(!audioOpen)return false;audio[audioFrame].push_back({2,action,value,{}});return true;}
} }

extern "C" {
EMSCRIPTEN_KEEPALIVE int th05_net_reset(int delay,int limit){
 using namespace th5w::coop;
 if(delay<0||delay>9||limit<0||limit>MaxPredictionFrames)return 0;
 active=false;audioOpen=false;audio.clear();hashes.clear();nextFrame=frontier=rollbacks=resimulated=predicted=lastBytes=worldRevision=0;
 Netplay::CoreConfig config;config.playerCount=Run().count;config.localPlayer=Run().local;config.inputDelay=delay;
 config.maxRollbackFrames=limit?limit:MaxPredictionFrames;config.allowPrediction=limit!=0;
 config.predictableButtons=15|32|64;config.directionButtons=15;config.prediction=Netplay::InputPrediction::ConfirmedHold;
 config.directTouchIsAbsolute=true;
 Netplay::RollbackJournalConfig storage;storage.maxFrames=10;storage.maxBytesPerFrame=1024*1024;storage.maxBlocksPerFrame=64;storage.coalesceRestore=true;storage.fastBulkCopy=true;
 if(!core.Reset(config)||!journal.Reset(storage))return 0;
 for(int frame=0;frame<delay;frame++)for(int seat=0;seat<Run().count;seat++)if(seat!=Run().local)core.SubmitRemoteInput(seat,frame,{});
 active=true;return 1;
}
EMSCRIPTEN_KEEPALIVE int th05_net_input(int seat,unsigned frame,int buttons,float x,float y,int motionMode){
 using namespace th5w::coop;
 if(!active||seat<0||seat>=Run().count||motionMode<0||motionMode>3||!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>8192||std::abs(y)>8192)return 0;
 Netplay::FrameInput in(static_cast<uint16_t>(buttons));in.x=x;in.y=y;in.unlimited=motionMode&1;
 // Relative touch samples are once-only deltas; absolute field targets can be
 // retained by prediction. Neither path reads a device during historical ticks.
 in.analogMode=motionMode>=2?Netplay::AnalogMode::DirectTouch:Netplay::AnalogMode::DirectTouchDelta;
 if(seat==Run().local){if(frame<unsigned(core.LocalFrameForCapture(0)))return 0;return core.ScheduleLocalInput(frame-core.LocalFrameForCapture(0),in);}
 auto result=core.SubmitRemoteInput(seat,frame,in);
 return result!=Netplay::RemoteInputResult::ConflictingConfirmedInput&&result!=Netplay::RemoteInputResult::TooOld&&result!=Netplay::RemoteInputResult::InvalidPlayer;
}
static int Advance(){
 using namespace th5w::coop;if(!active)return 0;
 if(core.HasRollbackRequest()){
  const unsigned target=core.RollbackFrame();unsigned restored=0;
  if(!journal.UndoTo(target,&restored)||restored!=target||!core.RewindSimulationTo(target))throw std::runtime_error("TH05 rollback restore failed");
  nextFrame=target;++rollbacks;
  audio.erase(audio.lower_bound(target),audio.end());hashes.erase(hashes.upper_bound(target),hashes.end());
 }
 const double start=emscripten_get_now();unsigned replayed=0;
 while(nextFrame<frontier){
  // A cheap correction can replay the whole prediction window in one slice.
  // Keep the wall-clock budget on slower devices or busy scenes.
  if(replayed>=MaxPredictionFrames||(replayed>0&&(emscripten_get_now()-start)*1000000>=Netplay::RollbackReplayBudget::SliceBudgetNs))return 0;
  if(!Simulate()){Commit();return 0;}++replayed;++resimulated;
 }
 Commit();if(!Simulate())return 0;++frontier;Commit();return 1;
}
EMSCRIPTEN_KEEPALIVE int th05_net_advance(){try{return Advance();}catch(const std::exception& error){th5w::coop::active=false;th5w::coop::audioOpen=false;th5w::web::ReportError(error.what());return -1;}}
EMSCRIPTEN_KEEPALIVE int th05_net_confirmed(){using namespace th5w::coop;int through=std::min(Confirmed(),int(nextFrame)-1);if(core.HasRollbackRequest())through=std::min(through,int(core.RollbackFrame())-1);return through;}
EMSCRIPTEN_KEEPALIVE unsigned th05_net_stat(int field){using namespace th5w::coop;switch(field){case 0:return rollbacks;case 1:return resimulated;case 2:return predicted;case 3:return lastBytes;case 4:return unsigned(journal.FrameCount());case 5:return nextFrame;case 6:return frontier;case 7:return unsigned(audio.size());case 8:return worldRevision;default:return 0;}}
EMSCRIPTEN_KEEPALIVE unsigned th05_net_hash(unsigned frame){using namespace th5w::coop;auto it=hashes.find(frame);return it==hashes.end()?0:it->second;}
EMSCRIPTEN_KEEPALIVE void th05_net_close(){using namespace th5w::coop;active=false;audioOpen=false;audio.clear();hashes.clear();journal.Clear();core.Clear();}
}
