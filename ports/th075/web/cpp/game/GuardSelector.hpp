#pragma once
#include "FighterCombat.hpp"
#include "FighterControls.hpp"
#include "BattleGeometry.hpp"
#include "ReplayFile.hpp"
namespace th075 {
enum class GuardResponse:u8 {None=0,Standing=11,WrongStanding=12,EmptyStanding=13,Crouching=21,WrongCrouching=22,EmptyCrouching=23,Air=31,ChipHit=41};
struct DefenseView {
    FighterMotion& motion;const FighterMotion& opponent;FighterCombat& combat;ControlCounts& input;
    u32 collisionFlags=0;bool cpuControlled=false,training=false,guardEstablished=false;u8 trainingGuard=0,difficulty=0;
};
GuardResponse select_guard_response(DefenseView&,u32 attackFlags,ReplayStreams&);
}
