#pragma once
#include "PauseActions.hpp"
#include "StageGameplay.hpp"
namespace th15 {
class StagePauseActions final:public PauseGameplayActions {
 StageGameplay& scene;AnmManager& animations;
public:
 StagePauseActions(StageGameplay& s,AnmManager& a):scene(s),animations(a){}
 bool restore_checkpoint()override{return scene.restore();}
 bool options_changed()override{return scene.battle.player&&scene.battle.player->configure_options();}
 bool life_hud(i32 lives,i32 pieces)override{return scene.hud.life(lives,pieces);}
 bool bomb_hud(i32 bombs,i32 pieces)override{return scene.hud.bombs(bombs,pieces);}
 bool power_notice()override{return scene.hud.notification(0,2);}
 bool resume_dialogue()override;
 bool resume_chapter_result()override{return animations.pause(scene.hud.result_notice,false);}
};
}
