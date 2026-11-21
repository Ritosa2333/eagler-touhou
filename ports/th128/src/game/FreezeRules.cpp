#include "FreezeRules.hpp"

#include <algorithm>
#include <bit>
#include <cmath>
#include <limits>

namespace th128::freeze {
namespace {
static_assert(sizeof(float) == 4 && std::numeric_limits<float>::is_iec559
              && std::numeric_limits<float>::digits == 24);

std::int32_t wrappingAdd(std::int32_t first, std::int32_t second) noexcept {
    const auto sum = static_cast<std::uint32_t>(first) + static_cast<std::uint32_t>(second);
    return std::bit_cast<std::int32_t>(sum);
}
}

bool canRelease(std::int32_t powerRaw, std::int32_t heldCount, std::int32_t perfectState) noexcept {
    return powerRaw >= ReleasePowerRaw && heldCount >= ReleaseHoldCount && perfectState == 0;
}

std::int32_t durationCount(std::int32_t savedPowerRaw) noexcept {
    return wrappingAdd(savedPowerRaw, -2000) / 80;
}

bool durationPending(std::int32_t savedPowerRaw, std::int32_t elapsedCount) noexcept {
    return elapsedCount < durationCount(savedPowerRaw);
}

std::int32_t addPowerRaw(std::int32_t powerRaw, std::int32_t deltaRaw) noexcept {
    return std::min(wrappingAdd(powerRaw, deltaRaw), PowerCapRaw);
}

std::int32_t baselineRechargeRaw(std::int32_t powerRaw) noexcept {
    return addPowerRaw(powerRaw, BaselineIncrementRaw);
}

std::optional<std::int32_t> heldRechargeRaw(std::int32_t heldCount) noexcept {
    if (heldCount < 20) return std::nullopt;
    if (heldCount <= 45) return 5;
    if (heldCount <= 80) return 10;
    if (heldCount <= 110) return 12;
    if (heldCount <= 140) return 13;
    return 15;
}

std::int32_t activationStoreRaw() noexcept {
    return 0;
}

std::int32_t addPerfectFreezeRaw(std::int32_t chargeRaw, std::int32_t deltaRaw) noexcept {
    return std::clamp(wrappingAdd(chargeRaw, deltaRaw), std::int32_t{0}, PerfectFreezeCapRaw);
}

std::optional<std::int32_t> spendPerfectFreezeRaw(std::int32_t chargeRaw) noexcept {
    if (chargeRaw < PerfectFreezeCostRaw || chargeRaw > PerfectFreezeCapRaw) return std::nullopt;
    return chargeRaw - PerfectFreezeCostRaw;
}

std::optional<std::int32_t> areaRewardRaw(float lastAreaPercent, std::int32_t savedPowerRaw) noexcept {
    if (!std::isfinite(lastAreaPercent) || lastAreaPercent < 0.0f || lastAreaPercent > 100.0f
        || savedPowerRaw < 0 || savedPowerRaw > PowerCapRaw) return std::nullopt;
    const double area = lastAreaPercent;
    if (area <= 10.0) return static_cast<std::int32_t>(area * 50.0);
    if (area <= 30.0) return 500 - static_cast<std::int32_t>((area - 10.0) * -25.0);
    return 2000;
}

}
