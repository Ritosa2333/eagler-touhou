#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

namespace eagler::presentation {

// Allow a browser callback that lands just before a logical deadline to serve
// it now. Callers subtract the full interval so this advance is carried to
// subsequent callbacks and does not increase the long-term simulation rate.
inline double WebFrameEarlyTolerance(double interval) noexcept
{
    return std::min(0.002, interval / 8.0);
}

inline double FrameEarlyTolerance(double interval) noexcept
{
#ifdef __EMSCRIPTEN__
    return WebFrameEarlyTolerance(interval);
#else
    (void)interval;
    return 0.0;
#endif
}

inline unsigned DueFrameSlots(double debt, double interval, double earlyTolerance,
                              unsigned limit = std::numeric_limits<unsigned>::max()) noexcept
{
    const double slots = std::floor((debt + earlyTolerance + 1.e-9) / interval);
    return static_cast<unsigned>(std::clamp(slots, 0.0, static_cast<double>(limit)));
}

// Short gaps can cross several deadlines. Keep at most two intervals for
// later callbacks, each of which still draws at most one tick. Longer stalls
// expire the old deadlines so regaining focus does not leave a catch-up burst.
inline double SingleFrameRemainingTime(double debt, double elapsed, double interval,
                                       double earlyTolerance, unsigned due,
                                       bool retainAfterShortGap) noexcept
{
    if (retainAfterShortGap && elapsed <= 2.0 * interval + earlyTolerance)
        return std::min(debt - interval, 2.0 * interval - earlyTolerance);
    return debt - due * interval;
}

}
