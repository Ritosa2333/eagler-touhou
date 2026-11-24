#include "MessageTimeControl.hpp"
#include <cmath>
#include <limits>

namespace th128::message {
namespace {

constexpr std::int32_t FramesPerSecond = 60;
constexpr std::int32_t VolumeStartRaw = 0;
constexpr std::int32_t VolumeEndRaw = -5000;

bool finalStage(int stage) noexcept {
    switch (stage) {
    case 3:
    case 5:
    case 8:
    case 10:
    case 13:
    case 15:
        return true;
    default:
        return false;
    }
}

}

Msg22Plan planMsg22(const Msg22Context& context) noexcept {
    Msg22Plan plan;
    if (context.stage < 0 || context.stage > 23) {
        plan.reason = Msg22Reason::InvalidStage;
        return plan;
    }
    if (!std::isfinite(context.clock)) {
        plan.reason = Msg22Reason::InvalidClock;
        return plan;
    }
    const float requested = finalStage(context.stage) ? 2.0f : 8.0f;
    const float scaled = (context.clock == 0.0f || context.clock >= 1.0f)
                             ? requested
                             : requested / context.clock;
    if (!std::isfinite(scaled) ||
        scaled <= static_cast<float>(std::numeric_limits<std::int32_t>::min()) ||
        scaled >= static_cast<float>(std::numeric_limits<std::int32_t>::max())) {
        plan.reason = Msg22Reason::ConversionOverflow;
        return plan;
    }
    const auto value = static_cast<std::int32_t>(scaled);
    const auto frames = static_cast<std::int64_t>(value) * FramesPerSecond;
    if (frames < std::numeric_limits<std::int32_t>::min() ||
        frames > std::numeric_limits<std::int32_t>::max()) {
        plan.reason = Msg22Reason::DurationOverflow;
        return plan;
    }
    plan.status = Msg22Status::BgmFadeOut;
    plan.reason = Msg22Reason::None;
    plan.request.requestedSeconds = static_cast<std::int32_t>(requested);
    plan.request.scaledSeconds = value;
    plan.request.durationFrames = static_cast<std::int32_t>(frames);
    plan.request.durationSeconds = value;
    plan.request.volumeStartRaw = VolumeStartRaw;
    plan.request.volumeEndRaw = VolumeEndRaw;
    plan.request.immediate = plan.request.durationFrames <= 0;
    return plan;
}

FadeOutStep fadeOutStep(std::int32_t remaining, std::int32_t duration) noexcept {
    FadeOutStep step;
    if (duration <= 0 || remaining <= 0)
        return step;
    --remaining;
    if (remaining <= 0) {
        step.remaining = 0;
        return step;
    }
    step.active = true;
    step.remaining = remaining;
    step.volumeRaw = static_cast<std::int32_t>(
        (static_cast<std::int64_t>(remaining) * 5000) / duration - 5000);
    return step;
}

}
