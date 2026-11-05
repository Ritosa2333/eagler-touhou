#pragma once
#include "Types.hpp"
namespace th08 {
// Original Supervisor::UpdateGameTime (00448418), with a supplied platform
// clock so pause/resume and counter rollover have the same semantics in a tab.
void accumulate_play_time(u32 (&time)[4],u32& previous,u32 now);
}
