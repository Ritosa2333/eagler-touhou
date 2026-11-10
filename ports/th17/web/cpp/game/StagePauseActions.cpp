#include "StagePauseActions.hpp"
namespace th17 {
bool StagePauseActions::dialogue_paused(bool paused){
 const auto* d=scene.battle.dialogue.get();if(!d)return true;
 for(const auto h:{d->player_portrait,d->name,d->boss_label,d->bubble})if(!animations.pause(h,paused))return false;
 for(const auto h:d->boss_portraits)if(!animations.pause(h,paused))return false;
 for(const auto h:d->text_handles)if(!animations.pause(h,paused))return false;
 return true;
}
}
