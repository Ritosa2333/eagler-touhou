#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace thprac::portable {

struct BytePatch {
    std::size_t offset;
    std::vector<std::uint8_t> expected;
    std::vector<std::uint8_t> replacement;
};

struct PatchSet {
    std::string id;
    // Zero disables the whole-file check. Production patch sets should set it.
    std::uint32_t expectedCrc32 = 0;
    std::vector<BytePatch> writes;
};

enum class PatchError {
    None,
    NullBuffer,
    FileMismatch,
    InvalidPatch,
    OutOfBounds,
    ExpectedBytesMismatch,
};

struct PatchResult {
    PatchError error = PatchError::None;
    std::size_t writeIndex = 0;
    std::size_t offset = 0;

    explicit operator bool() const { return error == PatchError::None; }
};

std::uint32_t Crc32(const std::uint8_t* data, std::size_t size);

// Applies a patch transactionally: all bounds, file identity and expected-byte
// checks finish before the first byte is modified.
PatchResult ApplyPatchSet(
    std::uint8_t* data,
    std::size_t size,
    const PatchSet& patchSet);

} // namespace thprac::portable
