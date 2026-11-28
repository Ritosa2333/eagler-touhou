#pragma once
#include "Types.hpp"

namespace th13 {
inline bool replay_fast_forward_restart(u32 physical_held, u32 consumed, bool advanced, bool paused, u32 scene_flags,
                                        bool finished) noexcept {
    return advanced && !paused && !finished && !(scene_flags & 4u) && (physical_held & 0x201u) && (consumed % 8u != 0);
}
}
