#pragma once
#include "StageAssets.hpp"
#include "GameBattle.hpp"
#include "GameHud.hpp"
#include "PopupManager.hpp"
#include "AnmSceneEffects.hpp"
namespace th15 {
// A normal next-stage factory borrows the same run objects. Scene-specific
// background, ECL, dialogue, reward and chapter snapshots live in StageGameplay.
class RunStageObjects {
 bool initialized=false,popups_ready=false;i32 ascii_bank;
 AnmSceneEffects animation_effects;
public:
 GameBattle battle;GameHud hud;PopupManager popups;std::string error;
 RunStageObjects(StageAssets&,AnmManager&,AnmEnvironment&,Rng& game,Rng& visual,SessionState&,BattleWorldServices&);
 bool initialize(bool configure_player=true);bool initialize_popups();bool replace_stage(StageAssets&);
 bool ready()const noexcept{return initialized;}
};
}
