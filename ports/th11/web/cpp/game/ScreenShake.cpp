#include "ScreenShake.hpp"
namespace th11 {
i32 ScreenShake::update(Rng& rng,Vec2& offset,bool stopped) noexcept {
    if(stopped)return 7;
    timer.tick();
    if(timer.current>=duration)return 7;
    // Preserve the x87 stores between multiply, divide and integer addition.
    const float product=float(double(wrapping_sub(end,start))*timer.fractional);
    const float quotient=float(double(product)/duration);
    const float amplitude=float(double(quotient)+start);
    const auto axis=[&](){const u32 choice=rng.next32()%3;return choice==0?0.f:choice==1?amplitude:-amplitude;};
    offset.x=axis();offset.y=axis();return 1;
}
}
