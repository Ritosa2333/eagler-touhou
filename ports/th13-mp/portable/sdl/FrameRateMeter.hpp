#pragma once
#include "FrameCadence.hpp"
#include <algorithm>
#include <cmath>
namespace touhou::sdl {
// The original fps manager (0x424990) counts logic frames, frameskip
// included, and divides by the wall time of a window of at least one second.
// Here the window is the time credited to the frame clock, and the clock's
// fractional phase at each end is folded in: display jitter only moves time
// between the window and the clock, so a game that runs every credited frame
// reads exactly 60, while clamped or stalled time reads as slowdown.
struct FrameRateMeter {
    double window = 0, phase = 0;
    unsigned frames = 0;
    float fps = 60;
    bool open = false;
    static double fraction(double credit) { return std::fmod(std::max(0., credit), FrameCadence::interval); }
    void reset() { *this = FrameRateMeter{}; }
    void restart() {
        window = 0;
        frames = 0;
        open = false;
    }
    void elapse(double seconds) {
        if (open && seconds > 0) window += seconds;
    }
    void frame(unsigned count = 1) {
        if (open) frames += count;
    }
    // credit = time the clock holds before this callback's elapse.
    float sample(double credit) {
        const double now = fraction(credit);
        if (!open) {
            open = true;
            window = 0;
            frames = 0;
            phase = now;
            return fps;
        }
        if (window + 1.e-9 < 1.) return fps;
        const double span = window + phase - now;
        if (span > 0) fps = float(std::min(1. / FrameCadence::interval, frames / span));
        window = 0;
        frames = 0;
        phase = now;
        return fps;
    }
};
}  // namespace touhou::sdl
