#pragma once
#include "../../cpp/game/ShotControl.hpp"
#include <vector>
namespace shot_control_test {
using namespace th09;
struct Fixture:ShotControlActions {
    ShotControlState state;AttackAreas areas;ShotResource resource;GameInput input;Vec3 position;bool boss_available=true;
    std::vector<std::array<u32,8>> events;
    static u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
    void play_sound(i32 id,i32 pan)override{events.push_back({0,u32(id),u32(pan)});}
    void begin_charge()override{events.push_back({1});}
    void end_charge()override{events.push_back({2});}
    bool opponent_boss_available()override{events.push_back({3});return boss_available;}
    void attack(i32 level,const std::string&)override{events.push_back({4,u32(level)});}
    void effect(i32 id,const Vec3& p)override{events.push_back({5,u32(id),bits(p.x),bits(p.y),bits(p.z)});}
    void fire(u32 set,i32 frame)override{events.push_back({6,set,u32(frame)});}
};
struct Field {u32 original,offset,size;};
#define SC(original,field) {original,offsetof(ShotControlState,field),sizeof(ShotControlState::field)}
inline const Field fields[]={SC(0x30384,charge),SC(0x30388,available),SC(0x3038c,full_charge),SC(0x30398,charged_fire),SC(0x303a4,normal_fire),SC(0x303b0,cooldown),SC(0x303bc,charging_time),SC(0x303c8,protection),SC(0x1b74,shock),SC(0x1b80,flags),SC(0,player_state)};
#undef SC
}
