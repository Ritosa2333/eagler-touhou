#pragma once
#include "NativeRecordCipher.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace th128::record {

class NativeScoreContainer {
    static void store32(std::vector<std::uint8_t>& output, std::size_t offset,
                        std::uint32_t value) {
        for (unsigned byte = 0; byte < 4; ++byte)
            output[offset + byte] = static_cast<std::uint8_t>(value >> (byte * 8));
    }

public:
    static std::uint32_t checksum(std::span<const std::uint8_t> block) {
        if (block.size() < 8 || block.size() > 1048576)
            throw std::runtime_error("checksum block is outside the owned bounded domain");
        std::uint32_t total = 0;
        for (std::size_t index = 8; index < block.size(); ++index) total += block[index];
        return total;
    }

    static std::vector<std::uint8_t> wrap(std::span<const std::uint8_t> compressed,
                                          std::uint32_t uncompressedSize) {
        if (compressed.empty() || compressed.size() > 2097152
            || !uncompressedSize || uncompressedSize > 1048576)
            throw std::runtime_error("container sizes are outside the owned bounded domain");
        const auto encrypted = NativeRecordCipher::score(compressed, true);
        std::vector<std::uint8_t> output(24, 0);
        output[0] = 'T';
        output[1] = '8';
        output[2] = '2';
        output[3] = '1';
        store32(output, 4, static_cast<std::uint32_t>(compressed.size() + 24));
        store32(output, 12, 0x100);
        store32(output, 16, static_cast<std::uint32_t>(compressed.size()));
        store32(output, 20, uncompressedSize);
        output.insert(output.end(), encrypted.begin(), encrypted.end());
        return output;
    }
};

}
