#pragma once
#include "AnmManager.hpp"
#include "ShtResource.hpp"
#include "ShotManager.hpp"
#include <array>
namespace th11 {
enum class OptionBehavior:u32 {None,Orbit,Directional};
#if PORT_TITLE == 14
void player_th14_bomb_displacement(i32&,i32&,Vec3,float)noexcept;
#endif
struct PlayerOption {
    i32 state=0;u32 reserved04[6]{};
    Vec2 polar_normal,polar_focus;u32 reserved2c[10]{};
    i32 target_x=0,target_y=0;
    i32 x=0,y=0,normal_x=0,normal_y=0,focus_x=0,focus_y=0;
    std::array<std::array<i32,2>,5> modes{};u32 reserved9c[3]{};
    float angle=0;u32 reservedac=0,animation=0,auxiliary=0,reservedb8[6]{};
    i32 index=0,snap=0;u32 flags=0;OptionBehavior behavior=OptionBehavior::None;u32 reservede0=0;
};
static_assert(sizeof(PlayerOption)==0xe4);
static_assert(offsetof(PlayerOption,target_x)==0x54);
static_assert(offsetof(PlayerOption,x)==0x5c);
static_assert(offsetof(PlayerOption,modes)==0x74);
static_assert(offsetof(PlayerOption,animation)==0xb0);
struct PlayerMotionState {
    Vec3 position; i32 x=0,y=0;
    i32 normal_speed=0,focus_speed=0,normal_diagonal=0,focus_diagonal=0;
    Vec3 delta,last_delta;i32 dx=0,dy=0,previous_dx=0,previous_dy=0;
    i32 direction=0,warp=0,warp_timer=0,focused=0,transition_timer=4;
    i32 option_lerp=30,standing_timer=0,fast_timer=0,weapon_mode=0,recall_timer=0;
    u32 flags=0; i32 option_count=0;float option_angle=0;
#if PORT_TITLE == 14
    float th14_movement_factor=1;
    Vec3 th14_bomb_shift{};
    float th14_scale_factor=1;u32 th14_scale_flags=0;
    std::array<std::array<i32,2>,33> th14_history{};
#endif
#if PORT_TITLE == 16
    float th16_speed_multiplier=1,th16_scale_factor=1;
    Vec3 th16_displacement{};
    Timer th16_charge_timer;
    u32 th16_charge_animation=0;
#endif
};
struct PlayerMotionInput {u32 held=0,pressed=0; i32 character=0,subtype=0;bool enemy_manager=false,enemies=false,bomb=false; int touch_mode=0;float touch_x=0,touch_y=0;};
struct PlayerMotionWorld {
    virtual ~PlayerMotionWorld()=default;
    virtual bool sound(i32){return false;}
    virtual bool rebuild_options(){return false;}
    virtual bool attract_items(){return false;}
#if PORT_TITLE == 14
    virtual u32 create_focus(){return 0;}
#endif
#if PORT_TITLE == 16
    virtual u32 create_movement_effect(i32){return 0;}
#endif
};
class PlayerMotion {
public:
    PlayerMotion(AnmResource&,AnmResource&,AnmManager&,PlayerMotionWorld&,u16 player_file=7,u16 bullet_file=0);
    PlayerMotionState state;std::array<PlayerOption,kPortTitle==16?12:8> options{};AnmVm body{};
#if PORT_TITLE == 16
    i32 th16_requested_count=0,th16_season_count=0;
    bool rebuild_options_th16(const ShtHeader&,const ShtHeader&,AnmResource&,const GameEconomy&,i32 character,i32 subseason);
    bool update_options_th16(u32 base,u32 count);
    bool update_th16(const PlayerMotionInput&);
#endif
    u32 focus_animation=0;i32 last_error=0;
#if PORT_TITLE == 14
    bool update_th14(const PlayerMotionInput&);
#endif
    bool update(const PlayerMotionInput&);
    bool update_warp(const PlayerMotionInput&);
    bool update_option(PlayerOption&);
    bool rebuild_options(const ShtHeader&,const GameEconomy&,i32 combination);
    bool reset_body(){return change_body(0);}
    void clear_options(){for(auto& o:options){o.state=0;interrupt(o.animation,1);interrupt(o.auxiliary,1);}state.option_count=0;}
    void recall_options(bool recall){state.flags=recall?state.flags|8:state.flags&~8u;for(auto& o:options){interrupt(o.animation,recall?3:2);interrupt(o.auxiliary,recall?3:2);}state.recall_timer=0;}
    void clear_focus(){erase(focus_animation);focus_animation=0;}
    void copy_shot_state(ShotPlayer&)const noexcept;
    void set_speeds(const ShtHeader&) noexcept;
private:
    AnmResource& player_resource;AnmResource& bullet_resource;AnmManager& animations;PlayerMotionWorld& world;u16 player_file,bullet_file;
    void interrupt(u32,i16);void erase(u32);void place(u32,Vec3);
    bool change_body(i32);bool afterimage();
};
}
