#include "PlayerOptions.hpp"
namespace th15 {
PlayerFixedPosition PlayerOptions::offset(i32 index,bool focus)const noexcept{
    const u32 at=(focus?0x90u:0x40u)-0x28u+u32(index)*8;Vec2 value;std::memcpy(&value,shots.header.parameters.data()+at,sizeof value);return {truncate_int(float(value.x*128.f)),truncate_int(float(value.y*128.f))};
}
bool PlayerOptions::configure(i32 power,i32 power_step,i32 max_power){
    if(power_step<=0||character<0||character>3){error="Invalid player option configuration";return false;}const i32 count=power/power_step;power_level=count;if(count<0||count>4){error="Player option power level outside range";return false;}
    if(power<max_power){for(auto& handles:visuals.option_handles){if(!animations.interrupt(handles[1],1)){error=animations.error;return false;}handles[1]=0;}}
    else for(i32 i=0;i<count;i++){auto& handle=visuals.option_handles[u32(i)][1];if(!animations.retire(handle)){error=animations.error;return false;}constexpr i32 scripts[]={11,13,12,9};handle=animations.create(resource,scripts[character],14,2);auto* vm=animations.registry.find(handle);if(!vm){error=animations.error;return false;}vm->visual.translation.y=-32.f;}
    if(motion.option_count==0)auxiliary=0;if(motion.option_count==count)return true;
    const i32 first=count*(count-1)/2;constexpr i32 scripts[]={10,11,10,8};
    for(i32 i=0;i<count;i++){auto& option=motion.options[u32(i)];auto& handle=visuals.option_handles[u32(i)][0];option.position=motion.position_fixed;if(!animations.retire(handle)){error=animations.error;return false;}option.index=i;option.normal_offset=offset(first+i,false);option.focus_offset=offset(first+i,true);const auto& delta=motion.focus?option.focus_offset:option.normal_offset;option.target={wrapping_add(motion.position_fixed.x,delta.x),wrapping_add(motion.position_fixed.y,delta.y)};option.position=option.target;handle=animations.create(resource,scripts[character],14,0);auto* vm=animations.registry.find(handle);if(!vm){error=animations.error;return false;}vm->visual.translation.y=-32.f;option.active=2;}
    for(i32 i=count;i<8;i++){motion.options[u32(i)].active=0;if(!animations.interrupt(visuals.option_handles[u32(i)][0],1)){error=animations.error;return false;}}motion.option_count=count;motion.snap_options();return true;
}
}
