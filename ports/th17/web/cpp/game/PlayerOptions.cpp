#include "PlayerOptions.hpp"
namespace th17 {
PlayerFixedPosition PlayerOptions::offset(i32 index,bool focused)const noexcept{
    const u32 at=(focused?0x90u:0x40u)-0x28u+u32(index)*8;Vec2 value;std::memcpy(&value,shots.header.parameters.data()+at,sizeof value);return {truncate_int(float(value.x*128.f)),truncate_int(float(value.y*128.f))};
}
bool PlayerOptions::follow_history(PlayerOption& option){
    if(option.index<0||option.index>=4){error="Player option history index outside range";return false;}
    const u32 first=u32(option.index)*8;
    option.target=motion.history[first+7];
    if(!motion.focus)option.normal_offset={wrapping_sub(option.target.x,motion.position_fixed.x),wrapping_sub(option.target.y,motion.position_fixed.y)};
    else{
        option.normal_offset={0,-3200};motion.history[first+8]={motion.position_fixed.x,wrapping_sub(motion.position_fixed.y,3200)};
        float fraction=.125f;
        for(u32 i=1;i<=7;i++){
            const auto& start=motion.history[first];const auto& end=motion.history[first+8];
            const float x=float(float(float(wrapping_sub(end.x,start.x))*fraction)+float(start.x));
            const float y=float(float(float(wrapping_sub(end.y,start.y))*fraction)+float(start.y));
            motion.history[first+i]={truncate_int(x),truncate_int(y)};
            fraction=float(fraction+.125f);
        }
    }
    option.target={wrapping_add(motion.position_fixed.x,option.normal_offset.x),wrapping_add(motion.position_fixed.y,option.normal_offset.y)};option.previous_focus=motion.focus;return true;
}
bool PlayerOptions::configure(i32 power,i32 power_step,i32 max_power){
    if(power_step<=0||character<0||character>=3){error="Invalid TH17 player option configuration";return false;}
    const i32 count=power/power_step;power_level=count;if(count<0||count>4){error="Player option power level outside range";return false;}
    if(power<max_power){for(u32 i=0;i<4;i++){auto& handle=visuals.option_handles[i][1];if(!animations.interrupt(handle,1)){error=animations.error;return false;}handle=0;}}
    else for(i32 i=0;i<count;i++){
        auto& handle=visuals.option_handles[u32(i)][1];if(!animations.retire(handle)){error=animations.error;return false;}
        constexpr i32 scripts[]={16,18,18};handle=animations.create(resource,scripts[character],-1,2);auto* vm=animations.registry.find(handle);if(!vm){error=animations.error;return false;}vm->visual.translation.y=-32.f;
    }
    if(motion.option_count==count)return true;
    const i32 first=count*(count-1)/2;constexpr i32 scripts[]={15,17,16};
    for(i32 i=0;i<count;i++){
        auto& option=motion.options[u32(i)];auto& handle=visuals.option_handles[u32(i)][0];option.position=motion.position_fixed;
        if(!animations.retire(handle)){error=animations.error;return false;}option.index=i;option.normal_offset=offset(first+i,false);option.focus_offset=offset(first+i,true);
        const auto& delta=motion.focus?option.focus_offset:option.normal_offset;option.target={wrapping_add(motion.position_fixed.x,delta.x),wrapping_add(motion.position_fixed.y,delta.y)};option.position=option.target;
        handle=animations.create(resource,scripts[character],14,0);auto* vm=animations.registry.find(handle);if(!vm){error=animations.error;return false;}vm->visual.translation.y=-32.f;option.active=2;
        if(character==2)option.movement_callback=[this](PlayerOption& p){return follow_history(p);};
    }
    for(i32 i=count;i<4;i++){motion.options[u32(i)].active=0;if(!animations.interrupt(visuals.option_handles[u32(i)][0],1)){error=animations.error;return false;}}
    motion.option_count=count;for(u32 i=0;i<4;i++)motion.options[i].snap=1;return true;
}
}
