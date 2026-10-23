// Private browser acceptance only. All simulation goes through mp_tick.
namespace th15::sdl {
namespace {
std::vector<u8> rollback_lab_recording;
touhou::sdl::Renderer::SurfaceCheckpoint rollback_lab_screen;
bool rollback_lab_valid_remote(unsigned seat,u32 frame,unsigned held,int mode,float x,float y){
 Netplay::FrameInput input;
 if(!multiplayer::InputLanes::Capture(held,(held&0x100u)!=0,mode,x,y,input))return false;
 const auto result=network.SubmitRemote(seat,frame,input);
 return result==Netplay::RemoteInputResult::Accepted||result==Netplay::RemoteInputResult::Duplicate||result==Netplay::RemoteInputResult::PredictionCorrect||result==Netplay::RemoteInputResult::RollbackRequired;
}
}
}
extern "C" {
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_begin(unsigned count,unsigned mode){
 using namespace th15::sdl;TH15_SYSTEM_SCOPE;
 if(!app||running||count<2||count>3||mode>2)return 0;
 th15::multiplayer::SessionSetup setup;setup.version=th15::multiplayer::SessionSetup::Version;setup.session_id=0x12345678;
 setup.seed=417;setup.player_count=count;setup.local_player=0;setup.difficulty=1;setup.automatic=false;setup.mode=mode;
 setup.requested_delay=setup.input_delay=0;setup.build={1,2,3,4};setup.selections=count==2?std::array<unsigned,3>{2,3,0}:std::array<unsigned,3>{0,1,3};
 if(!network.Reset(setup))return 0;
 std::array<std::unique_ptr<th15::multiplayer::NetplayRuntime>,3> remotes;
 for(unsigned seat=1;seat<count;++seat){remotes[seat]=std::make_unique<th15::multiplayer::NetplayRuntime>();setup.local_player=seat;if(!remotes[seat]->Reset(setup))return 0;}
 const auto peer=[&](unsigned seat)->th15::multiplayer::NetplayRuntime&{return seat?*remotes[seat]:network;};
 for(unsigned dst=0;dst<count;++dst)for(unsigned src=0;src<count;++src)if(dst!=src&&peer(dst).ApplySession(peer(src).Hello())!=Netplay::SessionPacketResult::Accepted)return 0;
 for(unsigned seat=0;seat<count;++seat){if(!peer(seat).CanSendReady())return 0;peer(seat).MarkLocalReady();}
 for(unsigned dst=0;dst<count;++dst)for(unsigned src=0;src<count;++src)if(dst!=src&&peer(dst).ApplySession(peer(src).Ready())!=Netplay::SessionPacketResult::Accepted)return 0;
 if(!network.CanStart())return 0;
 app->files.read_only=true;app->audio_device.music_enabled=false;app->loading_startup=false;app->loading_debt=0;
 presentation_fps=measured_fps=60;last_error.clear();rollback_lab_recording.clear();
 return th15_mp_start();
}
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_remote(unsigned seat,unsigned frame,unsigned held,int mode,float x,float y){
 using namespace th15::sdl;return seat>0&&seat<network.Setup().player_count&&rollback_lab_valid_remote(seat,frame,held,mode,x,y);
}
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_step(unsigned held,int mode,float x,float y,unsigned physical){
 using namespace th15::sdl;
 if(!app||!network.Configured()||app->exiting)return 0;
 if(app->loading_waiting()&&!app->complete_loading()){last_error=app->failure;return 0;}
 if(physical){app->touch.mode=mode;app->touch.x=x;app->touch.y=y;}
 app->graphics.backend.defer=true;const bool ok=mp_tick(held,physical!=0);
 if(!network.Correcting())app->graphics.backend.commit();else app->graphics.backend.cancel_present();
 app->graphics.backend.defer=false;return ok;
}
EMSCRIPTEN_KEEPALIVE unsigned th15_rollback_lab_replay_size(){
 using namespace th15::sdl;TH15_SYSTEM_SCOPE;rollback_lab_recording.clear();
 if(!mp_recording||!mp_replay.Encode(rollback_lab_recording,"ROLLBACK",mp_committed_score,mp_committed_complete))return 0;
 return unsigned(rollback_lab_recording.size());
}
EMSCRIPTEN_KEEPALIVE const unsigned char* th15_rollback_lab_replay_bytes(){return th15::sdl::rollback_lab_recording.data();}
EMSCRIPTEN_KEEPALIVE const char* th15_rollback_lab_backend_error(){return Netplay::WriteJournal::Error();}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_rollback_lab_details(){
 using namespace th15::sdl;static std::array<unsigned,10> value{};value.fill(0);if(!app)return value.data();
 value={network.Confirmed(),mp_recording?mp_replay.FrameCount():0u,app->pause?unsigned(app->pause->state.screen):0u,
 app->pending_load,app->frames,app->scene()?unsigned(app->progress.stage):0u,unsigned(app->fonts.rollback_layout().size()),
 app->scene()?unsigned(app->progress.stage_frame):0u,network.RollbackFrame(),network.CorrectionEnd()};return value.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned char* th15_rollback_lab_pixels(){
 using namespace th15::sdl;if(!app)return nullptr;rollback_lab_screen=app->graphics.backend.save_surfaces({GraphicsDevice::screen});
 return rollback_lab_screen.empty()?nullptr:rollback_lab_screen[0].rgba.data();
}
EMSCRIPTEN_KEEPALIVE unsigned th15_rollback_lab_pixels_size(){
 using namespace th15::sdl;return rollback_lab_screen.empty()?0:unsigned(rollback_lab_screen[0].rgba.size());
}
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_retire_title(){
 using namespace th15::sdl;if(!app||network.Correcting()||network.RollbackFrame()!=Netplay::INVALID_FRAME||!mp_frames.empty()||journal::FrameCount())return 0;
 const auto* title=app->animations.resource(16);if(!title)return 0;app->graphics.unload(*title);return app->graphics.error.empty();
}
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_next_stage(){
 using namespace th15::sdl;if(!app||network.Correcting()||network.RollbackFrame()!=Netplay::INVALID_FRAME||!mp_frames.empty()||journal::FrameCount())return 0;
 TH15_GAME_SCOPE;return app->complete_stage();
}
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_request_ending(){
 using namespace th15::sdl;if(!app||!app->scene()||network.Correcting()||network.RollbackFrame()!=Netplay::INVALID_FRAME||!mp_frames.empty()||journal::FrameCount())return 0;
 // Test the real loading/resource/output boundary. The service request does
 // not skip apply_destination or construct an ending outside the driver.
 TH15_GAME_SCOPE;return app->destination(th15::SessionDestination::NormalEnding);
}
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_finish_ending(){
 using namespace th15::sdl;if(!app||!app->ending||app->pending_load||app->pending_bank>=0||network.Correcting()||network.RollbackFrame()!=Netplay::INVALID_FRAME||!mp_frames.empty()||journal::FrameCount())return 0;
 TH15_GAME_SCOPE;return app->ending_destination(16);
}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_rollback_lab_lifecycle(){
 using namespace th15::sdl;static std::array<unsigned,8> value{};value.fill(0);if(!app)return value.data();
 value={app->ending?1u:0u,app->ending?unsigned(app->ending->script.run.first_seen_flags):0u,unsigned(app->pending_destination),
 unsigned(app->pending_bank),app->title?unsigned(app->title->state.screen):0u,app->title?unsigned(app->title->state.substate):0u,
 unsigned(journal::FrameCount()),network.Retired()?1u:0u};return value.data();
}
}
