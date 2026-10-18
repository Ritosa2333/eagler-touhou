#include "StageGameplay.hpp"
#include "PracticeSections.hpp"
namespace th15 {
u32 StageGameplay::practice_music_start_offset()noexcept{return assets.practice_effects.music_start_offset(assets.practice_active&&assets.practice.mode==1&&assets.practice.section==TH15_ST6_STARS);}
struct StageGameplay::Services final:DialogueSceneServices,CheckpointSceneServices,HudFrameServices,StageSceneServices,MessageSceneServices {
 StageGameplay& g;explicit Services(StageGameplay& owner):g(owner){}
 bool queue_music_control(i32 kind,i32 value)override{return g.platform.queue_music_control(kind,value);}
 bool unlock_music(i32 index)override{return g.platform.unlock_music(index);}
 bool complete_stage()override{return g.stage_completion?g.stage_completion():g.platform.complete_stage();}
 bool begin_game_over()override{return g.game_over_begin?g.game_over_begin():g.platform.begin_game_over();}
 bool begin_deformation(i32 mode,i32)override{return g.background_deformation->begin(mode);}
 bool update_deformation(StageScriptState& state,float)override{return g.background_deformation->update(state,g.battle.spell.flags&1)&&g.background_deformation->prepare_draw();}
 bool initialize_text(AnmVm& vm,i32 width,i32 height)override{return g.platform.initialize_text(vm,width,height);}
 bool paint_text(AnmVm& vm,const DialogueText& text)override{return g.platform.paint_text(vm,text);}
 i32 dialogue_text_extent(const std::string& text,i32 font)override{return g.platform.dialogue_text_extent(text,font);}
 bool dialogue_music(bool boss)override{g.bindings.message_state(g.battle.session,g.battle.spell);return g.messages.request(boss?-1:-3);}
 bool fade_dialogue_music(float duration)override{return g.platform.fade_dialogue_music(duration);}
 // MSG completion calls the clear controller directly. The separate ECL -2
 // request checks spell flag 0x80 and can enter the game-over menu instead.
 bool dialogue_stage_complete()override{return complete_stage();}
 bool dialogue_name_banner()override{return g.platform.dialogue_name_banner();}
 bool dialogue_sound(i32 id)override{return g.platform.audio(id,0,PlayerShots::SoundAction::play,true);}
 bool hud_sound(i32 id)override{return g.platform.audio(id,0,PlayerShots::SoundAction::play,true);}
 bool clear_dialogue_field()override{return g.clear_dialogue_field();}
 bool synchronize_resources()override{return g.platform.synchronize_resources();}
 bool retire_reward()override{if(!g.animations.retire(g.reward.state.animation))return false;g.hud.result_notice=0;return true;}
 bool retire_message()override{return g.messages.release();}
 bool retire_scene_effect()override{return g.platform.retire_scene_effect();}
 bool life_hud(i32 stock,i32 pieces)override{return g.hud.life(stock,pieces);}
 bool bomb_hud(i32 stock,i32 pieces)override{return g.hud.bombs(stock,pieces);}
 bool reset_gui()override{g.battle.enemy_world.boss_seconds=-1;return g.hud.reset_for_retry()&&g.platform.reset_gui();}
 bool stop_sounds()override{return g.platform.stop_sounds();}
 bool begin_restart_effect()override{return g.platform.begin_restart_effect();}
 bool begin_restart_overlay()override{return g.platform.begin_restart_overlay();}
 bool checkpoint_file(bool restoring)override{return g.platform.checkpoint_file(restoring);}
};
StageGameplay::StageGameplay(StageAssets& a,AnmManager& manager,AnmEnvironment& env,Rng& game,Rng& visual,SessionState& p,StageGameplayServices& h,i32* requested,RunStageObjects* retained,FrameScheduler* shared_scheduler):services(std::make_unique<Services>(*this)),animations(manager),environment(env),progress(p),assets(a),platform(h),scheduler(shared_scheduler?*shared_scheduler:owned_scheduler),owned_run(retained?nullptr:std::make_unique<RunStageObjects>(a,manager,env,game,visual,p,h)),run(retained?*retained:*owned_run),battle(run.battle),background(a.background,manager,env,*services,a.scenery),hud(run.hud),popups(run.popups),dialogue_host(manager,env,*services),messages(a.messages,*a.definition,a.dialogue_resources(),manager,dialogue_host,music,*services),bindings{hud,background,popups,messages,music,p},reward(manager,battle.score,battle.session,battle.enemy_world,*battle.collection,a.front),checkpoint(battle,background,manager,p,popups,music.current_music_wave,*services,a.enemy_banks,a.bullet){
 background_deformation=std::make_unique<StageDeformation>(manager,visual,bindings.screen_view,a.text);bindings.text_bank=a.text;bindings.assets=&a;battle.bind_progress(progress,requested);battle.bind_scene(bindings);battle.attach(scheduler);
 for(u32 i=0;i<a.enemy_banks.size();i++)battle.enemy_visuals.map_resource(i,a.enemy_banks[i]);battle.spell_visuals=a.spell_resources(0);
 const FrameCallback::Function functions[]={[](void* p)->i32{return static_cast<StageGameplay*>(p)->update_background();},[](void* p)->i32{return static_cast<StageGameplay*>(p)->update_popups();},[](void* p)->i32{return static_cast<StageGameplay*>(p)->update_dialogue();}};
 const i32 priorities[]={17,21,33};for(u32 i=0;i<callbacks.size();i++){callbacks[i].owner=this;callbacks[i].run=functions[i];callbacks[i].enabled=true;scheduler.add(callbacks[i],FramePass::Update,priorities[i]);}
}
StageGameplay::~StageGameplay(){for(auto& callback:callbacks)scheduler.remove(callback);battle.unbind_scene(bindings);if(!owned_run&&!suspended)battle.detach(scheduler);}
bool StageGameplay::suspend_for_transition(){if(suspended)return true;if(!prepared)return fail("Stage suspension outside a prepared scene");for(auto& callback:callbacks)scheduler.remove(callback);checkpoint.release_binding();battle.unbind_scene(bindings);if(!battle.detach(scheduler))return fail(battle.error);suspended=true;started=false;return true;}
bool StageGameplay::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Stage gameplay operation failed":reason;return false;}
bool StageGameplay::prepare(const StageCamera& camera){
 if(prepared)return fail("Stage gameplay already prepared");if(!assets.error.empty())return fail(assets.error);
 if(progress.stage!=assets.definition->id||progress.character!=assets.character)return fail("Stage assets differ from session selection");
 if(!initialize_player()||!initialize_background(camera)||!initialize_popups())return false;prepared=true;return true;
}
bool StageGameplay::initialize_player(bool configure){if(suspended)return fail("Player creation in suspended scene");return run.initialize(configure)||fail(run.error);}
bool StageGameplay::initialize_background(const StageCamera& camera){if(background_ready)return error.empty();if(suspended)return fail("Background creation in suspended scene");if(!background.initialize(camera))return fail(background.error);background_ready=true;callbacks[0].enabled=true;return true;}
bool StageGameplay::initialize_popups(){return run.initialize_popups()||fail(run.error);}
bool StageGameplay::enable_entry_updates(){if(suspended||!run.ready())return fail("Entry updates require initialized run objects");entry_updates=true;prepared=true;callbacks[0].enabled=background_ready;callbacks[1].enabled=callbacks[2].enabled=hud.root!=0;return true;}
bool StageGameplay::reset_for_entry(){
 if(!prepared||started||suspended)return fail("Stage gameplay reset outside prepared scene");
 battle.bullet_scene->reset();if(!battle.player->reset_for_stage())return fail(battle.player->error);
 if(!battle.items->reset())return fail(battle.items->error);
 if(!battle.enemies->clear()||!battle.laser_scene->manager.clear())return fail("Stage gameplay pool reset failed");
 progress.stage_frame=progress.run_clock=0;
 return true;
}
bool StageGameplay::start_for_entry(i32 destination){
 if(!prepared||started||suspended)return fail("Stage gameplay start outside prepared scene");bindings.scene_destination=destination;
 return start_main_script()&&initialize_hud(destination)&&enable_game_callbacks()&&(battle.player->configure_options()||fail(battle.player->error));
}
bool StageGameplay::start_main_script(){if(!prepared||suspended||main_started)return fail("Main stage script started outside scene entry");EnemySpawnRequest main;main.routine="main";if(!battle.spawn(main))return fail(battle.error);main_started=true;return true;}
bool StageGameplay::initialize_hud(i32 destination){bindings.scene_destination=destination;return hud.initialize({destination})||fail(hud.error);}
bool StageGameplay::enable_game_callbacks(){if(!prepared||!background_ready||!main_started||suspended)return fail("Game callback activation outside loaded scene");started=true;for(auto& callback:callbacks)callback.enabled=true;auto& frame=battle.active_frame();frame.player_enabled=frame.bomb_enabled=frame.bullets_enabled=frame.items_enabled=frame.enemies_enabled=frame.lasers_enabled=frame.spell_enabled=true;return battle.prepare_frame(frame)||fail(battle.error);}
bool StageGameplay::begin(i32 destination){return reset_for_entry()&&start_for_entry(destination);}
bool StageGameplay::step(const StageGameplayFrame& frame){
 if(!started&&!entry_updates)return fail("Stage gameplay is not active");if(!error.empty())return false;input=frame;input.battle.rate=progress.rate;input.battle.game_flags=progress.scene_flags;input.background.rate=progress.rate;input.dialogue.rate=progress.rate;input.dialogue.stage=progress.stage;
 if(!started)input.battle.player_enabled=input.battle.bomb_enabled=input.battle.bullets_enabled=input.battle.items_enabled=input.battle.enemies_enabled=input.battle.lasers_enabled=input.battle.spell_enabled=false;
 if(!battle.prepare_frame(input.battle))return fail(battle.error);const i32 result=scheduler.update();if(result<0)return fail(battle.error.empty()?"Stage callback failed":battle.error);return error.empty()&&battle.error.empty();
}
i32 StageGameplay::update_background(){if(!background.update(input.background)){fail(background.error);return i32(FrameAction::Error);}battle.active_frame().background_delta=environment.background_delta;return i32(FrameAction::Continue);}
i32 StageGameplay::update_popups(){popups.update(progress.rate);return i32(FrameAction::Continue);}
i32 StageGameplay::update_dialogue(){
 HudFrameContext context{&battle.enemy_world,&battle.player->motion.position,battle.spell.flags,messages.active(),assets.definition->hud_banners};
 if(!hud.update_before_dialogue(context,*services)){fail(hud.error);return i32(FrameAction::Error);}
 bindings.message_state(battle.session,battle.spell);if(!messages.update(input.dialogue)){fail(messages.error);return i32(FrameAction::Error);}
 context.dialogue=messages.active();if(!hud.update_after_dialogue(context)){fail(hud.error);return i32(FrameAction::Error);}return i32(FrameAction::Continue);
}
bool StageGameplay::chapter_reward(bool boss){
 if(!battle.enemy_world.chapter_total)return true;if(!reward.complete(boss,progress.chapter_deaths))return fail(reward.error);const auto& state=reward.state;
 hud.result_notice=state.animation;hud.result_grazes=state.grazes;hud.result_deaths=state.deaths;hud.result={state.display_percent,state.percent,state.bonus,state.display_bonus,state.base_bonus,state.duration};hud.flags=(hud.flags&~0x1000u)|0x800;hud.intro_age=state.age;return true;
}
bool StageGameplay::capture(i32 chapter){
 return checkpoint.capture(chapter,assets.practice_active?&assets.practice_effects:nullptr)||fail(checkpoint.error);
}
bool StageGameplay::restore(){return checkpoint.restore()||fail(checkpoint.error);}
bool StageGameplay::clear_dialogue_field(){return battle.bullet_scene->cancel_all(0)&&battle.laser_scene->cancel_all(0,false)&&battle.enemies->clear_field(false);}
void StageGameplay::apply_input(const GameInput& controls)noexcept{
 auto& frame=battle.active_frame();frame.held=controls.held;frame.pressed=controls.pressed;frame.game_flags=progress.scene_flags;
 input.battle.held=controls.held;input.battle.pressed=controls.pressed;input.battle.game_flags=progress.scene_flags;
 input.dialogue.held=controls.held;input.dialogue.pressed=controls.pressed;input.dialogue.shoot_frames=controls.duration[0];input.dialogue.skip_frames=controls.duration[9];
}
}
