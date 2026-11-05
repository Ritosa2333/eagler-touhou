#pragma once
#include "laser-fixture.hpp"
#include "render-fixture.hpp"
namespace th11::test {
struct LaserLineFixture:LaserFixture {
    AnmEnvironment animations;AnmResource resource;RenderFixture render;
    i32 collision_result=0;
    LaserInfinite infinite{};Vec3 boss{};bool boss_active=false;
    bool change_appearance(LaserLine& l,i32 script)override{return laser_line_appearance(l,script,resource,animations);}
    i32 collision(Vec3 p,float angle,float width,float length)override{events.push_back({6,collision_result,{p.x,p.y,0}});events.push_back({7,0,{angle,width,length}});return collision_result;}
    i32 warning_collision(Vec3 p,float angle,float width,float length)override{events.push_back({15,collision_result,{p.x,p.y,0}});events.push_back({16,0,{angle,width,length}});return collision_result;}
    bool cut(LaserLine&,Vec3 p,Vec3 size,bool reward,bool skip)override{events.push_back({8,(reward?1:0)|(skip?2:0),p});events.push_back({9,0,size});return true;}
    bool graze_effect(Vec3 p)override{events.push_back({10,156,p});return true;}
    bool graze_reward()override{events.push_back({11,0,{}});return true;}
    bool graze_register()override{events.push_back({12,0,{}});return true;}
    bool boss_position(Vec3& p)override{p=boss;return boss_active;}
    bool cut_infinite(LaserInfinite&,Vec3 p,Vec3 size,bool reward,bool skip)override{events.push_back({8,(reward?1:0)|(skip?2:0),p});events.push_back({9,0,size});return true;}
};
}
