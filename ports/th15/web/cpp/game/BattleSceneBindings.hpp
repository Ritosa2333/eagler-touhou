#pragma once
#include "GameHud.hpp"
#include "StageScene.hpp"
#include "PopupManager.hpp"
#include "MessageController.hpp"
namespace th15 {
class StageAssets;
// These objects belong to the stage. Commands reach the same HUD, background
// and dialogue that its scheduled callbacks update.
struct BattleSceneBindings {
 GameHud& hud;StageScene& background;PopupManager& popups;
 MessageController& messages;MessageSceneState& music;SessionState& progress;
 i32 scene_destination=0,text_bank=-1;ScreenGridViewport screen_view;StageAssets* assets=nullptr;
 void message_state(const PlayerLifeSession& player,const PlayerSpellStatus& spell)noexcept{music.game_flags=player.mode_flags;music.spell_flags=spell.flags;music.transition_counter=progress.transition;}
};
}
