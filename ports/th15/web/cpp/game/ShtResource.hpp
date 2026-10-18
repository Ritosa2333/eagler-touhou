#pragma once
#include "Types.hpp"
#include <array>
#include <vector>
namespace th15 {
struct ShotSpec {
    i8 interval=0,delay=0;i16 damage=0;Vec2 offset{},hitbox{};float angle=0,speed=0;
    u32 parameter=0;i8 option=0,type=0;i16 animation=0,sound=0;
    i8 sound_interval=0,sound_delay=0;
    u32 spawn_kind=0,update_kind=0,hit_kind=0,collision_kind=0;
    std::array<u32,8> behavior_parameters{};
    bool due(i32 frame)const noexcept{return interval>0&&frame%interval==delay;}
};
static_assert(sizeof(ShotSpec)==0x58);
static_assert(offsetof(ShotSpec,option)==0x20&&offsetof(ShotSpec,animation)==0x22&&offsetof(ShotSpec,sound)==0x24&&offsetof(ShotSpec,sound_interval)==0x26&&offsetof(ShotSpec,spawn_kind)==0x28);
struct ShtHeader {
    u16 version=0,group_count=0;float hitbox=0,attraction_speed=0,attraction_diameter=0;
    float speed=0,focus_speed=0,diagonal_speed=0,focus_diagonal_speed=0;
    i32 max_power_level=0,power_step=0;std::array<u8,0xe0-40> parameters{};
    i32 maximum_damage()const noexcept{i32 value=0;std::memcpy(&value,parameters.data(),4);return value;}
};
static_assert(sizeof(ShtHeader)==0xe0);
class ShtResource {
public:
    ShtHeader header;std::vector<std::vector<ShotSpec>> groups;
    bool open(const u8*,u32);
};
}
