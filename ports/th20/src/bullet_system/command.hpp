#pragma once
#include "bullet.hpp"
#include <bit>
#include <memory_resource>
#include <vector>
namespace th20::source::bullet {
struct Command { //47bca0, the ECL extended instruction record
    std::uint32_t words[11]{};
    float f(unsigned i) const noexcept {return std::bit_cast<float>(words[i]);}
    std::int32_t i(unsigned n) const noexcept {return recovered::signed_bits(words[n]);}
};
struct ShotMetadata { //47bb90; actual shared object, separate from its reference count
    float field_00{};
    std::pmr::vector<Command> commands;
    float fields_14[4]{};
    std::uint32_t fields_24[5]{};
    std::int16_t type{},field_3a{};
    std::int32_t field_3c=21,field_40=38;
    std::uint32_t field_44{};
    std::uint8_t field_48{},field_49{},padding_4a[2];
    ShotMetadata();
};
struct ShotParameters { //47bd20
    std::uint32_t fields_00[2]{};
    sprite::Vec3 position{};
    float angle{},angle_step{},speed{},speed_step{};
    std::int16_t count{},rows{};
};
static_assert(sizeof(Command)==0x2c&&sizeof(ShotMetadata)==0x4c&&sizeof(ShotParameters)==0x28);
static_assert(offsetof(ShotMetadata,commands)==4&&offsetof(ShotMetadata,type)==0x38);
float resolve_angle(std::int32_t view,float current,const sprite::Vec3&,float requested,float spread); //485740
float bullet_radius(std::int32_t); //485700
void execute_extended_commands(Bullet&); //47dcf0
namespace unrecovered {
void change_bullet_style(Bullet&,const Command&); //47dcf0 case9
void spawn_extended_bullets(Bullet&,const Command&); //47dcf0 case13, advances index twice
void spawn_extended_enemy(Bullet&,const Command&); //47dcf0 case24 ->4a8920
void spawn_extended_laser(Bullet&,const Command&); //47dcf0 case27, advances index itself
}
}
