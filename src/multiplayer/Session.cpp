#include "Session.hpp"
#include "InputLanes.hpp"
#include "SpectatorStageSchema.hpp"
#include "../game/GameState.hpp"
#include "../game/StageFlow.hpp"
#include "../game/Practice.hpp"
#include <ctime>
namespace th12::multiplayer {
Session session;
bool Session::fail(const char* message) { if(error_.empty())error_=message; return false; }
const char* Session::error() const {return error_.empty()?network.Error():error_.c_str();}
void Session::stop(){network.Clear();records_.clear();WorldState::clearPool();replay_.Clear();playback_.Clear();pendingStage_=0;audio_.clear();error_.clear();started_=loading_=false;active=false;committed_=restores_=resimulated_=checkpoints_=0;}
bool Session::configure(const uint32_t* words,unsigned count){
  SessionSetup setup; if(!DecodeSessionSetup(setup,words,count))return fail("Invalid TH12 room setup");
  stop(); return network.Reset(setup);
}
bool Session::start(){
  if(started_||!network.Configured())return fail("TH12 room was not configured");
  const auto& s=network.Setup(); Options o;o.count=s.player_count;o.local=s.local_player;o.seed=s.seed;
  o.stage=s.initial_stage();o.difficulty=int(s.difficulty);o.challenge=s.challenge;
  for(unsigned p=0;p<3;++p)o.selections[p]=int(s.selections[p]);
  practice::setEnabled(false);
  if(!begin(o))return fail("TH12 room resources do not match the starting stage");
  started_=true;return true;
}
bool Session::pump(){return *error()==0&&network.Pump(started_&&!loading_);}
bool Session::simulate(const Netplay::FrameDecision& decision){
  std::array<Input,3> inputs;
  if(!InputLanes::Decode(decision.inputs.data(),options.count,inputs))return fail("Invalid TH12 frame inputs");
  const auto f=network.NextFrame();
  // An all-actual frame can never acquire a correction request. Earlier
  // speculative frames retain their own checkpoints until confirmation.
  const bool checkpoint=network.AllowsRollback()&&decision.predictedMask;
  Record record{f,uint32_t(stageNumber),checkpoint?std::optional<WorldState>{std::in_place}:std::nullopt,decision.inputs,{}, {}};
  checkpoints_+=checkpoint;
  if(th12::frame==0)record.initializer=encodeStageEntry();
  if(!update(inputs))return fail("TH12 gameplay rejected a frame");
  record.sounds=sound_system::queue.events();
  if(!network.MarkSimulated(f,decision))return fail(network.Error());
  records_.push_back(std::move(record));return true;
}
bool Session::commit(){
  if(network.Correcting()||network.RollbackFrame()!=Netplay::INVALID_FRAME)return true;
  auto confirmed=network.Confirmed();const auto last=network.LastFrame();
  if(confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return true;
  const auto through=std::min(confirmed,last);
  if(network.AllowsRollback()&&!network.SetCommitLimit(through))return fail("TH12 confirmed frame fence failed");
  if(!replay_.Recording()&&!replay_.Begin(network.Setup(),uint64_t(std::time(nullptr))))return fail("TH12 Replay initialization failed");
  while(!records_.empty()&&records_.front().frame<=through){
    const auto& r=records_.front();std::array<Netplay::FrameInput,3> actual;
    if(r.frame!=committed_||!network.ConfirmedInputs(r.frame,actual)||actual!=r.inputs)return fail("TH12 actual input differs from the committed world");
    if(!replay_.Append(r.frame,r.stage,actual.data(),options.count))return fail("TH12 Replay capacity reached");
    if(!network.RecordSpectatorFrame(r.frame,r.stage,r.initializer,actual))return fail("TH12 spectator record failed");
    audio_.insert(audio_.end(),r.sounds.begin(),r.sounds.end());records_.pop_front();++committed_;
  }
  network.PublishConfirmedSpectatorHistoryAfterReconcile();return true;
}
bool Session::step(uint32_t held,bool pause,int motion,float x,float y,double nowMs){
  if(!started_||loading_)return pump();
  if(network.Spectator())return spectatorStep();
  if(network.Playback())return replayStep();
  if(!network.Pump(true,true))return false;
  if(!network.CanStart())return true;
  network.SetCapturePhase(paused||dialogue||fullGameFlow.status!=FullGameStatus::Playing,uint64_t(std::max(0.,nowMs)*1000));
  if(network.NeedsCapture()){
    Netplay::FrameInput input;
    if(!InputLanes::Capture(held,pause,motion,x,y,input)||!network.Capture(network.CaptureFrame(),input))return fail("TH12 input capture failed");
  }
  if(!network.FlushDeferredInputs())return false;
  if(network.RollbackFrame()!=Netplay::INVALID_FRAME){
    const auto target=network.RollbackFrame();
    const auto record=std::find_if(records_.begin(),records_.end(),[&](const Record& r){return r.frame==target;});
    if(record==records_.end()||!record->before)return fail("TH12 rollback checkpoint missing");
    record->before->restore();
    if(!(network.Correcting()?network.RestartCorrection(target):network.BeginCorrection(target)))return fail("TH12 rollback frontier rejected");
    records_.erase(record,records_.end());++restores_;
  }
  if(network.AllowsRollback()&&!network.Correcting()&&!network.WorldReady()&&!network.SetWorldReady(true))return fail("TH12 world frontier rejected");
  for(unsigned budget=0;budget<13;++budget){
    if(finished||fullGameFlow.status==FullGameStatus::NextStage||
       (network.Setup().mode==1&&network.NextFrame()>=network.CaptureFrame()))break;
    const auto decision=network.Prepare(network.NextFrame());if(!decision.canAdvance||(paused&&decision.predictedMask))break;
    const bool correcting=network.Correcting();
    if(!simulate(decision))return false;
    if(correcting)++resimulated_;
    if(network.Correcting()&&(network.NextFrame()==network.CorrectionEnd()||
       ((finished||fullGameFlow.status==FullGameStatus::NextStage)&&network.InputsReconciledBeforeNext()))){
      if(!network.EndCorrection(network.NextFrame()!=network.CorrectionEnd()))return fail("TH12 correction completion failed");
    }
    // Replaying an old input does not consume this callback's new game tick.
    // The 13-frame budget covers the 12-frame correction plus one fresh tick.
    if(!correcting&&network.Setup().mode!=1)break;
  }
  if(!commit())return false;
  if(replay_.AtCapacity()&&settled())finished=true;
  if(finished&&settled()&&!network.Retired())network.Finish();
  return *error()==0;
}
unsigned Session::nextStage()const{return pendingStage_?pendingStage_:unsigned(fullGameFlow.nextStage);}
bool Session::stageNeeded() const {return started_&&!loading_&&(pendingStage_||(fullGameFlow.status==FullGameStatus::NextStage&&settled()));}
bool Session::beginStageLoad(){
  if(!stageNeeded()||!records_.empty())return fail("TH12 stage boundary has unconfirmed input");
  if(!network.ReadOnly()&&network.WorldReady()&&!network.SetWorldReady(false))return fail("TH12 stage barrier rejected");
  WorldState::clearPool();
  loading_=true;return true;
}
bool Session::stageLoaded(){
  if(loading_&&network.Spectator()){loading_=false;pendingStage_=0;return true;}
  if(!loading_||!startNextStage())return fail("TH12 next stage could not be initialized");
  loading_=false;pendingStage_=0;return true;
}
bool Session::replayBytes(std::vector<uint8_t>& out,const char* name){return replay_.Encode(out,name,uint32_t(score),(fullGameFlow.status==FullGameStatus::Result||fullGameFlow.status==FullGameStatus::Ending)&&!options.challenge);}
bool Session::restart(){
  if(!restartRequested||!finished||!paused||!menuConfirm||menuAction!=3||menuSelection!=0||!network.Retired()||network.ReadOnly())return fail("TH12 restart is not confirmed");
  if(!network.BeginNextRun((network.Setup().seed+1)&65535))return fail(network.Error());
  replay_.Clear();records_.clear();audio_.clear();committed_=0;started_=false;return start();
}
bool Session::spectatorStep(){
  if(!network.Pump(true))return false;
  auto& playback=network.StagePlayback();
  if(!playback.Ready())return true;
  const auto* segment=playback.History().Locate(playback.Seeking()?playback.Target():playback.Position());
  if(segment&&segment->checkpoint.stage!=unsigned(stageNumber)){pendingStage_=segment->checkpoint.stage;return true;}
  return playback.Step([&](const Netplay::SpectatorStageEvent& event){
    StageEntry entry;
    return event.schema==SpectatorStageSchema&&decodeStageEntry(event.state,entry)&&entry.stage==event.stage&&restoreStageEntry(entry);
  },[&](const Netplay::SpectatorFramePacket& packet,bool historical){
    std::array<Input,3> decoded;
    if(!InputLanes::Decode(packet.inputs.data(),options.count,decoded)||!update(decoded))return false;
    if(!historical){const auto sounds=sound_system::queue.events();audio_.insert(audio_.end(),sounds.begin(),sounds.end());}
    return true;
  },[]{return true;},1)||fail("TH12 spectator playback failed");
}
bool Session::loadReplay(const uint8_t* bytes,size_t count,const uint32_t* build){
  ReplayArchive archive;
  if(!archive.Load(bytes,count)||!build||!std::equal(archive.Setup().build.begin(),archive.Setup().build.end(),build))return fail("TH12 Replay build mismatch");
  stop();playback_=std::move(archive);
  if(!network.BeginPlayback(playback_.Setup())||!start())return false;
  return true;
}
bool Session::seekReplay(uint32_t frame){
  // Native playback reconstructs every preceding input from the shared seed.
  if(!network.Playback()||frame!=0)return fail("TH12 Replay seek requires a stage reconstruction");
  auto setup=playback_.Setup();if(!network.BeginPlayback(setup))return false;
  started_=false;return start();
}
bool Session::replayStep(){
  const auto f=network.NextFrame();
  if(f>=playback_.FrameCount())return true;
  const auto* input=playback_.FrameAt(f);
  if(!input)return fail("TH12 Replay input missing");
  if(fullGameFlow.status==FullGameStatus::NextStage)return true;
  if(!network.FeedPlayback(f,input->data(),options.count))return fail(network.Error());
  const auto decision=network.Prepare(f);if(!decision.canAdvance)return fail("TH12 Replay input frontier failed");
  std::array<Input,3> decoded;
  if(!InputLanes::Decode(input->data(),options.count,decoded)||!update(decoded)||!network.MarkSimulated(f,decision))return fail("TH12 Replay simulation failed");
  const auto sounds=sound_system::queue.events();audio_.insert(audio_.end(),sounds.begin(),sounds.end());
  return true;
}
}
