#pragma once
#include "Types.hpp"
#include <array>
namespace th11 {
struct SpellTimeParts {i32 seconds=999,hundredths=99;bool valid=false;};
SpellTimeParts decode_spell_time(i32 encoded)noexcept;
// Original wall-clock sampler 40c310. Gameplay-frame time and elapsed real
// time are deliberately separate; replays carry the original real-time value.
class SpellTiming {
public:
    double start=0; i32 last_frames=0,encoded=0;u32 index=0;
    std::array<u32,20> records{};
    bool update(u32& flags,i32 frame_count,double now,bool replay)noexcept;
};
}
