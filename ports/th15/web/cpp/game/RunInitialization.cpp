#include "RunInitialization.hpp"
namespace th15 {
bool RunInitialization::fail(const std::string& reason){if(error.empty())error=reason.empty()?"Run construction failed":reason;return false;}
ReplayRunState RunInitialization::state(){auto& s=scene();return {progress,s.battle.session,s.battle.score,s.battle.enemy_world,s.battle.player->motion,s.music.current_music_wave};}
bool RunInitialization::initialize(const StageCamera& view,i32 selected,const u8* bytes,u32 size){
 if(!run.scene()||!error.empty()||driver.driver())return fail("Run construction cannot initialize this scene");
 camera=view;destination=selected;replay_bytes=bytes;replay_size=size;registered=false;scene().battle.bind_records(&records);progress.transition=progress.replay?1:0;
 SessionInitialization controller(progress,scene().battle.session,scene().battle.score,scene().battle.enemy_world,records,*this);
 return controller.initialize()||fail(controller.error);
}
bool RunInitialization::bomb_hud(i32 stock,i32 pieces){return scene().hud.bombs(stock,pieces)||fail(scene().hud.error);}
bool RunInitialization::create_player(){return scene().initialize_player(false)||fail(scene().error);}
bool RunInitialization::configure_player(){
 if(run.practice&&run.practice->enabled&&(run.practice->force_boss_move_down||run.practice->all_clear_bonus||run.practice->flip_screen_y)&&!progress.replay)run.practice->assisted=true;
 // Purple 43c68c: apply after vanilla stock setup, before the recording's
 // initial snapshot. Native score storage uses display units divided by 10.
 if(run.practice&&run.practice->active&&!progress.replay){
  const auto& p=run.practice->run;if(!p.valid(progress.difficulty))return fail("Invalid practice initial state");
  auto& b=scene().battle;b.session.extra_lives=p.life;b.session.life_pieces=p.life_fragment;
  b.session.bombs=p.bomb;b.session.bomb_pieces=p.bomb_fragment;b.session.power=p.power;
  b.score.score=i32(p.score/10);b.score.point_value=p.value*100;b.score.graze_total=p.graze;
  if(!bomb_hud(p.bomb,p.bomb_fragment))return false;
 }
 return scene().battle.player->configure_options()||fail(scene().battle.player->error);
}
bool RunInitialization::register_session(){registered=true;return true;}
bool RunInitialization::create_replay(){auto snapshot=state();if(replay.live()||replay.replay())return replay.prepare_stage(snapshot)||fail(replay.error);return (replay_bytes?replay.open(replay_bytes,replay_size,progress.stage,snapshot):replay.initialize_live(snapshot))||fail(replay.error);}
bool RunInitialization::reset_replay(){auto snapshot=state();return replay.prepare_stage(snapshot)||fail(replay.error);}
bool RunInitialization::create_background(){return stage_definition(progress.stage)!=nullptr||fail("Background stage definition unavailable");}
bool RunInitialization::create_gui(){scene().hud.prepare_stage_assets();scene().battle.enemy_world.boss_seconds=-1;if(run.practice&&run.practice->enabled)run.practice->lock_timer.reset();return scene().hud.error.empty()||fail(scene().hud.error);}
bool RunInitialization::reset_gui(){return create_gui();}
bool RunInitialization::create_bullets(){scene().battle.bullet_scene->reset();return scene().battle.bullet_scene->error.empty()||fail(scene().battle.bullet_scene->error);}
bool RunInitialization::create_items(){return scene().battle.items->reset()||fail(scene().battle.items->error);}
bool RunInitialization::create_lasers(){return scene().battle.laser_scene->manager.clear()||fail(scene().battle.laser_scene->error);}
bool RunInitialization::create_pause_menu(){return platform.prepare_pause_menu()||fail("Pause menu preparation failed");}
bool RunInitialization::create_popups(){return scene().initialize_popups()||fail(scene().error);}
bool RunInitialization::create_checkpoint_storage(){return platform.prepare_checkpoint_storage()||fail("Checkpoint storage preparation failed");}
bool RunInitialization::create_enemies(){return scene().battle.enemies->clear()||fail(scene().battle.enemies->error);}
bool RunInitialization::restore_enemies(){return platform.restore_enemy_checkpoint(scene())||fail("Enemy checkpoint restoration failed");}
bool RunInitialization::create_bomb(){
 if(!scene().battle.bomb||!scene().battle.bomb->error.empty())return fail("Bomb controller unavailable during construction");
 if(run.practice&&run.practice->active&&run.practice->run.reisen_shield){
  if(progress.character!=3)return fail("Practice shields require Reisen");
  if(!static_cast<BombReisen*>(scene().battle.bomb.get())->practice_shields(run.practice->run.reisen_shield))return fail("Practice shield construction failed");
 }return true;
}
bool RunInitialization::create_spell(){return scene().battle.spell_card.error.empty()||fail(scene().battle.spell_card.error);}
bool RunInitialization::load_stage_music(){return platform.load_stage_theme(progress.stage)||fail("Stage music loading failed");}
bool RunInitialization::load_player_music(bool boss){return platform.load_player_theme(progress.stage,boss)||fail("Player music loading failed");}
bool RunInitialization::finish_scene(){if(!registered)return fail("Session callback phase not registered");return driver.attach(camera,destination)||fail(driver.error);}
}
