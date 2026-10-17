#include "archive.hpp"
#include <iostream>
#include <stdexcept>
#include <string>

using namespace th20::source;
namespace {
void expect(bool condition, const char* message) { if (!condition) throw std::runtime_error(message); }
template<class F> void rejected(F&& operation, const char* message) {
    try { operation(); } catch (const std::exception&) { return; }
    throw std::runtime_error(message);
}
struct BitWriter {
    Bytes data;
    unsigned position = 0;
    void put(std::uint32_t value, unsigned count) {
        while (count--) {
            if (position % 8 == 0) data.push_back(0);
            data.back() |= static_cast<std::uint8_t>(((value >> count) & 1) << (7 - position % 8));
            ++position;
        }
    }
    void literal(std::uint8_t value) { put(1, 1); put(value, 8); }
    void reference(std::uint32_t distance, std::uint32_t count) {
        put(0, 1); put(distance, 13); put(count - 3, 4);
    }
    Bytes end() { put(0, 1); put(0, 13); return data; }
};
}
int main() {
    try {
        LzssDecoder decoder;
        BitWriter overlapping;
        overlapping.literal('A'); overlapping.literal('B'); overlapping.reference(1, 6);
        auto result = decoder.decode(overlapping.end(), 8);
        expect(std::string(result.begin(), result.end()) == "ABABABAB", "Overlapping dictionary copy failed");
        BitWriter retained;
        retained.reference(1, 8);
        const auto stream = retained.end();
        result = decoder.decode(stream, 8);
        expect(std::string(result.begin(), result.end()) == "ABABABAB", "Dictionary was incorrectly cleared between streams");
        decoder.reset();
        expect(decoder.decode(stream, 8) == Bytes(8, 0), "Explicit dictionary reset failed");
        rejected([&] { decoder.decode(Bytes{}, 1); }, "Truncated stream was accepted");
        rejected([&] { decoder.decode(stream, 2); }, "Oversized expansion was accepted");
        expect(decoder.decode(Bytes{}, 0).empty(), "Empty zero-padded end marker failed");
        BitWriter ring;
        Bytes expected;
        for (unsigned i = 0; i < 8200; ++i) {
            const auto value = static_cast<std::uint8_t>(i);
            ring.literal(value); expected.push_back(value);
        }
        ring.reference(8191, 6);
        // At the end of the 8200 literals, positions 8191,0,1,2,3,4 contain
        // literals 8190,8191,8192,8193,8194,8195, respectively.
        for (unsigned i = 8190; i < 8196; ++i) expected.push_back(static_cast<std::uint8_t>(i));
        expect(decoder.decode(ring.end(), static_cast<std::uint32_t>(expected.size())) == expected,
               "Dictionary wraparound failed");
        Bytes crypt{0,1,2,3,4,5,6,7};
        decrypt(crypt, {0, 1, 8, 8});
        expect(crypt == Bytes(8, 0), "Permutation and key sequence failed");
        Bytes suffix{1,2,3,4,5,6,7,8,0xAA};
        decrypt(suffix, {0, 0, 8, 8});
        // The original subtracts BOTH the odd byte and the short remainder:
        // nine bytes with block eight process seven bytes, leaving two intact.
        expect(suffix == Bytes({4,7,3,6,2,5,1,8,0xAA}), "Odd-byte plus short-remainder rule differs");
        Bytes short_block{1,2,3};
        decrypt(short_block, {0xFF, 9, 16, 16});
        expect(short_block == Bytes({1,2,3}), "Short unencrypted tail was changed");
        rejected([&] { decrypt(crypt, {0,0,0,0}); }, "Zero crypt block was accepted");
        rejected([&] { Archive malformed(Bytes(16,0)); }, "Bad archive magic accepted");
        rejected([&] { Archive malformed(Bytes{}); }, "Truncated archive accepted");
        std::cout << "Archive source tests passed: crypt boundaries, LZSS overlap/retention/wrap, malformed inputs\n";
        return 0;
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
