// Monotonic time for the TH_SDL3 (wasm) build. The game clock rides the
// display-callback timeline (sdl_loop_time from ApplicationHost) so frame
// deadlines cannot drift from presentation. Sleep is not blockable on the
// browser thread; the rAF cadence already paces every caller, so Sleep is an
// honest no-op here (documented deviation: deadline pacing is rAF-driven).
#pragma once

#include <cstdint>

namespace th20::web::time {
// Seconds on the display timeline (same clock the frame loop advances).
double monotonic_seconds();
// Milliseconds on the same timeline (timeGetTime role).
std::uint32_t monotonic_milliseconds();
// Unix timestamp for replay/save metadata (system wall clock).
std::int64_t unix_seconds();
// Sleep has no blockable thread in the browser; callers' pacing is owned by
// the rAF cadence. Kept so call sites stay structurally identical.
void sleep(std::uint32_t milliseconds);
}
