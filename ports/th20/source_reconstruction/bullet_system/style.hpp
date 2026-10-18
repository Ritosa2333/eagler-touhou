#pragma once
#include "command.hpp"
namespace th20::source::bullet {
struct ColorStyle {std::uint32_t words[5];};
struct Style { //BSS5c06a8 +158*type, initialized by401280
    std::uint32_t script;
    ColorStyle colors[16];
    float radius;
    std::uint32_t draw_group,cancel_type,field_150,child_script;
};
static_assert(sizeof(Style)==0x158&&offsetof(Style,radius)==0x144);
extern const Style styles[50];
void restart_animation(sprite::AnimationFile&,sprite::Animation&,std::int32_t); //4383a0
std::int32_t __cdecl remap_sprite(sprite::Animation*,std::int32_t); //47d580
}
