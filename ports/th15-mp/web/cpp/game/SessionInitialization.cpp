#include "SessionInitialization.hpp"
namespace th15 {
bool SessionInitialization::check(bool ok,const char* message){if(!ok&&error.empty())error=message;return ok;}
bool SessionInitialization::initialize(){
    if(state.character<0||state.character>3||state.stage<0||state.stage>7||state.difficulty<0||state.difficulty>4)return check(false,"Invalid session selection");
    state.scene_flags|=4;state.rate=1;state.scene_flags&=~0x4000u;state.stage_frame=state.run_clock=0;
    if(!state.replay&&!check(records.mark_stage(state.character+state.subcharacter,state.stage,state.difficulty),"Stage record unavailable"))return false;
    if(state.new_run){
        if(state.stage==7&&state.difficulty<4)state.difficulty=4;
        const u32 mode=(player.mode_flags>>4)&3;const bool legacy=(player.mode_flags&0x300)==0;
        const auto best=records.high_score({mode==2?RecordScope::Spell:mode?RecordScope::Stage:RecordScope::FullRun,state.character+state.subcharacter,state.difficulty,state.stage,state.spell_id,legacy});
        state.high_score=best.score;state.high_score_extra=mode?0:best.extra;
        if(!(player.mode_flags&8))state.continues=0;
        score.graze_total=0;score.score=0;score.point_items=0;player.power=0;player.power_step=1;
        player.bombs=3;if(!check(host.bomb_hud(player.bombs,player.bomb_pieces),"Initial bomb HUD unavailable"))return false;
        player.bomb_pieces=0;player.life_pieces=0;score.life_piece_tier=0;score.chapter_value=0;score.chapter_count=0;score.collection_timer=0;state.alternating_pieces=0;
        state.stage_deaths.fill(0);state.chapter_deaths=0;player.deaths=0;
        state.initial_point_value=1000000;score.max_point_value=state.difficulty==0?25000000:50000000;score.point_value=truncate_int(float(float(state.initial_point_value)/100.f)*100.f);state.continue_budget=state.difficulty==4?0:5;
        if(mode==2){player.extra_lives=0;player.bombs=0;if(!check(host.bomb_hud(0,player.bomb_pieces),"Spell practice bomb HUD unavailable"))return false;}
        else if(!mode)player.extra_lives=2;
        else player.extra_lives=state.configured_lives?wrapping_add(state.configured_lives,-1):9;
        if(!check(host.create_player(),"Player creation failed"))return false;
        const i32 power=wrapping_mul(player.power_step,4);
        if(mode==2||state.stage>1)player.power=power>score.max_power?score.max_power:power<player.power_step?player.power_step:power;
        else player.power=player.power_step>score.max_power?score.max_power:player.power_step;
        if(!check(host.configure_player(),"Player options configuration failed"))return false;
        player.mode_flags&=~4u;
        if(!state.replay&&!check(records.count_play(state.character+state.subcharacter,legacy),"Play count update failed"))return false;
    }else if(u32(score.score)>u32(state.high_score))state.high_score=score.score;
    score.difficulty=state.difficulty;enemies.difficulty=state.difficulty;enemies.mode_flags=player.mode_flags;
    if(!check(host.register_session(),"Session callbacks unavailable"))return false;
    if(player.mode_flags&2){if(!check(host.reset_replay(),"Replay restart failed")||!check(host.reset_gui(),"GUI restart failed")||!check(host.create_background(),"Stage background creation failed"))return false;}
    else {
        if(!check(host.create_replay(),"Replay creation failed")||!check(host.create_background(),"Stage background creation failed")||!check(host.create_gui(),"GUI creation failed")||!check(host.create_bullets(),"Bullet manager creation failed")||!check(host.create_items(),"Item manager creation failed")||!check(host.create_lasers(),"Laser manager creation failed")||!check(host.create_pause_menu(),"Pause menu creation failed")||!check(host.create_popups(),"Popup manager creation failed")||!check(host.create_checkpoint_storage(),"Checkpoint storage creation failed"))return false;
    }
    if(player.mode_flags&9){if(!check(host.restore_enemies(),"Enemy checkpoint restore failed"))return false;}
    else if(!check(host.create_enemies(),"Enemy manager creation failed"))return false;
    if(!check(host.create_bomb(),"Bomb controller creation failed")||!check(host.create_spell(),"Spell controller creation failed"))return false;
    if(!(player.mode_flags&0x40)){
        if((player.mode_flags&0x30)!=0x20&&!check(host.load_stage_music(),"Stage music preparation failed"))return false;
        if(!check(host.load_player_music(false),"Player theme preparation failed")||!check(host.load_player_music(true),"Boss theme preparation failed"))return false;
    }
    state.stage_deaths[state.stage+1]=0;state.chapter_deaths=0;state.startup_frames=60;
    state.scene_flags&=~4u;player.mode_flags&=~11u;enemies.mode_flags=player.mode_flags;
    return check(host.finish_scene(),"Session startup finalization failed");
}
}
