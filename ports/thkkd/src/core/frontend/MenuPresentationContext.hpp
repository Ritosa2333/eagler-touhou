// 图形句柄与计时全局保持原位型；标题记录复用既有上下文。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include <array>
namespace kaikeidou {
struct MenuPresentationAssets {
 std::array<std::uint32_t,5> object_graphs{}; // A78E34..A78E44。
 std::array<std::uint32_t,2> help_fonts{}; // A74BA4、A74BA8。
};
struct MenuPresentationTiming {
 ObjectRecord<256> data{}; // A71D08：index0、基准4、平均毫秒8、60个间隔16。
 std::uint32_t previous_tick=0; // B3E890。
};
struct MenuPresentationRegisters {std::uint32_t eax=0,ecx=0;};
}
