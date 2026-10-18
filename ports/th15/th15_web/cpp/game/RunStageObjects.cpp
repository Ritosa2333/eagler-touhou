#include "RunStageObjects.hpp"
namespace th15 {
RunStageObjects::RunStageObjects(StageAssets& a,AnmManager& manager,AnmEnvironment& env,Rng& game,Rng& visual,SessionState& p,BattleWorldServices& h):ascii_bank(a.ascii),animation_effects(manager,game,visual,a.effect),battle(manager,env,game,visual,h,a.scripts,a.shots,a.character,{a.player,a.bullet,a.effect}),hud(manager,p,battle.session,battle.score,a.front,a.ascii,a.logo),popups(manager){}
bool RunStageObjects::initialize(bool configure_player){if(initialized)return error.empty();if(!battle.initialize(configure_player)){error=battle.error;return false;}initialized=true;return true;}
bool RunStageObjects::initialize_popups(){if(popups_ready)return error.empty();if(!popups.initialize(ascii_bank)){error=popups.error;return false;}popups_ready=true;return true;}
bool RunStageObjects::replace_stage(StageAssets& a){if(!initialized){error="Run objects unavailable during stage replacement";return false;}if(a.character!=battle.selected_character()){error="Stage replacement changed the run character";return false;}if(!battle.replace_stage(a.scripts,a.enemy_banks,a.spell_resources(0))){error=battle.error;return false;}hud.stage_resource(a.logo);return true;}
}
