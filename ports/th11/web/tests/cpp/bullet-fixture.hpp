#pragma once
#include "../../cpp/game/BulletState.hpp"
#include <vector>
namespace th11::test {
struct BulletEvent {i32 type,a,b;float x;};
struct BulletFixture:BulletEnvironment {
    BulletState bullet{};AnmSprite sprite{};std::vector<BulletEvent> events;std::vector<BulletEmitter> emissions;
    bool sound(i32 id,float x,bool positional)override{events.push_back({positional?1:0,id,0,x});return true;}
    bool cancel(BulletState&)override{events.push_back({2,0,0,0});return true;}
    bool fire(const BulletEmitter& e)override{emissions.push_back(e);events.push_back({3,0,0,0});return true;}
};
}
