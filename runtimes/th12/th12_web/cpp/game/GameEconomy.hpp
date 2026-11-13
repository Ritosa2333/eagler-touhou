#pragma once
#include "Types.hpp"
#if PORT_TITLE == 16
#include "SeasonTh16.hpp"
#endif
namespace th11 {
#if PORT_TITLE == 14
struct Th14ItemCombo {i32 total=0,count=0,timer=0;Vec3 position{};};
#endif
// Gameplay counters. Score is stored in units of ten, as in the original.
struct GameEconomy {
    i32 score_units=0,power=0,point_value=0,communication=0;
    i32 lives=2,life_fragments=0,difficulty=1,rank=0;
    i32 max_power=400,power_step=100,graze=0,max_point_value=0;
#if PORT_TITLE == 16
    i32 th16_graze_chain=0;
    i32 th16_death_count=0;
    i32 th16_subseason=0;
    Th16SeasonState th16_season;
    i32 th16_extend_index=0;
    i32 th16_bombs=3,th16_bomb_fragments=0,th16_point_items=0;
    i32 th16_auto_total=0,th16_auto_count=0;
    Vec3 th16_auto_position{};
    float th16_cancel_energy=0;
#endif
#if PORT_TITLE == 14
    i32 th14_point_items=0;
    i32 th14_special_drop_count=0;
    // Snapshot +0x54 is retained by the native replay memcpy; its reset is
    // known (4375fd), but no gameplay consumer has been identified.
    i32 th14_reserved_54=0;
    i32 th14_bombs=3,th14_bomb_fragments=0,th14_life_awards=0;
    Th14ItemCombo th14_combo;
#endif
    void add_score(i32 amount) noexcept;
    void add_point_value(i32 amount) noexcept;
    void add_rank(i32 amount) noexcept;
    i32 point_item_score() const noexcept;
};
#if PORT_TITLE == 14
Vec3 th14_graze(GameEconomy&,Vec3 player,Vec3 contact)noexcept;
#endif
#if PORT_TITLE == 16
struct Th16GrazeServices {
    virtual ~Th16GrazeServices()=default;
    virtual bool effect(Vec3,i32)=0;
    virtual bool popup(Vec3,i32,u32)=0;
    virtual bool sound(i32,float)=0;
    virtual bool item(i32,Vec3,float,float,i32)=0;
};
bool player_th16_graze(GameEconomy&,Vec3 player,Vec3 contact,Th16GrazeServices&);
#endif
}
