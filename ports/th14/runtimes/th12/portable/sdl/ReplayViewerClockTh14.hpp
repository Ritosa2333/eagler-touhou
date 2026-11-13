#pragma once
#include <algorithm>
#include <cmath>
namespace touhou::sdl {
// Only the explicitly opened browser replay viewer uses this clock. Ordinary
// play, the title's replay menu and loading retain their existing dispatch.
struct ReplayViewerClockTh14 {
    double debt=0;
    void reset()noexcept{debt=0;}
    static bool valid_rate(double rate)noexcept{return rate==0||rate==.5||rate==1||rate==2||rate==4||rate==8;}
    unsigned advance(double seconds,double rate)noexcept{
        if(!std::isfinite(seconds)||seconds<0||!valid_rate(rate)){reset();return 0;}
        if(rate==0)return 16; // Full speed remains bounded so commands can run.
        debt=std::min(16./60.,debt+std::min(seconds,.25)*rate);
        const auto ticks=unsigned(std::floor((debt+1e-9)*60.));
        debt=std::max(0.,debt-double(ticks)/60.);return ticks;
    }
};
}
