#pragma once

#include <SDL3/SDL_timer.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace eagler::presentation {

// SDL builds that fall back to gettimeofday on Web inherit the coarse wall
// clock. Use the browser's monotonic timer for elapsed-time calculations.
inline Uint64 FrameCounter() noexcept
{
#ifdef __EMSCRIPTEN__
    return static_cast<Uint64>(emscripten_get_now() * 1000.0);
#else
    return SDL_GetPerformanceCounter();
#endif
}

inline Uint64 FrameCounterFrequency() noexcept
{
#ifdef __EMSCRIPTEN__
    return 1'000'000;
#else
    return SDL_GetPerformanceFrequency();
#endif
}

inline double FrameTimeSeconds() noexcept
{
    return static_cast<double>(FrameCounter()) / static_cast<double>(FrameCounterFrequency());
}

}
