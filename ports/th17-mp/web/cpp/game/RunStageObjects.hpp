#pragma once
#include "StageAssets.hpp"
#include "AnmSceneEffects.hpp"
#include "PopupManager.hpp"
namespace th17 {
// A run owns the player, pools and HUD. Stage services may change while the
// outgoing background finishes; effect hooks must outlive both stage scenes.
class RunStageObjects {
    struct Services;std::unique_ptr<Services> services;
    i32 ascii_bank;bool initialized=false,popups_ready=false;
public:
    AnmSceneEffects animation_effects;GameBattle battle;PopupManager popups;
    std::string error;
    RunStageObjects(StageAssets&,AnmManager&,AnmEnvironment&,Rng&,Rng&,BattleWorldServices&);
    ~RunStageObjects();
    void bind(BattleWorldServices&)noexcept;
    bool initialize();bool initialize_popups();bool replace_stage(StageAssets&);
    bool ready()const noexcept{return initialized;}
};
}
