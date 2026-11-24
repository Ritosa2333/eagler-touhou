#pragma once
#include <algorithm>
#include <cstdint>
#include <optional>

namespace soew {
// ReC98 th02_main.asm loc_BEE0 waits one counted retrace at full speed.
// Nominal PC-9801 24 kHz mode: 21,052,600 / (8 * 106 * 440) Hz.
inline constexpr unsigned pc98_refresh_num=526315,pc98_refresh_den=9328;

// Absolute deadlines avoid rounding drift. A badly late frame rebases the
// schedule so that it cannot be followed by a near-immediate catch-up frame.
struct DisplayCadence {
    std::uint64_t origin_us=0,frame=0,late_us=0;
    unsigned rate=pc98_refresh_num,denominator=pc98_refresh_den;
    void reset(std::uint64_t now_us){reset(now_us,pc98_refresh_num,pc98_refresh_den);}
    void reset(std::uint64_t now_us,unsigned num,unsigned den=1){origin_us=now_us;frame=late_us=0;rate=num;denominator=den;}
    std::uint64_t period_numerator()const{return 1'000'000ull*denominator;}
    std::uint64_t deadline()const{return origin_us+(frame*period_numerator()+rate-1)/rate;}
    bool consume(std::uint64_t now_us){
        const auto due=deadline();
        if(now_us<due)return false;
        late_us=now_us-due;
        if(late_us>=period_numerator()/rate/2){origin_us=now_us;frame=0;}
        ++frame;
        return true;
    }
};

// One opportunity per rendered retrace, even after a stall. The original
// scripted slowdown consumes extra retraces, not extra simulation steps.
struct LockedGameClock {
    unsigned retraces=0;
    unsigned advance(unsigned slowdown){
        if(++retraces<slowdown)return 0;
        retraces=0;return 1;
    }
    unsigned subframe(unsigned slowdown,unsigned scale)const{
        return slowdown==1?scale:std::min(scale,retraces*scale/slowdown);
    }
};

// Measure completed presents, independently of the timer and simulation.
class FrameRateMeter {
    static constexpr std::uint64_t sample_us=1'000'000;
    std::uint64_t first_us_=0;
    std::optional<std::uint64_t> last_us_;
    unsigned intervals_=0;
    std::optional<unsigned> tenths_;
public:
    void reset(){*this={};}
    std::optional<unsigned> tenths(std::uint64_t now_us)const{
        return last_us_&&now_us-*last_us_<=sample_us?tenths_:std::nullopt;
    }
    void submitted(std::uint64_t now_us){
        if(!last_us_||now_us-*last_us_>sample_us){
            reset();
            first_us_=now_us;
        }else{
            ++intervals_;
            const auto elapsed=now_us-first_us_;
            if(elapsed>=sample_us){
                tenths_=static_cast<unsigned>((intervals_*10*sample_us+elapsed/2)/elapsed);
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
