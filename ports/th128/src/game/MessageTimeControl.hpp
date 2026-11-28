#pragma once
#include <cstdint>

namespace th128::message {

enum class Msg22Status { Unsupported, BgmFadeOut };
enum class Msg22Reason { None, InvalidStage, InvalidClock, ConversionOverflow, DurationOverflow };

struct BgmFadeOutRequest {
    static constexpr std::int32_t QueueOperation = 5;
    std::int32_t requestedSeconds = 0;
    std::int32_t scaledSeconds = 0;
    std::int32_t durationFrames = 0;
    std::int32_t durationSeconds = 0;
    std::int32_t volumeStartRaw = 0;
    std::int32_t volumeEndRaw = -5000;
    std::int32_t fadeMode = 1;
    bool immediate = false;
};

struct Msg22Plan {
    Msg22Status status = Msg22Status::Unsupported;
    Msg22Reason reason = Msg22Reason::InvalidStage;
    BgmFadeOutRequest request;
};

struct Msg22Context {
    int stage = 0;
    float clock = 0;
};

struct FadeOutStep {
    bool active = false;
    std::int32_t remaining = 0;
    std::int32_t volumeRaw = -5000;
};

Msg22Plan planMsg22(const Msg22Context& context) noexcept;
FadeOutStep fadeOutStep(std::int32_t remaining, std::int32_t duration) noexcept;

}
