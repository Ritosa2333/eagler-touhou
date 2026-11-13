#include "ShotSchedule.hpp"
#if PORT_TITLE == 16
#include "SeasonTh16.hpp"
#endif
namespace th11 {
#if PORT_TITLE == 16
bool Th16ShotSchedule::update(i32 state,u32 held,const float* rate,Th16FireFrames& requests){
    // Full4455d0, including the independent120-frame firing clock.
    if(state!=1){locked_id=0;locked=0;return true;}
    if(regular.current<0){if(held&1){if(long_timer.current<0)long_timer.set(0,rate);regular.set(0,rate);}}
    if(regular.current>=0){
        if(regular.current!=regular.previous&&!requests.fire(regular.current,long_timer.current))return false;
        if(regular.current>=14){if(held&1)regular.advance(-14);else regular.set(-1,rate);}else regular.tick();
    }
    if(long_timer.current>=0){
        if(long_timer.current>=119){if(held&1)long_timer.advance(-119);else long_timer.set(-1,rate);}else long_timer.tick();
    }return true;
}
bool shot_th16_select(const ShtResource& main,const ShtResource& season,i32 power,i32 step,bool focused,const Th16SeasonState& energy,i32 frame,i32 long_frame,Th16ShotRequests& requests){
    // Complete445470: the high two bytes select a family and group, the
    // low byte selects a record. 26/27 are the secondary clock period/delay.
    if(step<=0)return false;const i32 level=power/step;
    const i32 main_group=focused?wrapping_add(level,wrapping_add(main.header.max_power_level,1)):level;
    const i32 season_group=energy.level();
    auto emit=[&](const ShtResource& resource,i32 group,u32 family){
        if(group<0||u32(group)>=resource.groups.size())return false;
        const auto& specs=resource.groups[group].shots;
        for(u32 index=0;index<specs.size();++index){
            const auto& spec=specs[index];const i8 long_period=i8(u16(spec.sound)&255),long_delay=i8(u16(spec.sound)>>8);
            const i32 divisor=long_period?i32(long_period):i32(spec.interval),delay=long_period?i32(long_delay):i32(spec.delay),clock=long_period?long_frame:frame;
            if(!divisor||(divisor==-1&&clock==INT32_MIN))return false;
            if(clock%divisor==delay&&!requests.spawn(((u32(group)|family)<<8)|index,frame))return false;
        }return true;
    };
    return emit(main,main_group,0)&&emit(season,season_group,0x100);
}
#endif
i32 ShotSchedule::begin(i32 state,i32 warp,u32 held,const float* rate)noexcept{
    if(state!=1)return -1;
    if(timer.current<0){if((kPortTitle!=14&&warp>=99)||!(held&1))return -1;timer.set(0,rate);}
    return timer.current!=timer.previous?timer.current:-1;
}
void ShotSchedule::finish(u32 held,const float* rate)noexcept{
    if(timer.current>=14){if(held&1)timer.advance(-14);else timer.set(-1,rate);}
    else timer.tick();
}
i32 ShotSchedule::update(i32 state,i32 warp,u32 held,const float* rate)noexcept{
    const i32 emitted=begin(state,warp,held,rate);
    if(state==1&&timer.current>=0)finish(held,rate);
    return emitted;
}
}
