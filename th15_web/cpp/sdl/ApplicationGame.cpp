#include "ApplicationState.hpp"
#include <cstdio>
#include <ctime>
#include <SDL3/SDL.h>
namespace th15::sdl {
bool ApplicationState::audio(i32 id,float pan,PlayerShots::SoundAction action,bool){if(action==PlayerShots::SoundAction::pan){if(id<0||id>=77)return fail("Sound pan voice outside range");audio_device.sound_pan(u32(id),truncate_int(float(float(pan*1000.f)/192.f)));return true;}if(action==PlayerShots::SoundAction::stop)return audio_device.effects.stop(id)||fail(audio_device.effects.error);return audio_device.effects.positioned(id,pan)||fail(audio_device.effects.error);}
bool ApplicationState::life_hud(i32 stock,i32 pieces){return scene()&&scene()->hud.life(stock,pieces)||fail(scene()?scene()->hud.error:"Life HUD has no scene");}
bool ApplicationState::bomb_hud(i32 stock,i32 pieces){return scene()&&scene()->hud.bombs(stock,pieces)||fail(scene()?scene()->hud.error:"Bomb HUD has no scene");}
bool ApplicationState::game_over(){return pause&&pause->open(PauseEntrance::GameOver)||fail(pause?pause->error:"Game-over menu has no owner");}
bool ApplicationState::notice(i32 id){return scene()&&scene()->hud.notification(0,id)||fail(scene()?scene()->hud.error:"Notice has no scene");}
bool ApplicationState::popup(const Vec3& position,i32 value,u32 color){if(!scene())return fail("Popup has no scene");scene()->popups.number(position,value,color);return true;}
bool ApplicationState::cancellation_effect(i32 script,const Vec3& position,const Vec3& velocity){if(!scene())return fail("Cancellation effect has no scene");const auto handle=scene()->battle.effects.tracked(script,position);auto* vm=animations.registry.find(handle);if(!vm)return fail(scene()->battle.effects.error.empty()?animations.error:scene()->battle.effects.error);vm->interpolators.position.begin(30,6,{0,0,0},{velocity.x,velocity.y,velocity.z});return true;}
bool ApplicationState::graze_spark(const Vec3& position){return scene()&&scene()->battle.record_graze(position)||fail("Graze has no battle owner");}
bool ApplicationState::graze_flash(){if(!scene())return fail("Graze flash has no battle owner");scene()->battle.graze_flash();return true;}
bool ApplicationState::graze_resonance(float value){if(!scene())return fail("Graze resonance has no battle owner");scene()->battle.graze_resonance(value);return true;}
// Native 0x474f90 consumes the gameplay RNG, so shake also belongs to replay state.
bool ApplicationState::shake(const ScreenShakeSpec& spec){motion.push_back(std::make_unique<ScreenMotionFrame>(scheduler,environment.game_rng,spec,[this](){return ScreenShakeContext{exiting,scene()!=nullptr,progress.scene_flags,progress.rate,environment.resolution_scale};},[this](Vec2 world,Vec2 screen){views.view(DrawCamera::Playfield).offset=world;views.view(DrawCamera::Interface).offset=screen;}));return true;}
// Native 0x474db0 uses the visual RNG; consuming game_rng here desynchronizes replays.
bool ApplicationState::nudge(const ScreenNudgeSpec& spec){motion.push_back(std::make_unique<ScreenMotionFrame>(scheduler,visual,spec,[this](){return ScreenShakeContext{exiting,scene()!=nullptr,progress.scene_flags,progress.rate,environment.resolution_scale};},[this](Vec2 world,Vec2 screen){views.view(DrawCamera::Playfield).offset=world;views.view(DrawCamera::Interface).offset=screen;}));return true;}
// All four native bomb virtual query methods return zero. Their actual damage
// is submitted by the character's reconstructed DamageSources controller.
bool ApplicationState::bomb_damage(const DamageQuery&,i32& value){value=0;return true;}
// GameBattle executes the typed special-update/damage/contact/death rules.
// These platform extension hooks do not apply a second copy of those rules.
int ApplicationState::enemy_callback(EnemyRuntime&,float){return 0;}
bool ApplicationState::enemy_additional_damage(EnemyState&,i32,i32& value){value=0;return true;}
bool ApplicationState::enemy_contact(EnemyState&,AnmVm*,i32& value,bool& handled){value=0;handled=false;return true;}
bool ApplicationState::enemy_distortion(EnemyState&,float){return fail("Enemy distortion bypassed its stage-owned grid");}
bool ApplicationState::enemy_death_callback(EnemyRuntime&){return true;}
bool ApplicationState::spell_title(AnmVm& vm,const std::string& value){DialogueText request{0,value,0,0,0xffffffff};request.right_aligned=true;return text(vm,request);}
bool ApplicationState::spell_sound(i32 id){return sound(id);}
bool ApplicationState::queue_music_control(i32 code,i32 value){return audio_device.queue_music(code,value,code==5?"FadeOut":"dummy")||fail(audio_device.error);}
bool ApplicationState::unlock_music(i32 index){return records.unlock_music(index)||fail(records.error);}
bool ApplicationState::complete_stage(){return scene()&&scene()->stage_completion&&scene()->stage_completion()||fail("Stage completion callback unavailable");}
bool ApplicationState::begin_game_over(){return game_over();}
bool ApplicationState::initialize_text(AnmVm& vm,i32,i32){return text(vm,{0,"",0,0,0xffffffff});}
bool ApplicationState::paint_text(AnmVm& vm,const DialogueText& value){return text(vm,value);}
bool ApplicationState::dialogue_music(bool boss){return boss?start_boss_music():start_stage_music();}
bool ApplicationState::fade_dialogue_music(float value){return fade_music(value);}
bool ApplicationState::dialogue_stage_complete(){return complete_stage();}
bool ApplicationState::dialogue_name_banner(){const auto* definition=stage_definition(progress.stage);return definition&&scene()&&scene()->hud.name_banner(definition->hud_banners)||fail(scene()?scene()->hud.error:"Dialogue banner has no HUD owner");}
bool ApplicationState::dialogue_sound(i32 id){return sound(id);}
bool ApplicationState::clear_dialogue_field(){return scene()&&scene()->clear_dialogue_field()||fail(scene()?scene()->error:"Dialogue clear has no scene");}
bool ApplicationState::begin_deformation(i32,i32){return fail("Stage deformation bypassed its stage-owned controller");}
bool ApplicationState::update_deformation(StageScriptState&,float){return fail("Stage deformation bypassed its stage-owned controller");}
bool ApplicationState::synchronize_resources(){renderer.flush();return preload_run();}
bool ApplicationState::retire_reward(){if(!scene())return fail("Chapter reward has no scene");return animations.retire(scene()->reward.state.animation)||fail(animations.error);}
bool ApplicationState::retire_message(){return scene()&&scene()->messages.release()||fail(scene()?scene()->messages.error:"Message retirement has no scene");}
bool ApplicationState::retire_scene_effect(){return animations.retire(restart_effect_handle)||fail(animations.error);}
bool ApplicationState::reset_gui(){return scene()&&scene()->hud.reset_for_retry()||fail(scene()?scene()->hud.error:"GUI reset has no scene");}
bool ApplicationState::stop_sounds(){audio_device.effects.suspend();return true;}
bool ApplicationState::begin_restart_effect(){if(!assets())return fail("Chapter restart effect has no stage resource");animations.retire(restart_effect_handle);restart_effect_handle=animations.create_overlay(assets()->effect,0);auto* vm=animations.registry.find(restart_effect_handle);if(!vm||(!vm->geometry.overlay&&(!animations.effect||!animations.effect(*vm,0))))return fail(animations.error.empty()?"Chapter restart effect unavailable":animations.error);return animations.interrupt(restart_effect_handle,9)||fail(animations.error);}
bool ApplicationState::begin_restart_overlay(){return restart_overlay(250);}
bool ApplicationState::checkpoint_file(bool restoring){
 if(!scene())return fail("Pointdevice checkpoint has no scene");auto& saved=scene()->checkpoint;
 if(restoring){const auto header=saved.file_header(i64(std::time(nullptr)),0);if(checkpoint_encoder){checkpoint_header=header;return true;}if(checkpoint_bytes.empty()||!CheckpointFile::update_header(checkpoint_bytes,header))return fail("Retry metadata has no captured checkpoint file");}
 else {CheckpointFile file;if(!saved.prepare_file(file,i64(std::time(nullptr)),0)||!file.payload(checkpoint_payload))return fail(saved.error.empty()?file.error:saved.error);checkpoint_header=file.header;checkpoint_encoder=std::make_unique<Lzss>();checkpoint_encoder->begin_encode(checkpoint_payload.data(),u32(checkpoint_payload.size()));return true;}
 return files.save_checkpoint(progress.character,progress.difficulty,checkpoint_bytes)||fail(files.error);
}
bool ApplicationState::pump_checkpoint(u32 budget){
 if(!checkpoint_encoder)return true;
 const auto start=SDL_GetTicksNS();bool done=false;
 do{const u32 chunk=std::min(budget,4096u);done=checkpoint_encoder->step_encode(chunk);budget-=chunk;}
 while(!done&&budget&&(budget>131072||SDL_GetTicksNS()-start<1000000));
 if(!done)return true;
 const auto& packed=checkpoint_encoder->encoded();const u32 sizes[]={u32(packed.size()),u32(checkpoint_payload.size())};std::memcpy(checkpoint_header.bytes.data()+0x58,sizes,8);
 checkpoint_bytes.assign(checkpoint_header.bytes.begin(),checkpoint_header.bytes.end());checkpoint_bytes.insert(checkpoint_bytes.end(),packed.begin(),packed.end());
 checkpoint_encoder.reset();checkpoint_payload.clear();return files.save_checkpoint(checkpoint_header.character(),checkpoint_header.difficulty(),checkpoint_bytes)||fail(files.error);
}
bool ApplicationState::finish_checkpoint(){while(checkpoint_encoder)if(!pump_checkpoint(UINT32_MAX))return false;return true;}
bool ApplicationState::ending_fade(){return screen_fade(200,20,81,true,true);}
bool ApplicationState::finish_replay(){if(progress.replay)return pause&&pause->open(PauseEntrance::RetryPause)||fail(pause?pause->error:"Replay completion menu unavailable");return prepare_live_replay(true);}
bool ApplicationState::destination(SessionDestination value){pending_destination=i32(value);return true;}
bool ApplicationState::load_scene(bool&){return fail("Scene loading bypassed the RunSession entrance controller");}
bool ApplicationState::activate_scene(){return fail("Scene activation bypassed the RunSession entrance controller");}
bool ApplicationState::release_background(){display->release_previous();return true;}
bool ApplicationState::load_checkpoint_file(bool& restored){
 restored=false;if(!scene())return fail("Pointdevice checkpoint loading has no scene");bool present=false;if(!files.checkpoint(progress.character,progress.difficulty,checkpoint_bytes,present))return fail(files.error);if(!present)return true;
 auto& saved=scene()->checkpoint;if(!saved.read_file(checkpoint_bytes.data(),checkpoint_bytes.size(),0)||!saved.restore())return fail(saved.error);restored=true;return true;
}
bool ApplicationState::restart_overlay(i32 script){if(!assets())return fail("Restart overlay has no stage resource");animations.retire(restart_handle);restart_handle=animations.create_overlay(assets()->front,script);return restart_handle!=0||fail(animations.error);}
bool ApplicationState::restart_effect(i32 label){return animations.interrupt(restart_effect_handle,label)||fail(animations.error);}
bool ApplicationState::prepare_stage_music(){return music_command(3);}
bool ApplicationState::start_stage_music(){const auto* definition=stage_definition(progress.stage);return definition&&audio_device.queue_music(2,0,"dummy")&&unlock_music(definition->music_unlock[0])||fail("Stage music unavailable");}
bool ApplicationState::start_boss_music(){const auto* definition=stage_definition(progress.stage);return definition&&audio_device.queue_music(2,1,"dummy")&&unlock_music(definition->music_unlock[1])||fail("Boss music unavailable");}
bool ApplicationState::seek_stage_music(double position){return audio_device.finish_music_requests()&&audio_device.seek_music(position)||fail(audio_device.error);}
bool ApplicationState::demo_fade(){return screen_fade(60,20,49,true,true);}

bool ApplicationState::update_overlays(){return true;}
bool ApplicationState::discard_stage_assets(){pending_destination=4;return true;}
bool ApplicationState::return_to_title(bool selection){pending_destination=selection?2:4;return true;}
bool ApplicationState::capture_previous_background(StageGameplay&){if(!display->previous_background)return fail("Departing background has no retained drawing owner");display->previous_background->begin_departure();return screen_fade(30,20,10,true,false);}
bool ApplicationState::prepare_background_transition(){if(!display->game)return fail("Arriving background has no drawing owner");display->game->background.begin_arrival();return true;}
bool ApplicationState::transition_banner(){return scene()&&animations.interrupt(scene()->hud.retry_intro,1)||fail(animations.error);}
bool ApplicationState::interrupt_entrance(){return animations.interrupt(transition_overlay,1)||fail(animations.error);}
bool ApplicationState::retire_restart_overlay(){return animations.retire(restart_handle)||fail(animations.error);}
bool ApplicationState::interrupt_resume_overlay(){return true;}
bool ApplicationState::restore_pending_progress(){return fail("Resume progress restore has no decoded checkpoint");}
bool ApplicationState::queue_music(i32 code){return music_command(code);}
bool ApplicationState::unlock_current_music(){const auto* definition=stage_definition(progress.stage);return definition&&unlock_music(definition->music_unlock[0])||fail("Current music record unavailable");}
bool ApplicationState::prepare_pause_menu(){return assets()&&animations.resource(assets()->front)||fail("Pause resource missing during construction");}
bool ApplicationState::prepare_checkpoint_storage(){return scene()!=nullptr||fail("Checkpoint owner missing during construction");}
bool ApplicationState::restore_enemy_checkpoint(StageGameplay& stage){auto& enemies=*stage.battle.enemies;if(!enemies.clear())return fail(enemies.error);enemies.timer.set(0);return true;}
bool ApplicationState::load_stage_theme(i32 stage){return stage_definition(stage)&&music_command(3)||fail("Stage music definition unavailable");}
bool ApplicationState::load_player_theme(i32 stage,bool boss){const auto* definition=stage_definition(stage);return definition&&audio_device.queue_music_track(boss?1:0,definition->music[boss?1:0])||fail(audio_device.error.empty()?"Player theme definition unavailable":audio_device.error);}
bool ApplicationState::prepare_ending(StageGameplay& game){const auto index=ending_index(progress.character,progress.subcharacter,game.battle.session.mode_flags,game.battle.session.deaths);char name[16];std::snprintf(name,sizeof name,"e%02d.msg",index+1);std::vector<u8> bytes;return read(name,bytes);}
bool ApplicationState::finish_practice(){return pause&&pause->open(PauseEntrance::Results)||fail(pause?pause->error:"Practice result menu unavailable");}
bool ApplicationState::queue_next_stage(){return fail("Next-stage request bypassed the run completion owner");}
bool ApplicationState::prepare_next_stage(i32 stage){return stage_definition(stage)!=nullptr||fail("Next stage unavailable");}
bool ApplicationState::stop_checkpoint_worker(){return finish_checkpoint();}
bool ApplicationState::save_records(){return save_settings();}
bool ApplicationState::clear_session_links(){return true;}
bool ApplicationState::reset_transition(){if(display->screen())display->screen()->replacement={};return true;}
bool ApplicationState::disable_pause_callbacks(){if(pause)pause->disable();return true;}
bool ApplicationState::queue_exit_music(i32 code){return music_command(code);}
bool ApplicationState::reset_audio_slots(){audio_device.effects.suspend();return true;}
bool ApplicationState::suspend_music(){audio_device.pause_music(true);return true;}
bool ApplicationState::finish_audio_requests(){return audio_device.finish_music_requests()||fail(audio_device.error);}
bool ApplicationState::capture_background(StageGameplay&,AnmManager&,u32& handle,bool field){return display->capture(handle,!field)||fail(display->error);}
bool ApplicationState::preserve_current_music(std::string& wave,double& position){return audio_device.current_music(wave,position)||fail(audio_device.error);}
bool ApplicationState::start_game_over_music(){return music("th128_08.wav")&&music_command(2);}
bool ApplicationState::read_replay_slot(i32 slot,std::shared_ptr<Replay>& value){return read_slot(slot,value);}
bool ApplicationState::save_named_replay(i32 slot,const std::array<char,9>& name){return save_slot(slot,name);}
bool ApplicationState::prepare_replay_save(bool cleared){return prepare_live_replay(cleared);}
// Native PauseMenu entry 3 opens the operation manual (43ed30), not settings.
bool ApplicationState::open_options(float offset){
 close_options();pause_manual=std::make_unique<Manual>(animations,*this,19);pause_manual->offset_x=offset;manual_update.owner=this;manual_update.enabled=true;manual_update.run=[](void* p)->i32{auto& app=*static_cast<ApplicationState*>(p);if(!app.pause_manual||app.pause_manual->update(app.manual_pressed,app.manual_repeated,app.progress.rate))return 1;app.fail(app.pause_manual->error);return 5;};return scheduler.add(manual_update,FramePass::Update,11)>=0||fail("Pause manual callback registration failed");
}
bool ApplicationState::options_finished()const{return pause_manual&&pause_manual->finished;}
bool ApplicationState::close_options(){scheduler.remove(manual_update);if(pause_manual){for(auto handle:pause_manual->choices)animations.retire(handle);animations.retire(pause_manual->page);pause_manual.reset();pending_page=-1;}return true;}
i32 ApplicationState::scene_destination()const{return current_destination;}
bool ApplicationState::resume_looping_sounds(){audio_device.effects.resume();return true;}
bool ApplicationState::resume_music(){audio_device.pause_music(false);return true;}
bool ApplicationState::resume_saved_music(const std::string& wave,double position){return music(wave)&&music_command(2)&&seek_stage_music(position);}
bool ApplicationState::destination(PauseDestination value){pending_destination=i32(value);return true;}
bool ApplicationState::screen_fade(i32 duration,i32 update,i32 draw,bool covering,bool full_screen){auto fade=std::make_unique<ScreenFade>(scheduler,renderer,graphics,environment,duration,update,draw,0,covering,full_screen);fade->context=[this](){return ScreenFadeContext{exiting,scene()!=nullptr,progress.scene_flags,progress.rate};};fades.push_back(std::move(fade));return true;}
bool ApplicationState::background_fade(i32 duration,i32 update,i32 draw){return screen_fade(duration,update,draw,false,false);}
}
