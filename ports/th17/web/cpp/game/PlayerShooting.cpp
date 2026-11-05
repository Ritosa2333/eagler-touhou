#include "PlayerShooting.hpp"
namespace th17 {
bool PlayerShooting::blocked(){
    shot.set(-1);continuous.set(-1);auxiliary=0;auxiliary_active=false;charge.set(0);
    if(!host.retire(charge_handle))return false;charge_handle=0;
    return host.sound(30,ShotSoundAction::stop,0)&&host.sound(55,ShotSoundAction::stop,0);
}
void PlayerShooting::wrap(Timer& timer,i32 period,float rate)noexcept{
    timer.rate_index=0;timer.previous=timer.current;const float step=rate>.99f&&rate<1.01f?1.f:rate;
    timer.fractional=float(timer.fractional+float(step*float(-period)));timer.current=truncate_int(timer.fractional);
}
void PlayerShooting::move_origin(const Vec3& p)noexcept{
    motion.position_fixed={truncate_int(float(p.x*128.f)),truncate_int(float(p.y*128.f))};
    motion.position.x=float(float(motion.position_fixed.x)*.0078125f);motion.position.y=float(float(motion.position_fixed.y)*.0078125f);
    for(u32 i=0;i<4;i++)motion.options[i].snap=1;
}
bool PlayerShooting::stop_charge(){
    if(!host.sound(30,ShotSoundAction::stop,0)||!host.sound(55,ShotSoundAction::stop,0)||!host.retire(charge_handle))return false;
    charge_handle=0;return true;
}
bool PlayerShooting::fire_charge(float rate){
    const Vec3 saved=motion.position;move_origin(charge_origin);
    const bool result=host.fire(wrapping_add(charge.current,40),0);move_origin(saved);
    if(!result)return false;charge.tick(&rate);return true;
}
bool PlayerShooting::update(i32 state,bool held,float rate,i32 character,i32 animal,u32 roaring_flags,i32 roaring_kind){
    if(state!=1){auxiliary=0;auxiliary_active=false;if(charge.current){if(!stop_charge()){error="Charge cleanup failed";return false;}charge.set(0);}return true;}
    if(character==2){
        const bool wolf=(roaring_flags&2)&&roaring_kind==1,eagle=(roaring_flags&2)&&roaring_kind==3;
        if(!eagle&&(wolf||motion.focus)){
            if(wolf&&charge.current>=0){if(!held)return true;if(!host.retire(charge_handle)){error="Charge release animation failed";return false;}charge_handle=0;charge.set(-40);charge_origin=motion.position;return true;}
            if(!wolf){
                if(held&&charge.current>=0){
                    if(!charge.current){if(!host.sound(30,ShotSoundAction::play,motion.position.x)){error="Charge audio failed";return false;}charge_handle=host.charge_indicator(animal==0?20:19,motion.position);if(!charge_handle){error="Charge animation failed";return false;}}
                    charge.tick(&rate);
                    if(!host.sound(30,ShotSoundAction::pan,motion.position.x)||!host.sound(55,ShotSoundAction::pan,motion.position.x)){error="Charge pan update failed";return false;}
                    if(charge.current==60&&(!host.sound(30,ShotSoundAction::stop,0)||!host.sound(55,ShotSoundAction::play,motion.position.x)||!host.interrupt(charge_handle,2))){error="Charge completion failed";return false;}
                    return true;
                }
                if(charge.current>0){if(!stop_charge()){error="Charge release failed";return false;}if(charge.current>=60){charge.set(-40);charge_origin=motion.position;}else charge.set(0);return true;}
                if(charge.current>=0)return true;
            }
            if(!fire_charge(rate)){error="Charged shot failed";return false;}if(charge.current>=0)charge.set(0);return true;
        }
        if(charge.current>0){if(!stop_charge()){error="Charge cancellation failed";return false;}if(charge.current<60)charge.set(0);else{charge.set(-40);charge_origin=motion.position;}}
        else if(charge.current<0){const i32 focus=motion.focus;motion.focus=1;const bool result=fire_charge(rate);motion.focus=focus;if(!result){error="Charged shot failed";return false;}}
    }
    if(shot.current<0){if(!held)goto continuous_timer;if(continuous.current<0)continuous.set(0);shot.set(0);}
    if(shot.current!=shot.previous&&!host.fire(shot.current,continuous.current)){error="Player shot callback failed";return false;}
    if(shot.current<14)shot.tick(&rate);else if(!held)shot.set(-1);else wrap(shot,14,rate);
continuous_timer:
    if(continuous.current<0)return true;
    if(continuous.current>118){if(held)wrap(continuous,119,rate);else continuous.set(-1);return true;}
    continuous.tick(&rate);return true;
}
}
