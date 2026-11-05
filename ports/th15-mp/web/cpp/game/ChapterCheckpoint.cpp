#include "ChapterCheckpoint.hpp"
namespace th15 {
bool ChapterCheckpoint::check(bool ok,const char* reason){if(!ok&&error.empty())error=reason;return ok;}
void ChapterCheckpoint::count_next_retry(){
    if(progress.stage==1&&progress.chapter==0)return;
    saved_progress.stage_deaths[0]=wrapping_add(saved_progress.stage_deaths[0],1);
    saved_progress.stage_deaths[progress.stage+1]=wrapping_add(saved_progress.stage_deaths[progress.stage+1],1);
    saved_progress.chapter_deaths=wrapping_add(saved_progress.chapter_deaths,1);
}
bool ChapterCheckpoint::capture(i32 chapter,PracticePatchEffects* practice){
    if(progress.stage<0||progress.stage>7)return check(false,"Checkpoint stage outside range");
    available=false;if(!check(host.synchronize_resources(),"Checkpoint resource boundary failed"))return false;
    if(progress.chapter!=chapter)progress.run_clock=0;progress.chapter=chapter;
    // Purple 43dd58 executes after the native chapter comparison/assignment.
    if(practice&&practice->chapter_set!=-1){progress.chapter=practice->chapter_set;practice->chapter_set=-1;}
    enemies.current_chapter=progress.chapter;
    score.graze_chapter=0;enemies.chapter_total=enemies.chapter_defeated=0;
    saved_progress=progress;saved_player=player;saved_score=score;saved_total=enemies.chapter_total;saved_defeated=enemies.chapter_defeated;saved_rank=enemies.rank;saved_music=music;
    saved_progress.checkpoint_power=player.power>player.power_step?player.power:player.power_step;
    saved_progress.chapter_deaths=0;count_next_retry();progress.chapter_deaths=0;
    if(!check(host.clear_saved_animations(),"Checkpoint animation pool clear failed")||!check(host.save_player(),"Player snapshot failed")||!check(host.save_enemies(),"Enemy snapshot failed")||!check(host.save_background(),"Background snapshot failed")||!check(host.save_bullets(),"Bullet snapshot failed")||!check(host.save_items(),"Item snapshot failed")||!check(host.save_effects(),"Effect snapshot failed")||!check(host.save_popups(),"Popup snapshot failed")||!check(host.save_bomb(),"Bomb snapshot failed"))return false;
    available=true;
    if((player.mode_flags&0x300)&&!check(host.checkpoint_file(false),"Checkpoint file save failed")){available=false;return false;}
    // Purple 43dece hooks the return, after all checkpoint copies/file work.
    // This must not change saved_total: retry restores the original snapshot.
    if(practice&&practice->extra_boss_chapter_bonus){enemies.chapter_total=1;practice->extra_boss_chapter_bonus=false;}
    return true;
}
void ChapterCheckpoint::restore_progress(){
    // Display options, input/replay mode and scene transition flags lie outside
    // the original saved progress and must survive a retry unchanged.
    progress.stage=saved_progress.stage;progress.starting_stage=saved_progress.starting_stage;progress.chapter=saved_progress.chapter;progress.stage_frame=saved_progress.stage_frame;progress.run_clock=saved_progress.run_clock;
    progress.character=saved_progress.character;progress.subcharacter=saved_progress.subcharacter;progress.difficulty=saved_progress.difficulty;progress.continues=saved_progress.continues;progress.spell_id=saved_progress.spell_id;
    progress.initial_point_value=saved_progress.initial_point_value;progress.stage_deaths=saved_progress.stage_deaths;progress.chapter_deaths=saved_progress.chapter_deaths;progress.checkpoint_power=saved_progress.checkpoint_power;progress.alternating_pieces=saved_progress.alternating_pieces;
    player.extra_lives=saved_player.extra_lives;player.life_pieces=saved_player.life_pieces;player.deaths=saved_player.deaths;player.power=saved_player.power;player.power_step=saved_player.power_step;player.bombs=saved_player.bombs;player.bomb_pieces=saved_player.bomb_pieces;
    score=saved_score;enemies.current_chapter=progress.chapter;enemies.chapter_total=saved_total;enemies.chapter_defeated=saved_defeated;enemies.rank=saved_rank;music=saved_music;
}
bool ChapterCheckpoint::restore(){
    if(!available)return check(false,"No chapter checkpoint available");
    if(!check(host.synchronize_resources(),"Checkpoint resource boundary failed")||!check(host.retire_reward(),"Chapter reward retirement failed")||!check(host.retire_message(),"Message retirement failed"))return false;
    restore_progress();const i32 penalty=progress.chapter_deaths<50?progress.chapter_deaths:50,power=wrapping_sub(progress.checkpoint_power,penalty);player.power=power<player.power_step?player.power_step:power;count_next_retry();
    if((player.mode_flags&0x300)!=0x200&&!check(host.retire_scene_effect(),"Restart effect retirement failed"))return false;
    if(!check(host.restore_player(),"Player restore failed")||!check(host.restore_enemies(),"Enemy restore failed")||!check(host.restore_background(),"Background restore failed")||!check(host.restore_bullets(),"Bullet restore failed")||!check(host.restore_items(),"Item restore failed")||!check(host.reset_spell(),"Spell reset failed")||!check(host.clear_lasers(),"Laser clear failed")||!check(host.restore_effects(),"Effect restore failed")||!check(host.restore_popups(),"Popup restore failed")||!check(host.restore_bomb(),"Bomb restore failed")||!check(host.life_hud(player.extra_lives,player.life_pieces),"Life HUD restore failed")||!check(host.bomb_hud(player.bombs,player.bomb_pieces),"Bomb HUD restore failed")||!check(host.reset_gui(),"GUI restore failed")||!check(host.stop_sounds(),"Retry sound reset failed"))return false;
    progress.scene_flags|=0x10000;progress.restart_frames=0;progress.startup_frames=60;
    if((player.mode_flags&0x300)!=0x200&&!check(host.begin_restart_effect(),"Restart effect failed"))return false;
    if(!check(host.begin_restart_overlay(),"Restart overlay failed"))return false;
    if((player.mode_flags&0x300)&&!check(host.checkpoint_file(true),"Checkpoint file restore failed"))return false;
    progress.rate=1;return true;
}
}
