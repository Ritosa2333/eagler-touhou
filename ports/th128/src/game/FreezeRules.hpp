#pragma once

#include <cstdint>
#include <optional>

namespace th128::freeze {

inline constexpr std::int32_t ReleasePowerRaw = 3000;
inline constexpr std::int32_t ReleaseHoldCount = 45;
inline constexpr std::int32_t PowerCapRaw = 10000;
inline constexpr std::int32_t PerfectFreezeCostRaw = 10000;
inline constexpr std::int32_t PerfectFreezeCapRaw = 30000;
inline constexpr std::int32_t BaselineIncrementRaw = 5;

bool canRelease(std::int32_t powerRaw, std::int32_t heldCount, std::int32_t perfectState) noexcept;
std::int32_t durationCount(std::int32_t savedPowerRaw) noexcept;
bool durationPending(std::int32_t savedPowerRaw, std::int32_t elapsedCount) noexcept;
std::int32_t addPowerRaw(std::int32_t powerRaw, std::int32_t deltaRaw) noexcept;
std::int32_t baselineRechargeRaw(std::int32_t powerRaw) noexcept;
std::optional<std::int32_t> heldRechargeRaw(std::int32_t heldCount) noexcept;
std::int32_t activationStoreRaw() noexcept;
std::int32_t addPerfectFreezeRaw(std::int32_t chargeRaw, std::int32_t deltaRaw) noexcept;
std::optional<std::int32_t> spendPerfectFreezeRaw(std::int32_t chargeRaw) noexcept;
std::optional<std::int32_t> areaRewardRaw(float lastAreaPercent, std::int32_t savedPowerRaw) noexcept;

}
