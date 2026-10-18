#include "stage_reset.hpp"
#include "power.hpp"
#include <algorithm>
#include <bit>
#include <cstring>
namespace th20::source::player_entity {
namespace {
void option_event(Player& player,int event,StageResetServices& host){for(auto& option:player.options){host.interrupt(option.handle_dc,event);host.interrupt(option.handle_e0,event);}for(auto& option:player.secondary_options){host.interrupt(option.handle_dc,event);host.interrupt(option.handle_e0,event);}player.fields_20e4[1]=0;}
}
void finish_stage_visibility(Player& player,StageResetServices& host){player.entity_flags|=2;option_event(player,3,host);}
void restore_stage_visibility(Player& player,StageResetServices& host){player.entity_flags&=~2u;option_event(player,2,host);}
void disable_for_stage(Player& player,StageResetServices& host){host.hide_animation(player.animation);player.state=6;finish_stage_visibility(player,host);}
void reset_for_stage(Player& player,StageResetServices& host){
    player.state=1;recovered::timer_set(player.timers_644[0],0);recovered::timer_set(player.timers_644[1],0);player.entity_flags&=~9u;
    host.delete_animation(player.handle_60c);player.handle_60c=0;host.delete_animation(player.handle_610);player.handle_610=0;
    clear_shots(player.shots,*player.services);restore_stage_visibility(player,host);host.refresh_power(player);player.fields_20e4[2]=0x3f800000u;player.entity_flags&=~4u;
    player.interpolation_220c.duration=0;player.collision_expansion=1;player.clock_scale=1;
    for(auto& previous:player.vectors_20fc)std::memcpy(&previous,&player.fixed_position,sizeof(previous));player.field_2204=8; //4ff690 returns fixed coordinates at+620
    for(unsigned i=0;i<4;++i){float speed;std::memcpy(&speed,static_cast<const std::uint8_t*>(player.shot_data)+0x10+i*4,4);player.speeds_20b4[i]=fixed_coordinates({speed,0}).x;}
    player.fields_2080[0]=std::bit_cast<std::uint32_t>(5.0f);player.fields_2080[1]=std::bit_cast<std::uint32_t>(30.0f);player.fields_2080[2]=player.fields_2080[3]=std::bit_cast<std::uint32_t>(70.0f);player.fields_2080[4]=std::bit_cast<std::uint32_t>(128.0f);
    player.normal_radius=player.focus_radius=3.0f;player.normal_extent={1.5f,1.5f,5};player.focus_extent={1.5f,1.5f,5};
    player.field_2204=8;for(unsigned i=0;i<5;++i)player.animation_scripts[i]=i;
    auto& field=host.session().player_table.field_1e4;const int clamped=std::clamp(std::bit_cast<int>(field),0,1);field=std::bit_cast<std::uint32_t>(clamped);
    if(clamped==0&&!(host.session().flags&1u)&&player.view_index==1)disable_for_stage(player,host);
}
}
