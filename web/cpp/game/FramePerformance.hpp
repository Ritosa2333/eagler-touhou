#pragma once
#include "Types.hpp"
namespace th17 {
// Original 461c80 samples the presentation/update clock once a second. Its
// slowdown counters exclude paused and game-over scenes and tolerate >=57 FPS.
struct FramePerformance {
    double origin=0,delivered=0,requested=0;u32 pending=0;float fps=0;
    void rebase(double seconds)noexcept{origin=seconds;pending=0;}
    void reset_result()noexcept{delivered=requested=0;}
    void sample(double seconds,bool scene,u32 flags,u32 skipped=0)noexcept;
};
}
