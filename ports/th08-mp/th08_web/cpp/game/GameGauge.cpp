#include "GameGauge.hpp"
namespace th08 {
void GaugeThresholds::configure(u8 character)noexcept{
    *this={};
    if(character==3){minimum=-5000;human_bonus=-3000;}
    else if(character==10){minimum=-5000;human_bonus=-3000;maximum=5000;youkai_bonus=3000;}
    else if(character>=4){if(character&1){minimum=-2000;human=-2001;}else{maximum=2000;youkai=2001;}}
}
void GameGauge::add(i16 amount,bool bomb,bool force)noexcept{
    if(bomb&&!force)return;
    values.gauge=i16(u16(values.gauge)+u16(amount));
    if(values.gauge<thresholds.minimum)values.gauge=thresholds.minimum;
    else if(values.gauge>thresholds.maximum)values.gauge=thresholds.maximum;
    values.gauge_copy=values.gauge;
}
void GameRank::add(i32 amount)noexcept{
    fraction=wrapping_add(fraction,amount);
    while(fraction>99){value=wrapping_add(value,1);fraction=wrapping_sub(fraction,100);}
    if(value>maximum)value=maximum;
}
void GameRank::subtract(i32 amount)noexcept{
    fraction=wrapping_sub(fraction,amount);
    while(fraction<0){value=wrapping_sub(value,1);fraction=wrapping_add(fraction,100);}
    if(value<minimum)value=minimum;
}
}
