#pragma once
#include <algorithm>
#include <cmath>
#if defined(PORT_TITLE) && PORT_TITLE == 14
#include "FrameDispatchTh14.hpp"
#endif
namespace touhou::sdl {
struct FrameCadence {
    static constexpr double interval=1./60.;
#if defined(PORT_TITLE) && PORT_TITLE == 14
    // Retail46a720 advances expired deadlines without replaying their outer
    // update/draw chains. Host display opportunities still own the polling.
    FrameDispatchTh14 dispatch;
    double now=0;
    void reset(){now=0;dispatch.reset();}
    unsigned advance(double seconds){
        now+=seconds;
        return dispatch.begin(now,false).update?1u:0u;
    }
#else
    // Other titles retain their existing bounded catch-up policy.
    double debt=0;
    void reset(){debt=0;}
    unsigned advance(double seconds){
        debt=std::min(.1,debt+std::clamp(seconds,0.,.1));
        const auto ticks=std::min(4u,unsigned(std::floor((debt+1.e-9)/interval)));
        debt=std::max(0.,debt-ticks*interval);return ticks;
    }
#endif
};
}
