#pragma once
#include "Types.hpp"
#include <array>

namespace th11 {
// Network adapters supply one complete confirmed input array per logical tick.
// Game rules never depend on the local seat or on a browser clock.
struct MultiplayerOptions {
    unsigned seat_count=2,local_seat=0,stage=1,seed=1;
    int difficulty=1;
    std::array<int,3> selections{};
    bool valid()const noexcept {
        if(seat_count<2||seat_count>3||local_seat>=seat_count||stage<1||stage>7||difficulty<0||difficulty>4)return false;
        for(unsigned i=0;i<seat_count;++i)if(selections[i]<0||selections[i]>5)return false;
        return (stage==7)==(difficulty==4);
    }
};
struct MultiplayerInput {
    u32 held=0;
    int touch_mode=0; // 0 keyboard; 1 bounded target; 2 unlimited target; 3 analog vector.
    float touch_x=0,touch_y=0;
    bool pause=false;
};
struct MultiplayerSeatView {
    bool active=false,ghost=false;
    unsigned seat=0,life_state=0,power_taps=0;
    int selection=0,lives=0,life_fragments=0,power=0,max_power=0,power_step=0;
    i64 score=0;
    int graze=0,communication=0;
    float x=0,y=0;
    float fast_speed=0,slow_speed=0;
};
}
