#pragma once
#include "Types.hpp"
#include <array>

namespace th13 {
struct SoundDefinition {
    i32 id, sample;
    i16 volume, renewal;
    u32 flags, reserved;
};
extern const std::array<SoundDefinition, 71> sound_definitions;
extern const std::array<const char*, 60> sound_samples;
}
