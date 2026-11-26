// 恢复完整 0x0065a180..0x0065a928 道具拾取，按槽序奖励后清空，再统一结算资源。
#pragma once
#include "core/player/ItemPool.hpp"
#include "core/player/PlayerOptions.hpp"
#include "core/math/Random.hpp"

namespace kaikeidou {
struct ItemPickupContext {
    std::uint32_t character = 0, weapon = 0;
    std::uint32_t pattern_integer_1 = 0, pattern_integer_3 = 0;
    std::uint64_t secondary_first_field_28 = 0;
};
struct ItemPickupServices {
    virtual ~ItemPickupServices() = default;
    virtual void reset_options(OptionsPool& options, const ObjectRecord<240>& player, const OptionsContext& context,
        std::uint32_t first, std::uint32_t last) {
        reset_player_options(options, player, context, first, last);
    }
};
namespace item_pickup_detail {
inline std::int64_t signed_word(std::uint32_t value) {
    return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
}
inline double real(std::uint64_t bits) { double result; std::memcpy(&result, &bits, 8); return result; }
inline std::uint32_t power_level(std::uint32_t power, unsigned precision) {
    using namespace binary_float;
    const auto value = binary64(to_binary64(multiply(integer(power), binary64(0x3f847ae147ae147bull), precision)));
    auto result = truncate(value);
    if (value.negative && value.mantissa && (value.exponent < 0 ||
        (value.exponent < 63 && (value.mantissa & ((std::uint64_t(1) << (63 - value.exponent)) - 1))))) --result;
    return result;
}
inline std::uint32_t select(std::uint32_t character, std::uint32_t first, std::uint32_t second, std::uint32_t third) {
    return character == 1 ? second : (character == 2 ? third : first);
}
}
inline void update_item_pickup(ItemPool& pool, ObjectRecord<240>& player, OptionsPool& options,
    ItemPickupContext& context, std::array<std::uint32_t, 100>& sounds, RandomPrecision precision,
    ItemPickupServices& services) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const auto signed_value = item_pickup_detail::signed_word;
    const auto before = item_pickup_detail::power_level(player.word(8), working);
    const auto add_secondary = [&](std::uint64_t increment, bool clamp) {
        context.secondary_first_field_28 = to_binary64(add(binary64(context.secondary_first_field_28), binary64(increment), working));
        if (clamp && item_pickup_detail::real(context.secondary_first_field_28) > 140.)
            context.secondary_first_field_28 = 0x4061800000000000ull;
    };
    const auto score = [&](std::uint32_t increment) { player.word(0x10, player.word(0x10) + increment); };
    const auto power = [&](std::uint32_t increment) { player.word(8, player.word(8) + increment); };
    const auto small_secondary = [&]() {
        add_secondary(0x3ff2666666666666ull, false);
        ++context.pattern_integer_1;
    };
    const auto large_secondary = [&]() {
        add_secondary(0x4018000000000000ull, false);
        context.pattern_integer_3 += 3u;
    };
    for (auto& item : pool.items) {
        if (signed_value(item.word(0)) < 1) continue;
        auto opposite_x = binary64(player.quad(0x90)); opposite_x.negative = !opposite_x.negative;
        const auto dx = binary64(to_binary64(add(binary64(item.quad(0x18)), opposite_x, working)));
        auto opposite_y = binary64(player.quad(0x98)); opposite_y.negative = !opposite_y.negative;
        const auto dy = binary64(to_binary64(add(binary64(item.quad(0x20)), opposite_y, working)));
        const auto distance_squared = add(multiply(dx, dx, working), multiply(dy, dy, working), working);
        if (distance_squared.mantissa && distance_squared.exponent >= 8) continue;
        const auto reward = item.word(0x10);
        switch (item.word(8)) {
        case 0:
            if (reward == 0) power(5);
            else if (reward == 3) { score(10); small_secondary(); }
            else if (reward == 2) { power(5); add_secondary(0x3ff0000000000000ull, true); }
            else { score(10); add_secondary(0x3ff0000000000000ull, true); }
            break;
        case 1:
            if (reward == 0) power(100);
            else if (reward == 3) {
                score(10); add_secondary(0x4008000000000000ull, false); context.pattern_integer_1 += 3u;
            } else if (reward == 2) { power(100); add_secondary(0x4018000000000000ull, true); }
            else { score(10); add_secondary(0x4018000000000000ull, true); }
            break;
        case 2:
            if (reward == 0) power(400);
            else { score(10); add_secondary(0x4028000000000000ull, true); }
            break;
        case 3:
            if (reward == 0) score(100u + player.word(0x1c) * 10u);
            else {
                if (reward == 3) small_secondary(); else add_secondary(0x3ff0000000000000ull, true);
                score(10);
            }
            break;
        case 4:
            if (reward == 0) player.word(0x20, player.word(0x20) + item_pickup_detail::select(context.character, 1, 2, 1));
            else if (reward == 3) small_secondary(); else add_secondary(0x3ff0000000000000ull, true);
            score(10);
            break;
        case 5:
            if (reward == 0) {
                player.word(0x20, player.word(0x20) + item_pickup_detail::select(context.character, 12, 24, 12)); score(100);
            } else {
                if (reward == 3) large_secondary(); else add_secondary(0x4010000000000000ull, true);
                score(10);
            }
            break;
        case 6:
            if (reward == 0) {
                player.word(0x1c, player.word(0x1c) + item_pickup_detail::select(context.character, 2, 1, 1)); score(100);
            } else { score(10); add_secondary(0x4010000000000000ull, true); }
            break;
        case 7:
            if (reward == 0) {
                player.word(0x1c, player.word(0x1c) + item_pickup_detail::select(context.character, 4, 2, 2)); score(1000);
            } else {
                if (reward == 3) large_secondary(); else add_secondary(0x4028000000000000ull, true);
                score(10);
            }
            break;
        default: break;
        }
        item.word(0, 0);
    }
    if (signed_value(player.word(8)) > 400) player.word(8, 400);
    const auto after = item_pickup_detail::power_level(player.word(8), working);
    if (signed_value(after - before) > 0) {
        services.reset_options(options, player, {context.character, context.weapon}, before, after);
        sounds[15] = 1;
    }
    if (signed_value(player.word(0x20)) >= 384) {
        player.word(0x20, player.word(0x20) - (384u - player.word(0xe4)));
        if (signed_value(player.word(0x18)) < 8) { player.word(0x18, player.word(0x18) + 1u); sounds[17] = 1; }
    }
    if (signed_value(player.word(0x1c)) >= 64) {
        player.word(0x1c, player.word(0x1c) - 64u);
        if (signed_value(player.word(0x14)) < 8) { player.word(0x14, player.word(0x14) + 1u); sounds[16] = 1; }
    }
    if (signed_value(player.word(0x10)) > 999999999 || signed_value(player.word(0x10)) < 0) player.word(0x10, 999999999);
}
}
