#pragma once
#include "Types.hpp"
#include <array>

namespace th13 {
// v1.00c: 0045fea0 walks 71 records at 004bad90; sample pointers at 004bb320.
// These are voice IDs, not sample indices (several voices share a sample).
struct SoundDefinition {
    i32 id, sample;
    i16 volume, renewal;
    u32 flags, reserved;
};
extern const std::array<SoundDefinition, 71> sound_definitions;
extern const std::array<const char*, 60> sound_samples;
}
