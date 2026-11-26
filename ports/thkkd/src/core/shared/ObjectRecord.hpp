// 对象尚未命名的字段按原字节保留，已确认字段以显式 little-endian 访问。
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

namespace kaikeidou {
template<std::size_t Size> struct ObjectRecord {
    std::array<std::uint8_t, Size> data{};
    std::uint32_t word(std::size_t offset) const noexcept {
        return std::uint32_t(data[offset]) | (std::uint32_t(data[offset + 1]) << 8) |
            (std::uint32_t(data[offset + 2]) << 16) | (std::uint32_t(data[offset + 3]) << 24);
    }
    void word(std::size_t offset, std::uint32_t value) noexcept {
        for (unsigned i = 0; i < 4; ++i) data[offset + i] = static_cast<std::uint8_t>(value >> (i * 8));
    }
    std::uint64_t quad(std::size_t offset) const noexcept {
        return word(offset) | (std::uint64_t(word(offset + 4)) << 32);
    }
    void quad(std::size_t offset, std::uint64_t value) noexcept {
        word(offset, static_cast<std::uint32_t>(value));
        word(offset + 4, static_cast<std::uint32_t>(value >> 32));
    }
};
}
