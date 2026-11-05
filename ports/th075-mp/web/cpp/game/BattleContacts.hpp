#pragma once
#include "OwnedObjects.hpp"
#include "CollisionPipeline.hpp"
namespace th075 {
struct BattleContactSignals:CollisionSignals {virtual void select_side(u8)=0;};
class BattleContacts {
public:
    CollisionBounds bounds;BattleRecords records;u8 difficulty=2;
    void advance(const std::array<FighterActor*,2>&,const std::array<OwnedObjects*,2>&,GameMode,BattlePhase,ReplayStreams&,BattleContactSignals&);
};
}
