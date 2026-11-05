#pragma once
#include "Timer.hpp"
namespace th15 {
struct ItemState {Vec3 position{},velocity{};float reserved=0;Timer age{0,0,0,0,0},auxiliary{0,0,0,0,0};i32 state=0,kind=0,appearance=0;float attraction_speed=0;i32 delay=0,stamp=0;u32 flags=0;i32 extra0=0,extra1=0;};
static_assert(sizeof(ItemState)==104);
enum class ItemKind:i32 {Power=1,Point=2,LargePower=3,LifePiece=4,Life=5,BombPiece=6,Bomb=7,FullPower=8,CancelPoint=9,CancelPower=10,CancelBonus=11,AlternatingPiece=12,CancelGraze=13,CancelGrazeLarge=14,CancelGrazeLargest=15};
}
