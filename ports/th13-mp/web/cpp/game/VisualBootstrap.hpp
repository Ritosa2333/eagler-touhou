#pragma once

#include "Rng.hpp"
#include <array>
#include <cstdint>

namespace th13 {

// 0045ac90: DAT_004be8a0[0..255] receives the upper seven bits of
// successive 16-bit visual RNG values during font initialization.
inline std::array<std::uint8_t, 256> visual_font_noise(GameRandom& visual) {
    std::array<std::uint8_t, 256> noise{};
    for (auto& value : noise) value = std::uint8_t(visual.step16() >> 9);
    return noise;
}

}  // namespace th13
