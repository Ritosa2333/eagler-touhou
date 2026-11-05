#include "pbg4/Lzss.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

int main()
{
    // Every archive entry / Replay starts with its own zeroed dictionary.
    for (int length : {1, 3, 17, 128, 8191, 8193, 32768})
    {
        std::vector<u8> input(length);
        for (int i = 0; i < length; ++i) input[i] = u8((i * 17 + i / 31) & 255);
        i32 size = 0;
        u8 *compressed = Lzss::Compress(input.data(), length, &size);
        assert(compressed);
        u8 *decoded = Lzss::Decompress(compressed, size, nullptr, length);
        assert(decoded && std::memcmp(decoded, input.data(), length) == 0);
        std::free(decoded);
        std::free(compressed);
    }
    std::array<u8, 32> input{};
    input.fill('X');
    i32 size = 0;
    u8 *compressed = Lzss::Compress(input.data(), input.size(), &size);
    std::array<u8, 64> output{};
    output.fill(0xa5);
    assert(Lzss::Decompress(compressed, size, output.data(), 16) == nullptr);
    assert(std::all_of(output.begin() + 16, output.end(), [](u8 value) { return value == 0xa5; }));
    assert(Lzss::Decompress(compressed, size, nullptr, 16) == nullptr);
    assert(Lzss::Decompress(compressed, size, output.data(), input.size()) == output.data());
    std::free(compressed);
    // Reference dictionary position 1 for 3 bytes, then a zero end marker.
    // It must decode to zeroes even after the previous entry contained X's.
    const u8 zeroReference[] = {0, 4, 0, 0};
    std::array<u8, 3> zeros{1, 1, 1};
    assert(Lzss::Decompress(const_cast<u8 *>(zeroReference), sizeof(zeroReference), zeros.data(), zeros.size()) == zeros.data());
    assert((zeros == std::array<u8, 3>{0, 0, 0}));
    u8 truncated[] = {0};
    assert(Lzss::Decompress(truncated, sizeof(truncated), output.data(), 16) == nullptr);
    assert(Lzss::Decompress(truncated, -1, output.data(), 16) == nullptr);
    std::cout << "TH07 LZSS: PASS (round trips, output bounds, truncation, independent dictionaries)\n";
}
