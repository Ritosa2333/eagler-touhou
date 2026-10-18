#include "Timer.hpp"
namespace th08 {
// TH08 0x406660 / Supervisor::TickTimer 0x447421. The normal path keeps fraction.
i32 Timer::tick(const FrameTiming& t) noexcept {
    previous=current;
    if(t.rate<=0.99f){
        fraction=Scalar::add(fraction,t.rate);
        if(fraction>=1){current=wrapping_add(current,1);fraction=Scalar::sub(fraction,1);}
    }else current=wrapping_add(current,1);
    return current;
}
// TH08 0x447295. Preserve the two force-step adjustments when a negative amount
// redirects to decrement, and the single-precision store before each comparison.
void Timer::increment(i32 amount,const FrameTiming& t) noexcept {
    if(t.force_step){current=wrapping_add(current,1);fraction=0;previous=-999;}
    if(t.rate>0.99f){current=wrapping_add(current,amount);return;}
    if(amount<0){decrement(wrapping_sub(0,amount),t);return;}
    previous=current;
    fraction=Scalar::add_scaled(fraction,amount,t.rate);
    while(fraction>=1){current=wrapping_add(current,1);fraction=Scalar::sub(fraction,1);}
}
// TH08 0x44735b.
void Timer::decrement(i32 amount,const FrameTiming& t) noexcept {
    if(t.force_step){current=wrapping_sub(current,1);fraction=0;previous=-999;}
    if(t.rate>0.99f){current=wrapping_sub(current,amount);return;}
    if(amount<0){increment(wrapping_sub(0,amount),t);return;}
    previous=current;
    fraction=Scalar::sub_scaled(fraction,amount,t.rate);
    while(fraction<0){current=wrapping_sub(current,1);fraction=Scalar::add(fraction,1);}
}
}
