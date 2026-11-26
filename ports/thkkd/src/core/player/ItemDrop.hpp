// 恢复完整 0x006593f0 死亡掉落，保持七次空槽搜索、字段写入与角度换算的 double 写回。
#pragma once
#include "core/player/ItemPool.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/Random.hpp"

namespace kaikeidou {
inline void drop_death_items(ItemPool& pool, const ObjectRecord<240>& player, std::uint32_t character,
    GameMath& functions, RandomPrecision precision) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    auto player_x = binary64(player.quad(0x90)); player_x.negative = !player_x.negative;
    const auto dx = to_binary64(add(binary64(0x4068000000000000ull), player_x, working));
    const auto radians = to_binary64(functions.atan2(0xc060000000000000ull, dx));
    const auto pi = binary64(0x400921fb54442d28ull);
    const auto degrees_180 = binary64(0x4066800000000000ull);
    const auto base_degrees = to_binary64(divide(multiply(binary64(radians), degrees_180, working), pi, working));
    for (std::uint32_t element = 0; element < 7; ++element) {
        const auto power_bits = player.word(8);
        const auto power = power_bits >> 31 ? static_cast<std::int64_t>(power_bits) - 0x100000000ll : power_bits;
        std::uint32_t kind = 0;
        if (character == 2) {
            if (power < 130) kind = 0;
            else if (power < 225) kind = element == 3 ? 1u : 0u;
            else if (power < 320) kind = element == 2 || element == 4 ? 1u : 0u;
            else if (power < 400) kind = element == 1 || element == 3 || element == 5 ? 1u : 0u;
            else kind = element == 3 ? 2u : (element == 1 || element == 5 ? 1u : 0u);
        } else {
            if (power < 200) kind = 0;
            else if (power < 300) kind = element == 3 ? 1u : 0u;
            else if (power < 400) kind = element == 2 || element == 4 ? 1u : 0u;
            else kind = element == 1 || element == 3 || element == 5 ? 1u : 0u;
        }
        const auto index = find_free_item(pool);
        if (index == -1) continue;
        auto& item = pool.items[static_cast<std::size_t>(index)];
        item.word(0, 1); item.word(0x10, 0); item.word(8, kind); item.word(4, 0); item.word(0xc, 0);
        item.quad(0x18, player.quad(0x90)); item.quad(0x20, player.quad(0x98));
        const auto degree_offset = integer((element - 3u) * 12u);
        const auto degrees = to_binary64(add(degree_offset, binary64(base_degrees), working));
        const auto angle = to_binary64(divide(multiply(binary64(degrees), pi, working), degrees_180, working));
        item.quad(0x28, to_binary64(multiply(functions.cosine(angle), binary64(0x4020000000000000ull), working)));
        item.quad(0x30, to_binary64(multiply(functions.sine(angle), binary64(0x4020000000000000ull), working)));
        item.quad(0x38, 0xc008000000000000ull);
    }
}
}
