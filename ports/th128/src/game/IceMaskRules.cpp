#include "IceMaskRules.hpp"
#include <array>
#include <bit>
#include <cmath>
#include <functional>

namespace th128::ice {

namespace {

bool coordinate(float value) noexcept {
    const auto magnitude = std::abs(value);
    return std::isfinite(value) && magnitude <= 16777216.f;
}

double extendedDelta(float value, double anchor) noexcept {
    if (std::abs(value) >= 0x1p-32f || anchor == 0.)
        return double(value);
    int exponent = 0;
    const auto fraction = std::frexp(std::abs(anchor), &exponent);
    if (fraction == .5 && ((anchor > 0. && value < 0.f) || (anchor < 0. && value > 0.f)))
        --exponent;
    const auto quantum = std::ldexp(1., exponent - 64);
    const auto scaled = double(value) / quantum;
    const auto lower = std::floor(scaled);
    const auto remainder = scaled - lower;
    const auto ticks = lower + (remainder > .5 || (remainder == .5 && std::fmod(lower, 2.) != 0.) ? 1. : 0.);
    return ticks * quantum;
}

std::uint32_t loadWord(std::span<const std::uint8_t> bytes, std::size_t offset) noexcept {
    return std::uint32_t(bytes[offset]) | std::uint32_t(bytes[offset + 1]) << 8 |
           std::uint32_t(bytes[offset + 2]) << 16 | std::uint32_t(bytes[offset + 3]) << 24;
}

void storeWord(std::span<std::uint8_t> bytes, std::size_t offset, std::uint32_t word) noexcept {
    for (std::uint32_t index = 0; index < 4; ++index)
        bytes[offset + index] = std::uint8_t(word >> (index * 8));
}

bool overlap(std::span<const std::uint8_t> first, std::span<const std::uint8_t> second) noexcept {
    if (first.empty() || second.empty())
        return false;
    const std::less<const std::uint8_t*> less;
    return less(first.data(), second.data() + second.size()) && less(second.data(), first.data() + first.size());
}

}

std::optional<std::uint32_t> countIceMask(std::span<const std::uint8_t> mask) noexcept {
    if (mask.size() != IceMaskBytes)
        return std::nullopt;
    std::uint32_t result = 0;
    for (std::size_t offset = 0; offset < mask.size(); offset += 4)
        result += std::popcount(loadWord(mask, offset));
    return result;
}

std::optional<IceMaskUpdate> updateIceMask(
    std::span<std::uint8_t> mask, const IceBitmapView& bitmap,
    const IceMaskPosition& position, IceMaskOperation operation) noexcept {
    if (mask.size() != IceMaskBytes || bitmap.height > IceMaskHeight || bitmap.wordsPerRow > IceMaskWordsPerRow ||
        bitmap.height * bitmap.wordsPerRow > MaxIceBitmapWords ||
        bitmap.bytes.size() != std::size_t(bitmap.height) * bitmap.wordsPerRow * 4 ||
        !coordinate(position.x) || !coordinate(position.y) || overlap(mask, bitmap.bytes) ||
        (operation != IceMaskOperation::Merge && operation != IceMaskOperation::Subtract))
        return std::nullopt;
    IceMaskUpdate result;
    const double halfWidth = double(bitmap.wordsPerRow) * 16.;
    const auto left = static_cast<std::int32_t>((384. - halfWidth) + extendedDelta(position.x, 384.));
    result.signedRemainder = left % 32;
    result.startWord = static_cast<std::int32_t>(((192. - halfWidth) + extendedDelta(position.x, 192.)) * .03125);
    const auto halfHeight = double(bitmap.height) * .5;
    result.startRow = static_cast<std::int32_t>(-halfHeight + extendedDelta(position.y, -halfHeight));
    const auto shift = std::uint32_t(result.signedRemainder) & 31u;
    auto write = [&](std::int32_t row, std::int32_t column, std::uint32_t source) {
        if (row < 0 || row >= std::int32_t(IceMaskHeight) || column < 0 || column >= std::int32_t(IceMaskWordsPerRow))
            return;
        const auto offset = std::size_t(row * std::int32_t(IceMaskWordsPerRow) + column) * 4;
        const auto before = loadWord(mask, offset);
        const auto after = operation == IceMaskOperation::Merge ? before | source : before & ~source;
        storeWord(mask, offset, after);
        ++result.wordWrites;
        result.changedBits += std::popcount(before ^ after);
    };
    for (std::uint32_t row = 0; row < bitmap.height; ++row) {
        for (std::uint32_t column = 0; column < bitmap.wordsPerRow; ++column) {
            const auto source = loadWord(bitmap.bytes, std::size_t(row * bitmap.wordsPerRow + column) * 4);
            const auto destinationColumn = result.startWord + std::int32_t(column);
            write(result.startRow + std::int32_t(row), destinationColumn, source >> shift);
            if (result.signedRemainder > 0)
                write(result.startRow + std::int32_t(row), destinationColumn + 1,
                      source << (32u - std::uint32_t(result.signedRemainder)));
        }
    }
    return result;
}

IceMaskSelection selectIceMask(std::uint32_t selector) noexcept {
    if (selector == 0)
        return {IceMaskDisposition::Absent, selector, 0, 0};
    constexpr std::array<std::uint32_t, 5> heights{21, 25, 33, 49, 65};
    if (selector > heights.size())
        return {IceMaskDisposition::Unsupported, selector, 0, 0};
    return {IceMaskDisposition::Known, selector, heights[selector - 1], selector <= 3 ? 1u : 2u};
}

std::optional<IceMaskAttachmentPlan> planIceMaskAttachment(
    std::uint32_t selector, const IceMaskPosition& position) noexcept {
    const auto profile = selectIceMask(selector);
    if (profile.disposition != IceMaskDisposition::Known || !coordinate(position.x) || !coordinate(position.y) ||
        !std::isfinite(position.z) || std::abs(position.z) > 16777216.f)
        return std::nullopt;
    constexpr std::array<std::uint32_t, 5> sites{0x40eda8, 0x40edf8, 0x40ee45, 0x40ee8f, 0x40eec2};
    IceMaskAttachmentPlan result;
    result.profile = profile;
    result.position = position;
    result.effectCallSite = sites[selector - 1];
    result.script = selector <= 3 ? 1 : 0;
    return result;
}

}
