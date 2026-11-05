#include "PlayerGauge.hpp"
namespace th08 {
void update_player_gauge(PlayerGaugeState& s,PlayerFormState& form,const Timer& shooting,GameGauge& gauge,bool gui,i32 bomb,const Vec3& position,const FrameTiming& timing,PlayerGaugeActions& actions){
    if(!gui&&form.frames>=30&&!bomb){
        if(shooting.current>=0){
            if(s.idle.current>0)s.idle.decrement(1,timing);
            else {
                const float increment=number(300)<form.transition.value()?21.0f:(form.transition.value()/number(15)).to_float();
                i32 amount=Scalar::truncate(increment);if(!form.focused)amount=wrapping_sub(0,amount);
                gauge.add(i16((Extended::from_int(amount)*number(timing.rate)).truncate_int()),false,false);form.transition.tick(timing);
            }
        }else {
            if(s.idle.current>=4)form.transition.set(0);
            if(s.idle.current>=30){
                if(gauge.value()>=-9&&gauge.value()<=9)gauge.set(0);
                else {
                    const i32 amount=gauge.youkai_bonus()?-5:gauge.youkai()?-3:gauge.value()>0?-2:!gauge.human()?2:!gauge.human_bonus()?3:5;
                    gauge.add(i16((Extended::from_int(amount)*number(timing.rate)).truncate_int()),false,false);
                }
            }else s.idle.tick(timing);
        }
    }
    if((gauge.human_bonus()||gauge.youkai_bonus())&&!s.effect)s.effect=actions.gauge_effect(position);
    if(s.effect){s.effect->position=position;if(!gauge.human_bonus()&&!gauge.youkai_bonus()){s.effect->active=0;s.effect=nullptr;}}
}
}
