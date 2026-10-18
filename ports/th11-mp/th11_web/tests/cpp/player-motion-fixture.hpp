#pragma once
#include "../../cpp/game/PlayerMotion.hpp"
namespace th11::test {
struct PlayerMotionFixture:PlayerMotionWorld {
    PlayerMotion motion;PlayerMotionInput input;std::vector<i32> events;
    PlayerMotionFixture(AnmResource& p,AnmResource& b,AnmManager& a):motion(p,b,a,*this,0,0){}
    bool sound(i32 id)override{events.push_back(id);return true;}
    bool rebuild_options()override{events.push_back(100);return true;}
    bool attract_items()override{events.push_back(200);return true;}
};
}
