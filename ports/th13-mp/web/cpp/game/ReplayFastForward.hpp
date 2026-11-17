#pragma once
#include "Types.hpp"

namespace th13 {
// 00448e50 (Update 30): physical Ctrl/Z restarts the Update chain until
// ReplayInf+0x210 reaches the next multiple of eight. Recorded keys do not
// control this gate. Loading, pause and EOF must never spin a frozen cursor.
inline bool replay_fast_forward_restart(u32 physical_held, u32 consumed,
                                        bool advanced, bool paused,
                                        u32 scene_flags, bool finished) noexcept {
    return advanced && !paused && !finished && !(scene_flags & 4u) &&
           (physical_held & 0x201u) && (consumed % 8u != 0);
}
}  // namespace th13
