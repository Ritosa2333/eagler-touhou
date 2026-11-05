#include "RollbackState.hpp"
namespace th075::sdl {
std::unique_ptr<RollbackState> Application::capture_rollback()const{return battle&&battleRenderer?std::make_unique<RollbackState>(*this):nullptr;}
bool Application::restore_rollback(const RollbackState& state){if(!battle||!battleRenderer||sceneCode!=0)return false;state.restore(*this);return true;}
bool Application::rollback_predictable()const{
 if(sceneCode!=0||!battle||battle->progress.paused||battle->progress.pauseFade)return false;
 const auto& rules=battle->simulation.rules;
 // These phases only mutate the checkpointed battle/HUD/background. Waiting
 // for every remote frame here makes the opening and each recovery run at
 // network speed despite enabling rollback. Victory dialogue owns textures
 // and the following menu/replay lifecycle, so keep that boundary confirmed.
 switch(rules.phase){
 case BattlePhase::Intro:case BattlePhase::Active:case BattlePhase::RoundKnockout:case BattlePhase::RoundRecovery:return true;
 case BattlePhase::MatchKnockout:return rules.countdown>10;
 default:return false;
 }
}
}
