#pragma once
#include "BattleGeometry.hpp"
#include "FighterAnimation.hpp"
#include "FighterControls.hpp"
namespace th075 {
// Common movement decisions shared by the original character controllers.
struct FighterMovement {
    u8 usedAirDashes=0,usedAirJumps=0;i16 recoveryDelay=0;
    void select(FighterMotion&,const FighterMotion& opponent,const ControlCounts&,FighterAnimation&);
    void aerial_recovery(float height,i8 facing,i16 health,u32 frameFlags,const ControlCounts&,bool cpuControlled,bool training,bool manualRecovery,ReplayRandom&,FighterAnimation&);
};
}
