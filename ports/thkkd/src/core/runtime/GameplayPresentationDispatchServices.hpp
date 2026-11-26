// 分派目标枚举值固定为原入口，便于主侧接入现有恢复模块。
#pragma once
#include "core/frontend/EndingFadePresentation.hpp"
namespace kaikeidou {
enum class GameplayPresentationComponent:std::uint32_t {
    routine_611db0=0x611db0,routine_6128f0=0x6128f0,routine_618c30=0x618c30,
    routine_61d0f0=0x61d0f0,routine_619b80=0x619b80,routine_65cd00=0x65cd00,
    routine_619a80=0x619a80,routine_61d450=0x61d450,routine_612480=0x612480,
    routine_644bb0=0x644bb0,routine_613560=0x613560,routine_65cc50=0x65cc50,
    routine_613250=0x613250,routine_61d180=0x61d180,routine_611ff0=0x611ff0
};
struct GameplayPresentationDispatchServices:EndingFadePresentationServices {
    virtual void tint_color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
    virtual void present_component(GameplayPresentationComponent component)=0;
    virtual std::int32_t input_count(std::uint32_t binding)=0;
    virtual void draw_slow_marker(std::uint64_t x_bits,std::uint64_t y_bits,
        std::uint64_t scale_bits,std::uint64_t angle_bits,std::uint32_t graph,
        std::uint32_t transparent,std::uint32_t turn)=0;
    virtual void draw_fps(std::uint32_t x,std::uint32_t y)=0;
};
}
