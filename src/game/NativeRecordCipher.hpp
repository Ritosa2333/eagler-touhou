#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

namespace th128::record {

class NativeRecordCipher {
public:
    static std::vector<std::uint8_t> apply(std::span<const std::uint8_t> input,
                                           std::size_t blockSize,
                                           std::uint8_t initialMask,
                                           std::uint8_t increment,
                                           bool encrypt) {
        if (input.size() > 2097152
            || (blockSize != 16 && blockSize != 128 && blockSize != 2048))
            throw std::runtime_error("cipher input is outside the owned native domain");
        std::vector<std::uint8_t> output(input.begin(), input.end());
        const auto tail = input.size() % blockSize;
        const auto excluded = (tail < blockSize / 4 ? tail : 0) + (input.size() & 1);
        const auto count = excluded < input.size() ? input.size() - excluded : 0;
        auto mask = initialMask;
        for (std::size_t base = 0; base < count; base += blockSize) {
            const auto length = std::min(blockSize, count - base);
            std::size_t sequence = 0;
            for (std::size_t parity = 0; parity < 2; ++parity) {
                auto position = static_cast<std::ptrdiff_t>(length) - 1
                    - static_cast<std::ptrdiff_t>(parity);
                for (; position >= 0; position -= 2) {
                    const auto reversed = base + static_cast<std::size_t>(position);
                    const auto forward = base + sequence++;
                    if (encrypt) output[forward] = input[reversed] ^ mask;
                    else output[reversed] = input[forward] ^ mask;
                    mask = static_cast<std::uint8_t>(mask + increment);
                }
            }
        }
        return output;
    }

    static std::vector<std::uint8_t> score(std::span<const std::uint8_t> input,
                                           bool encrypt) {
        return apply(input, 16, 0xac, 0x35, encrypt);
    }

    static std::vector<std::uint8_t> replay(std::span<const std::uint8_t> input,
                                            bool encrypt) {
        if (encrypt) {
            const auto first = apply(input, 128, 0x7d, 0x36, true);
            return apply(first, 2048, 0x5e, 0xe7, true);
        }
        const auto first = apply(input, 2048, 0x5e, 0xe7, false);
        return apply(first, 128, 0x7d, 0x36, false);
    }
};

}
