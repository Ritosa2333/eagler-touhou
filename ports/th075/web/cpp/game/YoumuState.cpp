#include "YoumuState.hpp"
namespace th075 {void reset_youmu(FighterActor& a,YoumuState& s,FighterCoreEvents& events){s.impactX=0;s.impactY=0;s.emissionIndex=0;s.emissionDelay=0;s.effectLoops=0;s.summonWait=0;s.phantomSpawned=0;s.phantomTime=0;s.phantomAttacking=0;s.farRangeFrames=0;s.history.clear();s.introWait=s.captionWait=0;a.knockdownTime=0;a.commandTimers.fill(0);a.reset_round(events);}}
