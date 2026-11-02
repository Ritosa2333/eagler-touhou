#pragma once
#include "SpellStatus.hpp"
#include <array>
#include <string>
namespace th17 {
// Presentation clock from the native frame-present path. It does not advance
// combat or RNG, and playback takes its encoded result from the replay header.
class SpellCaptureClock {
public:
    i32 displayed_frames=0,encoded_time=0,completed=0;
    double began=0,paused=0;
    std::string error;
    static bool valid(i32)noexcept;
    static i32 encode(double seconds)noexcept;
    bool present(PlayerSpellStatus&,i32 active_frames,double seconds,bool replay,std::array<i32,20>& times);
};
}
