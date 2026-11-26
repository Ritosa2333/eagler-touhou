// 恢复 0x00657ed0 及 0x00657dd0，只写原函数指定字段。
#pragma once
#include "core/player/OptionsPool.hpp"
#include <cstring>

namespace kaikeidou {
namespace options_detail {
inline std::int64_t signed_word(std::uint32_t value) {
    return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
}
// FLD/FSTP 将 binary64 sNaN 的 quiet 位设为 1，保留符号和其余 payload。
inline std::uint64_t stored_bits(std::uint64_t value) {
    const auto absolute = value & 0x7fffffffffffffffull;
    return absolute > 0x7ff0000000000000ull ? value | 0x0008000000000000ull : value;
}
inline std::uint64_t bits(double value) { std::uint64_t result; std::memcpy(&result, &value, 8); return result; }
}
inline void initialize_player_option(PlayerOption& option, std::uint64_t field_28, std::uint64_t field_30,
    std::uint64_t field_18, std::uint64_t field_20) {
    option.quad(0x28, options_detail::stored_bits(field_28));
    option.quad(0x30, options_detail::stored_bits(field_30));
    option.quad(0x18, options_detail::stored_bits(field_18));
    option.quad(0x20, options_detail::stored_bits(field_20));
}
inline void reset_player_options(OptionsPool& pool, const ObjectRecord<240>& player, const OptionsContext& context,
    std::uint32_t first, std::uint32_t last) {
    const auto selector = context.character * 10u + context.weapon;
    const auto end = options_detail::signed_word(last);
    const bool clear_both = selector == 0 || selector == 1 || selector == 2 || selector == 11 || selector == 12;
    const bool positions_only = selector == 10;
    const bool clear_40_only = selector == 21;
    const bool clear_38_and_positions = selector == 22;
    if (clear_both || positions_only || clear_40_only || clear_38_and_positions) {
        for (auto index = first; options_detail::signed_word(index) < end; ++index) {
            auto& option = pool.options.at(static_cast<std::size_t>(index));
            if (clear_both || clear_38_and_positions) option.quad(0x38, 0);
            if (clear_both || clear_40_only) option.quad(0x40, 0);
            if (clear_both || positions_only || clear_38_and_positions) {
                option.quad(8, options_detail::stored_bits(player.quad(0x90)));
                option.quad(0x10, options_detail::stored_bits(player.quad(0x98)));
            }
        }
    }
    const auto set = [&](unsigned index, double a, double b, double c, double d) {
        initialize_player_option(pool.options.at(index), options_detail::bits(a), options_detail::bits(b), options_detail::bits(c), options_detail::bits(d));
    };
    if (selector == 0 || selector == 1 || selector == 11) {
        const double top_y = selector == 0 ? -20. : (selector == 1 ? 20. : 0.);
        const double bottom_y = selector == 1 ? 32. : -32.;
        if (end >= 4) {
            set(0, -16., top_y, -32., 0.); set(1, 16., top_y, 32., 0.);
            set(2, -8., bottom_y, selector == 11 ? -8. : -16., bottom_y);
            set(3, 8., bottom_y, selector == 11 ? 8. : 16., bottom_y);
        } else if (end >= 3) {
            set(0, -16., top_y, -32., 0.);
            if (selector == 1) { set(1, 0., bottom_y, 0., bottom_y); set(2, 16., top_y, 32., 0.); }
            else { set(1, 16., top_y, 32., 0.); set(2, 0., bottom_y, 0., bottom_y); }
        } else if (end >= 2) {
            set(0, -16., top_y, -32., 0.); set(1, 16., top_y, 32., 0.);
        } else set(0, 0., bottom_y, 0., bottom_y);
    } else if (selector == 2) {
        if (end >= 4) pool.options[0].quad(0x18, 0x3fe0000000000000ull);
        else if (end >= 3) pool.options[0].quad(0x18, 0x3fe5555555555555ull);
        else if (end >= 2) pool.options[0].quad(0x18, 0x3ff0000000000000ull);
    } else if (selector == 12) {
        if (end >= 4) {
            set(0, -.08, 64., -.39, 64.); set(1, .08, 64., .39, 64.);
            set(2, -.24, 64., -1.17, 64.); set(3, .24, 64., 1.17, 64.);
        } else if (end >= 3) {
            set(0, 0., 64., 0., 64.); set(1, -.16, 64., -.78, 64.); set(2, .16, 64., .78, 64.);
        } else if (end >= 2) {
            set(0, -.08, 64., -.39, 64.); set(1, .08, 64., .39, 64.);
        } else set(0, 0., 64., 0., 64.);
    } else if (selector == 20) {
        pool.options[0].quad(0x50, options_detail::bits(end >= 4 ? 32. : (end >= 3 ? 24. : (end >= 2 ? 16. : 8.))));
    } else if (selector == 21) {
        const unsigned count = end >= 4 ? 4u : (end >= 3 ? 3u : (end >= 2 ? 2u : 1u));
        for (unsigned index = 0; index < count; ++index)
            pool.options[index].quad(0x50, options_detail::bits(index < 2 ? 2. : (index == 2 ? 3. : 4.)));
        const std::array<double, 4> field_58{{4., 5., 6., 7.}};
        for (unsigned index = 0; index < count; ++index) pool.options[index].quad(0x58, options_detail::bits(field_58[index]));
        if (count == 4) {
            set(0, 0., 112., 0., 208.); set(1, 0., 96., 0., 160.); set(2, 0., 80., 0., 112.); set(3, 0., 64., 0., 64.);
        } else if (count == 3) {
            set(0, 0., 96., 0., 160.); set(1, 0., 80., 0., 112.); set(2, 0., 64., 0., 64.);
        } else if (count == 2) {
            set(0, 0., 80., 0., 112.); set(1, 0., 64., 0., 64.);
        } else set(0, 0., 64., 0., 64.);
    }
}
}
