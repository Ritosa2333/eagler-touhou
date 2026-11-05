#pragma once
#include "../../cpp/game/LaserState.hpp"
#include <vector>
namespace th11::test {
struct LaserEvent {i32 kind,id;Vec3 position;};
struct LaserFixture:LaserWorld {
    LaserLine laser{};std::vector<LaserEvent> events;std::vector<LaserLineParameters> emissions;
    bool sound(i32 id,float x,bool positional)override{events.push_back({positional?1:0,id,{x,0,0}});return true;}
    bool spawn_line(const LaserLineParameters& p)override{events.push_back({2,0,{}});emissions.push_back(p);return true;}
    bool change_appearance(LaserLine&,i32 script)override{events.push_back({3,script,{}});return true;}
    bool cancel_effect(Vec3 p,i32 script)override{events.push_back({4,script,p});return true;}
    bool cancel_reward(Vec3 p)override{events.push_back({5,8,p});return true;}
    bool cancel_shot(Vec3 p,float angle)override{events.push_back({13,0,p});events.push_back({14,0,{angle,0,0}});return true;}
};
}
