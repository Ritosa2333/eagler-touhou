#pragma once
#include "BattleGeometry.hpp"
namespace th075 {
struct BodySeparation {
    i8 leftOccupant=-1,rightOccupant=-1;
    bool advance(FighterMotion& first,const CollisionRectangle& firstBody,FighterMotion& second,const CollisionRectangle& secondBody);
};
}
