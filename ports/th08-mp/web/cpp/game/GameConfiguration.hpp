#pragma once
#include "Types.hpp"
namespace th08 {
struct GameConfiguration {
    i16 controller[9]{};
    u16 reserved12=0;
    i32 version=0x80001;
    i16 pad_x=0,pad_y=0;
    u8 lives=2,bombs=2,color16=0,music=1,sounds=1,difficulty=1,windowed=1,frameskip=0;
    u8 effects=2,slow_mode=0,shot_slow=0;
    i8 music_volume=100,sound_volume=100,reserved29[15]{};
    u32 options=0;
};
static_assert(sizeof(GameConfiguration)==0x3c);
static_assert(offsetof(GameConfiguration,slow_mode)==0x25);
}
