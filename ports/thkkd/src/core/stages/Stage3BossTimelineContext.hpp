// 三面关底新增的原全局实数槽；与StagePatternContext既有布局分别映射。
#pragma once
#include <array>
#include <cstdint>
namespace kaikeidou {
struct Stage3BossTimelineContext {
    std::array<std::uint64_t,5> reals_5_to_9{}; // A87DE8、A87DF0、A87DF8、A87E00、A87E08。
    std::array<std::uint64_t,10> reals_20_to_29{}; // A87E60..A87EA8。
};
}
