#pragma once
#include "CharacterAttacks.hpp"
namespace th09 {
void interpolate_attack_travel(AttackActor&,const TravelAttackState&,float duration=90);
void add_mystia_attack_behaviors(std::array<AttackBehavior,27>&);
void add_field_attack_behaviors(std::array<AttackBehavior,27>&);
}
