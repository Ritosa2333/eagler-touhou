#pragma once
#include "Types.hpp"

namespace th11 {
// Replay455e60 runs at update priority30, after normal ANM29. Registry401280
// handles return6 by restarting its update list; rendering runs only when the
// list finally completes. The mask is physical input, before Replay12 replaces
// normalized gameplay keys. Stage activation455ed0 clears Replay+210.
inline bool th14_replay_repeat(bool controller,u32 mode,u32 controller_flags,
                              u32 physical_held,u32 callback_frame) noexcept {
    return controller&&!(controller_flags&4u)&&mode==1u&&
        (physical_held&0x201u)&&(callback_frame&7u)!=0;
}
}
