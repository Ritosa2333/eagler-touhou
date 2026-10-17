#pragma once
#include "Types.hpp"
namespace th15 {
// Sampled after presentation, independently of the fixed gameplay timer.
// The original replay stores the encoded elapsed wall time for each spell.
class SpellPresentationClock {
public:
 enum class Event { idle,started,completed };
 i32 sequence=0,completed_frames=0,encoded=0;double origin=0;
 bool needs_sample(u32 flags)const noexcept{return bool(flags&1)!=bool(flags&0x40);}
 Event sample(u32& flags,i32 active_frames,double now)noexcept;
 static i32 encode(i32 seconds,i32 hundredths)noexcept;
 static bool valid(i32 value)noexcept;
 static i32 replay_value(i32 value)noexcept{return valid(value)?value:encode(999,99);}
};
}
