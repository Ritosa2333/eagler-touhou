#include "BattleCheckpoint.hpp"
namespace th15 {
BattleCheckpoint::BattleCheckpoint(GameBattle& g,StageScene& s,AnmManager& a,SessionState& p,PopupManager& v,std::string& wave,CheckpointSceneServices& h,const std::array<i32,6>& banks,i32 bullet_bank):game(g),animations(a),progress(p),popups(v),scene(h),player(*g.player,a.registry,animation_pool),enemies(*g.enemies,a,animation_pool,banks),background(s),bullets(*g.bullet_scene,a,animation_pool,bullet_bank),items(*g.items),effects(g.effects,a.registry,animation_pool),chapter(p,g.session,g.score,g.enemy_world,wave,*this){
    previous_alternating_counter=g.items->alternating_counter;g.items->alternating_counter=&p.alternating_pieces;
    switch(g.selected_character()){case 0:bomb=std::make_unique<BombCheckpoint>(static_cast<BombReimu&>(*g.bomb),animation_pool);break;case 1:bomb=std::make_unique<BombCheckpoint>(static_cast<BombMarisa&>(*g.bomb),animation_pool);break;case 2:bomb=std::make_unique<BombCheckpoint>(static_cast<BombSanae&>(*g.bomb),animation_pool);break;case 3:bomb=std::make_unique<BombCheckpoint>(static_cast<BombReisen&>(*g.bomb),animation_pool);break;}
}
bool BattleCheckpoint::check(bool result,const std::string& failure){if(!result&&error.empty())error=failure;return result;}
bool BattleCheckpoint::capture(i32 id,PracticePatchEffects* practice){error.clear();const bool ok=chapter.capture(id,practice);if(!ok&&error.empty())error=chapter.error;return ok;}
bool BattleCheckpoint::restore(){error.clear();const bool ok=chapter.restore();if(!ok&&error.empty())error=chapter.error;return ok;}
bool BattleCheckpoint::save_player(){return check(player.capture(),player.error);}
bool BattleCheckpoint::save_enemies(){return check(enemies.capture(),enemies.error);}
bool BattleCheckpoint::save_background(){return check(background.capture(),background.error);}
bool BattleCheckpoint::save_bullets(){return check(bullets.capture(),bullets.error);}
bool BattleCheckpoint::save_items(){return check(items.capture(),items.error);}
bool BattleCheckpoint::save_effects(){return check(effects.capture(),effects.error);}
bool BattleCheckpoint::save_popups(){popups.save();return true;}
bool BattleCheckpoint::save_bomb(){return bomb&&check(bomb->capture(),bomb->error);}
bool BattleCheckpoint::restore_player(){return check(player.restore(),player.error);}
bool BattleCheckpoint::restore_enemies(){return check(enemies.restore(),enemies.error);}
bool BattleCheckpoint::restore_background(){return check(background.restore(),background.error);}
bool BattleCheckpoint::restore_bullets(){return check(bullets.restore(),bullets.error);}
bool BattleCheckpoint::restore_items(){return check(items.restore(),items.error);}
bool BattleCheckpoint::restore_effects(){return check(effects.restore(),effects.error);}
bool BattleCheckpoint::restore_popups(){popups.restore();return true;}
bool BattleCheckpoint::restore_bomb(){return bomb&&check(bomb->restore(),bomb->error);}
bool BattleCheckpoint::reset_spell(){return check(game.spell_card.abort(),game.spell_card.error);}
bool BattleCheckpoint::clear_lasers(){return check(game.laser_scene->manager.clear(),game.laser_scene->manager.error);}
}
