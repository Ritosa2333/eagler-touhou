#pragma once

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ecl_fixture {

using Bytes = std::vector<std::uint8_t>;

inline void require(bool condition, const std::string& message) {
    if (!condition)
        throw std::runtime_error(message);
}

inline void put16(Bytes& bytes, std::size_t offset, std::uint16_t value) {
    bytes.at(offset) = static_cast<std::uint8_t>(value);
    bytes.at(offset + 1) = static_cast<std::uint8_t>(value >> 8);
}

inline void put32(Bytes& bytes, std::size_t offset, std::uint32_t value) {
    for (std::size_t byte = 0; byte < 4; ++byte)
        bytes.at(offset + byte) = static_cast<std::uint8_t>(value >> (byte * 8));
}

inline std::uint32_t get32(const Bytes& bytes, std::size_t offset) {
    std::uint32_t value = 0;
    for (std::size_t byte = 0; byte < 4; ++byte)
        value |= std::uint32_t(bytes.at(offset + byte)) << (byte * 8);
    return value;
}

inline std::uint32_t floatBits(float value) {
    std::uint32_t result;
    std::memcpy(&result, &value, sizeof(result));
    return result;
}

inline Bytes instruction(std::uint16_t opcode, std::vector<std::uint32_t> words = {},
                         std::int32_t time = 0, std::uint16_t mask = 0,
                         std::uint8_t rank = 255, int parameters = -1,
                         std::uint32_t stackOffset = 0) {
    Bytes bytes(16 + words.size() * 4);
    put32(bytes, 0, static_cast<std::uint32_t>(time));
    put16(bytes, 4, opcode);
    put16(bytes, 6, static_cast<std::uint16_t>(bytes.size()));
    put16(bytes, 8, mask);
    bytes[10] = rank;
    bytes[11] = static_cast<std::uint8_t>(parameters < 0 ? words.size() : parameters);
    put32(bytes, 12, stackOffset);
    for (std::size_t word = 0; word < words.size(); ++word)
        put32(bytes, 16 + word * 4, words[word]);
    return bytes;
}

inline Bytes join(const std::vector<Bytes>& parts) {
    Bytes result;
    for (const auto& part : parts)
        result.insert(result.end(), part.begin(), part.end());
    return result;
}

inline std::vector<std::uint32_t> stringWords(const std::string& value) {
    const std::size_t length = (value.size() + 4) & ~std::size_t(3);
    Bytes bytes(length + 4);
    put32(bytes, 0, static_cast<std::uint32_t>(length));
    std::memcpy(bytes.data() + 4, value.data(), value.size());
    std::vector<std::uint32_t> result;
    for (std::size_t offset = 0; offset < bytes.size(); offset += 4)
        result.push_back(get32(bytes, offset));
    return result;
}

inline Bytes program(const std::vector<std::pair<std::string, Bytes>>& subs,
                     const std::vector<std::string>& anim = {},
                     const std::vector<std::string>& includes = {}) {
    Bytes bytes(36);
    std::memcpy(bytes.data(), "SCPT", 4);
    put16(bytes, 4, 1);
    put32(bytes, 8, 36);
    put32(bytes, 16, static_cast<std::uint32_t>(subs.size()));
    const auto section = [&](const char* magic, const std::vector<std::string>& names) {
        const auto start = bytes.size();
        bytes.resize(start + 8);
        std::memcpy(bytes.data() + start, magic, 4);
        put32(bytes, start + 4, static_cast<std::uint32_t>(names.size()));
        for (const auto& name : names) {
            bytes.insert(bytes.end(), name.begin(), name.end());
            bytes.push_back(0);
        }
        bytes.resize((bytes.size() + 3) & ~std::size_t(3));
    };
    section("ANIM", anim);
    section("ECLI", includes);
    const auto table = bytes.size();
    put16(bytes, 6, static_cast<std::uint16_t>(table - 36));
    bytes.resize(table + subs.size() * 4);
    for (const auto& sub : subs) {
        bytes.insert(bytes.end(), sub.first.begin(), sub.first.end());
        bytes.push_back(0);
    }
    bytes.resize((bytes.size() + 3) & ~std::size_t(3));
    for (std::size_t index = 0; index < subs.size(); ++index) {
        const auto start = bytes.size();
        put32(bytes, table + index * 4, static_cast<std::uint32_t>(start));
        bytes.resize(start + 16);
        std::memcpy(bytes.data() + start, "ECLH", 4);
        put32(bytes, start + 4, 16);
        bytes.insert(bytes.end(), subs[index].second.begin(), subs[index].second.end());
    }
    return bytes;
}

}
