#include "GrazeRules.hpp"

#include <bit>
#include <cmath>
#include <limits>

namespace th128::graze {
namespace {

static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559);

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 65536.0f;
}

}

Plan::Plan(Globals updated, std::array<CallRequest, 3> calls, std::uint32_t count) noexcept
    : updated(updated), calls_(calls), count_(count) {}

std::span<const CallRequest> Plan::requests() const noexcept {
    return {calls_.data(), count_};
}

std::optional<Plan> planGraze(Globals state, Position bulletPosition) noexcept {
    if (state.powerRaw > std::numeric_limits<std::int32_t>::max() - 100
        || !bounded(bulletPosition.horizontal) || !bounded(bulletPosition.vertical)
        || !bounded(bulletPosition.depth)) {
        return std::nullopt;
    }
    Globals updated = state;
    if (updated.grazeCount < 99999999) ++updated.grazeCount;
    updated.powerRaw += 100;
    if (updated.powerRaw > 10000) updated.powerRaw = 10000;
    std::array<CallRequest, 3> requests{};
    std::uint32_t count = 0;
    if (state.powerRaw < 10000 && updated.powerRaw >= 10000) {
        requests[count++] = {0x43e0db, 0x458410, ThresholdSoundRequest{74}};
    } else if (state.powerRaw < 3000 && updated.powerRaw >= 3000) {
        requests[count++] = {0x43e0f9, 0x458410, ThresholdSoundRequest{73}};
    }
    requests[count++] = {0x43e119, 0x407ec0,
        EffectRequest{175, 23, EffectBankSource::BulletManagerAnm8a7f9c, bulletPosition}};
    requests[count++] = {0x43e129, 0x4584a0, PositionalSoundRequest{44, bulletPosition.horizontal}};
    return Plan{updated, requests, count};
}

CallWords serializeCallRequest(const CallRequest& request) noexcept {
    CallWords words{};
    words[0] = static_cast<std::uint32_t>(request.payload.index());
    words[1] = request.callSite;
    words[2] = request.callTarget;
    if (const auto* sound = std::get_if<ThresholdSoundRequest>(&request.payload)) {
        words[3] = static_cast<std::uint32_t>(sound->soundId);
    } else if (const auto* effect = std::get_if<EffectRequest>(&request.payload)) {
        words[4] = static_cast<std::uint32_t>(effect->scriptId);
        words[5] = static_cast<std::uint32_t>(effect->layer);
        words[6] = static_cast<std::uint32_t>(effect->bankSource);
        words[7] = std::bit_cast<std::uint32_t>(effect->position.horizontal);
        words[8] = std::bit_cast<std::uint32_t>(effect->position.vertical);
        words[9] = std::bit_cast<std::uint32_t>(effect->position.depth);
    } else {
        const auto& positional = std::get<PositionalSoundRequest>(request.payload);
        words[3] = static_cast<std::uint32_t>(positional.soundId);
        words[7] = std::bit_cast<std::uint32_t>(positional.positionX);
    }
    return words;
}

std::optional<CallerDecision> grazeCallerDecision(UpdateSite site, std::int32_t helperReturn,
                                                 std::uint32_t bulletFlags) noexcept {
    if ((site != UpdateSite::HalfStep && site != UpdateSite::FullStep)
        || (helperReturn != 0 && helperReturn != 2)) {
        return std::nullopt;
    }
    const auto continuation = site == UpdateSite::HalfStep ? 0x40888bU : 0x408c93U;
    if (helperReturn == 0 || (bulletFlags & 4U)) {
        return CallerDecision{CallerAction::NoGrazeCall, 0, 0, 0, continuation};
    }
    return CallerDecision{CallerAction::GrazeCallRequired,
                          site == UpdateSite::HalfStep ? 0x408883U : 0x408b67U,
                          0x43e090, 4, continuation};
}

}
