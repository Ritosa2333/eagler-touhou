#include "../game/CallbackNames.hpp"
#include "World.hpp"
#ifdef TH_ENABLE_THPRAC
#include "../game/PracticeRuntime.hpp"
#endif
#include <cstdlib>
namespace th10::browser {
namespace {
template<class T> GameSystemCallbacks** system(T** p){return reinterpret_cast<GameSystemCallbacks**>(p);}
struct SessionResources final:GameSessionResourceEnvironment {
    World& w;
    explicit SessionResources(World& world):w(world){
        game=&w.state.game;
#ifdef TH_ENABLE_THPRAC
        practice=&w.state.practice;
#endif
        scores=&w.scores.data;current=&w.actors.session;
        GameSystemCallbacks** slots[]={system(&w.state.replay),system(&w.backgrounds.current),system(&w.backgrounds.previous),system(&w.actors.gui),system(&w.actors.player),system(&w.actors.bullets),system(&w.actors.items),system(&w.actors.lasers),system(&w.actors.results),system(&w.actors.hints),system(&w.actors.popups),system(&w.actors.enemies),system(&w.actors.effects),system(&w.actors.bomb),system(&w.actors.spell)};
        std::memcpy(objects,slots,sizeof(slots));current_stage=&w.state.current_stage;configuration=reinterpret_cast<const u8*>(&w.state.configuration);new_game=&w.new_game;practice_lives=&w.state.practice_shortcut;engine_flags=&w.state.engine_flags;display_flags=&w.state.configuration.display_flags;pending_screen=&w.state.pending_screen;
        drawing_resource=&w.resource_drawing;updating_resource=&w.resource_updating;pending_upload=&w.audio.manager.commands[0].kind;loader_stop_requested=&w.loader_stop;loader_running=&w.loader_running;menu_state=reinterpret_cast<i32*>(&w.state.quitting);background_color=&w.state.background_color;loading_animation=&w.common.value->loading_animation;
        recorded_time=&w.state.active_time;rendered_time=&w.state.total_time;rate=&w.engine.speed;chain=&w.chain;callbacks=&w.engine.callback_environment;update_callback=callback_id::SessionUpdate;draw_callback=callback_id::SessionDraw;loader_callback=0x417c70;
    }
    GameSession* allocate_session() override{return static_cast<GameSession*>(std::malloc(sizeof(GameSession)));}
    void release_session(GameSession* p) override{std::free(p);}
    void evict_graphics_resources() override{} // Resources are resident native browser textures.
    void begin_loading(CallbackToken token) override{if(token!=loader_callback)__builtin_trap();w.loading=true;w.loading_progress={};w.loader_stop=0;w.loader_running=0;}
    void sleep(u32 milliseconds) override{if(milliseconds!=16)__builtin_trap();w.audio.update();w.audio.pump();} // Complete the original music-command barrier before gameplay starts.
    bool create_object(SessionObject kind,i32 mode) override{return w.create_object(kind,mode);}
    void destroy_object(SessionObject kind,GameSystemCallbacks* p) override{w.destroy_object(kind,p);}
    void prepare_replay() override{w.prepare_replay();}
    void reload_gui() override{w.hud->controller_stage=&w.actors.session->replay_mode;GuiResources{*w.actors.gui,*w.hud}.load_stage();}
    void discard_gui_stage() override{GuiResources{*w.actors.gui,*w.hud}.discard_stage();}
    void clear_enemies() override{w.clear_enemies(true);}
    void stop_music() override{w.music().stop();}
    void load_music(i32 slot,const char* name) override{w.music().prepare(slot,name);}
    void music_command(i32 command) override{w.audio.manager.queue_music(command,0,"dummy");}
    void finish_loading(bool success) override{w.loading=false;if(!success)w.fail();}
    void save_score() override{w.scores.save();}
    u32 create_loading_animation(const Vec3& p) override{return w.engine.manager.create_at(*w.common.value->effects,6,p,false,AnimationPlacement::WorldBack,w.engine,w.engine);}
};
struct SessionFrame final:GameSessionEnvironment {
    World& w;AnmManager* manager;
    explicit SessionFrame(World& world):w(world),manager(&w.engine.manager){
        game=&w.state.game;scores=&w.scores.data;replay=&w.state.replay;stage=&w.backgrounds.current;previous_stage=&w.backgrounds.previous;items=&w.actors.items;gui=&w.actors.gui;enemies=&w.actors.enemies;animations=&manager;
        GameSystemCallbacks** slots[]={system(&w.actors.results),system(&w.actors.player),system(&w.actors.bullets),system(&w.actors.enemies),system(&w.actors.items),system(&w.actors.lasers),system(&w.actors.effects),system(&w.actors.bomb),system(&w.actors.popups),system(&w.actors.spell),system(&w.actors.hints)};
        std::memcpy(systems,slots,sizeof(slots));spell_foreground=&w.actors.spell->foreground_entry;loading_animation=&w.common.value->loading_animation;intro_animation=&w.common.value->introduction_animation;held=reinterpret_cast<const u32*>(&w.input.player_profiles[0].input.raw);engine_flags=&w.state.engine_flags;display_flags=&w.state.configuration.display_flags;pending_screen=&w.state.pending_screen;rate=&w.engine.speed;current_stage=&w.state.current_stage;
#ifdef TH_ENABLE_THPRAC
        practice=&w.state.practice;
#endif
    }
    void restart_stage() override{w.backgrounds.current->restart(w.backgrounds.script);}
    void fade_previous_stage() override{w.backgrounds.previous->fade_to_black(w.effects);}
    void fade_in_stage() override{w.backgrounds.current->fade_in(&w.engine.speed);}
    void hide_screen(i32 frames) override{ScreenEffect::create(ScreenEffectKind::HideScreen,frames,0,0,0,43,w.effects);}
    void stop_loader() override{w.loading=false;}
    void clear_bullets() override{w.clear_bullets();}
    void activate_player() override{w.activate_player();}
    void clear_enemies() override{w.clear_enemies(true);}
    void clear_lasers() override{w.clear_lasers();}
    void activate_replay() override{w.activate_replay();}
    void spawn_stage_controller() override{w.spawn_enemy("main",EnemySpawnParameters{});}
    void activate_gui() override{w.hud->activate();}
    void configure_player() override{w.configure_player();}
    void play_music(i32 track,bool stage_start) override{
        i32 slot=0;
#ifdef TH_ENABLE_THPRAC
        // thprac_th10.cpp:2308 th10_bgm (0x4183e0) overrides the fresh
        // stage-entry song for a custom practice section that THBGMTest selects
        // for the boss theme. The original pushes the stage-relative song index;
        // this port loads the stage theme in slot 0 and the boss theme in slot
        // 1, so the section's BGM is the second entry of the stage's song pair.
        if(stage_start&&practice_boss_bgm(w.state.practice)){
            slot=1;track=static_cast<i32>(reinterpret_cast<uintptr_t>(w.state.current_stage->music));
        }
#endif
        w.music().play(slot,track,stage_start);
    }
    void music_command(i32 command) override{w.audio.manager.queue_music(command,0,"dummy");}
    void delete_stage(Stage* p) override{w.backgrounds.destroy(p);}
    void update_score_display() override{w.hud->update_score();}
};
}
bool World::start(i32 mode){if(actors.session)__builtin_trap();
#ifdef TH_ENABLE_THPRAC
    // A non-zero replay mode is playback; cheats are unavailable while one runs.
    state.practice.replay=mode!=0;
#endif
    SessionResources env(*this);auto* session=GameSessionResources::create(mode,env);if(!session)return false;effects.controller_flags=&session->session_flags;if(hud)hud->controller_stage=&session->replay_mode;return true;}
void World::advance_loading_step(){if(!loading)return;SessionResources env(*this);GameSessionResources{*actors.session,env}.load_step(loading_progress);
#ifdef TH_ENABLE_THPRAC
    // One-shot advanced-practice setup. The original th10_patch_main hook runs
    // after the session finishes allocating its objects, so apply the run
    // parameters and script patches only once the loading barrier is passed.
    if(!loading&&!error)apply_practice(*this,state);
#endif
}
void World::advance_loading(){while(loading)advance_loading_step();}
void World::stop_session(){if(!actors.session)return;loading=false;auto* session=actors.session;SessionResources env(*this);GameSessionResources{*session,env}.shutdown();std::free(session);effects.controller_flags=nullptr;}
void World::shutdown(){
    const auto previous_screen=state.pending_screen;state.pending_screen=3;stop_session();state.game.flags&=~0xb;
    SessionResources env(*this);for(u32 i=0;i<static_cast<u32>(SessionObject::Count);i++){const auto kind=static_cast<SessionObject>(i);if(auto* value=env.object(kind))destroy_object(kind,value);}state.pending_screen=previous_screen;
}
i32 World::update_session(){SessionFrame env(*this);return actors.session->update(env);}
void World::activate_session(){SessionFrame env(*this);actors.session->activate_objects(env);}
}
