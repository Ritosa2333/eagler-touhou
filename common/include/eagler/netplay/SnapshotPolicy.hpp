#pragma once

#include <algorithm>
#include <cstdint>
#include <limits>

namespace Netplay
{
// A current frame with exact input may still depend on an earlier prediction.
// Only retire a journal after reconciliation AND a contiguous confirmed prefix.
constexpr bool NeedsRollbackSnapshot(std::uint32_t frame,
                                     std::uint32_t confirmedThrough,
                                     std::uint8_t predictedMask,
                                     bool resimulation,
                                     bool elideConfirmedResimulation = false)
{
    // A rebuilt authoritative prefix also needs no further undo state.
    // Keep the conservative replay policy available for paired measurements.
    return (resimulation && !elideConfirmedResimulation) || predictedMask != 0 ||
        (frame != 0 && (confirmedThrough == std::numeric_limits<std::uint32_t>::max() ||
                       confirmedThrough < frame - 1));
}

// Decide the REAL batch before choosing its final, hardware-sampled slot.
// A lead/end-of-test cap must not strand that slot outside the executed batch.
constexpr unsigned BoundedCaptureBatch(std::uint64_t due, std::uint32_t next,
                                       std::uint32_t maxInclusive,
                                       std::uint32_t endExclusive,
                                       unsigned limit = 8)
{
    if (next > maxInclusive || next >= endExclusive)
        return 0;
    return static_cast<unsigned>(std::min<std::uint64_t>({
        due, limit, static_cast<std::uint64_t>(maxInclusive) - next + 1,
        static_cast<std::uint64_t>(endExclusive) - next}));
}
} // namespace Netplay

