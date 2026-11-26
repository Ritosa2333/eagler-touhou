#pragma once
#include "core/stages/ExBossControlServices.hpp"
namespace kaikeidou {
struct ExBossHeavyServices:ExBossControlServices{
    virtual std::uint32_t draw_line(std::uint32_t x1,std::uint32_t y1,std::uint32_t x2,std::uint32_t y2,std::uint32_t color,std::uint32_t width)=0;
    virtual std::uint32_t color(std::uint32_t red,std::uint32_t green,std::uint32_t blue)=0;
};
}
