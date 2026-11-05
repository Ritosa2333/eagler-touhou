#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace Netplay
{
// Shared wall-clock time-sync policy used by the TH06/TH07 rollback drivers.
// This affects only presentation/simulation pacing; it never changes logical
// input frames, confirmation, rollback ownership or deterministic game state.
struct FramePacingPolicy
{
    static constexpr std::int32_t MaxAbsoluteLeadFrames = 30;
    static constexpr double DeadbandFrames = 0.5;
    static constexpr double ScalePerLeadFrame = 0.003;
    static constexpr double MinScale = 0.98;
    static constexpr double MaxScale = 1.02;
    static constexpr double SmoothingFactor = 0.08;
    static constexpr double UnitySnapThreshold = 0.0002;

    static constexpr std::int32_t SignedFrameDelta(std::uint32_t lhs,
                                                    std::uint32_t rhs)
    {
        return static_cast<std::int32_t>(lhs - rhs);
    }

    static constexpr std::int32_t InferLead(std::uint32_t localFrame,
                                            std::uint32_t senderFrame,
                                            std::int16_t senderFrameAdvantage)
    {
        const std::int32_t localAdvantage = SignedFrameDelta(localFrame, senderFrame);
        const std::int32_t advantageDifference =
            localAdvantage - static_cast<std::int32_t>(senderFrameAdvantage);
        return advantageDifference / 2;
    }

    static constexpr bool AcceptLead(std::int32_t inferredLead)
    {
        return inferredLead >= -MaxAbsoluteLeadFrames &&
               inferredLead <= MaxAbsoluteLeadFrames;
    }

    static double UpdateScale(double currentScale, double recommendedLead)
    {
        const double deadbandLead = std::abs(recommendedLead) < DeadbandFrames
            ? 0.0 : recommendedLead;
        const double desiredScale = std::clamp(
            1.0 + deadbandLead * ScalePerLeadFrame, MinScale, MaxScale);
        currentScale += (desiredScale - currentScale) * SmoothingFactor;
        if (std::abs(currentScale - 1.0) < UnitySnapThreshold)
            currentScale = 1.0;
        return currentScale;
    }
};
} // namespace Netplay
