#pragma once

#include <eagler/netplay/DirectTouchState.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>

namespace Netplay
{
enum class DirectTouchEquivalenceReject : std::uint8_t
{
    None,
    Metadata,
    DeltaShape,
    TraceGap,
    UnlimitedOrInactive,
    CrossAxis,
    DirectionOrBoundary,
};

struct DirectTouchFrameTrace
{
    std::uint32_t frame = INVALID_FRAME;
    FrameInput used{};
    DirectTouchState applied{};
    DirectTouchState remaining{};
    float leftMargin = 0.0f;
    float rightMargin = 0.0f;
    float topMargin = 0.0f;
    float bottomMargin = 0.0f;
};

struct DirectTouchEquivalenceResult
{
    bool equivalent = false;
    float carryX = 0.0f;
    float carryY = 0.0f;
    std::size_t framesChecked = 0;
    DirectTouchEquivalenceReject reject = DirectTouchEquivalenceReject::None;
    std::size_t rejectIndex = 0;
};

inline bool SameDirectTouchMetadata(const FrameInput &predicted, const FrameInput &actual)
{
    return predicted.analogMode == AnalogMode::DirectTouchDelta &&
           actual.analogMode == AnalogMode::DirectTouchDelta &&
           predicted.buttons == actual.buttons && predicted.touchUsed == actual.touchUsed &&
           predicted.touchBomb == actual.touchBomb && predicted.unlimited == actual.unlimited &&
           !actual.unlimited;
}

inline DirectTouchEquivalenceResult ProveDirectTouchEquivalent(
    const DirectTouchFrameTrace *traces, std::size_t count, const FrameInput &actual)
{
    DirectTouchEquivalenceResult result{};
    if (!traces || count == 0 || !SameDirectTouchMetadata(traces[0].used, actual))
    {
        result.reject = DirectTouchEquivalenceReject::Metadata;
        return result;
    }

    const float correctionX = actual.x - traces[0].used.x;
    const float correctionY = actual.y - traces[0].used.y;
    if (!std::isfinite(correctionX) || !std::isfinite(correctionY))
    {
        result.reject = DirectTouchEquivalenceReject::DeltaShape;
        return result;
    }
    const bool xOnly = correctionX != 0.0f && correctionY == 0.0f;
    const bool yOnly = correctionY != 0.0f && correctionX == 0.0f;
    if (!xOnly && !yOnly)
    {
        result.reject = DirectTouchEquivalenceReject::DeltaShape;
        return result;
    }

    for (std::size_t i = 0; i < count; ++i)
    {
        const DirectTouchFrameTrace &trace = traces[i];
        if (trace.frame == INVALID_FRAME ||
            (i != 0 && trace.frame != traces[i - 1].frame + 1u))
        {
            result.reject = DirectTouchEquivalenceReject::TraceGap;
            result.rejectIndex = i;
            return result;
        }

        if (i != 0 && trace.used.analogMode != AnalogMode::DirectTouchDelta)
        {
            result.equivalent = true;
            result.framesChecked = i;
            return result;
        }
        if (trace.used.unlimited || !trace.applied.active || !trace.remaining.active)
        {
            result.reject = DirectTouchEquivalenceReject::UnlimitedOrInactive;
            result.rejectIndex = i;
            return result;
        }

        if (xOnly)
        {
            if (trace.applied.y != 0.0f || trace.remaining.y != 0.0f)
            {
                result.reject = DirectTouchEquivalenceReject::CrossAxis;
                result.rejectIndex = i;
                return result;
            }
            const float corrected = trace.applied.x + correctionX;
            if ((correctionX > 0.0f &&
                 (trace.applied.x <= 0.0f || trace.remaining.x <= 0.0f ||
                  corrected <= 0.0f || corrected >= trace.rightMargin)) ||
                (correctionX < 0.0f &&
                 (trace.applied.x >= 0.0f || trace.remaining.x >= 0.0f ||
                  corrected >= 0.0f || -corrected >= trace.leftMargin)))
            {
                result.reject = DirectTouchEquivalenceReject::DirectionOrBoundary;
                result.rejectIndex = i;
                return result;
            }
        }
        else
        {
            if (trace.applied.x != 0.0f || trace.remaining.x != 0.0f)
            {
                result.reject = DirectTouchEquivalenceReject::CrossAxis;
                result.rejectIndex = i;
                return result;
            }
            const float corrected = trace.applied.y + correctionY;
            if ((correctionY > 0.0f &&
                 (trace.applied.y <= 0.0f || trace.remaining.y <= 0.0f ||
                  corrected <= 0.0f || corrected >= trace.bottomMargin)) ||
                (correctionY < 0.0f &&
                 (trace.applied.y >= 0.0f || trace.remaining.y >= 0.0f ||
                  corrected >= 0.0f || -corrected >= trace.topMargin)))
            {
                result.reject = DirectTouchEquivalenceReject::DirectionOrBoundary;
                result.rejectIndex = i;
                return result;
            }
        }
        ++result.framesChecked;
    }

    result.equivalent = true;
    result.carryX = correctionX;
    result.carryY = correctionY;
    return result;
}
} // namespace Netplay
