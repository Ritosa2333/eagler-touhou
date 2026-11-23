#pragma once
#include "game/BulletCollisionRules.hpp"
#include "game/BulletRules.hpp"
namespace th128 { class StageWorld; namespace laser::selector3 {struct Position;} }
namespace th128::coopphysics {
bool columnContact(laser::selector3::Position,float,float&,std::array<unsigned,3>&);
bool dispatchColumns(const std::array<unsigned,3>&,std::string&);
bool terminalDrain(const StageWorld*);
bullet::Position aim(bullet::Position origin,bullet::Position fallback);
void contact(bullet::collision::Vec2 center,float bulletRadius,float playerRadius,std::optional<bullet::collision::Geometry>& geometry);
}
