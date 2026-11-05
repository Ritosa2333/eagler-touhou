#pragma once
#include "FrameCadence.hpp"
#include <algorithm>
namespace touhou::sdl {
// Presentation-rate detection is deliberately separate from FrameCadence:
// it may enable extra visual samples, but it never changes fixed game ticks.
struct PresentationCadence {
    static constexpr unsigned enable_samples=6,disable_samples=12;
    unsigned fast=0,slow=0;bool high_refresh=false;
    static bool fast_sample(double seconds){return seconds>0&&seconds<FrameCadence::interval*.9;}
    void reset(){fast=slow=0;high_refresh=false;}
    bool advance(double seconds){
        if(seconds<=0)return high_refresh;
        const bool faster=fast_sample(seconds);
        if(faster){fast=std::min(enable_samples,fast+1);slow=0;if(fast>=enable_samples)high_refresh=true;}
        else {fast=0;if(high_refresh){slow=std::min(disable_samples,slow+1);if(slow>=disable_samples){high_refresh=false;slow=0;}}}
        return high_refresh;
    }
};

// A detected high-refresh session only becomes presentable after a real game
// tick has produced a prev/current endpoint pair. Once primed, isolated late
// display callbacks must not expose the current endpoint: PresentationCadence's
// hysteresis owns the decision to leave high-refresh presentation.
struct PresentationGate {
    bool primed=false;
    void reset(){primed=false;}
    bool advance(bool eligible,bool tick_due){
        if(!eligible){primed=false;return false;}
        if(tick_due)primed=true;
        return primed;
    }
};
}
