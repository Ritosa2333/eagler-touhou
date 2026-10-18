#pragma once

#include <cstdint>
#include <limits>

namespace Netplay
{
// Rare reliable duplicate for a stalled acknowledgement frontier. This never
// samples input or alters the frame clock, and is not a per-frame reliable lane.
class InputRepairBudget
{
public:
    static constexpr std::uint64_t StalledMs = 80;
    static constexpr std::uint64_t RetryMs = 80;

    bool ShouldRepair(std::uint32_t firstUnacknowledged, bool hasPayload,
                      std::uint64_t now)
    {
        if (!hasPayload || firstUnacknowledged == std::numeric_limits<std::uint32_t>::max())
        {
            tracked_ = false;
            return false;
        }
        if (!tracked_ || firstUnacknowledged != first_ || now < progressMs_ ||
            now < lastRepairMs_)
        {
            tracked_ = true;
            first_ = firstUnacknowledged;
            progressMs_ = lastRepairMs_ = now;
            return false;
        }
        if (now - progressMs_ < StalledMs || now - lastRepairMs_ < RetryMs)
            return false;
        lastRepairMs_ = now;
        return true;
    }

private:
    bool tracked_ = false;
    std::uint32_t first_ = 0;
    std::uint64_t progressMs_ = 0;
    std::uint64_t lastRepairMs_ = 0;
};
} // namespace Netplay
