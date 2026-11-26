// 死亡解析直接访问的六个全局字段；第六个整数独立于既有五整数发弹参数。
#pragma once
#include <cstdint>

namespace kaikeidou {
struct EnemyDeathContext {
    std::uint32_t stage = 0, part = 0, subphase = 0, character = 0, boss_state = 0;
    std::uint32_t pattern_integer_5 = 0;
};
struct EnemyDeathServices {
    virtual ~EnemyDeathServices() = default;
    virtual std::uint32_t color(std::uint32_t red, std::uint32_t green, std::uint32_t blue) = 0;
    virtual void advance_phase(std::uint32_t mode) = 0;
};
}
