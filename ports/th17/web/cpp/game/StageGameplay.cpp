#include "StageGameplay.hpp"
namespace th17 {
struct StageGameplay::Services final:BattleWorldServices,DialogueServices,GameFrameHost,GameProgressionHost,StageSceneServices,GameStartHost {
    StageGameplay& g;explicit Services(StageGameplay& owner):g(owner){}
    bool previous_bomb_display(i32 count,i32 pieces)override{return !g.battle.hud||g.battle.hud->bombs(count,pieces);}
    bool create_player()override{g.battle.score.max_power=wrapping_mul(g.assets.shots.header.max_power_level,100);return g.objects.initialize()||g.fail(g.objects.error);}
    bool configure_player()override{return g.battle.player->configure_options()||g.fail(g.battle.player->options.error);}
    bool audio(i32 id,float pan,PlayerShots::SoundAction action,bool queued)override{return g.platform.audio(id,pan,action,queued);}
    bool life_hud(i32 count,i32 pieces)override{return !g.battle.hud||g.battle.hud->lives(count,pieces);}
    bool bomb_hud(i32 count,i32 pieces)override{return !g.battle.hud||g.battle.hud->bombs(count,pieces);}
    bool game_over()override{return g.platform.game_over();}
    bool notice(i32 kind)override{return g.battle.hud&&g.battle.hud->result(0,kind);}
    bool popup(const Vec3& p,i32 amount,u32 color)override{g.popups.number(p,amount,color);return g.platform.popup(p,amount,color);}
    bool trophy(i32 id)override{return g.platform.trophy(id);}
    bool scene_message(i32 id)override{return g.message(id);}
    bool message_complete(bool& done)override{done=!g.battle.dialogue||g.battle.dialogue->complete_pulse!=0;return true;}
    bool dialogue_present()const noexcept override{return false;}
    bool spell_background_visible(bool visible)override{g.background_flags=(g.background_flags&~1u)|(visible?1:0);g.background.draw_objects=visible;return true;}
    bool spell_prepare_hud(bool start)override{return g.battle.hud&&g.battle.hud->prepare_spell(start);}
    bool spell_title(AnmVm& vm,const std::string& text)override{return g.platform.spell_title(vm,text);}
    bool spell_result(i32 bonus,bool failed)override{return g.battle.hud&&g.battle.hud->result(bonus,failed?1:0);}
    bool chapter(i32 id)override{g.battle.enemy_world.current_chapter=id;g.battle.enemy_world.requested_chapter=id;return true;}
    bool boss_segment(i32 boss,i32 index,float value,u32 color)override{return g.battle.hud&&g.battle.hud->segment(boss,index,value,color);}
    bool boss_segments(i32 count)override{return g.battle.hud&&g.battle.hud->segments(count);}
    bool stage_logo()override{return g.battle.hud&&g.battle.hud->stage_logo(g.hud_context());}
    bool background_interrupt(i32 label)override{return g.background.interrupt(label)||g.fail(g.background.error);}
    bool clear_dialogue_field()override{return g.battle.bullet_scene->cancel_all(0)&&g.battle.laser_scene->cancel_all(0,false)&&g.battle.enemies->clear_field(false);}
    bool dialogue_text(AnmVm& vm,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string& text)override{return g.platform.dialogue_text(vm,color,shadow,style,x,y,text);}
    bool dialogue_sound(i32 id)override{return audio(id,0,PlayerShots::SoundAction::play,true);}
    // MSG opcode 19 creates its own title animation after changing the music.
    bool dialogue_boss_music()override{return g.music(true,false);}
    bool dialogue_music_fade(i32 seconds)override{return g.platform.music_fade(seconds);}
    bool dialogue_next_stage()override{return g.next_stage();}
    bool dialogue_boss_name()override{return g.battle.hud&&g.battle.hud->ensure_boss_name();}
    bool dialogue_notice(u32 handle)override{if(!g.battle.hud)return false;g.battle.hud->notice_handle=handle;return true;}
    i32 initialize_stage()override{return g.initialize_stage();}
    bool start_stage()override{return g.start_stage();}
    i32 loading_state()const noexcept override{return g.platform.loading_state();}
    bool retire_loading()override{return g.platform.retire_loading();}
    bool return_to_title()override{return g.platform.return_to_title();}
    bool transition(const SceneTransition& spec)override{return g.platform.transition(spec);}
    bool pause_stage_music()override{return g.platform.music_control(3,0);}
    bool interrupt_stage_visual()override{auto* vm=g.scene_animations[3];if(!vm)return true;if(!g.animations.interrupt_effect(*vm,1,g.battle.active_frame().rate))return false;vm->pending_interrupt=1;return true;}
    bool reload_stage_music(i32 index)override{return g.platform.music_control(0,index);}
    bool flush_audio()override{return g.platform.flush_audio();}
    bool resume_environment(double seconds)override{return g.platform.resume_environment(seconds);}
    bool advance_scene_animations()override{for(auto* vm:g.scene_animations)if(vm&&g.animations.tick_instance(*vm)<0)return g.fail(g.animations.error);return true;}
    bool finish_player_options()override{return g.battle.player->finish_stage_options();}
    bool stop_bomb()override{return g.battle.bomb&&g.battle.bomb->stop();}
    bool record_play_time()override{return g.platform.record_play_time();}
    bool finish_practice()override{return g.platform.finish_practice();}
    bool begin_deformation(i32 mode,i32)override{return g.deformation.begin(mode)||g.fail(g.deformation.error);}
    bool update_deformation(StageScriptState& state,float rate)override{return (g.deformation.update(state,g.battle.spell.flags&1,rate)&&g.deformation.prepare_draw())||g.fail(g.deformation.error);}
};
StageGameplay::StageGameplay(StageAssets& a,AnmManager& anm,AnmEnvironment& env,Rng& game,Rng& visual,GameRun& state,GameRecords& data,SpellRecords& spells,StageGameplayServices& host,RunStageObjects* retained):services(std::make_unique<Services>(*this)),assets(a),animations(anm),environment(env),game_random(game),visual_random(visual),run(state),records(data),spell_records(spells),platform(host),owned_objects(retained?nullptr:std::make_unique<RunStageObjects>(a,anm,env,game,visual,*services)),objects(retained?*retained:*owned_objects),battle(objects.battle),background(a.background,anm,env,*services,a.scenery),deformation(anm,visual,battle.grid_viewport,a.text),popups(objects.popups){
    objects.bind(*services);battle.historical_web_replay=run.historical_web_replay;
    const FrameCallback::Function actions[]={[](void* p)->i32{return static_cast<StageGameplay*>(p)->update_game();},[](void* p)->i32{return static_cast<StageGameplay*>(p)->update_background();},[](void* p)->i32{auto& g=*static_cast<StageGameplay*>(p);g.popups.update(g.battle.active_frame().rate);return 1;}};
    const i32 priorities[]={16,17,21};for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.owner=this;c.run=actions[i];c.enabled=true;scheduler.add(c,FramePass::Update,priorities[i]);}
    battle.attach(scheduler);
}
StageGameplay::~StageGameplay(){for(auto& callback:callbacks)scheduler.remove(callback);if(!owned_objects&&!suspended)battle.detach(scheduler);}
bool StageGameplay::suspend_for_transition(){if(suspended)return true;if(!prepared)return fail("Unprepared stage cannot transfer run objects");for(auto& callback:callbacks)scheduler.remove(callback);if(!battle.detach(scheduler))return fail(battle.error);suspended=true;started=false;background_draw.depart();return true;}
bool StageGameplay::update_previous_background(float rate){if(!suspended)return fail("Outgoing background is still active");return background.update({rate,background_flags,background_transition_age})||fail(background.error);}
bool StageGameplay::fail(const std::string& text){if(error.empty())error=text.empty()?"Stage gameplay operation failed":text;return false;}
HudStartContext StageGameplay::hud_context()const noexcept{return {run.scoring.mode_flags,run.screen,battle.stage,run.character,run.animal,run.difficulty,run.scoring.continues,true,run.practice_record};}
bool StageGameplay::prepare(const StageCamera& camera,const ReplayStageState* replay,const ReplayRunState* retained,bool defer_start){
    if(prepared)return fail("Stage gameplay already prepared");if(!assets.ready())return fail(assets.error);
    const bool fresh=!retained&&!objects.ready();
    if(!assets.definition||run.stage!=assets.definition->stage||run.character!=assets.character||run.animal!=assets.animal||u32(run.difficulty)>4)return fail("Stage resources differ from run selection");
    battle.stage=run.stage;battle.score.difficulty=run.difficulty;battle.session.animal=run.animal;
    battle.enemy_world.mode_flags=run.scoring.mode_flags;battle.enemy_world.spell_id=run.spell_id;battle.enemy_world.script_value_101=run.demo_slot;battle.session.replay_state=(run.scoring.mode_flags&2)?1:0;
    if(retained){
        if(replay||retained->stage!=run.stage||retained->character!=run.character||retained->animal!=run.animal||retained->difficulty!=run.difficulty)return fail("Retained run differs from stage selection");
        restore_run(*retained);battle.score.max_power=wrapping_mul(assets.shots.header.max_power_level,100);
        if(!objects.initialize())return fail(objects.error);
    }
    GameStart start(run,records,battle.score,battle.session,battle.items->roaring.state,battle.graze_total,*services);
    if(!start.begin(fresh))return fail(start.error);battle.enemy_world.mode_flags=run.scoring.mode_flags;
    // The original fresh-run replay owner is created after the player. It
    // resets the game RNG here and leaves the title's visual RNG untouched.
    // Later stages seed both streams before replacing their stage resources.
    if(fresh){if(replay)game_random.seed=replay->seed;game_random.calls=0;}
    if(replay){
        const auto& s=replay->run;if(replay->stage!=run.stage||s.stage!=run.stage||s.character!=run.character||s.animal!=run.animal||s.difficulty!=run.difficulty)return fail("Replay initial state differs from stage selection");
        replay_entry=std::make_unique<ReplayStageState>(*replay);restore_run(s);battle.session.replay_state=1;
        for(u32 i=0;i<capture_times.size();i++)capture_times[i]=signed_bits(replay->reserved[i]);
    }
    if((!battle.hud&&!battle.configure_hud(hud_context(),assets.hud_resources(),false))||!battle.configure_spell(spell_records,assets.spell_resources())||!battle.configure_dialogue(*assets.definition,assets.messages,*services,assets.dialogue_resources()))return fail(battle.error);
    if(!background.initialize(camera))return fail(background.error);if(!objects.initialize_popups())return fail(objects.error);
    controller=std::make_unique<GameFrame>(run,*battle.hud,battle.score,battle.enemy_world,*services);
    if(replay)controller->counters=replay->run.counters;
    progression=std::make_unique<GameProgression>(run,records,battle.score,battle.session,*battle.hud,*services);
    if(defer_start){run.scene_flags|=0x800;battle.world_active(false);background_draw.arrive();}
    prepared=true;return true;
}
void StageGameplay::restore_run(const ReplayRunState& s){
    run.stage=s.stage;run.initial_stage=s.initial_stage;run.character=s.character;run.animal=s.animal;run.difficulty=s.difficulty;run.spell_id=s.spell_id;
    run.scoring.continues=s.continues;run.initial_point_value=s.initial_point_value;run.replay_reserved_40=s.reserved_40;run.replay_reserved_60=s.reserved_60;
    auto& b=battle;b.stage=s.stage;b.character=s.character;b.graze_total=s.grazes;b.graze_chapter=s.chapter_grazes;
    b.score={s.score,s.point_value,s.max_point_value,s.point_items,s.life_piece_tier,s.difficulty,s.max_power};
    auto& p=b.session;p.power=s.power;p.power_step=s.power_step;p.animal=s.animal;p.deaths=s.deaths;p.extra_lives=s.lives;p.life_pieces=s.life_pieces;p.bombs=s.bombs;p.bomb_pieces=s.bomb_pieces;p.initial_bombs=s.initial_bombs;
    auto& w=b.enemy_world;w.rank=s.rank;w.current_chapter=s.chapter;w.chapter_total=s.chapter_total;w.chapter_defeated=s.chapter_defeated;w.spell_id=s.spell_id;
    auto& r=b.items->roaring.state;const Timer reward=r.reward_age,countdown=r.countdown;r=s.roaring;r.reward_age=reward;r.countdown=countdown;r.reward_age.set(s.roaring.reward_age.current);r.countdown.set(s.roaring.countdown.current);
}
ReplayRunState StageGameplay::capture_run()const noexcept{
    ReplayRunState s;s.stage=run.stage;s.initial_stage=run.initial_stage;s.character=run.character;s.animal=run.animal;s.difficulty=run.difficulty;s.spell_id=run.spell_id;s.continues=run.scoring.continues;s.initial_point_value=run.initial_point_value;s.reserved_40=run.replay_reserved_40;s.reserved_60=run.replay_reserved_60;
    const auto& b=battle;s.grazes=b.graze_total;s.chapter_grazes=b.graze_chapter;
    const auto& v=b.score;s.score=v.score;s.point_value=v.point_value;s.max_point_value=v.max_point_value;s.point_items=v.point_items;s.life_piece_tier=v.life_piece_tier;s.max_power=v.max_power;
    const auto& p=b.session;s.power=p.power;s.power_step=p.power_step;s.deaths=p.deaths;s.lives=p.extra_lives;s.life_pieces=p.life_pieces;s.bombs=p.bombs;s.bomb_pieces=p.bomb_pieces;s.initial_bombs=p.initial_bombs;
    const auto& w=b.enemy_world;s.rank=w.rank;s.chapter=w.current_chapter;s.chapter_total=w.chapter_total;s.chapter_defeated=w.chapter_defeated;
    ReplayRunState values;s.roaring=b.items->roaring.state;values.assign(s);if(controller)values.counters=controller->counters;return values;
}
bool StageGameplay::reset_world(){
    const Vec2 previous_bounds=battle.bullet_scene->manager.context.reflection_bounds;
    battle.bullet_scene->reset();if(run.historical_web_replay)battle.bullet_scene->manager.context.reflection_bounds=previous_bounds;
    if(!battle.player->reset_for_stage())return fail(battle.player->error);
    if(!battle.items->items.reset())return fail(battle.items->items.error);
    if(!battle.enemies->clear()||!battle.laser_scene->manager.clear())return fail("Stage pool reset failed");
    controller->counters={};return true;
}
bool StageGameplay::activate_world(){
    if(started)return fail("Stage main script already active");if(!reset_world())return false;
    if(replay_entry){replay_entry->restore_player(battle.player->motion);if(!battle.player->configure_options())return fail(battle.player->options.error);}
    if(!platform.stage_input_ready(*this))return fail("Stage input activation failed");
    EnemySpawnRequest main;main.routine="main";if(!battle.spawn(main))return fail(battle.error);
    if(!battle.hud->initialize(hud_context()))return fail(battle.hud->error);
    if(!battle.player->configure_options())return fail(battle.player->options.error);
    if(!battle.items->roaring.enter_stage())return fail(battle.items->roaring.error);started=true;battle.world_active(true);return true;
}
i32 StageGameplay::initialize_stage(){
    auto& hud=*battle.hud;if(!animations.interrupt(hud.stage_clear_notice,1)||!animations.interrupt(hud.chapter_result_notice,1)){fail(animations.error);return -1;}
    hud.flags&=~0x100u;hud.notice_age.set(0);
    if(run.scene_flags&8){if(!platform.return_to_title())return -1;run.screen=(run.application_flags&0x2000)?2:3;return 1;}
    if(run.scene_flags&0x800){if(!platform.transition({2,30,10,0}))return -1;return 0;}
    if(!started&&!activate_world())return -1;
    if(!(run.scoring.mode_flags&0x40)&&(run.scoring.mode_flags&0x30)!=0x20&&!platform.music_control(0,assets.definition->stage_music_index))return -1;
    return 0;
}
bool StageGameplay::start_stage(){
    if(!(run.scene_flags&0x800))return true;run.scene_flags&=~0x800u;
    if(!activate_world()||!platform.music_control(alternate_music?4:3,0))return false;
    if(alternate_music&&!platform.music_control(4,0))return false;
    if(!platform.music_control(2,0)||!platform.unlock_music(assets.definition->stage_music_index)||!platform.retire_loading())return false;
    controller->age.set(0);return true;
}
bool StageGameplay::next_stage(){return progression&&(progression->next_stage()||fail(progression->error));}
bool StageGameplay::music(bool boss,bool show_title){
    if(alternate_music&&!platform.music_control(4,0))return false;
    const i32 index=boss?assets.definition->boss_music_index:assets.definition->stage_music_index;
    return platform.music_control(2,boss?1:0)&&platform.unlock_music(index)&&(!show_title||animations.create(assets.logo,boss?2:1,-1,0)!=0);
}
bool StageGameplay::message(i32 id){
    if(id==-1||id==-3)return music(id==-1);
    if(id==-2)return (battle.spell.flags&0x80)?platform.game_over():next_stage();
    return fail("Unexpected stage message destination");
}
i32 StageGameplay::update_game(){
    const i32 result=controller->update({battle.active_frame().rate,battle.active_frame().pressed});
    battle.active_frame().game_flags=run.scene_flags;if(result==i32(FrameAction::Error))fail(controller->error);return result;
}
i32 StageGameplay::update_background(){
    if(!background.update({battle.active_frame().rate,background_flags,background_transition_age})){fail(background.error);return i32(FrameAction::Error);}
    // STD camera motion belongs to the 3D background. The game-world motion
    // used by players/enemies and ANM flag 0x8000 have separate native owners.
    return i32(FrameAction::Continue);
}
bool StageGameplay::step(const StageGameplayFrame& frame){
    if(!prepared||suspended||!error.empty())return fail("Stage gameplay is not ready");input=frame;input.battle.game_flags=run.scene_flags;
    background.draw_objects=background_flags&1;
    if(!battle.prepare_frame(input.battle))return fail(battle.error);
    if(scheduler.update()<0)return fail(battle.error.empty()?"Stage callback failed":battle.error);
    return battle.error.empty()||fail(battle.error);
}
bool StageGameplay::presented(double seconds){
    if(!battle.spell_card)return true;auto& card=*battle.spell_card;
    return card.result_clock.present(battle.spell,card.active_frames,seconds,battle.session.replay_state!=0,capture_times)||fail(card.result_clock.error);
}
}
