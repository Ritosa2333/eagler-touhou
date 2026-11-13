#pragma once
#include "AsciiText.hpp"
#if PORT_TITLE == 14
#include <atomic>
#endif
namespace th11 {
#if PORT_TITLE == 14
class ControlPublicationTh14;
#endif
// TH14 original42e4d0/42e5c0: one-second frame windows and the bottom-right readout.
struct FrameStatistics {
    double window_start=0;
#if PORT_TITLE == 14
    // Loader43600b/436010 and Main FPS/slowdown consumers share individual
    // counters. Keep each scalar access defined without locking the pair.
    std::atomic<double> actual{0},nominal{0};
#else
    double actual=0,nominal=0;
#endif
    u32 frames=0,fast_windows=0;
    float fps=0;
    void sample(double seconds,bool active,u32 ticks=1);
#if PORT_TITLE == 14
    void sample_control(double seconds,const ControlPublicationTh14& control,u32 ticks=1);
    void sample_configured(double seconds,const ControlPublicationTh14& control,const u8& frame_skip);
#endif
    void reset_run(){nominal=0;actual=0;}
    float slowdown()const;
    AsciiRequest label()const;
};
}
