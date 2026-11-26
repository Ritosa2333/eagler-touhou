// 三角函数与瞄准角的游戏数值接口，返回原调用方随后参与运算的有效数。
#pragma once
#include "core/math/GameAtan2.hpp"

namespace kaikeidou {
struct GameMath {
    virtual ~GameMath() = default;
    virtual binary_float::Number atan2(std::uint64_t y, std::uint64_t x) = 0;
    virtual binary_float::Number cosine(std::uint64_t angle) = 0;
    virtual binary_float::Number sine(std::uint64_t angle) = 0;
};
struct RecoveredGameMath final : GameMath {
    binary_float::Number atan2(std::uint64_t y, std::uint64_t x) override { return game_atan::finite_atan2(y, x); }
    binary_float::Number cosine(std::uint64_t angle) override { return binary_float::binary64(crt_sse2::cosine_bits(angle)); }
    binary_float::Number sine(std::uint64_t angle) override { return binary_float::binary64(crt_sse2::sine_bits(angle)); }
};
}
