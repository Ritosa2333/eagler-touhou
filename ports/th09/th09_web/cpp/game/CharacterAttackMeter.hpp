#pragma once
#include "Combo.hpp"
#include "AttackQueueActions.hpp"
namespace th09 {
class CharacterAttackMeter {
public:
    static bool consume(ComboState&,i32 character,i32 rank,i32 side,const Vec3&,AttackQueueActions&);
};
}
