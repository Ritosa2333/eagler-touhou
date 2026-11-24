#include "game/PlayerShotTickRules.hpp"
#include <bit>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

using namespace th128::playershot;

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

void store(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t value) {
    for (std::uint32_t index = 0; index < 4; ++index)
        bytes[offset + index] = std::uint8_t(value >> (index * 8));
}

float readFloat() {
    std::uint32_t bits = 0;
    std::cin >> bits;
    return std::bit_cast<float>(bits);
}

std::array<std::uint8_t, TickSlotBytes> readSlot() {
    std::string encoded;
    std::cin >> encoded;
    check(encoded.size() == TickSlotBytes * 2, "exact native shot image required");
    auto digit = [](char character) {
        if (character >= '0' && character <= '9')
            return character - '0';
        check(character >= 'a' && character <= 'f', "invalid native shot image hexadecimal");
        return character - 'a' + 10;
    };
    std::array<std::uint8_t, TickSlotBytes> result{};
    for (std::size_t offset = 0; offset < result.size(); ++offset)
        result[offset] = std::uint8_t(digit(encoded[offset * 2]) * 16 + digit(encoded[offset * 2 + 1]));
    return result;
}

void printSlot(const std::array<std::uint8_t, TickSlotBytes>& slot) {
    constexpr char digits[] = "0123456789abcdef";
    for (const auto value : slot)
        std::cout << digits[value >> 4] << digits[value & 15];
    std::cout << '\n';
}

std::uint32_t identity(const std::array<std::uint8_t, TickSlotBytes>& slot) {
    return std::uint32_t(slot[0x74]) | std::uint32_t(slot[0x75]) << 8 |
           std::uint32_t(slot[0x76]) << 16 | std::uint32_t(slot[0x77]) << 24;
}

