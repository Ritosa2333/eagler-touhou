#pragma once
#include "../../cpp/game/PlayerLife.hpp"
#include <vector>
namespace player_life_test {
using namespace th09;
struct Fixture:PlayerLifeActions {
    PlayerMotion motion;ShotControlState control;AnmVm body{};AttackAreas areas;Rng random;ShotResource resource;DamageRules rules;PlayfieldLimits limits;
    PlayerLife life{motion,control,body,areas,random,*this};bool boss_available=true;
    i32 flash_duration[2]{};u32 flash_color[2]{};
    std::vector<std::array<u32,8>> events;
    static u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
    void play_sound(i32 id,i32 pan)override{events.push_back({0,u32(id),u32(pan)});}
    void begin_charge()override{events.push_back({1});}
    void end_charge()override{events.push_back({2});}
    bool opponent_boss_available()override{events.push_back({3});return boss_available;}
    void attack(i32 level,const std::string&)override{events.push_back({4,u32(level)});}
    void effect(i32 id,const Vec3& p)override{events.push_back({5,u32(id),bits(p.x),bits(p.y),bits(p.z)});}
    void fire(u32 set,i32 frame)override{events.push_back({6,set,u32(frame)});}
    void play_positioned_sound(i32 id,float x)override{events.push_back({7,u32(id),bits(x)});}
    void opponent_wins(i32 side)override{events.push_back({8,u32(side)});}
    void slotted_effect(i32 id,const Vec3& p,i32 slot,u32 color)override{events.push_back({9,u32(id),bits(p.x),bits(p.y),bits(p.z),u32(slot),color});}
    void critical_health()override{events.push_back({10});}
    void damage_flash(i32 side,i32 duration,u32 color)override{flash_duration[side]=duration;flash_color[side]=color;}
    void end_focus()override{events.push_back({11});}
    void remove_shield()override{}
    void flush_combo()override{events.push_back({12});}
    void reset_ai()override{events.push_back({13});}
    void charge(float n)override{events.push_back({14,bits(n)});}
};
}
