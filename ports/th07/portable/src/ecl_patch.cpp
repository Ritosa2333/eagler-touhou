#include "thprac/portable/ecl_patch.hpp"

#include <algorithm>

namespace thprac::portable {

std::uint32_t Crc32(const std::uint8_t* data, std::size_t size)
{
    std::uint32_t crc = 0xffffffffu;
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            const std::uint32_t mask = 0u - (crc & 1u);
            crc = (crc >> 1u) ^ (0xedb88320u & mask);
        }
    }
    return ~crc;
}

PatchResult ApplyPatchSet(
    std::uint8_t* data,
    std::size_t size,
    const PatchSet& patchSet)
{
    if (data == nullptr && size != 0) {
        return {PatchError::NullBuffer, 0, 0};
    }
    if (patchSet.expectedCrc32 != 0 && Crc32(data, size) != patchSet.expectedCrc32) {
        return {PatchError::FileMismatch, 0, 0};
    }

    for (std::size_t i = 0; i < patchSet.writes.size(); ++i) {
        const auto& write = patchSet.writes[i];
        if (write.expected.empty() || write.expected.size() != write.replacement.size()) {
            return {PatchError::InvalidPatch, i, write.offset};
        }
        if (write.offset > size || write.expected.size() > size - write.offset) {
            return {PatchError::OutOfBounds, i, write.offset};
        }
        if (!std::equal(write.expected.begin(), write.expected.end(), data + write.offset)) {
            return {PatchError::ExpectedBytesMismatch, i, write.offset};
        }
    }

    for (const auto& write : patchSet.writes) {
        std::copy(write.replacement.begin(), write.replacement.end(), data + write.offset);
    }
    return {};
}

} // namespace thprac::portable
