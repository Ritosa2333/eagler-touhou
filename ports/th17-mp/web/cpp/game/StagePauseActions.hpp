#pragma once
#include "PauseActions.hpp"
#include "StageGameplay.hpp"
namespace th17 {
class StagePauseActions final:public PauseGameplayActions {
 StageGameplay& scene;AnmManager& animations;
public:
 StagePauseActions(StageGameplay& s,AnmManager& a):scene(s),animations(a){}
 bool options_changed()override{return scene.battle.player&&scene.battle.player->configure_options();}
 bool life_hud(i32 lives,i32 pieces)override{return scene.battle.hud&&scene.battle.hud->lives(lives,pieces);}
 bool bomb_hud(i32 bombs,i32 pieces)override{return scene.battle.hud&&scene.battle.hud->bombs(bombs,pieces);}
 bool power_notice()override{return scene.battle.hud&&scene.battle.hud->result(0,2);}
 bool dialogue_paused(bool);bool resume_dialogue()override{return dialogue_paused(false);}
 bool resume_chapter_result()override{return scene.battle.hud&&animations.pause(scene.battle.hud->chapter_result_notice,false);}
};
}
