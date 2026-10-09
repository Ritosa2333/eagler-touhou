#include "ApplicationState.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <cstring>
#if TH15_DEVELOPMENT_HARNESS
#include <emscripten.h>
namespace {std::array<double,4> frame_profile{};}
extern "C" EMSCRIPTEN_KEEPALIVE const double* th15_probe_performance(){return frame_profile.data();}
#define PROFILE_BEGIN const double profile_begin=emscripten_get_now()
#define PROFILE_AT(slot) frame_profile[slot]=emscripten_get_now()
#else
#define PROFILE_BEGIN
#define PROFILE_AT(slot)
#endif
namespace th15::sdl {
Application::Application():state(std::make_unique<ApplicationState>()){}
Application::~Application()=default;
bool Application::initialize(bool device){return state->initialize(device);}
bool Application::step(u32 held,u32 pressed,u32 repeated,float fps,bool focus){return state->step(held,pressed,repeated,fps,focus);}
const std::string& Application::error()const{return state->failure;}
const i32* Application::projection()const{return state->projection();}
bool ApplicationState::fail(const std::string& why){if(failure.empty())failure=why.empty()?"Application operation failed":why;return false;}
bool ApplicationState::read(const std::string& name,std::vector<u8>& bytes){
 const auto at=name.find_last_of("/\\");const auto base=name.substr(at==std::string::npos?0:at+1);
 if(base.empty()||base=="."||base=="..")return fail("Invalid game asset name");
 const auto found=resource_cache.find(base);if(found!=resource_cache.end()){bytes=found->second;return true;}
#if defined(TH_ENABLE_THCRAP)
 // Same per-title prepared-resource override as TH10/11. Never fetch patches here.
 const bool message=base.size()>4&&base.substr(base.size()-4)==".msg";
 const bool help=base.rfind("help_",0)==0&&base.size()>4&&base.substr(base.size()-4)==".png";
 // This Runtime reconstructs JP 1.00b. Never mount the 1.00a text bank on it.
 const bool text_bank=base=="text.anm";
 if(message||help||text_bank){size_t size=0;auto* patched=static_cast<u8*>(SDL_LoadFile(("/thcrap/th15/"+(text_bank?std::string("text.v1.00b.anm"):base)).c_str(),&size));
  if(patched){if(size>16u*1024u*1024u){SDL_free(patched);return fail("THCRAP resource outside allowed size");}bytes.assign(patched,patched+size);SDL_free(patched);resource_cache.emplace(base,bytes);return true;}}
#endif
 if(!archive_bytes.empty()){const auto entry=archive_names.find(base);if(entry==archive_names.end()||!archive.read(entry->second,bytes))return fail("Missing or invalid archive entry: "+base);if(bytes.size()<=1024*1024&&base.size()>=4&&base.substr(base.size()-4)!=".wav"&&base.substr(base.size()-4)!=".anm")resource_cache.emplace(base,bytes);return true;}
 size_t size=0;auto* data=static_cast<u8*>(SDL_LoadFile(("/assets/"+base).c_str(),&size));if(!data)return fail("Missing game asset: "+base);
 bytes.assign(data,data+size);SDL_free(data);if(bytes.size()<=1024*1024&&base.size()>=4&&base.substr(base.size()-4)!=".wav"&&base.substr(base.size()-4)!=".anm")resource_cache.emplace(base,bytes);return true;
}
bool ApplicationState::prepare_platform(){
 if(platform_prepared)return true;
 if(SDL_GetPathInfo("/th15.dat",nullptr)){
 // Read directly into the archive owner: SDL_LoadFile plus assign kept two
 // full temporary archive buffers alive and forced an unnecessary heap growth.
 auto* file=SDL_IOFromFile("/th15.dat","rb");if(!file)return fail("Unable to open original th15.dat");const auto size=SDL_GetIOSize(file);
 if(size<16||size>128*1024*1024){SDL_CloseIO(file);return fail("Invalid original th15.dat size");}
 archive_bytes.resize(size_t(size));const bool complete=SDL_ReadIO(file,archive_bytes.data(),archive_bytes.size())==archive_bytes.size();const bool closed=SDL_CloseIO(file);
 if(!complete||!closed)return fail("Unable to read original th15.dat");if(!archive.open(archive_bytes.data(),archive_bytes.size()))return fail("Invalid original th15.dat archive");for(u32 i=0;i<archive.entries.size();i++){const auto& name=archive.entries[i].name;const auto slash=name.find_last_of("/\\");const auto base=name.substr(slash==std::string::npos?0:slash+1);const auto inserted=archive_names.emplace(base,i);if(!inserted.second){const auto first=inserted.first->second;std::vector<u8> left,right;
 // The original archive contains two identical se_cardget.wav entries.
 // Preserve the first entry while rejecting genuinely ambiguous resources.
 if(name!=archive.entries[first].name||!archive.read(first,left)||!archive.read(i,right)||left!=right)return fail("Ambiguous archive asset: "+base);}}}
 if(!graphics.initialize())return fail(graphics.error);platform_prepared=true;return true;
}
bool ApplicationState::prepare_loading(){
 if(!prepare_platform())return false;std::vector<u8> bytes;
 loading_environment.resolution_scale=1;loading_environment.raster_scale=float(graphics.render_scale);
 for(const auto& bank:std::array<std::pair<const char*,i32>,2>{{{"sig.anm",1},{"ascii.anm",2}}}){
  if(!loading_animations.resource(bank.second)&&(!read(bank.first,bytes)||!loading_animations.load(bank.second,bytes.data(),bytes.size())||!graphics.preload(*loading_animations.resource(bank.second))))return fail("Loading artwork: "+loading_animations.error+graphics.error);
 }
 return draw_loading(true);
}
bool ApplicationState::loading_waiting()const{return loading_startup||pending_load!=0;}
bool ApplicationState::draw_loading(bool signature){
 graphics.presentation.reset();
 if(!loading_animations.retire(loading_signature)||!loading_animations.retire(loading_prayer))return fail(loading_animations.error);
 if(signature){loading_signature=loading_animations.create(1,0);if(!loading_signature)return fail(loading_animations.error);}
 loading_prayer=loading_animations.create(2,17,-1,0,{960,784,0});if(!loading_prayer)return fail(loading_animations.error);
 loading_startup=signature;loading_frames=0;loading_debt=0;
 return render_loading();
}
bool ApplicationState::render_loading(){
 renderer.invalidate();graphics.backend.pipeline()=touhou::graphics::PipelineState{};graphics.configure_game(1);
 if(!graphics.select_target(nullptr,0)||!graphics.clear_target(0xff000000,nullptr))return fail(graphics.error);
 ScreenViews loading_views{renderer,loading_environment};if(!loading_views.camera(DrawCamera::Fullscreen,false))return fail("Loading camera unavailable");
 const auto* credit=loading_animations.registry.find(loading_signature);
 for(u32 layer=0;layer<42;++layer){
  if(!renderer.draw_layer(loading_animations.registry.layer(layer)))return fail(renderer.error);
  if(credit&&layer==u32(credit->visual.layer)){renderer.flush();graphics.draw_startup_branding(credit->visual.color);}
 }
 renderer.flush();graphics.present();
 return true;
}
bool ApplicationState::render_loading(bool signature,unsigned frames){
 (void)signature;
 for(unsigned frame=0;frame<std::min(frames,120u);++frame){
  if(!loading_animations.update(false)||!loading_animations.update(true))return fail(loading_animations.error);
  ++loading_frames;
 }
 return render_loading();
}
bool ApplicationState::advance_loading(double delta){
 // Native title worker 46052a waits for owner +644 == 180. Advance only
 // the isolated loading ANM clock at 60Hz, never gameplay or replay clocks.
 loading_debt+=std::clamp(delta,0.,.1);
 while(loading_debt+1e-9>=1./60.&&loading_frames<180){
  loading_debt=std::max(0.,loading_debt-1./60.);
  if(!loading_animations.update(false)||!loading_animations.update(true))return fail(loading_animations.error);
  ++loading_frames;
 }
 if(!render_loading())return false;
 if(loading_frames>=180){loading_startup=false;loading_debt=0;}
 return true;
}
bool ApplicationState::initialize(bool device){
 if(initialized)return fail("Application already initialized");if(!prepare_platform())return false;if(!loading_animations.resource(2)&&!prepare_loading())return false;
 if(!fonts.initialize()||!audio_device.initialize(*this,device))return fail(fonts.error.empty()?audio_device.error:fonts.error);
 audio_device.initial_music_byte_offset=[this](){return practice.enabled&&scene()?scene()->practice_music_start_offset():0u;};
 // Restart overlays draw their patterned fifth panel when the screen has alpha.
 // Derive this from the actual SDL surface, as the original format check does.
 environment.raster_scale=float(graphics.render_scale);
 environment.render_target_has_alpha=graphics.pixels(GraphicsDevice::screen)->format==touhou::graphics::PixelFormat::Bgra8;
 environment.screen_offsets={320,16,320,16};
 environment.playfield_origin={128,16};
 if(!files.initialize(records,config,visual))return fail(files.error);
 records.checkpoint_remove=[this](i32 character,i32 difficulty){if(checkpoint_encoder&&checkpoint_header.character()==character&&checkpoint_header.difficulty()==difficulty){checkpoint_encoder.reset();checkpoint_payload.clear();}return files.remove_checkpoint(character,difficulty);};
 volumes.music_volume=config.music_volume();volumes.sound_volume=config.sound_volume();volumes.controller_option=config.bytes[0x24];std::memcpy(controller.values.data(),config.bytes.data()+4,20);
 volume(volumes.music_volume,volumes.sound_volume,0);
 animations.resource_release=[this](const AnmResource& resource){graphics.unload(resource);};
 std::vector<u8> bytes;for(const auto& bank:std::array<std::pair<const char*,i32>,7>{{{"text.anm",0},{"sig.anm",1},{"ascii.anm",2},{"effect.anm",8},{"title.anm",16},{"title_v.anm",17},{"help.anm",19}}}){
  if(!read(bank.first,bytes)||!animations.load(bank.second,bytes.data(),bytes.size()))return fail("Application ANM load: "+std::string(bank.first)+": "+animations.error);
  animations.name_resource(bank.second,bank.first);shared.emplace(bank.first,bank.second);if(bank.second==2)environment.fallback_sprite_resource=animations.resource(2);
  if(!graphics.preload(*animations.resource(bank.second)))return fail(graphics.error);
 }
 for(i32 style=0;style<23;style++)if(!fonts.preload(style))return fail(fonts.error);
 if(!read("musiccmt.txt",bytes)||!comments.open(bytes.data(),bytes.size()))return fail(comments.error);
 scene_effects=std::make_unique<AnmSceneEffects>(animations,environment.game_rng,visual,8);
 display=std::make_unique<SceneDisplay>(scheduler,animations,environment,graphics,renderer,views,captions,0,2);if(!display->initialize())return fail(display->error);
 for(u32 index=0;index<animation_updates.size();index++){auto& callback=animation_updates[index];callback.owner=this;callback.enabled=true;callback.run=index==0?[](void* p)->i32{return static_cast<ApplicationState*>(p)->animations.update(true)?1:5;}:[](void* p)->i32{return static_cast<ApplicationState*>(p)->animations.update(false)?1:5;};if(scheduler.add(callback,FramePass::Update,index==0?9:34)<0)return fail("Application animation callback registration failed");}
 camera.direction={0,0,1};camera.up={0,1,0};camera.fov=0.785398185f;
 initialized=true;return begin_title(0);
}
ApplicationState::~ApplicationState(){title.reset();ending.reset();release_run();for(auto& c:animation_updates)scheduler.remove(c);fades.clear();display.reset();scene_effects.reset();animations.resource_release=nullptr;}
bool ApplicationState::save_settings(){if(!finish_checkpoint())return false;config.bytes[0x22]=u8(volumes.music_volume);config.bytes[0x23]=u8(volumes.sound_volume);config.bytes[0x24]=volumes.controller_option;std::memcpy(config.bytes.data()+4,controller.values.data(),20);return files.save(records,config)||fail(files.error);}
void ApplicationState::release_run(){
 graphics.presentation.reset();
 const bool had_run=run!=nullptr;close_options();motion.clear();if(display)display->detach();pause.reset();flow.reset();initialization.reset();session.reset();
 if(run&&scene()){selection_player=scene()->battle.session;selection_score=scene()->battle.score;}
 run.reset();if(had_run){fades.clear();for(const auto bank:{0,2,8})animations.retire_resource(bank);restart_handle=restart_effect_handle=transition_overlay=0;}animations.collect_resources();for(auto& callback:animation_updates)callback.enabled=true;
 if(!scene_effects&&initialized)scene_effects=std::make_unique<AnmSceneEffects>(animations,environment.game_rng,visual,8);
 environment.paused=false;environment.screen_translation={};environment.background_delta={};environment.screen_offsets={320,16,320,16};environment.playfield_origin={128,16};views.configure(environment);renderer.invalidate();graphics.backend.pipeline()=touhou::graphics::PipelineState{};graphics.configure_game(1);
}
bool ApplicationState::begin_title(i32 reason){
 remember_title();ending.reset();title.reset();release_run();current_destination=4;
 title=std::make_unique<TitleScene>(progress,selection_player,selection_score,records,selection,volumes,controller,comments,animations,*this,scheduler,TitleSceneResources{16,17,2,0,19});
 title->frame.demo_idle=title_demo_idle;title->frame.demo_index=title_demo_index;title->frame.saved_difficulty=title_saved_difficulty;title->saved_replay_selection=title_saved_replay_selection;
 title->state.return_reason=reason;title->live=replay?replay->live():nullptr;return title->error.empty()||fail(title->error);
}
bool ApplicationState::preload_run(){
 if(!assets())return fail("Run has no loaded resources");for(const auto& loaded:assets()->loaded_animations()){auto* resource=animations.resource(loaded.second);if(!resource||!graphics.preload(*resource))return fail(graphics.error.empty()?"Run animation unavailable: "+loaded.first:graphics.error);}return true;
}
bool ApplicationState::bind_run_display(){
 if(!scene()||!session||!session->driver()||!assets())return fail("Game display binding requires an active session");
 pause=std::make_unique<RunPause>(*scene(),*session->driver(),progress,records,animations,*this,frame_skip,assets()->front);if(!pause->prepare())return fail(pause->error);
 session->driver()->auto_focus=config.auto_focus();if(replay->live())live_description=replay->live()->description();else if(replay->replay_file()){const auto& bytes=replay->replay_file()->decoded();if(bytes.size()>=live_description.size())std::copy_n(bytes.begin(),live_description.size(),live_description.begin());}
 return display->attach(*scene(),progress,records,*this,*this,pause.get(),&live_description)||fail(display->error);
}
bool ApplicationState::begin_run(){
 if(!save_settings())return false;remember_title();title.reset();ending.reset();release_run();scene_effects.reset();for(auto& callback:animation_updates)callback.enabled=false;
 replay=std::make_unique<SessionReplay>(config,environment.game_rng,visual);
 progress.stage=selected_stage;progress.starting_stage=selected_stage;progress.scene_flags=0;progress.restart_frames=0;progress.rate=1;
 practice.replay=progress.replay;
 if(!progress.replay&&practice.enabled&&practice.cheats)practice.assisted=true;
 run=std::make_unique<RunGameplay>(*this,animations,environment,environment.game_rng,visual,progress,*this,&scheduler,&shared);
 run->practice=&practice;
 if(!run->construct(progress.stage,progress.character)||!preload_run())return fail(run->error);
 // Selection carries stocks/mode; active bomb state belongs to the departing
 // bomb object (native +0x24) and cannot survive construction of a fresh one.
 scene()->battle.session=selection_player;scene()->battle.session.bomb_state=0;
 session=std::make_unique<RunSession>(*run,progress,*replay,*this,*this);initialization=std::make_unique<RunInitialization>(*run,progress,records,*replay,*session,*this);
 if(!initialization->initialize(camera,current_destination,selected_replay.empty()?nullptr:selected_replay.data(),u32(selected_replay.size())))return fail(initialization->error);
 flow=std::make_unique<RunStageFlow>(*run,*session,progress,records,*initialization,*replay,*this,*this);
 if(!flow->bind(camera)||!bind_run_display())return fail(flow->error);completed_recording=false;return true;
}
bool ApplicationState::begin_ending(){
 if(!scene())return fail("Ending requires departing run state");selection_player=scene()->battle.session;selection_score=scene()->battle.score;
 release_run();ending=std::make_unique<EndingScene>(animations,records,*this,scheduler,0);return ending->initialize(progress,selection_player.mode_flags,selection_player.deaths)||fail(ending->error);
}
bool ApplicationState::apply_destination(){
 if(pending_destination<0)return true;const auto next=pending_destination;pending_destination=-1;
 if(next==13||next==10||next==11){
  // Native 44e600 restores the initial selection through 44f6c0 and sets
  // the new-run flag before either ordinary or replay reconstruction.
  if(next==10||next==11){selected_stage=progress.starting_stage;progress.new_run=true;}
  current_destination=next;return begin_run();
 }
 // Native 44e600 sends normal completion through the ending (15);
 // Extra and the ending itself return directly to title/results (16).
 if(next==15){current_destination=next;return begin_ending();}
 if(next==16){if(!save_settings())return false;return begin_title(3);}
 if(next==2||next==4||next==14){if(!save_settings())return false;return begin_title(next==2?4:next==14?3:progress.replay&&!(selection_player.mode_flags&0x40)?2:1);}
 if(next==3){exiting=true;return save_settings();}return fail("Unrecognized application destination: "+std::to_string(next));
}
bool ApplicationState::complete_loading(){
 if(pending_load){const unsigned task=pending_load;pending_load=0;
  if(task==1){if(!apply_destination())return false;}
  else if(task==2){if(!display->retain_background(*scene(),*this))return fail(display->error);pause.reset();current_destination=12;if(!flow->advance()||!preload_run()||!bind_run_display())return fail(flow->error);}
 }
 return true;
}
bool ApplicationState::step(u32 held,u32 pressed,u32 repeated,float fps,bool lost_focus){
 if(!initialized||exiting||!failure.empty())return false;
 if(!complete_loading())return false;
 PROFILE_BEGIN;
 manual_pressed=pressed;manual_repeated=repeated;
 if(pending_page>=0){char name[32];std::snprintf(name,sizeof name,"help_%.2d.png",pending_page+1);if(!read(name,pending_png))return false;pending_page=-1;if(title){if(!title->manual_page_ready())return fail(title->error);}else if(pause_manual){if(!pause_manual->page_loaded())return fail(pause_manual->error);}}
 if(pending_bank>=0&&ending){const auto bank=pending_bank;pending_bank=-1;if(!read(pending_animation_name,pending_animation))return false;pending_animation_name.clear();auto bytes=std::move(pending_animation);if(!ending->animation_ready(bank,bytes.data(),bytes.size()))return fail(ending->error);}
 if(title){title->controls({held,pressed,repeated,0},controller_buttons,numbered_chapter);if(scheduler.update()<0)return fail(title->error.empty()?title->frame.error.empty()?animations.error:title->frame.error:title->error);}
 else if(ending){ending->controls({held,pressed,frames,1},0);if(scheduler.update()<0)return fail(ending->error.empty()?ending->script.error:ending->error);}
 else if(session&&scene()){
  if(pause)pause->controls({pressed,repeated,lost_focus,true});SessionGameplayInput input;input.physical_pressed=pressed;input.fps=fps;input.scene.battle.held=held;input.scene.battle.pressed=pressed;input.touch=touch;
  if(display->game)input.scene.background=display->game->background.update_frame();if(display->previous_background){display->previous_background->rate=progress.rate;input.background_finished=(display->previous_background->state.flags&8)!=0;}
  if(!session->step(input))return fail(session->error.empty()?scene()->error:session->error);
 }else return fail("Application has no active scene");
 PROFILE_AT(0);
 audio_device.update();if(!audio_device.error.empty())return fail(audio_device.error);
 PROFILE_AT(1);
 if(!display->draw())return fail(display->error);
 PROFILE_AT(2);
 if(scene()){
  auto& battle=scene()->battle;auto& clock=battle.spell_card.clock;
  if(clock.needs_sample(battle.spell.flags)){
   double now=0;if(!seconds(now))return fail("Spell presentation clock unavailable");
   if(clock.sample(battle.spell.flags,battle.spell_card.active_frames,now)==SpellPresentationClock::Event::completed&&!(battle.session.mode_flags&0x300)){
    if(!replay||!replay->spell_timing(clock.sequence,clock.encoded))return fail(replay?replay->error:"Spell replay owner unavailable");
    clock.sequence=wrapping_add(clock.sequence,1);
   }
  }
 }
 audio_device.pump();frames++;
 if(!pump_checkpoint())return false;
 // Inter-stage flow retains the departing STD for the native transition.
 // Keep the completed composite visible while preparing the next scene;
 // the startup-only loading panel would clear it to a black frame here.
 if(flow&&flow->pending()&&(!(scene()->hud.flags&0x100)||scene()->hud.intro_age.current>=120)){graphics.presentation.reset();pending_load=2;return true;}
 fades.erase(std::remove_if(fades.begin(),fades.end(),[](const auto& value){return !value->active;}),fades.end());motion.erase(std::remove_if(motion.begin(),motion.end(),[](const auto& value){return !value->active;}),motion.end());animations.collect_resources();
 if(pending_bank>=0||pending_page>=0){if(!draw_loading(false))return false;pending_load=3;return true;}
 if(pending_destination==13||pending_destination==10||pending_destination==11||pending_destination==15){if(!draw_loading(false))return false;pending_load=1;return true;}
 const bool result=apply_destination()&&(graphics.error.empty()||fail(graphics.error));
 PROFILE_AT(3);
#if TH15_DEVELOPMENT_HARNESS
 for(int i=3;i>0;--i)frame_profile[i]-=frame_profile[i-1];frame_profile[0]-=profile_begin;
#endif
 return result;
}
const i32* ApplicationState::projection(){
 projected.fill(0);projected[0]=title?0:ending?2:run?1:-1;projected[1]=current_destination;projected[2]=progress.stage;projected[3]=progress.difficulty;projected[4]=progress.character;projected[5]=progress.stage_frame;projected[6]=i32(progress.scene_flags);projected[7]=i32(frames);
 if(title){projected[8]=i32(title->state.screen);projected[9]=title->state.substate;projected[10]=title->state.menu.cursor;projected[11]=title->state.menu.count;projected[12]=i32(selection_player.mode_flags);}
 else if(scene()){projected[8]=scene()->battle.score.score;projected[9]=scene()->battle.session.extra_lives;projected[10]=scene()->battle.session.bombs;projected[11]=scene()->battle.session.power;projected[12]=i32(scene()->battle.session.mode_flags);projected[13]=scene()->battle.session.deaths;projected[14]=pause?i32(pause->state.screen):0;projected[15]=scene()->battle.enemy_world.chapter_total;}
 return projected.data();
}
}
