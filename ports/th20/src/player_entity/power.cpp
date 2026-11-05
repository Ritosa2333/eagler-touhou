#include "power.hpp"
#include <algorithm>
#include <bit>
#include <cstring>
#include <xmmintrin.h>
namespace th20::source::player_entity {
namespace {
std::int32_t& field(game_session::Player& p,unsigned offset){return *reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uint8_t*>(&p)+offset);}
std::int32_t add(std::int32_t a,std::int32_t b){return std::bit_cast<std::int32_t>(std::uint32_t(a)+std::uint32_t(b));}
std::int32_t scaled(float value){return _mm_cvtt_ss2si(_mm_set_ss(value*128.0f));}
int script(const Player& p,unsigned offset,int variant){std::int32_t result;std::memcpy(&result,static_cast<const std::uint8_t*>(p.shot_data)+offset+std::size_t(variant)*4,4);return result;}
Player& context_player(Player& p){return *static_cast<Player*>(p.context->objects_04[0]);}
}
Fixed2 fixed_coordinates(const sprite::Vec2& value) noexcept{return {scaled(value.x),scaled(value.y)};}
Fixed2 add_fixed_coordinates(const Fixed2& a,const Fixed2& b) noexcept{return {add(a.x,b.x),add(a.y,b.y)};}
int clamped_power(game_session::Player& p) noexcept{auto& value=field(p,0x30);return value=std::clamp(value,0,400);}
int clamped_power_unit(game_session::Player& p) noexcept{auto& value=field(p,0x38);return value=std::clamp(value,100,400);}
int clamped_maximum_power(game_session::Player& p) noexcept{return field(p,0x34)=400;}
int power_level(game_session::Player& p) noexcept{const int power=clamped_power(p);const int unit=clamped_power_unit(p);return power/unit;}
void mark_options_changed(Player& player,std::uint32_t value) noexcept{for(auto& option:player.options)option.fields_f4[2]=value;for(auto& option:player.secondary_options)option.fields_f4[2]=value;}
void set_position(Player& player,float x,float y) noexcept{player.fixed_position=fixed_coordinates({x,y});player.position_614.x=static_cast<float>(player.fixed_position.x)/128.0f;player.position_614.y=static_cast<float>(player.fixed_position.y)/128.0f;mark_options_changed(player,1);}
void refresh_power(Player& player,int prior_level,PowerServices& host){
    host.select_view(player.view_index);player.shots.field_1255c=(player.shots.field_1255c&~1u)|1u;
    if(prior_level<0)(void)power_level(*player.context->current_player); //Original result intentionally discarded.
    const int level=power_level(*player.context->current_player);
    const int amount=clamped_power(*player.context->current_player);
    const int maximum=clamped_maximum_power(*player.context->current_player);
    if(amount<maximum){for(auto& option:player.options){host.interrupt(option.handle_e0,1);option.handle_e0=0;}}
    else for(int index=0;index<level;++index){auto& option=player.options[index];host.delete_animation(option.handle_e0);
        if(context_player(player).animation_file){auto& file=*context_player(player).animation_file;option.handle_e0=host.spawn(file,script(player,0xa8,host.script_variant()),-1,2);}
        host.animation(option.handle_e0).vector_5bc.y=-999.0f;
    }
    if(static_cast<std::int32_t>(context_player(player).fields_674[3])!=level){
        int index=0;for(;index<level;++index){auto& option=player.options[index];
            reinterpret_cast<std::uint8_t*>(option.fields_f4)[0x10]=host.global_player().focused_204c!=0;
            host.delete_animation(option.handle_dc);option.fields_f4[1]=index;
            option.offsets_80[0]=fixed_coordinates(host.option_offset(*player.context,level,index,false));
            option.offsets_80[1]=fixed_coordinates(host.option_offset(*player.context,level,index,true));
            auto& actual=context_player(player);const auto& selected=option.offsets_80[actual.focused_204c?1:0];
            option.vector_70=add_fixed_coordinates(actual.fixed_position,selected);option.vector_78=option.vector_70;
            option.handle_dc=host.spawn(*context_player(player).animation_file,script(player,0x88,host.script_variant()),14,0);
            host.animation(option.handle_dc).vector_5bc.y=-999.0f;
            option.state=2;option.fields_f4[5]=0;option.fields_f4[3]&=~2u;option.fields_f4[6]=0;
            host.initialize_option(option,index);
        }
        for(;index<10;++index){auto& option=player.options[index];option.state=0;option.fields_f4[5]=0;host.interrupt(option.handle_dc,1);}
        player.fields_674[3]=level;mark_options_changed(context_player(player),1);
    }
    for(auto& option:player.secondary_options){option.state=0;option.fields_f4[5]=0;option.fields_f4[3]&=~2u;}
}
}
