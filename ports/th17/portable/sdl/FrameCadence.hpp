#pragma once
#include <algorithm>
#include <cmath>
namespace touhou::sdl {
// Keep every 60 Hz update through short display stalls. Bound both work per
// callback and retained debt so a slow device cannot enter an endless spiral.
struct FrameCadence {
    static constexpr double interval=1./60.;
    double debt=0;
    // A caller may tolerate a small timestamp rounding error at a tick
    // boundary. Keep borrowed time negative so this never speeds up the
    // simulation or creates an extra update on a repeated timestamp.
    const double tolerance;
    explicit FrameCadence(double roundingTolerance=0):tolerance(std::clamp(roundingTolerance,0.,interval/2.)){}
    void reset(){debt=0;}
    unsigned advance(double seconds){
        debt=std::min(.1,debt+std::clamp(seconds,0.,.1));
        const auto ticks=std::min(4u,unsigned(std::max(0.,std::floor((debt+tolerance+1.e-9)/interval))));
        debt=std::max(-tolerance,debt-ticks*interval);return ticks;
    }
};
}
