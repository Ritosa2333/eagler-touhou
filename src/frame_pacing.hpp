#pragma once
#include <cstdint>
#include <optional>

namespace soew {
// Absolute deadlines keep fractional refresh intervals from accumulating drift.
// After a stall, skip missed presents rather than drawing them in a burst.
struct DisplayCadence {
    std::uint64_t origin_us=0,frame=0;
    unsigned hz=60;
    void reset(std::uint64_t now_us,unsigned refresh_hz){origin_us=now_us;frame=0;hz=refresh_hz;}
    std::uint64_t deadline()const{return origin_us+(frame*1'000'000+hz-1)/hz;}
    bool consume(std::uint64_t now_us){
        if(now_us<deadline())return false;
        frame=(now_us-origin_us)*hz/1'000'000+1;
        return true;
    }
};

// Measure completed presents, independently of the timer and simulation.
class FrameRateMeter {
    static constexpr std::uint64_t sample_us=1'000'000;
    std::uint64_t first_us_=0;
    std::optional<std::uint64_t> last_us_;
    unsigned intervals_=0;
    std::optional<unsigned> fps_;
public:
    void reset(){*this={};}
    std::optional<unsigned> fps(std::uint64_t now_us)const{
        return last_us_&&now_us-*last_us_<=sample_us?fps_:std::nullopt;
    }
    void submitted(std::uint64_t now_us){
        if(!last_us_||now_us-*last_us_>sample_us){
            reset();
            first_us_=now_us;
        }else{
            ++intervals_;
            const auto elapsed=now_us-first_us_;
            if(elapsed>=sample_us){
                fps_=static_cast<unsigned>((intervals_*sample_us+elapsed/2)/elapsed);
                first_us_=now_us;
                intervals_=0;
            }
        }
        last_us_=now_us;
    }
};

// Pause and focus changes can reset the host clock without advancing terrain.
constexpr unsigned display_subframe(unsigned previous,unsigned sampled,unsigned ticks){
    return ticks||sampled>=previous?sampled:previous;
}
}
