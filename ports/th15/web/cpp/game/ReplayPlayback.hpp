#pragma once
#include "Replay.hpp"
#include "GameInput.hpp"
namespace th15 {
// The file stores redundant edge words, but the original gameplay callback
// recalculates those edges and all held clocks from the restored held stream.
class ReplayPlayback {
 Replay& file;u32 number=0;i32 clock=-1;std::string failure;
public:
 GameInput input;PlayerTouch touch;u8 fps=0;bool finished=false;
 explicit ReplayPlayback(Replay& file):file(file){}
 bool select(u32 stage);void activate()noexcept{clock=0;}
 bool tick(bool active=true);i32 frame_clock()const noexcept{return clock;}
 const std::string& error()const noexcept{return failure;}
};
}
