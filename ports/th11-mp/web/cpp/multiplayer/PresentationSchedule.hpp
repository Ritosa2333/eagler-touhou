#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace th11::multiplayer {
// Display time follows complete native transactions. Network waits can finish
// the current visual pair, but never create a new simulation obligation.
class PresentationSchedule final {
    static constexpr double interval=1./60.;
    double elapsed_=0;
    std::uint32_t generation_=0,revision_=0,stage_=0,frame_=0;
    bool initialized_=false,primed_=false;
public:
    void reset(){elapsed_=0;initialized_=primed_=false;}
    float alpha()const{return float(std::clamp(elapsed_/interval,0.,1.));}
    bool observe(double seconds,bool eligible,std::uint32_t generation,
                 std::uint32_t revision,std::uint32_t stage,std::uint32_t frame,
                 double forward_phase){
        const bool continuous=initialized_&&generation==generation_&&revision==revision_&&
            stage==stage_&&frame>=frame_;
        if(!eligible||!continuous){elapsed_=0;primed_=false;}
        else if(frame>frame_){
            elapsed_=std::isfinite(forward_phase)?std::clamp(forward_phase,0.,interval):interval;
            primed_=true;
        }else if(primed_&&std::isfinite(seconds))elapsed_=std::clamp(elapsed_+std::max(0.,seconds),0.,interval);
        initialized_=true;generation_=generation;revision_=revision;stage_=stage;frame_=frame;
        return primed_&&eligible;
    }
};
}
