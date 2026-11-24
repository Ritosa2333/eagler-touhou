#pragma once

#include <cstdint>
#include <optional>
#include <span>

namespace th128::ice {

inline constexpr std::uint32_t IceMaskBytes = 21504;
inline constexpr std::uint32_t IceMaskHeight = 448;
inline constexpr std::uint32_t IceMaskWordsPerRow = 12;
inline constexpr std::uint32_t MaxIceBitmapWords = 2048;

struct IceMaskPosition {
    float x = 0, y = 0, z = 0;
};

struct IceBitmapView {
    std::span<const std::uint8_t> bytes;
    std::uint32_t height = 0;
    std::uint32_t wordsPerRow = 0;
};

enum class IceMaskOperation { Merge, Subtract };

struct IceMaskUpdate {
    std::int32_t startRow = 0;
    std::int32_t startWord = 0;
    std::int32_t signedRemainder = 0;
    std::uint32_t wordWrites = 0;
    std::uint32_t changedBits = 0;
};

std::optional<std::uint32_t> countIceMask(std::span<const std::uint8_t> mask) noexcept;
std::optional<IceMaskUpdate> updateIceMask(
    std::span<std::uint8_t> mask, const IceBitmapView& bitmap,
    const IceMaskPosition& position, IceMaskOperation operation) noexcept;

enum class IceMaskDisposition { Absent, Known, Unsupported };

struct IceMaskSelection {
    IceMaskDisposition disposition = IceMaskDisposition::Unsupported;
    std::uint32_t selector = 0;
    std::uint32_t height = 0;
    std::uint32_t wordsPerRow = 0;
};

IceMaskSelection selectIceMask(std::uint32_t selector) noexcept;

enum class IceMaskAttachmentContinuation { NeedsMergeThenEffect };

struct IceMaskAttachmentPlan {
    IceMaskSelection profile;
    IceMaskPosition position;
    IceMaskAttachmentContinuation continuation = IceMaskAttachmentContinuation::NeedsMergeThenEffect;
    std::uint32_t effectCallSite = 0;
    std::uint32_t effectTarget = 0x407ec0;
    std::int32_t layer = 23;
    std::int32_t script = 0;
    std::uint32_t nativeResourceSlot = 36;
    std::uint32_t resourceManagerOffset = 0x65ddc;
    std::uint32_t selectorStoreOffset = 0x58;
    std::uint32_t handleStoreOffset = 0x54;
};

std::optional<IceMaskAttachmentPlan> planIceMaskAttachment(
    std::uint32_t selector, const IceMaskPosition& position) noexcept;

}
