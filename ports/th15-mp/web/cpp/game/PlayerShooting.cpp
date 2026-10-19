#include "PlayerShooting.hpp"
namespace th15 {
void PlayerShooting::wrap(Timer& timer,i32 period,float rate)noexcept{
    timer.rate_index=0;timer.previous=timer.current;
    const float step=rate>.99f&&rate<1.01f?1.f:rate;
    timer.fractional=float(timer.fractional+float(step*float(-period)));timer.current=truncate_int(timer.fractional);
}
bool PlayerShooting::update(i32 player_state,bool held,float rate){
    if(player_state!=1){auxiliary=0;auxiliary_active=false;return true;}
    if(shot.current<0){if(!held)goto continuous_timer;if(continuous.current<0)continuous.set(0);shot.set(0);}
    if(shot.current!=shot.previous){if(!fire){error="Player shot callback unavailable";return false;}if(!fire(shot.current,continuous.current)){error="Player shot callback failed";return false;}}
    if(shot.current<14)shot.tick(&rate);else if(!held)shot.set(-1);else wrap(shot,14,rate);
continuous_timer:
    if(continuous.current<0)return true;
    if(continuous.current>118){if(held)wrap(continuous,119,rate);else continuous.set(-1);return true;}
    continuous.tick(&rate);return true;
}
}
