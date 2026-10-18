#pragma once
#include "PlayerHazards.hpp"
#include <array>
namespace th09 {
// Capture only in the WASI oracle harness; SDL/web builds carry no trace ring.
#if TH09_REPLAY_DIAGNOSTICS
struct HazardSnapshot {i32 side=0;u32 count=0;std::array<PlayerHazard,128> entries{};};
struct HazardTrace {
    bool enabled=false;u32 capacity=16;u32 count=0;u32 next=0;
    std::array<HazardSnapshot,16> records{};
    void push(i32 side,const PlayerHazards& h){
        if(!enabled)return;
        auto& r=records[next%capacity];r.side=side;r.count=h.count;
        for(u32 i=0;i<r.count;++i)r.entries[i]=h.entries[i];
        ++next;++count;
    }
};
inline HazardTrace hazard_trace;
#else
struct HazardTrace {void push(i32,const PlayerHazards&) {}};
inline HazardTrace hazard_trace;
#endif
// Last CPU decision per player, kept only by the replay harness.
struct CpuDecision {
    i32 side=-1;u32 frame=0;float px=0,py=0,tx=0,ty=0;
    i32 region=0,focus=0,chosen=0,tier=0,flags=0,hold=0,keys=0;
    i32 path=0,mirror=0,branch=0,probe=0,desired=0,sampled=0,move=0;
    u32 hazard_count=0;
};
#if TH09_REPLAY_DIAGNOSTICS
inline std::array<CpuDecision,3> cpu_decisions{};
#endif
}
