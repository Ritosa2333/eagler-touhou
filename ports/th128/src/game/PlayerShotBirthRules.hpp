#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

namespace th128::playershot {

inline constexpr std::uint32_t ShotSlotCount = 256;
inline constexpr std::uint32_t ShotSlotBytes = 0x78;
inline constexpr std::uint32_t ShtRecordBytes = 52;

struct ShtRecord {
    std::uint32_t fileOffset = 0;
    std::array<std::uint8_t, ShtRecordBytes> raw{};
};

struct ShtGroup {
    std::uint32_t fileOffset = 0;
    std::uint32_t unknownWord = 0;
    std::vector<ShtRecord> records;
};

struct FirstShotBirthPlan;

class ShtBank {
public:
    ShtBank() = default;
    ShtBank(const ShtBank&) = delete;
    ShtBank& operator=(const ShtBank&) = delete;
    ShtBank(ShtBank&&) = delete;
    ShtBank& operator=(ShtBank&&) = delete;
    bool load(std::span<const std::uint8_t> bytes, std::string& error);
    std::span<const std::uint8_t> bytes() const;
    const ShtGroup* group(std::uint32_t index) const;
    std::uint32_t groupCount() const;
    std::uint32_t recordCount() const;
    bool owns(const FirstShotBirthPlan& plan) const;
    std::weak_ptr<const std::uint8_t> generation() const { return generation_; }

private:
    std::vector<std::uint8_t> bytes_;
    std::vector<ShtGroup> groups_;
    std::uint32_t recordCount_ = 0;
    std::shared_ptr<const std::uint8_t> generation_;
};

struct ShotPosition {
    float x = 0, y = 0, z = 0;
};

struct ShotBirthInput {
    ShotPosition position;
    std::int32_t shotCount = 0;
    std::uint32_t timerFlags = 0;
    std::uint32_t timerClockIdentity = 0;
    bool zeroUnmodeledStateConfirmed = false;
};

enum class ShotBirthContinuation { NeedsMotion, NeedsAnimationAllocation, PoolFull };

struct RuntimeCallbackRequest {
    std::uint32_t recordOffset = 0;
    std::uint32_t tableIdentity = 0;
    std::uint32_t fileId = 0;
};

struct ShotAnimationRequest {
    std::uint32_t allocationSite = 0x43efd4;
    std::uint32_t allocationTarget = 0x468010;
    std::uint32_t bindSite = 0x43f011;
    std::uint32_t bindTarget = 0x4593b0;
    std::uint32_t registrationSite = 0x43f01c;
    std::uint32_t registrationTarget = 0x466d60;
    std::uint32_t playerResourceOffset = 0x10;
    std::uint32_t preAllocationCounterOffset = 0x130;
    std::uint32_t nativeDebugFlagsIdentity = 0x4d33f8;
    std::uint32_t nativeLockFlagMask = 0x8000;
    std::int32_t layer = 23;
    std::int32_t script = 0;
};

struct ShotMotionRequest {
    float angle = 0;
    float speed = 0;
    std::array<std::uint32_t, 3> targets{0x46a700, 0x46a700, 0x46ae00};
    std::uint32_t clockIdentity = 0x4b6f58;
    float clock = 1;
};

struct FirstShotBirthPlan {
    ShotBirthContinuation continuation = ShotBirthContinuation::NeedsMotion;
    std::uint32_t slot = 0;
    std::uint32_t examined = 0;
    std::uint32_t sourceRecordOffset = 0;
    ShtRecord record;
    ShotBirthInput input;
    std::array<std::uint8_t, ShotSlotBytes> slotImage{};
    ShotMotionRequest motion;
    ShotAnimationRequest animation;
    std::array<RuntimeCallbackRequest, 4> requiredCallbacks;
    std::int32_t sound = -1;
    std::weak_ptr<const std::uint8_t> resourceGeneration;
};

struct ShotMotionResult {
    ShotPosition velocity;
    float normalizedAngle = 0;
};

enum class ShotGroupContinuation { NeedsRuntimeInputs, Ready };

struct ShotGroupPlan {
    ShotGroupContinuation continuation = ShotGroupContinuation::NeedsRuntimeInputs;
    std::optional<std::uint32_t> group;
    std::uint32_t groupFileOffset = 0;
    std::uint32_t unknownGroupWord = 0;
    std::uint32_t requiredAreaIdentity = 0x4b4d60;
    std::uint32_t requiredDifficultyIdentity = 0x4b4d0c;
    std::uint32_t requiredPlayerShtOffset = 0xf7c;
};

std::optional<std::uint32_t> selectShotGroup(float cumulativeFrozenArea, std::int32_t difficulty) noexcept;
std::optional<ShotGroupPlan> planShotGroup(
    const ShtBank& bank, std::optional<float> actualCumulativeArea,
    std::optional<std::int32_t> actualDifficulty) noexcept;
std::optional<bool> shotRecordDue(const ShtRecord& record, std::int32_t shotCount) noexcept;
std::optional<FirstShotBirthPlan> planFirstShotBirth(
    const ShtBank& bank, std::uint32_t group, std::uint32_t record,
    std::span<const std::int32_t> occupancy, const ShotBirthInput& input) noexcept;
std::optional<FirstShotBirthPlan> planRecycledShotBirth(
    const ShtBank& bank, std::uint32_t group, std::uint32_t record,
    std::span<const std::int32_t> occupancy, const ShotBirthInput& input,
    std::span<const std::uint8_t> actualInactiveSlot) noexcept;
std::optional<FirstShotBirthPlan> completeFirstShotMotion(
    const ShtBank& bank, const FirstShotBirthPlan& pending, const ShotMotionResult& genuineMotion) noexcept;

}
