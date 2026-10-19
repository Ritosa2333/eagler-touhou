#include "MarisaState.hpp"
namespace th075 {
void reset_marisa(FighterActor& a,MarisaState& s,FighterCoreEvents& events){
    s.actionTicks=s.actionStage=s.volleyIndex=0;s.recoveryCounters.fill(0);s.effectCounters.fill(0);
    s.frame={};s.chargeMode=s.chargeCount=s.chargeRemaining=0;s.spellCounters.fill(0);
    s.introWait=s.captionWait=0;s.laser.visible=false;a.knockdownTime=0;a.commandTimers.fill(0);
    for(u32 i=0;i<37;i++){
        const float u=float(double(i)/36.0);
        s.laser.vertices[i*2]={0,0,0,1,0xffffffff,u,0};
        s.laser.vertices[i*2+1]={0,0,0,1,0xffffffff,u,1};
    }
    a.reset_round(events);
}
}
