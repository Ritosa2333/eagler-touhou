#include "SeasonTh16.hpp"
#include <algorithm>
#include "Timer.hpp"
namespace th11 {
i32 Th16SeasonState::level()const noexcept{
    i32 result=0;while(result<6&&count>=thresholds[result+1])++result;return result;
}
float Th16SeasonState::progress()const noexcept{
    const i32 n=level();if(n==6)return 1;
    return float(wrapping_sub(count,thresholds[n]))/float(costs[n+1]);
}
bool Th16SeasonState::collect()noexcept{
    // Complete43de50. The original ignores its stack argument.
    if(count>=maximum)return false;const i32 previous=level();
    count=std::min(wrapping_add(count,1),maximum);return previous!=level();
}
bool season_th16_gauge(const Th16SeasonState& state,i32& active,AnmVm& bar,AnmVm& levels,Th16SeasonGaugeServices& services){
    // 42c600 after the real root/child lookups. Callback and animation
    // consumers remain explicit so a missing selector cannot be ignored.
    const i32 level=state.level();bar.sprite_size.x=level&&state.count>=state.maximum?100.f:state.progress()*100.f;
    bar.th13_native_flags|=8;
    if(!level){
        levels.flags&=~2u;if(!services.children(levels,false))return false;
        if(active==1){if(!services.interrupt(bar,3))return false;bar.pending_interrupt=3;if(!services.update(bar))return false;}
        active=0;return true;
    }
    levels.flags|=2;if(!services.children(levels,true)||!services.interrupt(levels,level+7))return false;
    levels.pending_interrupt=i16(level+7);
    if(active==0){if(!services.interrupt(bar,2))return false;bar.pending_interrupt=2;if(!services.update(bar))return false;}
    active=1;return true;
}
i32 item_th16_cancel_reward(i32 type,i32& point_value,i32 maximum,float& energy)noexcept{
    // 42fe0d..42fe56, executing430dc0 and42c860 before its score request.
    constexpr float amounts[]={5,10,20,30,40,50};
    if(type<9||type>14)return INT32_MIN;const float amount=amounts[type-9];
    point_value=std::min(wrapping_add(point_value,truncate_int(amount*100.f)),maximum);
    energy=energy+amount;
    const i32 point=point_value/100,rounded=wrapping_sub(point,point%10);
    return signed_bits(u32(rounded/100)*10);
}
}
