#pragma once
#include "Types.hpp"
namespace th11 {
// Retail 46f050/46e834 set counters;481540 stops without another volume write.
struct Th14MusicFade {
    i32 remaining=0,total=0,mode=0,volume=0;
    void begin(i32 frames){remaining=total=frames;mode=1;}
    // 0: unchanged,1: volume write,2: stop.
    u32 tick(){
        if(mode!=1)return 0;
        remaining=wrapping_sub(remaining,1);
        if(remaining<=0){mode=0;return 2;}
        volume=wrapping_sub(signed_bits(u32(remaining)*5000u)/total,5000);
        return 1;
    }
};
}
