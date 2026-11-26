#pragma once
#include "core/stages/Stage6BossTimelineArithmetic.hpp"
namespace kaikeidou {
inline void ex_boss_late_timeline_color(ProjectilePools& pool,std::uint32_t slot,std::uint32_t red,std::uint32_t green,std::uint32_t blue){auto b=stage6_boss_timeline_detail::BulletView{pool,slot*240u};b.word(0x18,red);b.word(0x1c,green);b.word(0x20,blue);}
}
