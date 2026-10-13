#include "thprac/portable/ecl_patch.hpp"

#include <array>
#include <cassert>
#include <cstdint>

using namespace thprac::portable;

int main()
{
    const std::array<std::uint8_t, 9> crcFixture = {'1','2','3','4','5','6','7','8','9'};
    assert(Crc32(crcFixture.data(), crcFixture.size()) == 0xcbf43926u);

    std::array<std::uint8_t, 8> bytes = {0x10, 0x11, 0x12, 0x13, 0x20, 0x21, 0x22, 0x23};
    const auto original = bytes;
    PatchSet valid {
        "representative",
        Crc32(bytes.data(), bytes.size()),
        {
            {1, {0x11, 0x12}, {0xa1, 0xa2}},
            {6, {0x22, 0x23}, {0xb2, 0xb3}},
        },
    };
    assert(ApplyPatchSet(bytes.data(), bytes.size(), valid));
    assert((bytes == std::array<std::uint8_t, 8>{0x10, 0xa1, 0xa2, 0x13, 0x20, 0x21, 0xb2, 0xb3}));

    bytes = original;
    PatchSet mismatch = valid;
    mismatch.writes[1].expected[0] = 0xff;
    auto result = ApplyPatchSet(bytes.data(), bytes.size(), mismatch);
    assert(result.error == PatchError::ExpectedBytesMismatch);
    assert(result.writeIndex == 1);
    assert(bytes == original); // The first valid write was not partially applied.

    PatchSet outOfBounds {"bounds", 0, {{7, {0x23, 0x00}, {0, 0}}}};
    result = ApplyPatchSet(bytes.data(), bytes.size(), outOfBounds);
    assert(result.error == PatchError::OutOfBounds);
    assert(bytes == original);

    PatchSet wrongFile = valid;
    wrongFile.expectedCrc32 ^= 1u;
    result = ApplyPatchSet(bytes.data(), bytes.size(), wrongFile);
    assert(result.error == PatchError::FileMismatch);
    assert(bytes == original);
}
