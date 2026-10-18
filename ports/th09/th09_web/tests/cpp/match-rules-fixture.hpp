#pragma once
#include "../../cpp/game/MatchRules.hpp"
#include <vector>
namespace match_rules_test {
using namespace th09;
struct Fixture:MatchRuleActions {
    EclWorldState world;MatchRules rules;std::vector<std::array<i32,3>> events;
    Fixture():rules(world,*this){}
    void reward_enemy(i32 s,i32 kind)override{events.push_back({0,s,kind});}
    void play_sound(i32 id,i32 pan)override{events.push_back({1,id,pan});}
    void reward_notification(i32 s)override{events.push_back({2,s,0});}
};
struct Field{u32 original,offset;};
#define MATCH_FIELD(address,name) {address,offsetof(MatchProgress,name)}
inline const Field fields[]={MATCH_FIELD(0x4a7e40,round_frames),MATCH_FIELD(0x4a7e48,reward_stage),MATCH_FIELD(0x4a7e50,reward_stage_frames),MATCH_FIELD(0x4a7e54,rank_interval),MATCH_FIELD(0x4a7e58,maximum_rank),MATCH_FIELD(0x4a7e5c,reward_accumulator),MATCH_FIELD(0x4a7e60,total_frames),MATCH_FIELD(0x4a8104,last_reward),MATCH_FIELD(0x4a7e8c,stage),MATCH_FIELD(0x4a7e90,round),MATCH_FIELD(0x4a80d8,active_frames)};
#undef MATCH_FIELD
}
