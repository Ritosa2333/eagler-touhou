#include "PatchouliState.hpp"
namespace th075 {void reset_patchouli(FighterActor& a,PatchouliState& s,FighterCoreEvents& events){s.actionTicks=0;s.actionStage=0;s.emissionIndex=0;s.emissionDelay=0;s.effectLoops=0;s.blockEffectSent=0;s.orbitAngle=0;s.spinAngle=0;s.windForceTime=0;s.farRangeFrames=0;s.introWait=s.captionWait=0;a.knockdownTime=0;a.commandTimers.fill(0);a.reset_round(events);}}
