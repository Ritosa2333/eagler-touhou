// Included only by the development BrowserRuntime. The real gate, input core,
// frame journal, correction loop, sound commit and renderer execute unchanged.
#include <malloc.h>
#include <emscripten/heap.h>
void th15_probe_capture_reset();
extern "C++" {
namespace {
unsigned profile_remote=0,profile_first=0;
bool profile_dense=false;
th15::multiplayer::SessionSetup profile_setup;
struct ProfileView {th15::AnmEnvironment& environment;th15::Rng& visual;th15::SessionState& progress;th15::GameBattle* battle;th15::AnmManager& animations;};
ProfileView profile_world(){auto& a=*th15::sdl::app;return {a.environment,a.visual,a.progress,&a.scene()->battle,a.animations};}
Netplay::FrameInput profile_input(unsigned frame,unsigned seat,bool mismatch){
 Netplay::FrameInput input;
 input.buttons=1|(mismatch&&seat?((frame&1)?0x40:0x80):0);
 return input;
}
bool profile_accepted(Netplay::RemoteInputResult result){
 return result==Netplay::RemoteInputResult::Accepted||result==Netplay::RemoteInputResult::Duplicate||
        result==Netplay::RemoteInputResult::PredictionCorrect||result==Netplay::RemoteInputResult::RollbackRequired;
}
bool profile_receive(unsigned through,bool mismatch){
 if(through==~0u)return true;
 while(profile_remote<=through){
  for(unsigned seat=1;seat<profile_setup.player_count;++seat){
   const auto result=th15::sdl::network.SubmitRemote(seat,profile_remote,profile_input(profile_remote,seat,mismatch));
   if(!profile_accepted(result))return false;
  }
  ++profile_remote;
 }
 return true;
}
}
}
EMSCRIPTEN_KEEPALIVE int th15_probe_rollback_prepare(unsigned players,unsigned stage,unsigned dense){
 using namespace th15::sdl;
 if(!app||running||players<2||players>3||stage<1||stage>7)return 0;
 profile_setup={};auto& s=profile_setup;s.session_id=417;s.seed=417;s.mode=0;
 s.player_count=players;s.local_player=0;s.difficulty=stage==7?4:1;s.automatic=false;
 s.selections=stage==7?std::array<unsigned,3>{2,2,3}:std::array<unsigned,3>{0,1,3};s.build={1,2,3,4};
 for(unsigned seat=players;seat<3;++seat)s.selections[seat]=0;
 if(!network.Reset(s))return 0;
 std::array<th15::multiplayer::NetplayRuntime,3> peers;
 for(unsigned seat=1;seat<players;++seat){auto remote=s;remote.local_player=seat;if(!peers[seat].Reset(remote))return 0;}
 for(unsigned seat=1;seat<players;++seat)if(network.ApplySession(peers[seat].Hello())!=Netplay::SessionPacketResult::Accepted)return 0;
 network.MarkLocalReady();
 for(unsigned seat=1;seat<players;++seat){peers[seat].MarkLocalReady();if(network.ApplySession(peers[seat].Ready())!=Netplay::SessionPacketResult::Accepted)return 0;}
 if(!network.CanStart()||!th15_mp_start())return 0;
 if(stage!=s.initial_stage()){
  app->selected_stage=stage;app->mp_options.stage=stage;
  if(!app->begin_run())return 0;
  mp_stage=stage;
 }
 profile_remote=profile_first=0;profile_dense=dense!=0;
 app->graphics.presentation.enabled=false;app->audio_device.music_enabled=false;app->audio_device.suspend(false);
 for(auto& pilot:app->scene()->battle.pilots)if(pilot)pilot->player->life.invulnerability.set(30000);
 return 1;
}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_probe_memory_status(){
 using namespace th15::sdl;static std::array<unsigned,8> value{};
 const auto memory=mallinfo();
 unsigned snapshots=0;for(const auto& frame:mp_frames)if(frame.captured)++snapshots;
 value={unsigned(memory.uordblks),unsigned(memory.fordblks),unsigned(emscripten_get_heap_size()),
        app?unsigned(app->progress.stage):0,network.NextFrame(),snapshots,
        app&&app->completed_recording?1u:0u,app?unsigned(app->progress.stage_frame):0};
 return value.data();
}
EMSCRIPTEN_KEEPALIVE void th15_probe_memory_protect(){
 using namespace th15::sdl;
 if(app&&app->scene())for(auto& pilot:app->scene()->battle.pilots)
  if(pilot)pilot->player->life.invulnerability.set(1000000);
}
EMSCRIPTEN_KEEPALIVE const double* th15_probe_rollback_work(unsigned depth,unsigned mismatch){
 using namespace th15::sdl;static std::array<double,15> result{};result.fill(0);
 if(!app||running||depth>12||!mp_started)return nullptr;
 if(app->loading_waiting()&&!app->complete_loading())return nullptr;
 const auto frame=network.NextFrame();const bool ready=AppFrameState::ready(*app);
 if(!depth)profile_first=frame+1;
 const unsigned through=ready&&depth&&frame>=profile_first+depth?frame-depth:depth&&ready?profile_remote-1:frame;
 if(!profile_receive(through,mismatch!=0))return nullptr;
 if(profile_dense&&ready&&frame==180){
  auto& b=app->scene()->battle;th15::BulletShooter shot;shot.count=1932;shot.rows=1;shot.speed=.3f;shot.angle_step=6.283185307179586f/1932;shot.position={0,100,0};shot.pattern=2;
  if(!b.bullet_scene->manager.emit(shot,0))return nullptr;
  for(unsigned i=0;i<24;++i)b.items->spawn(i%2?2:9,{float(int(i)*12-140),80,0},.3f,1);
 }
 const auto corrections=mp_corrections,replayed=mp_replayed_frames;
 mp_profile_capture_ms=mp_profile_restore_ms=0;mp_profile_captures=0;
 th15_probe_capture_reset();
 const auto begin=emscripten_get_now();app->audio_device.pump();app->graphics.frame_drawn=false;app->graphics.backend.defer=true;
 const bool ok=mp_tick(profile_input(frame,0,mismatch!=0).buttons);
 const bool presented=app->graphics.backend.commit();app->graphics.backend.defer=false;app->audio_device.pump();
 result={emscripten_get_now()-begin,double(network.NextFrame()-frame),double(mp_corrections-corrections),double(mp_replayed_frames-replayed),double(mp_last_depth),double(network.NextFrame()),double(network.Confirmed()),ready?1.:0.,presented?1.:0.,app->scene()?double(app->scene()->battle.bullet_scene->manager.visible_count):0.,double(app->animations.registry.count()),ok?1.:0.,mp_profile_capture_ms,mp_profile_restore_ms,double(mp_profile_captures)};
 return result.data();
}
EMSCRIPTEN_KEEPALIVE int th15_probe_rollback_receive(unsigned through,unsigned mismatch){
 using namespace th15::sdl;
 return app&&mp_started&&through<=network.NextFrame()&&profile_receive(through,mismatch!=0);
}
EMSCRIPTEN_KEEPALIVE int th15_probe_rollback_input(unsigned seat,unsigned frame,unsigned buttons){
 using namespace th15::sdl;
 return app&&mp_started&&frame<=network.CaptureFrame()&&buttons<=0xffff&&
        profile_accepted(network.SubmitRemote(seat,frame,Netplay::FrameInput(std::uint16_t(buttons))));
}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_probe_rollback_clock(){
 using namespace th15::sdl;static std::array<unsigned,4> value;
 value={network.NextFrame(),network.CaptureFrame(),network.Confirmed(),mp_replay_until};return value.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_probe_rollback_fingerprint(){
 using namespace th15;using namespace th15::sdl;static std::array<unsigned,2> result{};
 if(!app||!app->scene())return nullptr;
 auto view=profile_world();
 rollback_test::ignore_presentation_identity=true;
 const auto hash=rollback_test::digest(view);result={unsigned(hash),unsigned(hash>>32)};return result.data();
}
EMSCRIPTEN_KEEPALIVE const char* th15_probe_rollback_trace(){
 using namespace th15;static std::string result;rollback_test::DigestTrace trace;auto view=profile_world();
 rollback_test::digest_trace=&trace;rollback_test::digest(view);rollback_test::digest_trace=nullptr;
 result="[";for(const auto& part:trace){if(result.size()>1)result+=',';char value[32];std::snprintf(value,sizeof value,"%016llx",static_cast<unsigned long long>(part.second));result+="[\""+part.first+"\",\""+value+"\"]";}result+=']';return result.c_str();
}
