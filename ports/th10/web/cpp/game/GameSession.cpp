#include "GameSession.hpp"
#include "GameProgression.hpp"
#include "ScreenEffect.hpp"
#ifdef TH_ENABLE_THPRAC
#include "PracticeRuntime.hpp"
#endif
namespace th10 {
// 0x418a00. Decay subtracts the previous speed before raising it to 18.
void decay_faith(GameEconomy& game) noexcept {
    if(game.faith_timer.current>0){game.faith_timer.advance(-1);return;}
    if(game.item_value>5000){game.item_value=wrapping_add(game.item_value,static_cast<i32>(0u-static_cast<u32>(game.enemy_activity)));if(game.enemy_activity<18)game.enemy_activity=18;if(game.item_value<5000)game.item_value=5000;}
}
void GameSession::reset_timer(float* rate) noexcept {if(!(timer_flags&1)){elapsed.rate=rate;timer_flags|=1;}elapsed.initialize(-1);}
static void enable(GameSystemCallbacks& callbacks){if(callbacks.update)callbacks.update->flags|=2;if(callbacks.draw)callbacks.draw->flags|=2;}
void GameSession::activate_objects(GameSessionEnvironment& env){
    session_flags&=~0x800u;env.clear_bullets();env.activate_player();
    auto& items=**env.items;std::memset(items.regular,0,sizeof(items.regular)+sizeof(items.faith));
    env.clear_enemies();env.clear_lasers();env.game->stage_frames=0;env.game->section_frames=0;env.activate_replay();env.spawn_stage_controller();env.activate_gui();
    enable(**env.systems[0]);enable(**env.systems[1]);env.configure_player();
    for(i32 i=2;i<10;++i)enable(**env.systems[i]);(*env.spell_foreground)->flags|=2;enable(**env.systems[10]);
}
// 0x418190. During a stage transition the old background fades for 30 frames
// before the new playfield is activated. Paused frames return 3 to the chain.
i32 GameSession::update(GameSessionEnvironment& env){
    auto& game=*env.game;auto& registry=(*env.animations)->registry;
    if(elapsed.current==0){
        if(session_flags&8){env.stop_loader();*env.pending_screen=(*env.engine_flags&0x1000)?2:3;return 1;}
        env.restart_stage();
        if(*env.previous_stage){env.fade_previous_stage();env.fade_in_stage();session_flags|=0x800;registry.interrupt((*env.gui)->notification,1);}
        else{
            activate_objects(env);if(!(game.flags&0x20))env.play_music((*env.current_stage)->reserved_024,true);
            registry.interrupt(*env.intro_animation,1);registry.interrupt(*env.loading_animation,1);*env.loading_animation=0;
        }
    }else if(elapsed.current==30&&(session_flags&0x800)){
        activate_objects(env);env.music_command((*env.display_flags&0x10)?4:3);env.play_music((*env.current_stage)->reserved_024,false);
        registry.interrupt(*env.loading_animation,1);*env.loading_animation=0;reset_timer(env.rate);
    }
    if(*env.previous_stage&&((*env.previous_stage)->draw_flags&8))env.delete_stage(*env.previous_stage);
    if(session_flags&4){session_flags|=0x80;return 1;}
    env.stop_loader();
    if(game.flags&0x20){
        if((*env.held&0x160b)||(session_flags&0x70))*env.pending_screen=(*env.engine_flags&0x1000)?2:4;
        if(elapsed.current==2940)env.hide_screen(60);
        else if(elapsed.current==3000)*env.pending_screen=(*env.engine_flags&0x1000)?2:4;
    }
    env.update_score_display();if(session_flags&0x70)return 3;
    if((*env.replay)->mode!=1){i32 frames;auto* value=(*env.scores)->characters[game.character*3+game.shot_type].statistics+4;std::memcpy(&frames,value,4);if(frames<215999999){frames=wrapping_add(frames,1);std::memcpy(value,&frames,4);}}
#ifdef TH_ENABLE_THPRAC
    // F6 no-faith-loss: suppress the faith decay call (0x418A2B upstream); the
    // stored value is never rewritten, only the decrement is skipped.
    if(!(env.practice&&practice_no_faith_loss(*env.practice))&&!(*env.enemies)->bosses[0]&&!(*env.gui)->dialogue&&elapsed.current>=90)decay_faith(game);
#else
    if(!(*env.enemies)->bosses[0]&&!(*env.gui)->dialogue&&elapsed.current>=90)decay_faith(game);
#endif
    ++game.stage_frames;++game.section_frames;elapsed.tick();return 1;
}
// 0x4187a0 / 0x4187d0. Counts are cleared only after loading has finished.
i32 GameSession::draw(AnmManager& animations) const noexcept {
    if(!(session_flags&4)){animations.reserved_050=0;animations.submitted_draws=0;animations.started_scripts=0;animations.flushed_batches=0;}return 1;
}
}
