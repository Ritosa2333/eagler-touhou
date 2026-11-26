// Boss时间轴的额外实数与符卡名称；旧发弹参数布局保持原来的两个实数。
#pragma once
#include <array>
#include <cstdint>
#include "core/shared/ObjectRecord.hpp"

namespace kaikeidou {
struct BossStageContext {
    std::array<std::uint64_t, 2> extra_reals{}; // A87DD0、A87DD8。
    std::array<std::uint8_t, 64> spell_name{}; // A7EAB8..A7EAF8。
    ObjectRecord<248> enemy_prefix; // A87EF0..A87FE8，保留Boss分配失败后对-1槽的原写入。
};
struct BossStageServices {
    virtual ~BossStageServices() = default;
    virtual std::int32_t allocate_boss(std::uint32_t kind) = 0;
    virtual void configure_phase(std::uint32_t spell, std::uint32_t seconds,
        std::uint32_t hp, std::uint32_t spell_index) = 0;
    virtual void begin_boss_effect() = 0;
    virtual void retire_boss(std::uint32_t mode) = 0;
    virtual std::uint32_t color(std::uint32_t red, std::uint32_t green, std::uint32_t blue) = 0;
};
}
