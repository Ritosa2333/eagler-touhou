#pragma once
#include "../../cpp/game/ShotManager.hpp"
namespace th11::test {
struct ShotFixture:ShotWorld {
    struct Sound{i32 id;float x;};std::vector<Sound> sounds;
    ShotManager manager;
    GameEconomy economy;
    Enemy enemy{};EnemyLink link{&enemy,nullptr,nullptr};
    ShotFixture(ShtResource& s,AnmResource& a,AnmManager& m):manager(s,a,m,*this,0){}
    bool sound(i32 id,float x)override{sounds.push_back({id,x});return true;}
};
}
