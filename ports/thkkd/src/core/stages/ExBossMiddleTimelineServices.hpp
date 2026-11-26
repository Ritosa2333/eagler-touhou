#pragma once
#include "core/entities/BossStageContext.hpp"
namespace kaikeidou {
struct ExBossMiddleTimelineServices:BossStageServices{
    virtual void draw_line(std::uint32_t x1_bits,std::uint32_t y1_bits,std::uint32_t x2_bits,std::uint32_t y2_bits,std::uint32_t color,std::uint32_t thickness)=0;
};
}
