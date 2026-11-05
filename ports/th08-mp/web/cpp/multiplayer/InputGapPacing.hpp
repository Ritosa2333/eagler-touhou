#pragma once
#include <algorithm>
#include <cstdint>

namespace th08::multiplayer {
// TH09's adopted H8 shortage policy. This is wall-clock scheduling state,
// never input delay, simulation state or rollback history. The caller drains
// received input before observing the contiguous remote frontier.
class InputGapPacing {
public:
    void Reset(std::uint64_t now) { scale_=1;updated_at_=now; }
    void Update(std::uint64_t now,std::uint32_t missing,bool enabled) {
        if(!enabled){Reset(now);return;}
        // Repeated pumps earn no additional time. A reversed clock cannot
        // move the anchor backwards and earn that time a second time later.
        if(now<=updated_at_)return;
        const double elapsed=double(now-updated_at_);updated_at_=now;
        const double target=1+std::clamp((double(missing)-4)/3,0.,1.);
        if(target>scale_)scale_=std::min(target,scale_+elapsed/50);
        else scale_=std::max(target,scale_-elapsed/150);
    }
    double Scale()const{return scale_;}
private:
    double scale_=1;
    std::uint64_t updated_at_=0;
};
}
