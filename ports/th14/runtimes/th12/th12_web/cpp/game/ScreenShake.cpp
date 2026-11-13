#include "ScreenShake.hpp"
namespace th11 {
i32 ScreenShake::update(Rng& rng,Vec2& offset,bool stopped,u32* last_rng_return,bool* used_rng
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    ,const ControlPublicationTh14* live_control
#endif
) noexcept {
    if(stopped)return 7;
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_DECODE)
    //46cf7a..46cfa0: only the phased callback has this Control predicate.
    if(phased&&live_control&&live_control->phased_shake_blocked())return 1;
#endif
    timer.tick();
    if(phased){
        const i32 middle=wrapping_add(ramp,hold),last=wrapping_add(middle,fall);float fraction=1;
        if(timer.current<ramp)fraction=timer.fractional/float(ramp);
        else if(timer.current>=middle){if(timer.current>=last)return 7;fraction=(float(u32(last))-timer.fractional)/float(u32(fall));}
        const float amplitude=float(start)*fraction;const auto axis=[&](){const u32 sample=rng.next32();if(last_rng_return)*last_rng_return=sample;if(used_rng)*used_rng=true;const u32 choice=sample%3;return choice==0?0.f:choice==1?amplitude:-amplitude;};
        offset.x=axis();offset.y=axis();return 1;
    }
    if(timer.current>=duration)return 7;
    // Preserve the x87 stores between multiply, divide and integer addition.
    // TH14 46cda0 converts both integers before the SSE multiply/divide/add.
    // Large signed amplitudes distinguish it from the older x87 path.
    const float product=kPortTitle==14||kPortTitle==16?float(float(wrapping_sub(end,start))*timer.fractional):float(double(wrapping_sub(end,start))*timer.fractional);
    const float quotient=kPortTitle==14||kPortTitle==16?float(product/float(duration)):float(double(product)/duration);
    const float amplitude=kPortTitle==14||kPortTitle==16?float(quotient+float(start)):float(double(quotient)+start);
    const auto axis=[&](){const u32 sample=rng.next32();if(last_rng_return)*last_rng_return=sample;if(used_rng)*used_rng=true;const u32 choice=sample%3;return choice==0?0.f:choice==1?amplitude:-amplitude;};
    offset.x=axis();offset.y=axis();return 1;
}
}
