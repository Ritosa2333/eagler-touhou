#include "PlayerOptions.hpp"
namespace th10 {
namespace {
using Point=PlayerOption::FixedPoint;
Point convert(const Vec3& value){return {(number(value.x)*number(100.f)).truncate_int(),(number(value.y)*number(100.f)).truncate_int()};}
Point add(Point a,Point b){return {wrapping_add(a.x,b.x),wrapping_add(a.y,b.y)};}
Point subtract(Point a,Point b){return {wrapping_add(a.x,static_cast<i32>(0u-static_cast<u32>(b.x))),wrapping_add(a.y,static_cast<i32>(0u-static_cast<u32>(b.y)))};}
}
// 0x426f70. Full-power auras are refreshed even if the option count is unchanged.
void Player::reconfigure_options(PlayerOptionsEnvironment& env){
    auto& registry=env.manager->registry;const i32 power=env.economy->power,character=env.economy->character,shot=env.economy->shot_type;
    if(power>=100){
        const i32 kind=wrapping_add(static_cast<i32>(static_cast<u32>(character)*3u),shot);
        for(auto& option:options){registry.request_delete(option.animations[1]);option.animations[1]=0;
            if(kind>=0&&kind<6)option.animations[1]=env.manager->create(*animation_file,0x14+kind%3,15,AnimationPlacement::WorldFront,*env.animations,*env.allocation);
        }
    }else for(auto& option:options)registry.interrupt(option.animations[1],1);
    i32 count=power/20;if(count>4)count=4;if(option_count==count)return;
    const i32 band_start[]={0,0,1,3,6};i32 i=0;
    for(;i<count;++i){
        auto& option=options[i];option.position=fixed_position;registry.request_delete(option.animations[0]);option.animations[0]=0;option.index=i;
        const i32 index=band_start[count]+i;
        if(character==0){
            option.offset=convert(profile->fast_options[index]);option.focused_offset=convert(profile->slow_options[index]);
            option.target=option.position=add(fixed_position,focused?option.focused_offset:option.offset);
            if(shot>=0&&shot<3)option.animations[0]=env.manager->create(*animation_file,0x11+shot,15,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
        }else if(character==1){
            if(shot==0){
                if(!focused)option.offset=convert(profile->fast_options[index]);option.focused_offset=convert(profile->slow_options[index]);
                option.target=env.global_player->position_history[(i+1)*8];
                if(focused&&!option.active)option.offset=i?options[i-1].offset:convert(profile->slow_options[band_start[count]]);
                option.on_update=env.trailing_callback;
                option.animations[0]=env.manager->create(*animation_file,0x11,15,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
                option.position=position_history[i*8];
            }else if(shot==1){
                option.offset=convert(profile->fast_options[index]);option.focused_offset=convert(profile->slow_options[index]);
                option.target=add(fixed_position,focused?option.focused_offset:option.offset);
                option.animations[0]=env.manager->create(*animation_file,0x12,15,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
            }else if(shot==2){
                option.offset=convert(profile->fast_options[index]);
                if(!focused||!option.active)option.target=option.focused_offset=add(fixed_position,option.offset);
                option.animations[0]=env.manager->create(*animation_file,0x13,15,AnimationPlacement::WorldBack,*env.animations,*env.allocation);
                if(focused)registry.interrupt(option.animations[0],3);option.on_update=env.anchored_callback;
            }
        }
        option.active=2;
    }
    for(;i<4;++i){options[i].active=0;registry.interrupt(options[i].animations[0],1);}
    option_count=count;for(auto& option:options)option.snap_next=1;
}
// 0x427960 / 0x427950: Marisa's trailing options interpolate eight history slots.
void Player::update_trailing_option(PlayerOption& option){
    const i32 base=option.index*8;option.target=position_history[base+8];
    if(!focused)option.offset=subtract(option.target,fixed_position);
    else{
        position_history[base+8]=add(fixed_position,option.offset);auto weight=number(.125f);
        for(i32 i=base+1;i<=base+7;++i){
            const auto first=position_history[base],delta=subtract(position_history[base+8],first);
            position_history[i]={(Extended::from_int(delta.x)*weight+Extended::from_int(first.x)).truncate_int(),(Extended::from_int(delta.y)*weight+Extended::from_int(first.y)).truncate_int()};
            weight=weight+number(.125f);
        }
    }
    option.target=add(fixed_position,option.offset);option.previous_focus=focused;
}
// 0x427ae0 / 0x427ad0: the anchored option preserves its world position in focus.
void Player::update_anchored_option(PlayerOption& option,AnmRegistry& registry){
    if(!focused){if(option.previous_focus)registry.interrupt(option.animations[0],6);option.focused_offset=option.position;}
    else{if(!option.previous_focus)registry.interrupt(option.animations[0],3);option.target=option.focused_offset;}
    option.previous_focus=focused;
}
}
