#pragma once
#include "AsciiText.hpp"
namespace th11 {
// Original 419d90/419ea0: one-second frame windows and the bottom-right readout.
struct FrameStatistics {
    double window_start=0,actual=0,nominal=0;
    u32 frames=0,fast_windows=0;
    float fps=0;
    void sample(double seconds,bool active,u32 ticks=1);
    void reset_run(){actual=nominal=0;}
    float slowdown()const;
    AsciiRequest label()const;
};
}
