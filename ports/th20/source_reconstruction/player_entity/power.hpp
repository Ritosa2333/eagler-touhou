#pragma once
#include "owner.hpp"
namespace th20::source::player_entity {
Fixed2 fixed_coordinates(const sprite::Vec2&) noexcept;        //4fee50
Fixed2 add_fixed_coordinates(const Fixed2&,const Fixed2&) noexcept; //4f58f0
int clamped_power(game_session::Player&) noexcept;             //4993b0
int clamped_power_unit(game_session::Player&) noexcept;        //4b81d0
int clamped_maximum_power(game_session::Player&) noexcept;     //4b8210
int power_level(game_session::Player&) noexcept;               //4b81a0
void mark_options_changed(Player&,std::uint32_t) noexcept;     //4fb290
void set_position(Player&,float x,float y) noexcept;           //4ffd80
class PowerServices {
public:
    virtual ~PowerServices()=default;
    virtual void select_view(int)=0;                           //4776a0
    virtual Player& global_player()=0;                        //460830(0)
    virtual void interrupt(std::uint32_t,int)=0;               //44ee90
    virtual void delete_animation(std::uint32_t&)=0;           //44fcd0
    virtual sprite::Animation& animation(std::uint32_t&)=0;    //44ced0
    virtual int script_variant()=0;                          //534130(global overlay0)
    virtual sprite::Vec2 option_offset(game_session::Context&,int level,int index,bool focus)=0; //4ff760/4ff630
    virtual std::uint32_t spawn(sprite::AnimationFile&,int script,int layer,std::uint32_t flags)=0; //450cb0
    virtual void initialize_option(Option&,int)=0;            //533180(global overlay0)
};
void refresh_power(Player&,int prior_level,PowerServices&);    //4faca0
PowerServices& power_services();
inline void refresh_power(Player& player,int prior_level){refresh_power(player,prior_level,power_services());}
namespace unrecovered {
int overlay_script_variant_00534130(runtime::CallbackOwner&);
sprite::Vec2 overlay_option_offset_004ff760(runtime::CallbackOwner&,int,int);
sprite::Vec2 overlay_option_offset_004ff630(runtime::CallbackOwner&,int,int);
void overlay_initialize_option_00533180(runtime::CallbackOwner&,Option&,int);
}
}
