// 服务枚举保留原入口，参数中的浮点数统一传存储位型。
#pragma once
#include <cstdint>
#include <string_view>
namespace kaikeidou {
enum class MenuStartComponent:std::uint32_t {routine_5e71e0=0x5e71e0,routine_5e6ff0=0x5e6ff0,routine_5e6c00=0x5e6c00,initialize_title_5e9490=0x5e9490};
struct MenuStartServices {
 virtual ~MenuStartServices()=default;
 virtual std::int32_t input_count(std::uint32_t binding)=0;
 virtual std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
 virtual void blend(std::uint32_t mode,std::uint32_t alpha)=0;
 virtual void draw_font_text(std::int32_t x,std::int32_t y,std::string_view text,std::uint32_t ink,std::uint32_t font,std::uint32_t edge,std::uint32_t vertical)=0;
 virtual void draw_rotated_sprite(std::uint32_t x_binary32_bits,std::uint32_t y_binary32_bits,std::uint64_t scale_binary64_bits,std::uint64_t angle_binary64_bits,std::uint32_t graph,std::uint32_t transparent,std::uint32_t turn)=0;
 virtual void change_stage_resources(std::int32_t stage)=0;
 virtual void present_component(MenuStartComponent component)=0;
 virtual void draw_help(std::string_view first,std::string_view second)=0;
 virtual void move_cursor(std::uint32_t x,std::uint32_t y)=0;
 virtual void present_cursor()=0;
};
}
