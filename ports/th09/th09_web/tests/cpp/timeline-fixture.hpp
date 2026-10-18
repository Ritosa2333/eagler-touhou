#pragma once
#include "../../cpp/game/EnemyTimeline.hpp"
namespace timeline_test {
using namespace th09;
struct Fixture:EnemyTimelineActions {
    EclProgram program;EnemyTimeline timeline;Rng random;
    EclVm created,bosses[4];i32 events[4]={-1,-1,-1,-1};u32 boss_mask=15,finished=0;
    std::vector<EnemySpawn> spawns;
    EclVm* spawn(const EnemySpawn& request) override {spawns.push_back(request);return &created;}
    EclVm* boss(i32 index) override {return index>=0&&index<4&&(boss_mask&(1u<<index))?&bosses[index]:nullptr;}
    i32* timeline_events() override{return events;}
    void finish_match() override{finished=1;}
};
}
