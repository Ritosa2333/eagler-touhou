#pragma once
#include "ChargeSample.hpp"
#include <vector>
namespace th09 {
// Explicit mod Replay track. Ordinary .rpy bytes stay on the native path when
// the room disables local charge priority.
class ChargeReplayTrack {
public:
    struct Entry {u32 frame=0,hold=0;std::array<ChargeSample,2> samples;};
    static constexpr u32 Limit=600000,RowBytes=88;
    std::array<std::vector<Entry>,10> stages;
    std::array<u32,10> cursors{};
    bool enabled=false,playing=false,invalid=false;
    void clear(){for(auto& stage:stages)stage.clear();cursors={};enabled=playing=invalid=false;}
    void begin(u32 stage,bool replay){if(stage>=10)return;cursors[stage]=0;if(!replay)stages[stage].clear();}
    bool record(u32 stage,u32 frame,bool hold,const std::array<ChargeSample,2>&);
    const Entry* peek(u32 stage)const;
    void pop(u32 stage){++cursors[stage];}
    bool load(const u8*,u32);
    std::vector<u8> trailer()const;
};
}
