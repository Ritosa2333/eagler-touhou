#include "ApplicationState.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <ctime>
namespace th17::sdl {
Application::Application():state(std::make_unique<ApplicationState>()){}Application::~Application()=default;
bool Application::initialize(bool device){return state->initialize(device);}bool Application::step(u32 h,u32 p,u32 r,float fps,bool lost){return state->step(h,p,r,fps,lost);}const std::string& Application::error()const{return state->failure;}const i32* Application::projection()const{return state->projection();}
bool ApplicationState::fail(const std::string& why){if(failure.empty())failure=why.empty()?"TH17 application operation failed":why;return false;}
bool ApplicationState::read(const std::string& requested,std::vector<u8>& bytes){
 const auto slash=requested.find_last_of("/\\");const auto name=requested.substr(slash==std::string::npos?0:slash+1);if(name.empty()||name=="."||name==".."||name.find(char(0))!=std::string::npos)return fail("Invalid game asset name");
 const auto cached=resource_cache.find(name);if(cached!=resource_cache.end()){bytes=cached->second;return true;}
 if(!archive_bytes.empty()){const auto at=archive_names.find(name);if(at==archive_names.end()||!archive.read(at->second,bytes))return fail("Missing or invalid game archive entry: "+name);}
 else {size_t n=0;auto* data=static_cast<u8*>(SDL_LoadFile(("/assets/"+name).c_str(),&n));if(!data)return fail("Missing game asset: "+name);bytes.assign(data,data+n);SDL_free(data);}
 if(bytes.size()<=1024*1024&&name.size()>=4&&name.substr(name.size()-4)!=".wav"&&name.substr(name.size()-4)!=".anm")resource_cache.emplace(name,bytes);return true;
}
bool ApplicationState::initialize(bool device){
 if(initialized)return true;if(!graphics.initialize()||!fonts.initialize())return fail(graphics.error.empty()?fonts.error:graphics.error);graphics.backend.defer=true;
 size_t n=0;auto* packed=static_cast<u8*>(SDL_LoadFile("/th17.dat",&n));if(packed){archive_bytes.assign(packed,packed+n);SDL_free(packed);if(!archive.open(archive_bytes.data(),archive_bytes.size()))return fail("Invalid TH17 asset archive");for(u32 i=0;i<archive.entries.size();i++)archive_names.emplace(archive.entries[i].name,i);}
 if(!files.initialize(records,config,visual))return fail(files.error);std::memcpy(controller.values.data(),config.bytes.data()+4,20);volumes={config.music_volume(),config.sound_volume(),u8(config.auto_focus())};
 if(!audio_device.initialize(*this,device)||!volume(volumes.music_volume,volumes.sound_volume,0))return fail(audio_device.error);
 animations.resource_release=[this](const AnmResource& resource){graphics.unload(resource);};std::vector<u8> bytes;
 for(const auto& bank:std::array<std::pair<const char*,i32>,7>{{{"ascii.anm",0},{"sig.anm",1},{"text.anm",6},{"effect.anm",8},{"title.anm",16},{"title_v.anm",17},{"help.anm",19}}}){
  AnmResource logical,detailed;const AnmResource* atlas=nullptr;
  if(!read(bank.first,bytes)||!logical.open(bytes.data(),bytes.size()))return fail("Application animation resource: "+std::string(bank.first));
  if(bank.second==0&&graphics.render_scale>=2){
   if(!read("ascii_1280.anm",bytes)||!detailed.open(bytes.data(),bytes.size()))return fail("High-resolution original ASCII atlas unavailable");
   if(logical.textures.size()!=detailed.textures.size()||logical.sprites.size()!=detailed.sprites.size())return fail("High-resolution original ASCII layout mismatch");
   // Native high-resolution glyphs include repacked small digits and symbols.
   // Normalize their rectangles to logical pixels before templates are bound;
   // scripts are still the original logical scripts and are initialized once.
   for(u32 i=0;i<logical.sprites.size();i++){auto& target=logical.sprites[i];const auto& source=detailed.sprites[i];if(target.texture!=source.texture||target.source_id!=source.source_id)return fail("High-resolution original ASCII sprite mismatch");const auto& lo=logical.textures[target.texture];const auto& hi=detailed.textures[source.texture];const float x=float(lo.width)/hi.width,y=float(lo.height)/hi.height;target.x=source.x*x;target.y=source.y*y;target.width=source.width*x;target.height=source.height*y;target.u0=source.u0;target.v0=source.v0;target.u1=source.u1;target.v1=source.v1;}
   atlas=&detailed;
  }
  if(!animations.load(bank.second,std::move(logical))||!animations.name_resource(bank.second,bank.first))return fail("Application animation load: "+std::string(bank.first)+": "+animations.error);shared.emplace(bank.first,bank.second);if(bank.second==0&&!animations.sprite_fallback(0))return fail("ASCII fallback atlas unavailable");
  if(!graphics.preload(*animations.resource(bank.second),false,atlas))return fail(graphics.error);
 }
 if(!animations.sprite_fallback(0))return fail("ASCII fallback atlas unavailable");for(i32 style=0;style<23;style++)if(!fonts.preload(style))return fail(fonts.error);
 if(!read("musiccmt.txt",bytes)||!comments.open(bytes.data(),bytes.size()))return fail(comments.error);if(!read("trophy.txt",bytes)||!trophy_catalog.open(bytes.data(),bytes.size(),&visual))return fail(trophy_catalog.error);
 scene_effects=std::make_unique<AnmSceneEffects>(animations,environment.game_rng,visual,8);if(!captions.initialize(0))return fail(captions.error);
 global_layers=std::make_unique<AnmDrawSchedule>(scheduler,animations,renderer,graphics,views);global_letters=std::make_unique<AsciiDrawSchedule>(scheduler,captions,renderer,views);
 const FrameCallback::Function functions[]={[](void* p)->i32{auto& app=*static_cast<ApplicationState*>(p);return !app.title||app.title->update()?1:5;},[](void* p)->i32{return static_cast<ApplicationState*>(p)->animations.update(true)?1:5;},[](void* p)->i32{return static_cast<ApplicationState*>(p)->animations.update(false)?1:5;},[](void* p)->i32{auto& app=*static_cast<ApplicationState*>(p);return !app.title||app.title->draw()?1:5;}};constexpr i32 priorities[]={7,10,35,70};
 for(u32 i=0;i<global_callbacks.size();i++){auto& callback=global_callbacks[i];callback.owner=this;callback.enabled=true;callback.run=functions[i];scheduler.add(callback,i==3?FramePass::Draw:FramePass::Update,priorities[i]);}
 camera.direction={0,0,1};camera.up={0,1,0};camera.fov=.785398185f;performance.rebase(double(SDL_GetTicksNS())/1e9);initialized=true;flow.current=AppScene::Loading;flow.pending=AppScene::Title;return flow.update()==1;
}
ApplicationState::~ApplicationState(){exiting=true;title.reset();ending.reset();release_game();fades.clear();for(auto& callback:global_callbacks)scheduler.remove(callback);global_letters.reset();global_layers.reset();scene_effects.reset();animations.resource_release=nullptr;}
void ApplicationState::remember_title()noexcept{if(title){title_demo_idle=title->frame.demo_idle;title_demo_index=title->frame.demo_index;title_saved_difficulty=title->frame.saved_difficulty;title_saved_replay_selection=title->saved_replay_selection;}}
bool ApplicationState::save_settings(){
 // Title choices are session globals in 1.00b. Bytes 0x1c..0x21 belong to
 // display settings; writing a selected difficulty there corrupts th17.cfg.
 config.bytes[0x22]=u8(volumes.music_volume);config.bytes[0x23]=u8(volumes.sound_volume);config.bytes[0x29]=(config.bytes[0x29]&~2u)|(volumes.controller_option?2u:0u);std::memcpy(config.bytes.data()+4,controller.values.data(),20);return files.save(records,config)||fail(files.error);
}
void ApplicationState::reset_graphics(){environment.paused=false;environment.render_target_has_alpha=true;environment.screen_translation={};environment.background_delta={};environment.screen_offsets={320,16,320,16};environment.playfield_origin={128,16};views.configure(environment);renderer.invalidate();graphics.pipeline()=touhou::graphics::PipelineState{};graphics.configure_game(1);}
bool ApplicationState::begin_loading(bool& available){available=true;return true;}bool ApplicationState::shutdown(){exiting=true;return save_settings();}bool ApplicationState::shutdown_delay(i32){return true;}
bool ApplicationState::release_title(){remember_title();title.reset();return true;}
bool ApplicationState::release_ending(){ending.reset();return true;}
bool ApplicationState::release_game(){
 close_options();fades.clear();display.reset();pause.reset();if(run_input&&flow.pending==AppScene::NextStage)retained_input=run_input->input;run_input.reset();if(!run)return true;
 selection_player=scene()->battle.session;selection_score=scene()->battle.score;
 if(flow.pending==AppScene::NextStage)return true;
 run.reset();for(i32 bank:{0,6,8})animations.retire_resource(bank);animations.collect_resources();reset_graphics();if(!scene_effects&&!exiting)scene_effects=std::make_unique<AnmSceneEffects>(animations,environment.game_rng,visual,8);return true;
}
bool ApplicationState::begin_title(){
 reset_graphics();if(!scene_effects)scene_effects=std::make_unique<AnmSceneEffects>(animations,environment.game_rng,visual,8);
 title=std::make_unique<TitleScene>(progress,selection,selection_player,selection_score,records,choices,volumes,controller,comments,trophy_catalog,spell_memory,animations,*this,scheduler);
 title->frame.demo_idle=title_demo_idle;title->frame.demo_index=title_demo_index;title->frame.saved_difficulty=title_saved_difficulty;title->saved_replay_selection=title_saved_replay_selection;title->state.return_reason=flow.title_return_reason;title->live=recording.get();return title->error.empty()||fail(title->error);
}
bool ApplicationState::preload_run(){if(!assets())return fail("Run resources unavailable");for(const auto& bank:assets()->loaded_animations()){auto* resource=animations.resource(bank.second);if(!resource||!graphics.preload(*resource))return fail(graphics.error.empty()?"Missing run animation: "+bank.first:graphics.error);}return true;}
bool ApplicationState::bind_display(StageGameplay* previous){
 if(!scene()||!assets()||!run_input)return fail("Run display requires active scene/input");pause=std::make_unique<RunPause>(*scene(),progress,records,animations,*this,frame_skip,assets()->front);if(!pause->prepare())return fail(pause->error);
 display=std::make_unique<SceneDisplay>(*scene(),animations,environment,graphics,renderer);if(!display->initialize(progress,records.game,records.spells,*this,assets()->text,assets()->ascii,previous))return fail(display->error);
 display->pause(&pause->visuals,this,&live_description);return true;
}
bool ApplicationState::begin_game(bool playback){
 if(!save_settings())return false;scene_effects.reset();reset_graphics();progress.scene_flags=0;progress.ending_age=0;progress.practice_record=playback;
 progress.historical_web_replay=playback&&selected_replay&&selected_replay->historical_web_recording();
 if(flow.fresh&&progress.historical_web_replay&&progress.stage!=1)return fail("This early WEB recording needs continuous playback from Stage 1");
 ReplayStageState entry;const ReplayStageState* initial=nullptr;if(playback){if(!selected_replay||!selected_replay->initial_state(progress.stage,entry))return fail("Requested original replay stage unavailable");initial=&entry;stage_seed=entry.seed;}
 else {stage_seed=environment.game_rng.seed;if(!flow.fresh)visual.seed=stage_seed;environment.game_rng.calls=0;}
 if(flow.fresh){run.reset();run=std::make_unique<RunGameplay>(*this,animations,environment,environment.game_rng,visual,progress,records.game,records.spells,*this,&shared);
  if(!playback){recording=std::make_unique<ReplayRecording>();auto& description=recording->description();std::memcpy(description.data()+0x88,&progress.character,4);std::memcpy(description.data()+0x8c,&progress.animal,4);std::memcpy(description.data()+0x90,&progress.difficulty,4);std::memcpy(description.data()+0x98,&progress.scoring.continues,4);std::memcpy(description.data()+0x9c,&progress.spell_id,4);description[10]=u8((progress.scoring.mode_flags>>4&1)|((progress.scoring.mode_flags&0x30)==0x20?2:0));}
  else recording.reset();completed_recording=false;if(!run->load(progress.stage,camera,initial))return fail(run->error);
 }else if(!run||!run->next(progress.stage,camera,initial))return fail(run?run->error:"Retained run unavailable");
 if(!preload_run())return false;if(!(progress.scoring.mode_flags&0x40)&&(!audio_device.queue_music_track(0,assets()->definition->stage_music)||!audio_device.queue_music_track(1,assets()->definition->boss_music)||!audio_device.finish_music_requests()))return fail(audio_device.error);run_input=std::make_unique<RunInput>(*scene(),progress,*this,playback?selected_replay.get():nullptr,recording.get(),stage_seed,flow.fresh);run_input->auto_focus=config.auto_focus();
 if(!flow.fresh)run_input->input=retained_input;
 if(recording){recording->use_current_web_rules();live_description=recording->description();}else if(selected_replay)std::copy_n(selected_replay->decoded().begin(),live_description.size(),live_description.begin());
 if(!bind_display(run->previous_scene())||!retire_loading())return false;
 if(flow.fresh&&transition_overlay){
  auto* vm=animations.registry.find(transition_overlay);
  if(!vm||(vm->visual.render_flags&0x60)){u32 handle=0;if(!begin_transition(handle)||!animations.interrupt(handle,7))return false;}
  if(!animations.interrupt(transition_overlay,1))return fail(animations.error);
 }
 performance.reset_result();progress.scoring.mode_flags&=~0xbu;scene()->battle.enemy_world.mode_flags=progress.scoring.mode_flags;return true;
}
bool ApplicationState::begin_ending(){ending=std::make_unique<EndingScene>(animations,records,*this,scheduler,6);const i32 index=preview_index;preview_index=-1;return ending->initialize(progress,index)||fail(ending->error);}
bool ApplicationState::draw_global(){
 if(!graphics.select_target(nullptr,0))return fail(graphics.error);graphics.clear(0xff000000);graphics.set_fog(false);views.camera(DrawCamera::Fullscreen,false);
 if(scheduler.draw()<0)return fail(title&&!title->error.empty()?title->error:global_layers->error.empty()?global_letters->error:global_layers->error);renderer.flush();graphics.present();return graphics.error.empty()||fail(graphics.error);
}
bool ApplicationState::step(u32 held,u32 pressed,u32 repeated,float fps,bool lost_focus){
 if(!initialized||exiting||!failure.empty())return false;if(loading_destination!=AppScene::Uninitialized)return step_loading();manual_pressed=pressed;manual_repeated=repeated;physical_input.update(held);
 if(pending_page>=0){pending_page=-1;if(title){if(!title->manual_page_ready())return fail(title->error);}else if(pause_manual&&!pause_manual->page_loaded())return fail(pause_manual->error);}
 if(pending_bank>=0&&ending){const auto bank=pending_bank;pending_bank=-1;auto bytes=std::move(pending_animation);if(!ending->animation_ready(bank,bytes.data(),bytes.size()))return fail(ending->error);}
 if(title)title->controls({held,pressed,repeated,0,1},controller_buttons);if(ending)ending->controls({held,pressed,physical_input.duration[0],1},0);
 if(scene()){
  if(pause)pause->controls({pressed,repeated,lost_focus,true});run_input->controls(held,fps,touch);StageGameplayFrame input;input.battle.held=held;input.battle.pressed=pressed;input.battle.rate=progress.rate;input.battle.practice=(progress.scoring.mode_flags&0x30)!=0;
  if(!scene()->step(input))return fail(scene()->error.empty()?run_input->error:scene()->error);
  if(run->previous_scene()&&!run->update_previous(progress.rate))return fail(run->error);
 }else {if(scheduler.update()<0)return fail(title&&!title->error.empty()?title->error:ending&&!ending->error.empty()?ending->error:animations.error);}
 audio_device.update();if(!audio_device.error.empty())return fail(audio_device.error);
 if(scene()){
  if(!display->draw())return fail(display->error);if(recording){auto& clock=scene()->battle.spell_card->result_clock;
   // Progression advances the run's stage before this final outgoing draw.
   // Its capture clock still belongs to the scene that produced the result.
   if(clock.completed&&!recording->spell_timing(u32(scene()->battle.stage),u32(clock.completed-1),clock.encoded_time))return fail(recording->error());}
  if(run->previous_scene()&&(run->previous_scene()->background_flags&8)){display->release_previous();if(!run->release_previous())return fail(run->error);}
  if(progress.screen!=0&&progress.screen!=7){if(progress.screen==2){flow.title_return_reason=4;flow.pending=AppScene::Title;}else flow.pending=AppScene(progress.screen);}
 }else if(!draw_global())return false;
 performance.sample(double(SDL_GetTicksNS())/1e9,scene()!=nullptr,progress.scene_flags,u32(frame_skip));
 fades.erase(std::remove_if(fades.begin(),fades.end(),[](const auto& item){return !item->active;}),fades.end());animations.collect_resources();audio_device.pump();frames++;
 return advance_flow();
}
const i32* ApplicationState::projection(){
 projected.fill(0);projected[0]=title?0:ending?2:run?1:-1;projected[1]=i32(flow.current);projected[2]=progress.stage;projected[3]=progress.difficulty;projected[4]=progress.character;projected[5]=scene()&&scene()->controller?scene()->controller->age.current:0;projected[6]=i32(progress.scene_flags);projected[7]=i32(frames);
 if(title){projected[8]=i32(title->state.screen);projected[9]=title->state.substate;projected[10]=title->state.menu.selected;projected[11]=title->state.menu.count;projected[12]=i32(selection.mode_flags);}else if(scene()){const auto& b=scene()->battle;projected[8]=b.score.score;projected[9]=b.session.extra_lives;projected[10]=b.session.bombs;projected[11]=b.session.power;projected[12]=i32(progress.scoring.mode_flags);projected[13]=b.session.deaths;projected[14]=pause?i32(pause->state.screen):0;projected[15]=b.enemy_world.current_chapter;}return projected.data();
}
}
