// Private browser acceptance only. All simulation goes through mp_tick.
namespace th15::sdl {
namespace {
std::vector<u8> rollback_lab_recording;
touhou::sdl::Renderer::SurfaceCheckpoint rollback_lab_screen;
struct RollbackTextureObservation {
 std::array<std::vector<u8>,4> bytes;
 std::array<u32,2> uploads{};
 bool valid=false;
};
struct RollbackTextureProbe {
 AnmResource resource;FrameCallback callback;std::array<u32,2> handles{};
 std::array<bool,2> owned{};
 RollbackTextureObservation current,edited;
 bool armed=false,frame_open=false;u32 executions=0,frame=Netplay::INVALID_FRAME,retained=0;
};
std::unique_ptr<RollbackTextureProbe> rollback_lab_textures;
FrameCallback rollback_lab_presentation_callback;
std::vector<unsigned> rollback_lab_presentation_trace;
bool rollback_lab_observe_textures(RollbackTextureProbe& probe,RollbackTextureObservation& result){
 TH15_SYSTEM_SCOPE;auto& graphics=app->graphics;result.valid=false;
 for(unsigned index=0;index<2;++index){
  const auto* image=graphics.pixels(probe.handles[index]);if(!image)return false;
  result.bytes[index]=image->pixels;
  const auto before=graphics.backend.stats.uploadBytes;
  graphics.backend.prepare(probe.handles[index]);
  result.uploads[index]=graphics.backend.stats.uploadBytes-before;
 }
 auto surfaces=graphics.backend.save_surfaces({probe.handles[0],probe.handles[1]});
 if(surfaces.size()!=2)return false;
 for(unsigned index=0;index<2;++index)result.bytes[index+2]=std::move(surfaces[index].rgba);
 result.valid=true;return true;
}
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
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_presentation_begin(){
 using namespace th15;using namespace th15::sdl;TH15_SYSTEM_SCOPE;
 if(!app||running||network.NextFrame()!=0||app->scheduler.contains(rollback_lab_presentation_callback))return 0;
 rollback_lab_presentation_trace.clear();rollback_lab_presentation_callback.enabled=true;
 rollback_lab_presentation_callback.run=[](void*)->i32{
  TH15_SYSTEM_SCOPE;const auto frame=network.NextFrame();
  // Select by the actual simulated frame, including every history step in a
  // bounded correction call. Changes happen in Update before Draw::begin.
  const bool enabled=(frame>=180&&frame<239)||(frame>=241&&frame<248)||(frame>=260&&frame<280);
  app->graphics.presentation.enabled=enabled;
  rollback_lab_presentation_trace.insert(rollback_lab_presentation_trace.end(),{frame,enabled?1u:0u,network.Correcting()?1u:0u});
  return i32(FrameAction::Continue);
 };
 return app->scheduler.add(rollback_lab_presentation_callback,FramePass::Update,-1000000)>=0;
}
EMSCRIPTEN_KEEPALIVE unsigned th15_rollback_lab_presentation_size(){return unsigned(th15::sdl::rollback_lab_presentation_trace.size());}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_rollback_lab_presentation_trace(){return th15::sdl::rollback_lab_presentation_trace.data();}
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
EMSCRIPTEN_KEEPALIVE const unsigned* th15_rollback_lab_journal(){
 static std::array<unsigned,4> value{};const auto stats=Netplay::WriteJournal::Stats();
 value={stats.frames,stats.restores,stats.restored_frames,unsigned(Netplay::WriteJournal::FrameCount())};return value.data();
}
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
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_texture_begin(unsigned armed){
 using namespace th15;using namespace th15::sdl;TH15_SYSTEM_SCOPE;
 if(!app||running||rollback_lab_textures||network.Correcting()||network.RollbackFrame()!=Netplay::INVALID_FRAME||!mp_frames.empty()||journal::FrameCount())return 0;
 auto probe=std::make_unique<RollbackTextureProbe>();
 // Fresh auxiliary ANM images establish that neither texture has ever passed
 // through changed(). They use the production loader, journaled pixels and GPU
 // cache; normal game assets and their draw commands remain untouched.
 for(unsigned index=0;index<2;++index){
  AnmTexture image;image.name=index?"rollback-static-control":"rollback-first-edit";
  image.width=image.pixel_width=16;image.height=image.pixel_height=16;
  image.format=image.pixel_format=1;image.pixels.resize(16*16*4);
  for(unsigned at=0;at<image.pixels.size();++at)image.pixels[at]=(at%4==3)?255:u8(1+(at+index*71)%254);
  probe->resource.textures.push_back(std::move(image));
 }
 if(!app->graphics.preload(probe->resource))return 0;
 const auto abandon=[&](){app->graphics.unload(probe->resource);return 0;};
 for(unsigned index=0;index<2;++index){
  probe->handles[index]=app->graphics.texture(probe->resource,index);
  const auto* image=app->graphics.pixels(probe->handles[index]);
  if(!image||image->pixels.empty())return abandon();
  probe->owned[index]=journal::OwnedByGameHeap(image->pixels.data());
 }
 probe->armed=armed!=0;probe->callback.owner=probe.get();probe->callback.enabled=true;
 probe->callback.run=[](void* owner)->i32{
  auto& probe=*static_cast<RollbackTextureProbe*>(owner);
  if(!probe.armed)return i32(FrameAction::Continue);
  TH15_SYSTEM_SCOPE;probe.armed=false;++probe.executions;
  probe.frame=network.NextFrame();probe.frame_open=journal::FrameOpen();probe.retained=u32(journal::FrameCount());
  // A one-shot probe executes inside a real retained game frame. Corrections
  // replay the normal game draw without repeating this deliberate test edit.
  if(!probe.frame_open||!app->graphics.clear_image(probe.handles[0])||!rollback_lab_observe_textures(probe,probe.edited)){
   last_error="TH15 first-edit texture probe failed";return i32(FrameAction::Error);
  }
  return i32(FrameAction::Continue);
 };
 if(app->scheduler.add(probe->callback,FramePass::Draw,1000000)<0)return abandon();
 rollback_lab_textures=std::move(probe);return 1;
}
EMSCRIPTEN_KEEPALIVE const unsigned* th15_rollback_lab_texture_observe(unsigned edited){
 using namespace th15;using namespace th15::sdl;TH15_SYSTEM_SCOPE;static std::array<unsigned,13> values{};
 if(!app||!rollback_lab_textures||journal::FrameOpen()||edited>1)return nullptr;
 auto& probe=*rollback_lab_textures;auto& observation=edited?probe.edited:probe.current;
 if(!edited&&!rollback_lab_observe_textures(probe,observation))return nullptr;
 values={observation.valid?1u:0u,probe.executions,probe.frame,probe.frame_open?1u:0u,probe.retained,
  probe.owned[0]?1u:0u,probe.owned[1]?1u:0u,observation.uploads[0],observation.uploads[1],
  u32(observation.bytes[0].size()),u32(observation.bytes[1].size()),u32(observation.bytes[2].size()),u32(observation.bytes[3].size())};
 return values.data();
}
EMSCRIPTEN_KEEPALIVE const unsigned char* th15_rollback_lab_texture_bytes(unsigned edited,unsigned index){
 using namespace th15::sdl;if(!rollback_lab_textures||edited>1||index>=4)return nullptr;
 const auto& probe=*rollback_lab_textures;const auto& observation=edited?probe.edited:probe.current;
 return observation.bytes[index].data();
}
EMSCRIPTEN_KEEPALIVE int th15_rollback_lab_texture_end(){
 using namespace th15::sdl;TH15_SYSTEM_SCOPE;
 if(!app||!rollback_lab_textures||network.Correcting()||network.RollbackFrame()!=Netplay::INVALID_FRAME||!mp_frames.empty()||journal::FrameCount())return 0;
 app->scheduler.remove(rollback_lab_textures->callback);app->graphics.unload(rollback_lab_textures->resource);
 rollback_lab_textures.reset();return app->graphics.error.empty();
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
