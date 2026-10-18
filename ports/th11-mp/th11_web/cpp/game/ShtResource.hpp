#pragma once
#include "Types.hpp"
#include <array>
#include <vector>
namespace th11 {
enum class ShotSpawn : u32 { None, Homing, OptionRotation, RandomAcceleration, ConvertedLaser };
enum class ShotUpdate : u32 { None, Homing, Accelerating, Attached, ConvertedLaser };
struct ShotSpec {
    i8 interval=0,delay=0;i16 damage=0;
    Vec2 offset,hitbox;
    float angle=0,speed=0;
    i8 option=0,type=0;i16 animation=0,hit_animation=0,sound=0;
    ShotSpawn spawn=ShotSpawn::None;
    ShotUpdate update=ShotUpdate::None;
    u32 hit_callback=0,extra_callback=0;
    bool due(i32 frame)const noexcept {return interval>0&&frame%interval==delay;}
};
static_assert(sizeof(ShotSpec)==0x34);
struct ShtHeader {
    u16 version=0,group_count=0;
    float hitbox=0,attraction_speed=0,attraction_diameter=0;
    float speed=0,focus_speed=0,diagonal_speed=0,focus_diagonal_speed=0;
    i32 max_power_level=0,power_step=0;
    std::array<Vec2,72> option_offsets{};
};
static_assert(sizeof(ShtHeader)==0x268);
struct ShotGroup {u32 metadata=0;std::vector<ShotSpec> shots;};
class ShtResource {
public:
    ShtHeader header;
    std::vector<ShotGroup> groups;
    bool open(const u8*,u32);
    i32 group_index(i32 power,i32 power_step,i32 character,i32 subtype,bool focused,i32 weapon_mode)const noexcept;
};
}
