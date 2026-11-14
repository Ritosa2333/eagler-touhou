#pragma once
#include "Rng.hpp"
#include <array>
namespace th11 {
// Retail467940 initializes this bitmap state before loading title animations.
// Its256-byte table uses the visual stream; score defaults use gameplay instead.
struct FontBitmapTh14 {
    static constexpr u32 width=1024,height=128,pitch=2048;
    std::array<u8,256> noise{};
    u32 format=26;
    void initialize(Rng& visual,bool argb4444_available=true) noexcept {
        for(auto& byte:noise)byte=u8(visual.next16()>>9);
        format=argb4444_available?26:21;
    }
};
}
