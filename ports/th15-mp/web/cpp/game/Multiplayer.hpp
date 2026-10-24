#pragma once
#include "Types.hpp"
#include "MultiplayerChapterReward.hpp"
#include <array>
namespace th15 {
struct MultiplayerOptions {
    unsigned seat_count=2,local_seat=0,stage=1,seed=1;
    int difficulty=1;
    bool challenge=false;
    std::array<int,3> selections{};
    i32 chapter_threshold=0;
    bool replay_legacy_stage_flow=false;
    bool valid()const noexcept {
        if(chapter_threshold&&!valid_multiplayer_chapter_threshold(u32(chapter_threshold)))return false;
        if(seat_count<2||seat_count>3||local_seat>=seat_count||stage<1||stage>7||difficulty<0||difficulty>4)return false;
        if((stage==7)!=(difficulty==4))return false;
        for(unsigned i=0;i<seat_count;++i)if(selections[i]<0||selections[i]>3)return false;
        return true;
    }
};
struct MultiplayerInput {u32 held=0;int touch_mode=0;float touch_x=0,touch_y=0;bool pause=false;};
}