int main() {
    std::array<std::uint8_t, TickSlotBytes> slot{};
    store(slot, 0x48, 1);
    store(slot, 0x74, 123);
    store(slot, 0xc, TickClockIdentity);
    store(slot, 0x10, 1);
    const ShotTickRecordBinding zero{123, 0, 0};
    check(!prepareOrdinaryShotTick(std::span<const std::uint8_t>{}, zero), "short native record must reject");
    check(!prepareOrdinaryShotTick(slot, zero, .5f), "unit clock domain is explicit");
    check(!prepareOrdinaryShotTick(slot, ShotTickRecordBinding{124, 0, 0}), "record identity mismatch must reject");
    check(!prepareOrdinaryShotTick(slot, ShotTickRecordBinding{123, 2, 0}), "kind2 must not become ordinary kind0");
    const auto unresolved = prepareOrdinaryShotTick(slot, std::nullopt);
    check(unresolved && unresolved->continuation == ShotTickContinuation::NeedsRecordBinding && unresolved->next == slot,
          "missing runtime binding must remain mandatory work without mutation");
    const auto pending = prepareOrdinaryShotTick(slot, zero);
    check(pending && pending->continuation == ShotTickContinuation::NeedsMotion, "proved callback zero permits motion request");
    check(pending->required.target == 0x46c1f0 && pending->required.callSite == 0x43f7d4, "real native motion request");
    check(!completeOrdinaryShotMotion(*pending, {0, 0, -0.f}), "native velocity Z must be positive zero");
    check(!completeOrdinaryShotMotion(*pending, {std::numeric_limits<float>::quiet_NaN(), 0, 0}), "nonfinite motion must reject");
    check(!finishShotTickAge(*pending), "age cannot advance before animation work");
    ShotTickPlan secondaryPending;
    secondaryPending.continuation = ShotTickContinuation::NeedsAnimationSync;
    secondaryPending.required.secondaryHandle = 456;
    check(!completeShotAnimationSync(secondaryPending), "unknown secondary lookup result must remain required work");
    auto risky = *pending;
    store(risky.next, 0x14, std::bit_cast<std::uint32_t>(1.f));
    check(bool(completeOrdinaryShotMotion(risky, {0x1p-40f, 0, 0})), "original returned integrator accepts tiny finite lateral motion");
    auto crossing = *pending;
    store(crossing.next, 0x14, std::bit_cast<std::uint32_t>(.125f));
    check(bool(completeOrdinaryShotMotion(crossing, {-.125f, 0, 0})), "actual zero crossing remains supported");
    ShotAxisAlignedAnimationInput axis;
    check(!axisAlignedShotAnimationBounds(axis), "no-parent and zero-offset proof is mandatory");
    axis.noParentAndZeroOffsetsConfirmed = true;
    axis.flags = 1u << 24;
    check(!axisAlignedShotAnimationBounds(axis), "rotated render mode cannot use axis-aligned substitute");
    axis.flags = 0;
    axis.secondaryScale[0] = 2;
    check(!axisAlignedShotAnimationBounds(axis), "unproved secondary scale cannot be omitted");
    ShotRotatedAnimationInput rotated;
    check(!rotatedShotAnimationBounds(rotated), "mode1 basis and ownership proof are required");
    rotated.animation.noParentAndZeroOffsetsConfirmed = true;
    rotated.animation.flags = 1u << 24;
    rotated.rotationBasisConfirmed = true;
    rotated.animation.spriteSize = {32, 64};
    check(bool(rotatedShotAnimationBounds(rotated)), "ordinary zero-rotation mode1 is supported");
    rotated.cosineSine = {1, 1};
    check(!rotatedShotAnimationBounds(rotated), "invalid rotation basis cannot become geometry");
    rotated.cosineSine = {1, 0};
    rotated.animation.flags = 2u << 24;
    check(!rotatedShotAnimationBounds(rotated), "mode2 is not a mode1 fallback");
    rotated.animation.flags = 1u << 24;
    rotated.animation.secondaryScale[0] = 2;
    check(!rotatedShotAnimationBounds(rotated), "unknown parent or secondary scale cannot be ignored");
    char operation = 0;
    while (std::cin >> operation) {
        if (operation == 'Y') {
            ShotRotatedAnimationInput input;
            input.animation.position = {readFloat(), readFloat(), readFloat()};
            input.animation.spriteSize = {readFloat(), readFloat()};
            input.animation.scale = {readFloat(), readFloat()};
            std::cin >> input.animation.flags;
            input.cosineSine = {readFloat(), readFloat()};
            input.animation.noParentAndZeroOffsetsConfirmed = true;
            input.rotationBasisConfirmed = true;
            const auto bounds = rotatedShotAnimationBounds(input);
            check(bool(bounds), "actual original mode1 bounds rejected");
            for (std::size_t index = 0; index < bounds->corners.size(); ++index)
                std::cout << (index ? " " : "") << std::bit_cast<std::uint32_t>(bounds->corners[index][0])
                          << ' ' << std::bit_cast<std::uint32_t>(bounds->corners[index][1]);
            std::cout << '\n';
            continue;
        }
        if (operation == 'X') {
            axis = {};
            axis.position = {readFloat(), readFloat(), readFloat()};
            axis.spriteSize = {readFloat(), readFloat()};
            axis.scale = {readFloat(), readFloat()};
            std::cin >> axis.flags;
            axis.noParentAndZeroOffsetsConfirmed = true;
            const auto bounds = axisAlignedShotAnimationBounds(axis);
            check(bool(bounds), "actual original axis-aligned bounds rejected");
            for (std::size_t index = 0; index < bounds->corners.size(); ++index)
                std::cout << (index ? " " : "") << std::bit_cast<std::uint32_t>(bounds->corners[index][0])
                          << ' ' << std::bit_cast<std::uint32_t>(bounds->corners[index][1]);
            std::cout << '\n';
            continue;
        }
        if (operation == 'B') {
            std::int32_t age = 0;
            std::cin >> age;
            ShotAnimationBounds bounds;
            for (auto& point : bounds.corners)
                point = {readFloat(), readFloat()};
            ShotTickPlan plan;
            plan.continuation = ShotTickContinuation::NeedsAnimationBounds;
            store(plan.next, 4, std::uint32_t(age));
            const auto result = completeShotAnimationBounds(plan, bounds);
            check(bool(result), "original bounds predicate rejected");
            std::cout << (result->continuation == ShotTickContinuation::NeedsAnimationSync) << '\n';
            continue;
        }
        slot = readSlot();
        if (operation == 'S') {
            ShotTickPlan plan;
            plan.next = slot;
            plan.continuation = ShotTickContinuation::NeedsAnimationBounds;
            const ShotAnimationBounds bounds{{{{200, 200}, {220, 200}, {200, 220}, {220, 220}}}};
            const auto result = completeShotAnimationBounds(plan, bounds);
            check(result && result->continuation == ShotTickContinuation::NeedsAnimationSync,
                  "native sync prefix requires a surviving shot");
            const auto& position = result->required.animationPosition;
            std::cout << std::bit_cast<std::uint32_t>(position.x) << ' '
                      << std::bit_cast<std::uint32_t>(position.y) << ' '
                      << std::bit_cast<std::uint32_t>(position.z) << '\n';
            continue;
        }
        if (operation == 'A') {
            ShotTickPlan plan;
            plan.next = slot;
            plan.continuation = ShotTickContinuation::ReadyForAgeTail;
            const auto result = finishShotTickAge(plan);
            check(bool(result), "original age tail rejected");
            printSlot(result->next);
            continue;
        }
        if (operation == 'P') {
            std::uint32_t target = 0;
            std::cin >> target;
            const auto plan = prepareOrdinaryShotTick(slot, ShotTickRecordBinding{identity(slot), 0, target});
            check(bool(plan), "original pre-call shot state rejected");
            std::cout << plan->required.callSite << ' ' << plan->required.target << ' ';
            printSlot(plan->next);
            continue;
        }
        const TickPosition motion{readFloat(), readFloat(), readFloat()};
        if (operation == 'I') {
            ShotTickPlan plan;
            plan.next = slot;
            plan.continuation = ShotTickContinuation::NeedsMotion;
            const auto result = completeOrdinaryShotMotion(plan, motion);
            check(bool(result), "native integrator state rejected");
            printSlot(result->next);
            continue;
        }
        check(operation == 'M' || operation == 'R', "unknown native tick operation");
        auto result = prepareOrdinaryShotTick(slot, ShotTickRecordBinding{identity(slot), 0, 0});
        check(bool(result), "native ordinary tick input rejected");
        if (result->continuation != ShotTickContinuation::Inactive) {
            result = completeOrdinaryShotMotion(*result, motion);
            check(bool(result), "native recomputed velocity rejected");
            if (operation == 'R') {
                result = completeShotAnimationLookup(*result, false);
                check(bool(result), "genuine native animation miss rejected");
                result = completeShotAnimationCleanup(*result);
                check(bool(result), "real completed cleanup rejected");
            }
        }
        printSlot(result->next);
    }
    check(std::cin.eof(), "truncated native tick golden input");
}
