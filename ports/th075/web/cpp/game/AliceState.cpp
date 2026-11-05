#include "AliceState.hpp"
namespace th075 {void reset_alice(FighterActor& a,AliceState& s,FighterCoreEvents& events){s.actionTicks=0;s.actionStage=0;s.volleyTicks=0;s.volleyCount=0;s.knockbackX=0;s.knockbackY=0;s.dollCenterX=0;s.dollCenterY=0;s.dollRadius=0;s.dollPhase=0;s.farRangeFrames=0;s.introWait=s.captionWait=0;a.knockdownTime=0;a.commandTimers.fill(0);a.reset_round(events);}}
