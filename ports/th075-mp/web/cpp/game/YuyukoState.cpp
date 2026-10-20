#include "YuyukoState.hpp"
namespace th075 {void reset_yuyuko(FighterActor& a,YuyukoState& s,FighterCoreEvents& events){s.impactX=0;s.impactY=0;s.aimAngle=0;s.emissionDelay=0;s.effectLoops=0;s.guardEffectSent=0;s.orbitRadius=0;s.orbitAngle=0;s.introWait=s.captionWait=0;a.commandTimers.fill(0);a.reset_round(events);}}
