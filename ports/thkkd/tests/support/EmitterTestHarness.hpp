// 发射对照使用同一初始字节样本，输出所有变化槽位及全局游标、声音和随机状态。
#pragma once
#include "core/entities/ProjectileEmitter.hpp"
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <memory>

namespace emitter_test {
inline std::uint32_t read() {
    std::uint32_t result = 0;
    for (unsigned i = 0; i < 4; ++i) {
        const auto value = std::cin.get();
        if (value == EOF) throw std::runtime_error("Emitter input truncated");
        result |= std::uint32_t(static_cast<unsigned char>(value)) << (i * 8);
    }
    return result;
}
inline std::uint64_t quad() { const auto low = read(); return low | (std::uint64_t(read()) << 32); }
inline void write(std::uint32_t value) { for (unsigned i = 0; i < 4; ++i) std::cout.put(static_cast<char>(value >> (i * 8))); }
inline const kaikeidou::ProjectilePools& baseline() {
    static const auto value = [] {
        auto pool = std::make_unique<kaikeidou::ProjectilePools>();
        std::uint32_t seed = 0x4b4b4450;
        for (auto& bullet : pool->bullets) for (unsigned offset = 0; offset < 240; offset += 4) {
            seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5;
            bullet.word(offset, seed);
        }
        for (auto& trigger : pool->sound_triggers) { seed ^= seed << 13; seed ^= seed >> 17; seed ^= seed << 5; trigger = seed; }
        return pool;
    }();
    return *value;
}
struct Case {
    unsigned control = 127, mode = 0;
    std::uint32_t random_cursor = 0, target_index = 0;
    kaikeidou::PatternParameters parameters;
    std::uint64_t player_x = 0, player_y = 0;
    kaikeidou::ProjectileEmission emission;
    std::unique_ptr<kaikeidou::ProjectilePools> initial;
};
inline Case input() {
    Case value;
    value.initial = std::make_unique<kaikeidou::ProjectilePools>(baseline());
    value.control = read(); value.mode = read(); value.random_cursor = read();
    value.parameters.game_frame = read(); value.parameters.difficulty = read();
    for (auto& word : value.parameters.integers) word = read();
    for (auto& word : value.parameters.reals) word = quad();
    value.player_x = quad(); value.player_y = quad();
    for (auto& cursor : value.initial->cursors) cursor = read();
    std::array<unsigned char, 375> occupied{};
    for (auto& byte : occupied) { const auto next = std::cin.get(); if (next == EOF) throw std::runtime_error("Emitter mask truncated"); byte = static_cast<unsigned char>(next); }
    for (std::size_t i = 0; i < 3000; ++i) value.initial->bullets[i].word(0, (occupied[i / 8] >> (i % 8)) & 1);
    auto& emission = value.emission;
    emission.allocation.kind = read(); emission.allocation.group = static_cast<std::int32_t>(read());
    for (auto& color : emission.allocation.color) color = read();
    emission.allocation.blend = read(); emission.allocation.opacity = read();
    emission.x = quad(); emission.y = quad(); emission.count = read();
    emission.spacing = quad(); emission.speed = quad(); emission.angle = quad();
    emission.absolute = read(); emission.configuration = read(); emission.allocation.delayed = read(); emission.allocation.sound = read();
    value.target_index = read();
    auto& selected = value.initial->bullets.at(value.target_index);
    selected.quad(0x50, quad()); selected.quad(0x58, quad()); selected.quad(0x60, quad());
    return value;
}
inline void output(const Case& input, const kaikeidou::ProjectilePools& final, std::uint32_t random_cursor) {
    write(random_cursor);
    for (auto cursor : final.cursors) write(cursor);
    for (auto trigger : final.sound_triggers) write(trigger);
    std::uint32_t count = 0;
    for (std::size_t i = 0; i < 3000; ++i) if (input.initial->bullets[i].data != final.bullets[i].data) ++count;
    write(count);
    for (std::size_t i = 0; i < 3000; ++i) if (input.initial->bullets[i].data != final.bullets[i].data) {
        write(static_cast<std::uint32_t>(i));
        std::cout.write(reinterpret_cast<const char*>(final.bullets[i].data.data()), 240);
    }
}
inline void binary_streams() { _setmode(_fileno(stdin), _O_BINARY); _setmode(_fileno(stdout), _O_BINARY); }
inline kaikeidou::RandomPrecision precision(unsigned control) {
    return control == 127 ? kaikeidou::RandomPrecision::bits24 : (control == 639 ? kaikeidou::RandomPrecision::bits53 : kaikeidou::RandomPrecision::bits64);
}
}
