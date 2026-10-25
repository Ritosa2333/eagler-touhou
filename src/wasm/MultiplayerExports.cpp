#include "../multiplayer/Session.hpp"
#include "../game/GameState.hpp"
#include "../game/StageFlow.hpp"
namespace {
std::vector<uint8_t> replayBytes;
std::array<uint32_t,8> rollbackStatus{};
}
extern "C" {
int th12_mp_configure(const uint32_t* words,int count){return count>0&&th12::multiplayer::session.configure(words,unsigned(count));}
int th12_mp_connect(const char* url){return th12::multiplayer::session.network.Connect(url);}
int th12_mp_spectator_connect(const char* url,const char* id){return th12::multiplayer::session.network.ConnectSpectator(url,id);}
int th12_mp_start(){return th12::multiplayer::session.start();}
int th12_mp_restart(){return th12::multiplayer::session.restart();}
int th12_mp_pump(){return th12::multiplayer::session.pump();}
int th12_mp_step(uint32_t held,int pause,int motion,float x,float y,double now){return th12::multiplayer::session.step(held,pause!=0,motion,x,y,now);}
void th12_mp_stop(){th12::multiplayer::session.stop();}
const char* th12_mp_error(){return th12::multiplayer::session.error();}
const uint32_t* th12_mp_status(){return th12::multiplayer::session.network.Status();}
const uint32_t* th12_mp_calibration_status(){return th12::multiplayer::session.network.CalibrationStatus();}
const uint32_t* th12_mp_rollback_status(){
  auto& s=th12::multiplayer::session;
  rollbackStatus={1,s.restores(),s.resimulated(),s.committed(),unsigned(s.network.Correcting()),s.network.RollbackFrame(),s.network.NextFrame(),unsigned(s.network.WorldReady())};return rollbackStatus.data();
}
uint32_t th12_mp_hash(){return th12::multiplayer::canonicalHash();}
const char* th12_mp_state(){return th12::multiplayer::snapshot().c_str();}
const void* th12_mp_visual_owners(){return th12::multiplayerVisualOwners.data();}
int th12_mp_visual_owners_count(){return int(th12::multiplayerVisualOwners.size());}
int th12_mp_stage_needed(){return th12::multiplayer::session.stageNeeded()?int(th12::multiplayer::session.nextStage()):0;}
int th12_mp_stage_begin(){return th12::multiplayer::session.beginStageLoad();}
int th12_mp_stage_loaded(){return th12::multiplayer::session.stageLoaded();}
const void* th12_mp_sounds(){return th12::multiplayer::session.audio().data();}
int th12_mp_sounds_count(){return int(th12::multiplayer::session.audio().size());}
void th12_mp_sounds_clear(){th12::multiplayer::session.clearAudio();}
int th12_mp_replay_encode(const char* name){return th12::multiplayer::session.replayBytes(replayBytes,name)?int(replayBytes.size()):0;}
const uint8_t* th12_mp_replay_data(){return replayBytes.data();}
int th12_mp_replay_validate(const uint8_t* bytes,int size){return size>0&&th12::multiplayer::ReplayArchive::Validate(bytes,size);}
int th12_mp_replay_load(const uint8_t* bytes,int size,const uint32_t* build){return size>0&&th12::multiplayer::session.loadReplay(bytes,size,build);}
int th12_mp_replay_frames(){return int(th12::multiplayer::session.replayFrames());}
int th12_mp_replay_rewind(){return th12::multiplayer::session.seekReplay(0);}
double th12_mp_paced_elapsed(double elapsed){return th12::multiplayer::session.network.PacedElapsed(elapsed);}
const char* th12_mp_spectator_state(){return th12::multiplayer::session.network.SpectatorPlaybackState();}
int th12_mp_spectator_control(const char* run,const char* action,uint32_t frame){return th12::multiplayer::session.network.SpectatorPlaybackControl(run,action,frame);}
}
