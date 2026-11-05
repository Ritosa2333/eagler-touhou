#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace touhou::sdl {
// Count changing visible frames. A cached redraw, an old correction tick or a
// stalled interpolation endpoint cannot manufacture presentation progress.
class VisibleFrameRate {
    double begin_=-1;
    double last_change_=-1;
    std::uint64_t epoch_=0,frame_=0;
    float alpha_=1,fps_=0;
    unsigned count_=0;
    bool seen_=false;
public:
    void reset()noexcept{begin_=last_change_=-1;count_=0;fps_=0;seen_=false;}
    float value()const noexcept{return fps_;}
    void sample(double now,std::uint64_t epoch,std::uint64_t frame,float alpha,bool presented,bool interpolating)noexcept{
        if(!std::isfinite(now))return;
        if(begin_<0||epoch!=epoch_){reset();begin_=now;epoch_=epoch;}
        alpha=interpolating?std::clamp(alpha,0.f,1.f):1.f;
        if(presented){
            if(!seen_||frame>frame_){frame_=frame;alpha_=alpha;seen_=true;last_change_=now;++count_;}
            else if(frame==frame_&&interpolating&&alpha>alpha_+1e-5f){alpha_=alpha;last_change_=now;++count_;}
        }
        if(now-begin_>=1000){fps_=float(count_*1000./(now-begin_));count_=0;begin_=now;}
        if(last_change_>=0&&now-last_change_>=1000)fps_=0;
    }
};
}
