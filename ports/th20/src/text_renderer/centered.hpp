#pragma once
#include "text.hpp"
namespace th20::source::text {
SIZE write_midpoint_animation_text(sprite::Controller&,sprite::Animation&,std::uint32_t foreground,
    std::uint32_t background,int font,int spacing,std::uint8_t* ready,std::function<void()> completion,const char* format,...); //44add0
SIZE write_centered_animation_text(sprite::Controller&,sprite::Animation&,std::uint32_t foreground,
    std::uint32_t background,int font,int spacing,std::uint8_t* ready,std::function<void()> completion,const char* format,...); //44b020
}
