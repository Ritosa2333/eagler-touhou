#pragma once
#include "../../cpp/game/CpuPlayer.hpp"
#include <vector>
namespace cpu_player_test {
using namespace th09;
struct Fixture:CpuActions {
    CpuState state;CpuContext context;ShotControlState shot;GameInput input;Rng random;PlayerHazards hazards;std::vector<std::array<i32,2>> events;
    void opponent_survival_display(i32 n)override{events.push_back({0,n});}
    void opponent_survival_expired()override{events.push_back({1,0});}
};
struct Field {u32 base,original,offset,size;};
#define CF(base,original,field) {base,original,offsetof(CpuContext,field),sizeof(CpuContext::field)}
#define CP(original,field) {0,original,offsetof(CpuContext,player)+offsetof(HazardPlayer,field),sizeof(HazardPlayer::field)}
inline const Field fields[]={CP(0,state),CP(0x1b74,invulnerability),CP(0x1b88,position),CP(0x1ca8,half_extent),CP(0x1c60,hit_bounds),
CF(0,0x1cdc,base_scale),CF(0,0x1ce4,effect_scale),CF(0,0xa8,health),CF(1,0x2c,level),CF(4,0x3044c,opponent_pending),CF(2,0x2ac3b8,spirit_count),CF(2,0x2ac3ac,normal_enemies),CF(0,0xc110,attack_areas),CF(1,0x34,field_flags),
CF(0,0x30f64,item_target),CF(7,0x2d74,priority_target),CF(8,0x2d74,first_target),CF(7,0x3380,target_flags),CF(5,0x4a7e4c,extra_damage)};
#undef CF
#undef CP
}
