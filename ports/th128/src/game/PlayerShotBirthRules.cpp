#include "PlayerShotBirthRules.hpp"
#include <algorithm>
#include <bit>
#include <cmath>

namespace th128::playershot {

namespace {

std::uint16_t word16(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::uint16_t(bytes[offset]) | std::uint16_t(bytes[offset + 1]) << 8;
}

std::uint32_t word32(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::uint32_t(bytes[offset]) | std::uint32_t(bytes[offset + 1]) << 8 |
           std::uint32_t(bytes[offset + 2]) << 16 | std::uint32_t(bytes[offset + 3]) << 24;
}

float floating(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::bit_cast<float>(word32(bytes, offset));
}

void store(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t value) noexcept {
    for (std::size_t index = 0; index < 4; ++index)
        bytes[offset + index] = std::uint8_t(value >> (8 * index));
}

void storeFloat(std::span<std::uint8_t> bytes, std::size_t offset, float value) noexcept {
    store(bytes, offset, std::bit_cast<std::uint32_t>(value));
}

bool bounded(float value) noexcept {
    return std::isfinite(value) && std::abs(value) <= 1048576.f;
}

bool motionTerm(float value) noexcept {
    const auto magnitude = std::abs(value);
    return std::isfinite(value) && magnitude <= 4096.f && (value == 0 || magnitude >= 0x1p-24f);
}

float exactThreeTerms(float first, float second, float third) noexcept {
    const auto firstInteger = static_cast<std::int64_t>(double(first) * 0x1p48);
    const auto secondInteger = static_cast<std::int64_t>(double(second) * 0x1p48);
    const auto thirdInteger = static_cast<std::int64_t>(double(third) * 0x1p48);
    const auto sum = firstInteger + secondInteger + thirdInteger;
    return sum == 0 ? float((double(first) + double(second)) + double(third)) : float(sum) * 0x1p-48f;
}

}

bool ShtBank::load(std::span<const std::uint8_t> source, std::string& error) {
    error.clear();
    auto fail = [&](const char* reason) { error = reason; return false; };
    if (source.size() < 0x268 || source.size() > 4u * 1024u * 1024u)
        return fail("SHT byte size outside bounded resource domain");
    const std::uint32_t count = word16(source, 2);
    if (word16(source, 0) != 3 || count == 0 || count > 256)
        return fail("SHT requires actual tag3 layout and bounded groups");
    const std::size_t tableEnd = 0x268 + count * 8;
    if (tableEnd > source.size())
        return fail("SHT truncated group table");
    std::vector<std::uint32_t> offsets;
    offsets.reserve(count);
    for (std::uint32_t index = 0; index < count; ++index) {
        const auto offset = word32(source, 0x268 + index * 8);
        if (offset < tableEnd || offset >= source.size())
            return fail("SHT group points outside record area");
        offsets.push_back(offset);
    }
    std::sort(offsets.begin(), offsets.end());
    offsets.erase(std::unique(offsets.begin(), offsets.end()), offsets.end());
    std::vector<ShtGroup> groups;
    groups.reserve(count);
    std::uint32_t records = 0;
    for (std::uint32_t index = 0; index < count; ++index) {
        ShtGroup group;
        group.fileOffset = word32(source, 0x268 + index * 8);
        group.unknownWord = word32(source, 0x26c + index * 8);
        const auto successor = std::upper_bound(offsets.begin(), offsets.end(), group.fileOffset);
        const std::size_t limit = successor == offsets.end() ? source.size() : *successor;
        std::size_t cursor = group.fileOffset;
        for (;;) {
            if (cursor >= limit)
                return fail("SHT group has no bounded negative-byte terminator");
            const auto period = std::bit_cast<std::int8_t>(source[cursor]);
            if (period < 0)
                break;
            if (period == 0 || limit - cursor < ShtRecordBytes || records >= 65536)
                return fail("SHT zero period, overlapping/truncated record or record budget");
            ShtRecord record;
            record.fileOffset = static_cast<std::uint32_t>(cursor);
            std::copy_n(source.begin() + cursor, ShtRecordBytes, record.raw.begin());
            group.records.push_back(record);
            ++records;
            cursor += ShtRecordBytes;
        }
        groups.push_back(std::move(group));
    }
    std::vector<std::uint8_t> copied(source.begin(), source.end());
    auto generation = std::make_shared<const std::uint8_t>(0);
    bytes_ = std::move(copied);
    groups_ = std::move(groups);
    recordCount_ = records;
    generation_ = std::move(generation);
    return true;
}

std::span<const std::uint8_t> ShtBank::bytes() const { return bytes_; }

const ShtGroup* ShtBank::group(std::uint32_t index) const {
    return index < groups_.size() ? &groups_[index] : nullptr;
}

std::uint32_t ShtBank::groupCount() const { return static_cast<std::uint32_t>(groups_.size()); }
std::uint32_t ShtBank::recordCount() const { return recordCount_; }

bool ShtBank::owns(const FirstShotBirthPlan& plan) const {
    return generation_ && plan.resourceGeneration.lock() == generation_;
}

std::optional<std::uint32_t> selectShotGroup(float cumulativeFrozenArea, std::int32_t difficulty) noexcept {
    if (!bounded(cumulativeFrozenArea) || cumulativeFrozenArea < 0 || difficulty < 0 || difficulty > 4)
        return std::nullopt;
    constexpr std::array<std::array<std::int32_t, 15>, 2> thresholds{{
        {0, 0, 50, 100, 150, 200, 300, 450, 600, 800, 1000, 1200, 1400, 1600, 2000},
        {50, 100, 200, 350, 500, 700, 900, 1100, 1300, 1500, 1700, 2000, 2300, 2600, 3000}}};
    const auto& selected = thresholds[difficulty == 0 ? 0 : 1];
    for (std::uint32_t index = 0; index < selected.size(); ++index)
        if (double(selected[index]) > double(cumulativeFrozenArea))
            return index;
    return 15;
}

std::optional<bool> shotRecordDue(const ShtRecord& record, std::int32_t shotCount) noexcept {
    const auto period = std::bit_cast<std::int8_t>(record.raw[0]);
    const auto phase = std::bit_cast<std::int8_t>(record.raw[1]);
    if (period <= 0 || shotCount < 0 || shotCount > 14)
        return std::nullopt;
    return shotCount % period == phase;
}

std::optional<ShotGroupPlan> planShotGroup(
    const ShtBank& bank, std::optional<float> actualCumulativeArea,
    std::optional<std::int32_t> actualDifficulty) noexcept {
    if (bank.groupCount() == 0)
        return std::nullopt;
    ShotGroupPlan result;
    if (!actualCumulativeArea || !actualDifficulty)
        return result;
    const auto selected = selectShotGroup(*actualCumulativeArea, *actualDifficulty);
    if (!selected || !bank.group(*selected))
        return std::nullopt;
    result.continuation = ShotGroupContinuation::Ready;
    result.group = selected;
    result.groupFileOffset = bank.group(*selected)->fileOffset;
    result.unknownGroupWord = bank.group(*selected)->unknownWord;
    return result;
}

std::optional<FirstShotBirthPlan> planFirstShotBirth(
    const ShtBank& bank, std::uint32_t groupIndex, std::uint32_t recordIndex,
    std::span<const std::int32_t> occupancy, const ShotBirthInput& input) noexcept {
    const auto* group = bank.group(groupIndex);
    if (!group || recordIndex >= group->records.size() || occupancy.size() != ShotSlotCount ||
        input.shotCount < 0 || input.shotCount > 14 || !input.zeroUnmodeledStateConfirmed ||
        !bounded(input.position.x) || !bounded(input.position.y) || !bounded(input.position.z))
        return std::nullopt;
    const auto& record = group->records[recordIndex];
    if (record.raw[0x1c] != 0 || record.raw[0x1d] != 0)
        return std::nullopt;
    for (std::size_t offset = 4; offset <= 0x18; offset += 4)
        if (!bounded(floating(record.raw, offset)))
            return std::nullopt;
    const auto angle = floating(record.raw, 0x14);
    if (std::abs(angle) > 4.f)
        return std::nullopt;
    FirstShotBirthPlan result;
    result.input = input;
    result.record = record;
    result.sourceRecordOffset = record.fileOffset;
    result.resourceGeneration = bank.generation();
    const auto free = std::find(occupancy.begin(), occupancy.end(), 0);
    if (free == occupancy.end()) {
        result.continuation = ShotBirthContinuation::PoolFull;
        result.slot = ShotSlotCount;
        result.examined = ShotSlotCount;
        return result;
    }
    result.slot = static_cast<std::uint32_t>(free - occupancy.begin());
    result.examined = result.slot + 1;
    store(result.slotImage, 0, 0xffffffffu);
    store(result.slotImage, 0xc, input.timerFlags & 1u ? input.timerClockIdentity : 0x4b6f58);
    store(result.slotImage, 0x10, input.timerFlags | 1u);
    storeFloat(result.slotImage, 0x14, input.position.x);
    storeFloat(result.slotImage, 0x18, input.position.y);
    storeFloat(result.slotImage, 0x1c, input.position.z);
    store(result.slotImage, 0x2c, word32(record.raw, 0x18));
    store(result.slotImage, 0x48, 1);
    store(result.slotImage, 0x60, std::uint32_t(std::bit_cast<std::int16_t>(word16(record.raw, 2))));
    store(result.slotImage, 0x68, word32(record.raw, 0xc));
    store(result.slotImage, 0x6c, word32(record.raw, 0x10));
    store(result.slotImage, 0x74, record.fileOffset);
    result.motion.angle = angle;
    result.motion.speed = floating(record.raw, 0x18);
    result.animation.script = std::int32_t(std::bit_cast<std::int16_t>(word16(record.raw, 0x1e))) + 5;
    result.sound = std::bit_cast<std::int16_t>(word16(record.raw, 0x22));
    constexpr std::array<std::uint32_t, 4> tables{0x4b1cd8, 0x4b1a14, 0x4d2e2c, 0x4b1cf8};
    for (std::size_t index = 0; index < tables.size(); ++index)
        result.requiredCallbacks[index] = {std::uint32_t(0x24 + index * 4), tables[index], word32(record.raw, 0x24 + index * 4)};
    return result;
}

std::optional<FirstShotBirthPlan> planRecycledShotBirth(
    const ShtBank& bank, std::uint32_t group, std::uint32_t record,
    std::span<const std::int32_t> occupancy, const ShotBirthInput& input,
    std::span<const std::uint8_t> actualInactiveSlot) noexcept {
    if (actualInactiveSlot.size() != ShotSlotBytes || word32(actualInactiveSlot, 0x48) != 0 ||
        (word32(actualInactiveSlot, 0x44) & 1u))
        return std::nullopt;
    auto derived = input;
    derived.timerFlags = word32(actualInactiveSlot, 0x10);
    derived.timerClockIdentity = word32(actualInactiveSlot, 0xc);
    derived.zeroUnmodeledStateConfirmed = true;
    auto result = planFirstShotBirth(bank, group, record, occupancy, derived);
    if (!result || result->continuation == ShotBirthContinuation::PoolFull)
        return result;
    const auto written = result->slotImage;
    std::copy(actualInactiveSlot.begin(), actualInactiveSlot.end(), result->slotImage.begin());
    for (const auto offset : {0u, 4u, 8u, 0xcu, 0x10u, 0x14u, 0x18u, 0x1cu,
                              0x2cu, 0x48u, 0x60u, 0x68u, 0x6cu, 0x74u})
        store(result->slotImage, offset, word32(written, offset));
    return result;
}

std::optional<FirstShotBirthPlan> completeFirstShotMotion(
    const ShtBank& bank, const FirstShotBirthPlan& pending, const ShotMotionResult& genuineMotion) noexcept {
    if (!bank.owns(pending) || pending.continuation != ShotBirthContinuation::NeedsMotion ||
        !bounded(genuineMotion.normalizedAngle) || std::abs(genuineMotion.normalizedAngle) > 4.f ||
        !motionTerm(genuineMotion.velocity.x) || !motionTerm(genuineMotion.velocity.y) ||
        std::bit_cast<std::uint32_t>(genuineMotion.velocity.z) != 0 ||
        !motionTerm(pending.input.position.x) || !motionTerm(pending.input.position.y) ||
        !motionTerm(floating(pending.record.raw, 4)) || !motionTerm(floating(pending.record.raw, 8)))
        return std::nullopt;
    FirstShotBirthPlan result = pending;
    result.continuation = ShotBirthContinuation::NeedsAnimationAllocation;
    storeFloat(result.slotImage, 0x20, genuineMotion.velocity.x);
    storeFloat(result.slotImage, 0x24, genuineMotion.velocity.y);
    storeFloat(result.slotImage, 0x28, genuineMotion.velocity.z);
    storeFloat(result.slotImage, 0x30, genuineMotion.normalizedAngle);
    storeFloat(result.slotImage, 0x14, exactThreeTerms(floating(pending.record.raw, 4), -genuineMotion.velocity.x, pending.input.position.x));
    storeFloat(result.slotImage, 0x18, exactThreeTerms(floating(pending.record.raw, 8), -genuineMotion.velocity.y, pending.input.position.y));
    return result;
}

}
