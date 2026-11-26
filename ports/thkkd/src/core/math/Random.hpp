// 保留 0x005e3440 与 0x005e3490 的顺序、32 位溢出及 x87 精度控制。
#pragma once
#include "core/math/RandomTable.hpp"
#include "core/math/BinaryFloat.hpp"
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace kaikeidou {
enum class RandomPrecision : unsigned { bits24 = 24, bits53 = 53, bits64 = 64 };


class Random {
    std::uint32_t cursor_;
    std::uint32_t next() {
        ++cursor_;
        if (cursor_ >= random_table.size()) cursor_ = 0;
        return static_cast<std::uint32_t>(random_table[cursor_]);
    }
public:
    explicit Random(std::uint32_t cursor) : cursor_(cursor) {
        if (cursor >= random_table.size()) throw std::out_of_range("Invalid random cursor");
    }
    std::uint32_t cursor() const noexcept { return cursor_; }
    std::uint32_t integer_bits(std::uint32_t maximum_bits, RandomPrecision precision, bool sse_conversion) {
        using namespace binary_float;
        const auto bits = static_cast<unsigned>(precision);
        auto result = multiply(fraction(next(), bits), integer(maximum_bits + 1u), bits);
        if (sse_conversion) result = rounded(result, 0, 53);
        return truncate(result);
    }
    binary_float::Number real_number(std::uint64_t maximum_bits, RandomPrecision precision) {
        using namespace binary_float;
        const auto maximum = binary64(maximum_bits);
        const auto bits = static_cast<unsigned>(precision);
        const auto factor = fraction(next(), bits);
        return multiply(factor, add(maximum, {std::uint64_t(1) << 63, 0, false}, bits), bits);
    }
    std::uint64_t real_bits(std::uint64_t maximum_bits, RandomPrecision precision) {
        return binary_float::to_binary64(real_number(maximum_bits, precision));
    }
};
}
