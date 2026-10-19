#pragma once
#include "laser-line-fixture.hpp"
#include "../../cpp/game/LaserManager.hpp"
namespace th11::test {
struct LaserManagerFixture:LaserLineFixture {LaserManager manager{resource,animations,*this,0};};
}
