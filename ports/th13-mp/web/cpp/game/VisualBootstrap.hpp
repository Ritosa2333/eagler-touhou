#pragma once

#include "Rng.hpp"
#include <array>
#include <cstdint>

namespace th13 {

inline std::array<std::uint8_t, 256> visual_font_noise(GameRandom& visual) {
    std::array<std::uint8_t, 256> noise{};
    for (auto& value : noise)
        value = std::uint8_t(visual.step16() >> 9);
    return noise;
}

}
