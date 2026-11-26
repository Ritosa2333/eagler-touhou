// 从 Replay 1.5 补丁的指令与文件样本恢复的字节格式。
#pragma once
#include <array>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace kaikeidou {
struct ReplayInput {
    std::array<bool, 4> directions{};
    bool action10 = false;
    bool action20 = false;
    bool action80 = false;
};

constexpr std::uint8_t pack_input(const ReplayInput& input) noexcept {
    return static_cast<std::uint8_t>((input.directions[0] ? 1 : 0) |
        (input.directions[1] ? 2 : 0) | (input.directions[2] ? 4 : 0) |
        (input.directions[3] ? 8 : 0) | (input.action10 ? 0x10 : 0) |
        (input.action20 ? 0x20 : 0) | (input.action80 ? 0x80 : 0));
}

constexpr ReplayInput unpack_input(std::uint8_t value) noexcept {
    return {{{(value & 1) != 0, (value & 2) != 0, (value & 4) != 0, (value & 8) != 0}},
            (value & 0x10) != 0, (value & 0x20) != 0, (value & 0x80) != 0};
}

inline std::uint32_t read_u32(const std::uint8_t* data) noexcept {
    return std::uint32_t(data[0]) | (std::uint32_t(data[1]) << 8) |
           (std::uint32_t(data[2]) << 16) | (std::uint32_t(data[3]) << 24);
}

constexpr std::uint32_t replay_checksum(std::uint32_t accumulator,
                                      std::uint32_t name0, std::uint32_t name1) noexcept {
    // 0x00b42cbf 至 0x00b42cf0，保留 32 位加法溢出与两次取余顺序。
    return accumulator % 119u + static_cast<std::uint32_t>(name0 + name1) % 119u;
}

struct Replay {
    static constexpr std::size_t footer_size = 62;
    std::vector<std::uint8_t> inputs;
    // 字段含义继续依据调用方确认；保留补丁写入顺序和原始值。
    std::array<std::uint32_t, 7> metadata{};
    std::array<std::uint8_t, 8> player_name{};
    std::array<std::uint8_t, 8> character_label{};
    std::array<std::uint8_t, 10> difficulty_label{};
    std::uint32_t accumulator = 0;
    std::uint32_t checksum = 0;

    static Replay parse(const std::vector<std::uint8_t>& data) {
        if (data.size() < footer_size) throw std::runtime_error("Replay footer truncated");
        Replay result;
        const auto count = data.size() - footer_size;
        const auto* tail = data.data() + count;
        for (std::size_t i = 0; i < 7; ++i) result.metadata[i] = read_u32(tail + i * 4);
        if (result.metadata[5] != count) throw std::runtime_error("Replay input count mismatch");
        result.inputs.assign(data.begin(), data.begin() + count);
        for (std::size_t i = 0; i < 8; ++i) {
            result.player_name[i] = tail[28 + i];
            result.character_label[i] = tail[36 + i];
        }
        for (std::size_t i = 0; i < 10; ++i) result.difficulty_label[i] = tail[44 + i];
        result.accumulator = read_u32(tail + 54);
        result.checksum = read_u32(tail + 58);
        if (result.checksum != replay_checksum(result.accumulator, read_u32(tail + 28), read_u32(tail + 32)))
            throw std::runtime_error("Replay footer checksum mismatch");
        return result;
    }

    std::vector<std::uint8_t> bytes() const {
        std::vector<std::uint8_t> data(inputs);
        auto append_u32 = [&](std::uint32_t value) {
            for (unsigned shift = 0; shift < 32; shift += 8)
                data.push_back(static_cast<std::uint8_t>(value >> shift));
        };
        for (auto value : metadata) append_u32(value);
        data.insert(data.end(), player_name.begin(), player_name.end());
        data.insert(data.end(), character_label.begin(), character_label.end());
        data.insert(data.end(), difficulty_label.begin(), difficulty_label.end());
        append_u32(accumulator);
        append_u32(checksum);
        return data;
    }
};
}
