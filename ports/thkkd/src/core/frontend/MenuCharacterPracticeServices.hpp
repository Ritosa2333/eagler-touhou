#pragma once
#include "core/frontend/MenuStartServices.hpp"
namespace kaikeidou {
struct MenuCharacterPracticeServices:MenuStartServices {
 virtual void draw_centered_font_text(std::uint64_t x_binary64_bits,std::uint64_t y_binary64_bits,std::string_view text,std::uint32_t ink,std::uint32_t font)=0;
 virtual void rectangle(std::int32_t left,std::int32_t top,std::int32_t right,std::int32_t bottom,std::uint32_t ink,std::uint32_t filled)=0;
};
}
