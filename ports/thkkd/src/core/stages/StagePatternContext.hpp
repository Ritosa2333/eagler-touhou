// 原发弹参数index6..19；index0..4保留在PatternParameters，index5保留在EnemyDeathContext。
#pragma once
#include <array>
#include <cstdint>

namespace kaikeidou {
struct StagePatternContext {
    std::array<std::uint32_t, 14> integers_6_to_19{}; // A78570..A785A4。
    std::uint64_t real_4 = 0; // A87DE0。
    std::array<std::uint64_t, 10> reals_10_to_19{}; // A87E10..A87E58。
    std::uint32_t boss_hp_multiplier = 0; // A82F8C，原IMUL按32位回绕。
};
}
