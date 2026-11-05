#pragma once
#include <algorithm>
#include <cmath>
namespace touhou::sdl {
// Match the original Touhou scheduler: a display callback may service at most
// one 60 Hz game tick. If several deadlines elapsed while the host was late,
// advance past those expired slots instead of replaying them as catch-up ticks.
struct FrameCadence {
    static constexpr double interval=1./60.;
    double debt=0;
    void reset(){debt=0;}
    double interpolation_alpha()const{
        return std::clamp(debt/interval,0.,1.);
    }
    unsigned advance(double seconds){
        debt+=std::max(0.,seconds);
        const auto slots=std::floor((debt+1.e-9)/interval);
        if(slots<1)return 0;
        debt=std::max(0.,debt-slots*interval);
        return 1;
    }
};
}
