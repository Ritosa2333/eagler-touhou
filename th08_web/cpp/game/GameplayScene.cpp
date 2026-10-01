#include "GameplayScene.hpp"
#include "InputController.hpp"
#include "PracticeRuntime.hpp"
#include "PracticeSections.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/WorldJournal.hpp"
#endif
namespace th08 {
bool GameplayScene::before_score_tables_write(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return !world_journal||world_journal->TouchAllRecords();
#else
    return true;
#endif
}
GameplayScene::GameplayScene(GameplaySession& s,TextureStore& t,AnmLibrary& l,AnmRenderer& r,GameplayPlatform& p,Chain* shared_chain,AsciiManager* shared_ascii)
 :session(s),textures(t),library(l),renderer(r),platform(p),chain(shared_chain?*shared_chain:owned_chain),animations(s.random),owned_ascii(animations,r,p),ascii(shared_ascii?*shared_ascii:owned_ascii),
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
  player_services(player_state,shots,s.pilot_views[0],s.pilot_values[0],s.gauge,s.rank,s.practice,l,animations,r,p,0,64),
#else
  player_services(player_state,shots,s.numbers,s.values,s.gauge,s.rank,s.practice,l,animations,r,p),
#endif
  player(player_state,shots,s.numbers,s.gauge,s.thresholds,s.rank,s.random,player_services.services()),
  screen(chain,r,s.random),effect_system(effect_pool,environment,animations,r,s.random,screen,player_state.shots.regions,s.values,player_state.context.replay_flags),
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
  items(player,s.pilot_views[0],s.pilot_values[0],s.gauge,s.rank,s.history,s.random,l,animations,r,player_services),
#else
  items(player,s.numbers,s.values,s.gauge,s.rank,s.history,s.random,l,animations,r,player_services),
#endif
  executor(s.random,player.timing,globals),
  enemies(program,executor,player.timing,s.random,s.numbers,s.values,s.rank,s.gauge,player,effect_system,items,projectile_pool,ascii,ascii_context,r,*this),
  bullets(projectile_pool,globals,s.random,player,items,effect_system,r,*this),
  presentation(globals,animations,p,*this),spells(globals,s.numbers,s.values,s.history,s.records,effect_system,background,animations,presentation,player_state.bomb,s.practice,enemies),spell_drawing(globals,s.records,r),
  gui(hud,display,dialogue_context,gui_context,s.numbers,s.values,s.config,animations,ascii,r,*this),
  dialogue(hud,display,dialogue_context,s.numbers,s.values,animations,p,r,*this),
  background_script(background,background_context,animations,*this),background_view(background,background_script,r,*this),spell_background(background,background_view,effect_system,animations),name_atlas(t,r),
  native_scene{player.timing,animations,screen,background,spell_background,presentation,dialogue_context.hud_redraw,screen_counter,&gui},
  player_world{effect_system,items,enemies,bullets,spells,presentation,hud,display,gui,background_script,screen,ascii,ascii_context,globals},
  background_flow(background_script,background_view,*this),bullet_flow(projectile_pool,bullets,items,effect_pool,s.random,*this),
  enemy_flow(enemies,globals,program,s.random,ascii,*this),effect_flow(effect_system,background,*this),gui_flow(gui,dialogue,*this),spell_flow(globals,spells,presentation,spell_drawing,animations,*this),
  menus(ascii.state,animations,r,s.numbers,s.values,s.config,s.statistics,*this),control(globals,menus.context,s.numbers,s.values,s.config,s.history,s.clears,s.statistics,s.random,r,control_actions){
    projectile_pool.reset();hud.implementation=&display;globals.gui=&hud;globals.live_values=this;enemies.bind_native(native_scene);player_services.bind(player_world);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for(u32 seat=1;seat<3;++seat)guest_pilots[seat-1]=std::make_unique<GuestPilot>(*this,seat);
    roster.bind(0,player,s.pilot_views[0],s.pilot_values[0],s.gauge);
    for(u32 seat=1;seat<3;++seat){
        roster.bind(seat,pilot(seat),s.pilot_views[seat],s.pilot_values[seat],s.guest_gauges[seat-1]);
        items.bind_player(seat,pilot(seat),s.pilot_views[seat],s.pilot_values[seat],s.guest_gauges[seat-1],pilot_services(seat));
    }
    bullets.bind_roster(roster);
    enemies.bind_roster(roster);
#endif
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
GameplayScene::GuestPilot::GuestPilot(GameplayScene& scene,u32 seat)
 :services(state,shots,scene.session.pilot_views[seat],scene.session.pilot_values[seat],scene.session.guest_gauges[seat-1],scene.session.rank,scene.session.practice,scene.library,scene.animations,scene.renderer,scene.platform,i32(seat),64+i32(seat)),
  simulation(state,shots,scene.session.numbers,scene.session.guest_gauges[seat-1],scene.session.guest_thresholds[seat-1],scene.session.rank,scene.session.random,services.services()){
    services.bind(scene.player_world);
}
bool GameplayScene::commit_inputs(const u16* buttons,u32 count){
    if(!buttons||count!=session.player_count||count<2||count>3)return false;
    for(u32 seat=0;seat<3;++seat)committed_buttons[seat]=seat<count?buttons[seat]:0;
    for(u32 seat=0;seat<count;++seat){pilot(seat).status().analog={};pilot(seat).status().touch_remainder={};}
    return true;
}
bool GameplayScene::commit_frame_inputs(const Netplay::FrameInput* inputs,u32 count){
    if(!inputs||count!=session.player_count||count<2||count>3)return false;
    for(u32 seat=0;seat<count;++seat)if(!multiplayer::ValidInputSample(inputs[seat]))return false;
    for(u32 seat=0;seat<count;++seat){
        committed_buttons[seat]=inputs[seat].buttons;auto& state=pilot(seat).status();
        state.analog=multiplayer::MovementSample(inputs[seat]);
    }
    for(u32 seat=count;seat<3;++seat)committed_buttons[seat]=0;
#ifdef TH_MULTIPLAYER_FIXTURES
    // Diagnostic-only lethal hit. InputButton::D is otherwise unused by the
    // recovered gameplay path. Because commit_frame_inputs runs after
    // WorldJournal/Audio BeginFrame, this exercises the same rollback/output
    // ownership as a real bullet collision while allowing one endpoint to
    // predict "no hit" until the delayed authoritative frame arrives.
    constexpr u16 fixture_death_button=8192;
    for(u32 seat=0;seat<count;++seat)if(committed_buttons[seat]&fixture_death_button){
        committed_buttons[seat]&=~fixture_death_button;
        auto& simulation=pilot(seat);auto& state=simulation.status();
        if(state.life.state==0){
            auto* created=projectile_pool.next_slot;if(!created)return false;
            for(i32 checked=0;checked<1536&&created->state;++checked)
                if((++created)->state==6)created=projectile_pool.bullets;
            if(created->state)return false;
            BulletEmission shot;shot.sprite=0;shot.color=0;shot.position=state.motion.movement.position;
            shot.angle=0;shot.speed=0;shot.ending_speed=0;shot.count=1;shot.layers=1;shot.pattern=0;
            bullets.emit(shot);if(bullets.invalid()||!created->state)return false;
            // The normal BulletSystem update later in this same game frame
            // performs the lethal collision. Keep the projectile stationary
            // and old enough to execute graze/hit checks immediately.
            created->state=1;created->position=state.motion.movement.position;
            created->velocity={};created->active_time.set(20);
        }
    }
#endif
    return true;
}
#endif
GameplayScene::~GameplayScene(){unload();}
bool GameplayScene::ControlActions::replay_stage(i32 stage){return scene.replay_stage_mask&(1u<<stage);}
void GameplayScene::ControlActions::update_enemy_name(){auto& g=scene.globals;
    // A thprac boss warp already pinned the boss name (MSGNameFix, mirroring
    // upstream th08_name_fix); the stage-progress heuristic would revert it to
    // the midboss name (e.g. Extra boss Mokou showing as Keine).
    const auto& practice=scene.session.practice;
    if(practice.active&&practice.run.mode==1&&practice.run.section)
        if(const i32 pinned=practice_boss_name_override(u32(g.stage),practice.run.section)){scene.copy_enemy_name(pinned);return;}
    scene.copy_enemy_name(EnemyNameAtlas::select(g.stage,bool(g.game_flags&0x4000),g.current_spell));}
void GameplayScene::ControlActions::release_loading_surface(){scene.platform.release_loading_surface();}
void GameplayScene::ControlActions::play_music(i32 slot,i32 song){if(!scene.practice_bgm_filter(0,song))scene.platform.play_music(slot,song);}
void GameplayScene::ControlActions::pause_audio(){if(!scene.practice_bgm_filter(2,0))scene.platform.menu_music(MenuMusic::Pause,0);}
bool GameplayScene::practice_bgm_filter(i32 command,i32 song){
    // Port of upstream ElBgmTest (thprac_games.h): while the everlasting-BGM
    // hotkey holds, duplicate starts, stops and pauses are swallowed so the
    // locked song keeps playing. The lock re-arms on the next play command.
    auto& p=session.practice;
    bool el=p.everlasting_bgm&&p.active&&!p.replay;
    if(p.run.section==TH08_ST6A_LS||(p.run.section>=TH08_ST6B_LS1&&p.run.section<=TH08_ST6B_LS5))el=false;
    const bool is_practice=(globals.game_flags&1)!=0;
    switch(command){
    case 0:
        if(p.el_bgm_lock==-1)p.el_bgm_lock=song;
        if(p.el_bgm_lock!=song){p.el_bgm_lock=-1;p.el_bgm_block=false;}
        else if(!p.el_bgm_block&&el){p.el_bgm_block=true;return false;}
        if(p.el_bgm_lock>=0&&p.el_bgm_lock!=song){p.el_bgm_lock=-1;p.el_bgm_block=false;}
        break;
    case 1:
        if(p.el_bgm_lock>=0){p.el_bgm_lock=-1;if(!is_practice||!el)p.el_bgm_block=false;}
        break;
    case 2:
        if(p.el_bgm_lock>=0)p.el_bgm_block=el;
        break;
    case 3:
        if(p.el_bgm_lock>=0&&!p.el_bgm_block&&el){p.el_bgm_block=true;return false;}
        break;
    default:break;
    }
    return p.el_bgm_block;
}
void GameplayScene::ControlActions::sound(i32 index){scene.sound(index);}
void GameplayScene::ControlActions::update_game_time(){scene.update_game_time();}
void GameplayScene::ControlActions::capture_arcade(){scene.capture_arcade();}
void GameplayScene::ControlActions::demo_fade(){scene.screen.create(ScreenEffectType::ArcadeFadeOut,120,0,0,0,21);scene.fade_music(3);}
AnmLoaded* GameplayScene::load(i32 slot,const char* path){auto bytes=platform.read(path);
    // THStage4ANM: patch our private copy before ANM decoding, preserving the
    // pristine resource/preload cache for subsequent ordinary runs.
    const auto& practice=session.practice;
    if(slot==4&&practice.active&&practice.run.mode==1&&practice.run.section&&(globals.stage==3||globals.stage==4)){
        const std::pair<u32,i32> patches[]{{0x8029c,0},{0x802b0,0},{0x802bc,4000},{0x802f8,0},{0x8030c,0},{0x802fc,1}};
        for(const auto& patch:patches){if(patch.first+4>bytes.size())return nullptr;std::memcpy(bytes.data()+patch.first,&patch.second,4);}
    }
    auto* result=library.load(slot,bytes.data(),bytes.size());if(result&&u32(slot)<32)owned_resources|=1u<<slot;return result;}
AnmLoaded* GameplayScene::get(i32 slot){return library.get(slot);}
void GameplayScene::release(i32 slot){renderer.flush();library.release(slot);if(u32(slot)<32)owned_resources&=~(1u<<slot);}
AnmVm* GameplayScene::moon(){return effect_system.fixed(64,{},12,0xffffffff);}
void GameplayScene::moon(AnmVm& vm){auto& effect=static_cast<EffectState&>(vm);if(effect.draw)effect.draw(effect,effect_system);}
void GameplayScene::effects(){effect_system.draw_background();}
void GameplayScene::copy_enemy_name(i32 index){if(index>=0)failed|=!hud.front||!name_atlas.copy(*hud.front,index);}
void GameplayScene::despawn_enemies(){
    // Original DespawnAllEnemies(0,0) retains bosses and protected enemies.
    struct Actions:EnemyScoreActions {GameplayScene& s;explicit Actions(GameplayScene& s):s(s){}void item(const Vec3& p,i32 kind,i32 mode)override{s.items.spawn(p,kind,mode);}void score_popup(const Vec3& p,i32 n,u32 color)override{s.ascii.create_score(p,n,color,s.ascii_context);}} actions(*this);
    i32 ignored=0;failed|=!enemies.population.cancel_for_score(0,ignored,actions);
}
void GameplayScene::publish_dialogue(){
    globals.game_flags=dialogue_context.flags;background.dialogue_state=dialogue_context.background_state;
    globals.spell_human_face=dialogue_context.faces[0];globals.spell_youkai_face=dialogue_context.faces[1];globals.spell_enemy_face=dialogue_context.faces[2];globals.spell_enemy_face2=dialogue_context.faces[3];
    player_state.context.game_flags=globals.game_flags;globals.gui_blocks_spawn=hud.boss_present;globals.dialogue_active=dialogue.present();
}
void GameplayScene::message(i32 entry){synchronize();failed|=!dialogue.read(entry);publish_dialogue();}
void GameplayScene::synchronize(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for(u32 seat=0;seat<session.player_count;++seat)if(roster.seats[seat].player&&roster.seats[seat].player->status().unlimited_movement_used)
        session.multiplayer_cheat_movement_used=true;
#endif
    auto& p=player_state;auto& n=session.numbers;const auto& limits=session.thresholds;
    auto& m=menus.context;m.flags=globals.game_flags;m.show_retry=globals.stage_completion;m.stage=globals.stage;m.character=globals.shot;m.spell=globals.current_spell;m.difficulty=globals.difficulty;m.spell_captured=bool(globals.spell_flags&512);m.times=hud.times;
    paused=m.pause_state!=0;retrying=m.show_retry!=0;
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    p.context.game_over=m.show_retry;
#endif
    animations.timing=player.timing;player_services.sync_values();p.stage_play_frames=signed_bits(control.state.play_frames);
    p.context.game_flags=globals.game_flags;p.context.pause=globals.paused;p.context.time_spell=u8(globals.spell_flags&1);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // ECL has one compatibility target; each pilot retains its own life owner.
    globals.player_state=p.life.state;globals.player_state_timer=p.life.timer;globals.player=p.motion.movement.position;
    for(u32 seat=1;seat<session.player_count;++seat){
        auto& other=pilot(seat);auto& state=other.status();other.timing=player.timing;
        pilot_services(seat).sync_values();state.stage_play_frames=p.stage_play_frames;
        state.context.game_flags=globals.game_flags;state.context.pause=globals.paused;state.context.time_spell=u8(globals.spell_flags&1);
    }
#else
    p.life.state=globals.player_state;p.life.timer=globals.player_state_timer;
#endif
    background_context={i32(globals.stage),bool(globals.game_flags&1024),bool(globals.game_flags&0x4000),p.motion.form.youkai!=0};
    if(globals.stage_interrupt){background.pending_interrupt=globals.stage_interrupt;globals.stage_interrupt=0;}
    dialogue_context.flags=globals.game_flags;dialogue_context.stage=i32(globals.stage);dialogue_context.character=globals.shot;dialogue_context.player_state=p.life.state;dialogue_context.background_state=background.dialogue_state;
    dialogue_context.faces[0]=globals.spell_human_face;dialogue_context.faces[1]=globals.spell_youkai_face;dialogue_context.faces[2]=globals.spell_enemy_face;dialogue_context.faces[3]=globals.spell_enemy_face2;
    globals.gui_blocks_spawn=hud.boss_present;globals.dialogue_active=dialogue.present();
    globals.pending_time=globals.spell_time_items;globals.uncollected_time_items=items.time_orb_count();
    gui_context.difficulty=globals.difficulty;gui_context.player=p.motion.movement.position;gui_context.stage_frames=enemies.state.frames;gui_context.human_frames=enemies.state.unfocused_frames;gui_context.youkai_frames=enemies.state.active_frames-enemies.state.unfocused_frames;
    gui_context.practice_replay=(globals.game_flags&9)==9;gui_context.time_stopped=time_stopped;gui_context.paused=paused;gui_context.retry=retrying;gui_context.spell_active=globals.spell_flags&1;gui_context.boss_exists=globals.boss_slots[0]!=nullptr;gui_context.input=p.input.buttons;
    if(auto* boss=globals.boss_slots[0]){gui_context.familiar_count=boss->familiar_count();gui_context.familiar_multiplier=boss->remaining_seconds;}else gui_context.familiar_count=gui_context.familiar_multiplier=0;
    environment.camera=background.camera;environment.player=p.motion.movement.position;environment.boss_present=globals.boss_slots[0]!=nullptr;environment.any_boss=0;for(auto* boss:globals.boss_slots)if(boss)environment.any_boss=1;
    if(auto* boss=globals.boss_slots[0]){environment.boss=boss->resolved_position;environment.boss_active=bool(boss->flags&1);}else{environment.boss={};environment.boss_active=0;}
    environment.dialogue=dialogue.present();environment.transition=bool(globals.game_flags&0x60);environment.tint.d3dColor=i32(globals.spell_panel_color);
    effect_system.paused=bool(globals.game_flags&1024);
    screen.context.timing=player.timing;screen.context.frozen=bool(globals.game_flags&1024);screen.context.paused=paused;screen.context.retry=retrying;
    ascii_context.player=p.motion.movement.position;ascii_context.gauge=n.gauge;ascii_context.point_value=n.point_value;ascii_context.human_limit=limits.minimum;ascii_context.youkai_limit=limits.maximum;ascii_context.human_effects=limits.human_bonus;ascii_context.youkai_effects=limits.youkai_bonus;ascii_context.human_tint=limits.human;ascii_context.youkai_tint=limits.youkai;ascii_context.paused=paused;ascii_context.retry=retrying;ascii_context.freeze_popups=bool(globals.game_flags&1024);ascii_context.effects=effect_pool.base_animation;ascii_context.demo=bool(globals.game_flags&2);ascii_context.arcade_origin=m.arcade_origin;ascii_context.arcade_size=m.arcade_size;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Shared gameplay context remains seat-zero and deterministic. The
    // bottom gauge's viewer-local selection is bound separately in AsciiManager.
    ascii_context.gauge=session.pilot_resources[0].gauge;
#endif
    presentation.context.game_flags=globals.game_flags;presentation.context.current_spell=globals.current_spell;
}
JobResult GameplayScene::boundary(i32 phase){if(phase==11)session.stall_frames=enemies.state.frames;if(phase==15)publish_dialogue();synchronize();return invalid()?JobResult::Error:JobResult::Continue;}
JobResult GameplayScene::update_player(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for(u32 seat=0;seat<session.player_count;++seat){
        auto& simulation=pilot(seat);auto& services=pilot_services(seat);
        // Multiplayer's hitbox display is an invariant, not a viewer-local
        // simulation option. A local Launcher preference used to enter
        // PlayerMotionState and fixed EffectPool state here, so endpoints with
        // different display settings could start from different canonical
        // worlds before any network input differed.
        simulation.status().input.always_hitbox=1;
        if(!services.prepare())return JobResult::Error;
        const bool updated=cooperation.seats[seat].spirit
            ?simulation.update_spirit(cooperation.seats[seat].drift_x,cooperation.seats[seat].drift_y)
            :simulation.update();
        if(!updated)return JobResult::Error;
        services.finish();
        if(!cooperation.seats[seat].spirit&&simulation.status().context.game_over)enter_spirit(seat);
    }
    update_cooperation();
#else
    player_state.input.always_hitbox=always_hitbox;if(!player_services.prepare()||!player.update())return JobResult::Error;player_services.finish();
#endif
    synchronize();return invalid()?JobResult::Error:JobResult::Continue;
}
JobResult GameplayScene::draw_players(bool impacts){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    const bool enhance=enhance_local_player_visibility&&!session.netplay.ReadOnly()&&
        session.local_player<session.player_count&&roster.eligible(session.local_player);
    for(u32 seat=0;seat<session.player_count;++seat){
        const auto old_alpha=renderer.multiplayer_player_alpha;
        if(enhance&&seat!=session.local_player){
            const auto& a=pilot(seat).status().motion.movement.position;
            const auto& b=pilot(session.local_player).status().motion.movement.position;
            const float dx=a.x-b.x,dy=a.y-b.y;
            if(dx*dx+dy*dy<84.f*84.f)renderer.multiplayer_player_alpha=104;
        }
        const bool drawn=pilot(seat).draw(ascii_context.arcade_origin,impacts);
        renderer.multiplayer_player_alpha=old_alpha;
        if(!drawn)return JobResult::Error;
    }
    if(!impacts){
        auto& text=ascii.state;const auto color=text.color;
        const Vec2 scale{text.scale_x,text.scale_y};const auto gui=text.gui,selected=text.selected;
        text.gui=text.selected=0;text.scale_x=text.scale_y=.8f;
        for(u32 seat=0;seat<session.player_count;++seat){
            const auto& p=pilot(seat).status().motion.movement.position;
            const auto& c=cooperation.seats[seat];
            if(c.power_taps>=4){text.color=0xffe2edbd;ascii.add_format({p.x+32.f,p.y-6.f,0},false,"P %u/8",u32(c.power_taps));}
            else if(c.progress&&c.target>=0){text.color=0xffd5efc8;ascii.add_format({p.x+30.f,p.y-8.f,0},false,"%u%%",u32(c.progress)*100/multiplayer::rescue_ticks);}
            if(enhance&&seat==session.local_player){text.color=0xfff3eee4;ascii.add_format({p.x+48.f,p.y+10.f,0},false,"P%u",seat+1);}
        }
        text.color=color;text.scale_x=scale.x;text.scale_y=scale.y;text.gui=gui;text.selected=selected;
    }
    return JobResult::Continue;
#else
    return player.draw(ascii_context.arcade_origin,impacts)?JobResult::Continue:JobResult::Error;
#endif
}
JobResult GameplayScene::update_ascii(){
    ascii.tick_popups(ascii_context,player.timing);if(menus.context.pause_state)menus.update_pause();
    const bool had_retry=menus.context.show_retry!=0;
    if(had_retry)menus.update_retry();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(had_retry&&!menus.context.show_retry&&menus.context.supervisor_state==2)reset_team_after_continue();
#endif
    globals.game_flags=menus.context.flags;globals.stage_completion=menus.context.show_retry;synchronize();ascii.tick_vms(ascii_context.demo);return animations.invalid?JobResult::Error:JobResult::Continue;
}
JobResult GameplayScene::update_control(){control.input.active_bullets=projectile_pool.active_count;control.input.fog=u32(background.fog.color.d3dColor);control.input.dialogue=dialogue.present();const auto result=control.update();synchronize();return invalid()?JobResult::Error:result;}
JobResult GameplayScene::update_replay(){
    if(!playback.update(globals.game_flags,session.config.slow_mode)){
        // Original attract recordings are intentionally shorter than a stage.
        // Imported/truncated files must stop at their input block boundary;
        // reading subsequent timing bytes as keys is not valid playback.
        menus.context.supervisor_state=7;return JobResult::Break;
    }
    publish_input(playback.input);control.input.replay_fps=playback.input.timing_level;
    synchronize();return JobResult::Continue;
}
void GameplayScene::publish_input(const ReplayInputState& input){
    player_state.input.buttons=input.current;player_state.bomb_input.previous_buttons=input.previous;
    dialogue_context.input=input.current;dialogue_context.previous_input=input.previous;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Dialogue is shared. The replay recorder updates input.current later in
    // the frame, so use the committed network inputs for every seat here.
    // Both peers must see the same skip/confirm decision during rollback.
    dialogue_context.input=committed_buttons[0];
    dialogue_context.previous_input=pilot(0).status().bomb_input.previous_buttons;
    for(u32 seat=1;seat<session.player_count;++seat){
        dialogue_context.input|=committed_buttons[seat]&(InputButton::Shoot|InputButton::Skip);
        dialogue_context.previous_input|=pilot(seat).status().bomb_input.previous_buttons&(InputButton::Shoot|InputButton::Skip);
    }
#endif
}
JobResult GameplayScene::update_recording(){recording.input.flags=globals.game_flags;recording.input.slow_mode=session.config.slow_mode;recording.update();publish_input(recording.input);return JobResult::Continue;}
JobResult GameplayScene::sample_replay_frame(){auto& state=recording_game?recording.frame_state:debug_frame;state.sample(session.random,control.state.restore_viewport);return JobResult::Continue;}
JobResult GameplayScene::finish_replay_frame(){bool boss_present=false;for(auto* boss:globals.boss_slots)boss_present|=boss!=nullptr;return playback.after_update(globals.game_flags,control.state.replay_mode,dialogue.present(),display.dialogue.skippable,boss_present);}
JobResult GameplayScene::draw_ascii(){
    if(playing_replay){
        ReplayTouchPoint points[10];const i32 count=platform.replay_touch_points(points,10);
        const u32 color=ascii.state.color;const float scale_x=ascii.state.scale_x,scale_y=ascii.state.scale_y;const i32 gui=ascii.state.gui,selected=ascii.state.selected;
        ascii.state.color=0xffffffff;ascii.state.scale_x=ascii.state.scale_y=.7f;ascii.state.gui=0;ascii.state.selected=0;
        for(i32 i=0;i<count;++i)ascii.add_string({points[i].x*640.f-5.f,points[i].y*480.f-7.f,0},"+",ascii_context.software_texturing);
        ascii.state.color=color;ascii.state.scale_x=scale_x;ascii.state.scale_y=scale_y;ascii.state.gui=gui;ascii.state.selected=selected;
    }
    ascii.draw_strings(ascii_context);ascii.state.string_count=0;menus.draw_pause();menus.draw_retry();if(ascii.state.demo.scriptIndex)renderer.draw_no_rotation(ascii.state.demo);return animations.invalid?JobResult::Error:JobResult::Continue;
}
void GameplayScene::bind_jobs(){
    auto bind=[&](ChainElement& job,i32 priority,bool draw,JobCallback callback){job.set_callback(callback);job.argument=this;chain.add(&job,priority,draw);};
    bind(player_calc,9,false,[](void* p){return static_cast<GameplayScene*>(p)->update_player();});
    bind(player_high,9,true,[](void* p){return static_cast<GameplayScene*>(p)->draw_players(false);});
    bind(player_low,10,true,[](void* p){return static_cast<GameplayScene*>(p)->draw_players(true);});
    bind(ascii_calc,1,false,[](void* p){return static_cast<GameplayScene*>(p)->update_ascii();});
    bind(control_calc,2,false,[](void* p){return static_cast<GameplayScene*>(p)->update_control();});
    bind(control_draw,5,true,[](void* p){return static_cast<GameplayScene*>(p)->control.draw();});
    if(playing_replay){bind(replay_calc,6,false,[](void* p){return static_cast<GameplayScene*>(p)->update_replay();});bind(replay_after,18,false,[](void* p){return static_cast<GameplayScene*>(p)->finish_replay_frame();});}
    if(recording_game)bind(record_calc,17,false,[](void* p){return static_cast<GameplayScene*>(p)->update_recording();});
    if(recording_game||(playing_replay&&playback.metadata().header.unknown6))bind(replay_bookkeeping,7,false,[](void* p){return static_cast<GameplayScene*>(p)->sample_replay_frame();});
    bind(ascii_high,14,true,[](void* p){auto& s=*static_cast<GameplayScene*>(p);s.ascii.draw_overlays(s.ascii_context);return JobResult::Continue;});
    bind(ascii_low,20,true,[](void* p){return static_cast<GameplayScene*>(p)->draw_ascii();});
    // Context adapters run after each owner at the same priority. They expose
    // original shared globals to the next owner without adding a simulated tick.
    constexpr i32 phases[]{8,11,12,13,14,15};for(u32 i=0;i<6;i++){auto& b=boundaries[i];b.scene=this;b.phase=phases[i];b.job.set_callback([](void* p){auto& b=*static_cast<Boundary*>(p);return b.scene->boundary(b.phase);});b.job.argument=&b;chain.add(&b.job,b.phase);}
}
bool GameplayScene::load(const GameplayLoad& wanted,bool initialize_values){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(wanted.initial)session.multiplayer_cheat_movement_used=false;
    if(session.player_count<2||session.player_count>3||session.local_player>=session.player_count)return false;
    for(u32 seat=1;seat<session.player_count;++seat)if(session.player_characters[seat]>=12)return false;
    session.player_characters[0]=u8(wanted.character);
    if(wanted.initial)multiplayer::reset(cooperation,u8(session.player_count));
    roster.count=session.player_count;
    for(u32 seat=0;seat<3;++seat){
        roster.seats[seat].available=seat<session.player_count;
        items.set_player_available(seat,seat<session.player_count);
    }
    gui.bind_multiplayer_resources(session.pilot_resources,session.player_count,session.local_player,roster,cooperation);
    ascii.bind_multiplayer_gauge(session.pilot_resources[session.local_player].gauge,
        session.local_player?session.guest_thresholds[session.local_player-1]:session.thresholds);
    for(u32 seat=0;seat<session.player_count;++seat){pilot(seat).set_player_count(session.player_count);pilot_services(seat).set_player_count(session.player_count);}
#endif
    if(loaded||wanted.stage<0||wanted.stage>=9||wanted.character<0||wanted.character>=12||wanted.difficulty<0||wanted.difficulty>4||((wanted.flags&0x4000)&&(wanted.spell<0||wanted.spell>=222)))return false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    multiplayer::begin_stage(cooperation);
#endif
    const bool retain_counters=!wanted.initial&&!(wanted.flags&0x4001)&&wanted.difficulty<4;
    const i32 previous_frames=enemies.state.frames,previous_human=enemies.state.unfocused_frames,previous_active=enemies.state.active_frames;
    failed=false;animations.invalid=false;player_services.reset();request=wanted;previous_input=0;player.timing={1,false};animations.timing=player.timing;
    playing_replay=initialize_values&&(wanted.flags&8);
    recording_game=initialize_values&&!playing_replay;if(recording_game&&wanted.initial)recording.reset();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Multiplayer input history is owned by the MP replay/session adapter.
    playing_replay=false;recording_game=false;
    for(u32 seat=0;seat<3;++seat)committed_buttons[seat]=previous_buttons[seat]=0;
#endif
    platform.begin_motion(wanted.stage,wanted.initial,playing_replay,recording_game);
    if(playing_replay&&(!playback.has_stage(wanted.stage)||playback.metadata().shot_type!=wanted.character||playback.metadata().difficulty!=wanted.difficulty))return false;
    menus.context=MenuContext{};menus.context.supervisor_state=wanted.supervisor_state;menus.context.system_time=now();
    // GameManager is a fresh owner on an initial title/restart entry in the
    // original game. Recreate its control state here as well; otherwise the
    // attract-mode timer survives into later demos and immediately satisfies
    // their exit thresholds. Next-stage loads deliberately retain the run.
    if(wanted.initial)control.state=GameplayControlState{};control.state.play_frames=0;
    GameplayStart startup(session,globals,control.state,menus.context,*this);
    globals.stage=wanted.stage;globals.shot=wanted.character;globals.difficulty=wanted.difficulty;globals.difficulty_mask=wanted.difficulty>=4?15:1u<<wanted.difficulty;globals.current_spell=i16(wanted.spell);globals.game_flags=wanted.flags;globals.paused=0;
    // Reset in place: the player owns almost a megabyte of bomb/shot storage.
    // A temporary here overlaps the result-state temporary during score saving.
    player_state.reset();player_state.context.game_flags=wanted.flags;
    // GameManager owns this rectangle separately from the zeroed Player.
    // Original RegisterChain: 0164d2ec=(8,16), 0164d2f4=(368,416).
    player_state.input.minimum={8,16};player_state.input.extent={368,416};
    display.dialogue.message=-1;globals.gui=&hud;hud.implementation=&display;
    dialogue_context.text=get(0);ascii.state.ascii=dialogue_context.ascii=get(1);ascii.state.capture=dialogue_context.capture=get(3);
    if(!dialogue_context.text||!dialogue_context.ascii||!dialogue_context.capture)return false;
    if(initialize_values&&!startup.before_player(wanted.initial))return false;
    presentation.context.text=dialogue_context.text;spell_drawing.digits=dialogue_context.ascii;
    std::memcpy(dialogue_context.clears,session.clears,sizeof(session.clears));
    const bool section_warp=session.practice.active&&session.practice.run.section!=0;
    if(!player_services.prepare()||!player.initialize({u8(wanted.character),wanted.initial,bool(wanted.flags&0x4000),u8(section_warp),{384,448}})){unload();return false;}player_services.finish();
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for(u32 seat=1;seat<session.player_count;++seat){
        auto& simulation=pilot(seat);auto& state=simulation.status();auto& services=pilot_services(seat);
        state.reset();services.reset();state.context.game_flags=wanted.flags;state.input.minimum={8,16};state.input.extent={368,416};simulation.timing=player.timing;
        if(!services.prepare()||!simulation.initialize({session.player_characters[seat],wanted.initial,bool(wanted.flags&0x4000),u8(section_warp),{384,448}})){unload();return false;}
        services.finish();
    }
    for(u32 seat=0;seat<session.player_count;++seat)pilot(seat).place_multiplayer_spawn(seat,session.player_count);
#endif
    if(initialize_values){
        if(!startup.after_player(player.profile(false).initial_bombs)){unload();return false;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        const bool fresh=wanted.initial||(wanted.flags&0x4001)||wanted.difficulty>=4;
        for(u32 seat=0;seat<session.player_count;++seat){
            auto& bank=session.pilot_resources[seat];
            if(fresh){bank.reset();bank.lives=session.numbers.lives;bank.power=session.numbers.power;bank.bombs=pilot(seat).profile(false).initial_bombs;}
            bank.deaths_stage=bank.bombs_used_stage=0;bank.gauge_copy=bank.gauge;
            pilot_services(seat).sync_values();
        }
#endif
        if(playing_replay){if(!playback.begin(wanted.stage,session,globals,player_state.context.miss_control)){unload();return false;}playback.input.current=playback.input.previous=0;replay_stage_mask=playback.stage_mask();if(wanted.initial&&playback.metadata().header.unknown6)sample_replay_frame();}
        startup.after_replay();std::memcpy(dialogue_context.clears,session.clears,sizeof(session.clears));
    }
    background_context={wanted.stage,false,bool(wanted.flags&0x4000),false};background_flow.context={wanted.keep_resources,dialogue_context.text};
    bullet_flow.context={wanted.initial,wanted.release_resources};enemy_flow.context={wanted.initial,wanted.keep_resources,wanted.release_resources};effect_flow.context={wanted.stage,wanted.spell,bool(wanted.flags&0x4000),wanted.keep_resources};gui_flow.context={wanted.initial,wanted.keep_resources,wanted.release_resources,u8(section_warp),wanted.spell};spell_flow.context={wanted.initial,wanted.keep_resources,wanted.release_resources};
    dialogue_context.flags=globals.game_flags;dialogue_context.stage=wanted.stage;dialogue_context.character=wanted.character;gui_context.difficulty=wanted.difficulty;items.difficulty=wanted.difficulty;
    if(!background_flow.attach(chain,wanted.stage)||!bullet_flow.attach(chain)||!enemy_flow.attach(chain)||!effect_flow.attach(chain)||!gui_flow.attach(chain)){unload();return false;}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Bomb announcements and enemy spellcards share the native spell owner.
    // Its portraits, banners and calculation/draw jobs must exist before the
    // first gameplay tick. Keep the ordinary baseline outside this MP change.
    if(!spell_flow.attach(chain)){unload();return false;}
#endif
    // These three counters belong to GameManager across stages. EnemyManager
    // increments them, but replacing that owner must not reset a whole run.
    if(retain_counters){enemies.state.frames=previous_frames;enemies.state.unfocused_frames=previous_human;enemies.state.active_frames=previous_active;}
    if(recording_game){const bool first=!recording.ready();if(!recording.begin(wanted.stage,session,globals,player_state.context.miss_control)){unload();return false;}if(first){recording.input.timing_level=60;sample_replay_frame();}}
    if(auto* header=background_script.program.header())std::memcpy(dialogue_context.song_paths,header->song_paths,sizeof(dialogue_context.song_paths));
    if(initialize_values){startup.after_resources(wanted.keep_resources,dialogue_context.song_paths,background.dialogue_state);time_stopped=false;ascii.state.blindness_color=0;screen_counter=2;control.state.load_state=0;}
    synchronize();ascii_context.effects=effect_pool.base_animation;ascii.reset();ascii.initialize_vms(ascii_context);
    menus.context.shot_bombs=number(player.profile(false).initial_bombs).truncate_int();
    if(!initialize_values){control.state.stage_mask=u16(1u<<wanted.stage);control.state.start_music=wanted.keep_resources&&(wanted.flags&0x4000)&&!spell_music(wanted.spell).pause_in_practice?2:1;}
    globals.frame_count_value=&enemies.state.frames;session.stall_frames=enemies.state.frames;
    if(initialize_values){
        if(!apply_practice(*this,session)){unload();return false;}
        // GuiFlow creates the stage-entry clock before thprac restores the
        // configured night value. Refresh that already-created VM so the
        // entrance graphic and the later result screen use the same clock.
        if(hud.times&&hud.times->SetSprite(&display.clock_intro,session.numbers.clock_time)){unload();return false;}
    }
    bind_jobs();loaded=true;synchronize();return ready();
}
void GameplayScene::unload(bool keep,bool release_all){
    if(loaded&&recording_game)recording.stop();
    loaded=false;background_flow.context.keep_resources=keep;bullet_flow.context.release_resources=release_all;enemy_flow.context.keep_resources=keep;enemy_flow.context.release_resources=release_all;
    effect_flow.context.keep_resources=keep;gui_flow.context.keep_resources=keep;gui_flow.context.release_resources=release_all;spell_flow.context.keep_resources=keep;spell_flow.context.release_resources=release_all;
    background_flow.detach();bullet_flow.detach();enemy_flow.detach();effect_flow.detach();gui_flow.detach();spell_flow.detach();screen.clear();
    for(auto* job:{&player_calc,&player_high,&player_low,&ascii_calc,&ascii_high,&ascii_low,&control_calc,&control_draw,&replay_calc,&replay_after,&record_calc,&replay_bookkeeping})chain.cut(job);
    for(auto& boundary:boundaries)chain.cut(&boundary.job);
    for(i32 slot=4;slot<20;slot++){const bool stage_resource=slot==4||slot==8||slot==9||slot==13||slot==18||slot==19;if((owned_resources&(1u<<slot))&&(stage_resource?!keep:release_all))release(slot);}
    if(release_all){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        for(i32 slot=64;slot<70;++slot)library.release(slot);
        for(auto& guest:guest_pilots)if(guest){guest->shots[0]=ShotResource{};guest->shots[1]=ShotResource{};}
#else
        release(5);
#endif
        shots[0]=ShotResource{};shots[1]=ShotResource{};
    }
}
bool GameplayScene::prepare_frame(u16 buttons,float rate,bool force_unit){
    if(!ready())return false;
    // Both the application-owned shared chain and the standalone scene enter
    // here. Capturing only in update() left the actual SDL Runtime's effect,
    // spell and ASCII endpoints uninitialized for its entire session.
    effect_system.snapshot_presentation();spell_drawing.snapshot_presentation();background_view.snapshot_spell_presentation();ascii.snapshot_presentation(ascii_context);
    bullets.snapshot_presentation();
    player.timing={rate,force_unit};player_state.input.buttons=buttons;player_state.bomb_input.previous_buttons=previous_input;dialogue_context.previous_input=previous_input;dialogue_context.input=buttons;menus.context.keys=buttons;menus.context.previous_keys=previous_input;previous_input=buttons;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for(u32 seat=0;seat<session.player_count;++seat){
        auto& simulation=pilot(seat);auto& state=simulation.status();simulation.timing=player.timing;
        state.input.buttons=committed_buttons[seat];state.bomb_input.previous_buttons=previous_buttons[seat];previous_buttons[seat]=committed_buttons[seat];
    }
#endif
    if(recording_game){recording.input.physical=buttons;publish_input(recording.input);}else if(playing_replay)publish_input(playback.input);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    else {
        ReplayInputState shared_input{};shared_input.current=buttons;shared_input.previous=player_state.bomb_input.previous_buttons;
        publish_input(shared_input);
    }
#endif
    // Practice cheats run after input publication so F6 can press the bomb key.
    update_practice(*this,session);
    synchronize();return !invalid();
}
bool GameplayScene::update(u16 buttons,float rate,bool force_unit){presentation::CalculationScope presentation_tick;if(!prepare_frame(buttons,rate,force_unit))return false;failed|=chain.run()<0;return !invalid();}
bool GameplayScene::draw(){if(!ready())return false;failed|=chain.run(true)<0;renderer.flush();return !invalid();}
}

