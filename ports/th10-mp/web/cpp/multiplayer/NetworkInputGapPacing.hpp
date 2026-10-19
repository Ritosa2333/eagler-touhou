#pragma once
#include <algorithm>
#include <cstdint>

namespace th10::multiplayer {

// Optional TH10-only experiment. NetplayRuntime keeps the original common
// InputGapPacing on the default-OFF path and locks this choice before input.
// The only observation is missing contiguous remote input at the preserved
// prediction frontier: callback cost, CPU debt and rollback cursor are absent.
class NetworkInputGapPacing {
public:
    void Reset(std::uint64_t now){scale_=1.;updated_at_=now;}

    void Update(std::uint64_t now,std::uint32_t missing,bool enabled){
        if(!enabled){Reset(now);return;}
        if(now<=updated_at_)return;
        double elapsed=double(now-updated_at_);updated_at_=now;
        const double target=Target(missing);
        if(target>scale_){
            // Keep the existing rise limit, including below the tail.
            scale_=std::min(target,scale_+elapsed/50.);
            return;
        }
        if(scale_>2.){
            // Release a deep brake in speed (1/scale) rather than spending
            // 1050 ms walking scale 8 -> 1. Stop at the current target or at
            // scale 2; unused elapsed time below 2 uses the original release.
            const double floor=std::max(2.,target);
            const double speed=1./scale_;
            const double to_floor=600.*(1./floor-speed);
            if(elapsed<to_floor){scale_=1./(speed+elapsed/600.);return;}
            scale_=floor;elapsed-=to_floor;
        }
        scale_=std::max(target,scale_-elapsed/150.);
    }

    double Scale()const{return scale_;}

private:
    static double Target(std::uint32_t missing){
        // Four missing frames remain free. B=5,6,7 is bit-for-bit the common
        // target expression; only B>=8 receives the progressive tail.
        if(missing<=7)return 1.+std::clamp((double(missing)-4.)/3.,0.,1.);
        constexpr double tail[]{2.375,3.5,5.375,8.};
        return tail[std::min(missing-8,std::uint32_t{3})];
    }
    double scale_=1.;
    std::uint64_t updated_at_=0;
};

} // namespace th10::multiplayer
