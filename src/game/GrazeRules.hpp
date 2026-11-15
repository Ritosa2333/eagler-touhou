#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <variant>

namespace th128::graze {

struct Globals {
    std::int32_t grazeCount;
    std::int32_t powerRaw;
};

struct Position {
    float horizontal;
    float vertical;
    float depth;
};

enum class EffectBankSource : std::uint32_t { BulletManagerAnm8a7f9c };

struct ThresholdSoundRequest {
    std::int32_t soundId;
};

struct EffectRequest {
    std::int32_t scriptId;
    std::int32_t layer;
    EffectBankSource bankSource;
    Position position;
};

struct PositionalSoundRequest {
    std::int32_t soundId;
    float positionX;
};

using CallPayload = std::variant<ThresholdSoundRequest, EffectRequest, PositionalSoundRequest>;

struct CallRequest {
    std::uint32_t callSite;
    std::uint32_t callTarget;
    CallPayload payload;
};

class Plan {
public:
    Globals updated;
    std::span<const CallRequest> requests() const noexcept;

private:
    std::array<CallRequest, 3> calls_;
    std::uint32_t count_;

    Plan(Globals updated, std::array<CallRequest, 3> calls, std::uint32_t count) noexcept;
    friend std::optional<Plan> planGraze(Globals state, Position bulletPosition) noexcept;
};

using CallWords = std::array<std::uint32_t, 10>;

enum class UpdateSite : std::uint8_t { HalfStep, FullStep };
enum class CallerAction : std::uint8_t { NoGrazeCall, GrazeCallRequired };

struct CallerDecision {
    CallerAction action;
    std::uint32_t callSite;
    std::uint32_t callTarget;
    std::uint32_t flagsSetAfterSuccess;
    std::uint32_t continuation;
};

std::optional<Plan> planGraze(Globals state, Position bulletPosition) noexcept;
CallWords serializeCallRequest(const CallRequest& request) noexcept;
std::optional<CallerDecision> grazeCallerDecision(UpdateSite site, std::int32_t helperReturn,
                                                 std::uint32_t bulletFlags) noexcept;

}
