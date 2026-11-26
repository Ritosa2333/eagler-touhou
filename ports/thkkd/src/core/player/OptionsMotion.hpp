// 恢复 0x005c2fe0 的完整子机运动与 0x005e4bb0 的缓动，有限数按原指令顺序舍入。
#pragma once
#include "core/player/OptionsPool.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/FiniteDivide.hpp"
#include <cmath>
#include <vector>

namespace kaikeidou {
struct OptionsMotionInput {
    // 依次为 0x00a733dc、0x00a733e0、0x00a733e8、0x00a733f4。
    std::array<std::uint32_t, 4> bindings{};
    std::array<std::uint32_t, 16> logical_input{};
    std::vector<std::array<std::uint32_t, 2>>* reads = nullptr;
};
namespace options_motion_detail {
struct Arithmetic {
    unsigned precision;
    using Number = binary_float::Number;
    Number n(double value) const { return binary_float::binary64(crt_sse2::bits(value)); }
    Number q(std::uint64_t value) const { return binary_float::binary64(value); }
    Number i(std::uint32_t value) const { return binary_float::integer(value); }
    Number add(Number a, Number b) const { return binary_float::add(a, b, precision); }
    Number sub(Number a, Number b) const { b.negative = !b.negative; return add(a, b); }
    Number mul(Number a, Number b) const { return binary_float::multiply(a, b, precision); }
    Number div(Number a, Number b) const { return binary_float::divide(a, b, precision); }
    std::uint64_t store(Number value) const { return binary_float::to_binary64(value); }
};
}
inline std::uint64_t ease_option_value(std::uint64_t current, std::uint64_t target, std::uint64_t divisor, unsigned precision) {
    const options_motion_detail::Arithmetic m{precision};
    return m.store(m.add(m.div(m.sub(m.q(target), m.q(current)), m.q(divisor)), m.q(current)));
}
inline void update_player_options(OptionsPool& pool, const ObjectRecord<240>& player, const OptionsContext& context,
    const OptionsMotionInput& input, unsigned precision, GameMath& functions) {
    const options_motion_detail::Arithmetic m{precision};
    const auto power = m.store(m.mul(m.i(player.word(8)), m.q(0x3f847ae147ae147bull)));
    const auto count = static_cast<std::int64_t>(std::floor(crt_sse2::real(power)));
    const auto selector = context.character * 10u + context.weapon;
    const auto key = [&](unsigned binding) {
        const auto index = input.bindings.at(binding);
        const auto value = input.logical_input.at(index);
        if (input.reads) input.reads->push_back({{index, value}});
        return value;
    };
    const auto ease = [&](PlayerOption& option, unsigned destination, std::uint64_t target, double divisor) {
        option.quad(destination, ease_option_value(option.quad(destination), target, crt_sse2::bits(divisor), precision));
    };
    const auto add_position = [&](PlayerOption& option) {
        option.quad(8, m.store(m.add(m.q(player.quad(0x90)), m.q(option.quad(0x38)))));
        option.quad(0x10, m.store(m.add(m.q(player.quad(0x98)), m.q(option.quad(0x40)))));
    };
    const auto polar = [&](PlayerOption& option, std::uint64_t angle, std::uint64_t radius) {
        option.quad(8, m.store(m.add(m.mul(functions.cosine(angle), m.q(radius)), m.q(player.quad(0x90)))));
        option.quad(0x10, m.store(m.add(m.mul(functions.sine(angle), m.q(radius)), m.q(player.quad(0x98)))));
    };
    if (selector == 0 || selector == 1 || selector == 11) {
        const bool slow = key(3) != 0;
        for (std::int64_t index = 0; index < count; ++index) {
            auto& option = pool.options.at(static_cast<std::size_t>(index));
            ease(option, 0x38, option.quad(slow ? 0x28 : 0x18), 5.);
            ease(option, 0x40, option.quad(slow ? 0x30 : 0x20), 5.);
            add_position(option);
        }
    } else if (selector == 2) {
        const bool slow = key(3) != 0;
        for (std::int64_t index = 0; index < count; ++index) {
            auto& option = pool.options.at(static_cast<std::size_t>(index));
            const auto frame = m.mul(m.i(player.word(0xa4)), m.n(.02));
            const auto phase = m.mul(m.mul(m.i(static_cast<std::uint32_t>(index)), m.q(0x400921fb54442d28ull)), m.q(pool.options[0].quad(0x18)));
            const auto angle = m.store(m.add(frame, phase));
            const auto horizontal = crt_sse2::real(option.quad(0x38));
            if (slow ? horizontal > 48. : horizontal < 192.)
                option.quad(0x38, m.store(slow ? m.sub(m.q(option.quad(0x38)), m.n(1.)) : m.add(m.q(option.quad(0x38)), m.n(1.))));
            if (crt_sse2::real(option.quad(0x40)) < 32.) option.quad(0x40, m.store(m.add(m.q(option.quad(0x40)), m.n(1.))));
            option.quad(8, m.store(m.add(m.mul(functions.cosine(angle), m.q(option.quad(0x38))), m.q(player.quad(0x90)))));
            const auto absolute_sine = m.store(functions.sine(angle)) & 0x7fffffffffffffffull;
            option.quad(0x10, m.store(m.sub(m.q(player.quad(0x98)), m.mul(m.q(absolute_sine), m.q(option.quad(0x40))))));
        }
    } else if (selector == 10) {
        if (key(3) == 0) for (std::int64_t index = 0; index < count; ++index) {
            auto& option = pool.options.at(static_cast<std::size_t>(index));
            const auto x = index == 0 ? player.quad(0x90) : pool.options.at(static_cast<std::size_t>(index - 1)).quad(8);
            ease(option, 8, x, 40.);
            const auto y = index == 0 ? player.quad(0x98) : pool.options.at(static_cast<std::size_t>(index - 1)).quad(0x10);
            const auto target = m.store(m.add(m.q(y), m.n(index == 0 ? 32. : 16.)));
            ease(option, 0x10, target, 40.);
        }
    } else if (selector == 12) {
        const bool slow = key(3) != 0;
        if (!slow) {
            if (key(2) != 0) {
                if (key(0) != 0) ease(pool.options[0], 0x190, crt_sse2::bits(.6), 20.);
                else if (key(1) != 0) ease(pool.options[0], 0x190, crt_sse2::bits(-.6), 20.);
                else ease(pool.options[0], 0x190, 0, 10.);
            } else if (key(0) != 0) ease(pool.options[0], 0x190, crt_sse2::bits(1.2), 35.);
            else if (key(1) != 0) ease(pool.options[0], 0x190, crt_sse2::bits(-1.2), 35.);
        }
        for (std::int64_t index = 0; index < count; ++index) {
            auto& option = pool.options.at(static_cast<std::size_t>(index));
            ease(option, 0x38, option.quad(slow ? 0x28 : 0x18), 5.);
            ease(option, 0x40, option.quad(slow ? 0x30 : 0x20), 5.);
            const auto angle = m.store(m.add(m.add(m.q(0x3ff921fb54442d28ull), m.q(pool.options[0].quad(0x190))), m.q(option.quad(0x38))));
            polar(option, angle, option.quad(0x40));
        }
    } else if (selector == 20) {
        auto& special = pool.options[0];
        special.quad(0x190, m.store(m.add(m.q(special.quad(0x190)), m.n(.018))));
        const auto advance = m.add(m.add(m.mul(functions.sine(special.quad(0x1a0)), m.n(.015)), m.n(.025)), m.q(special.quad(0x198)));
        special.quad(0x198, m.store(advance));
        special.quad(0x1a0, m.store(m.add(m.q(special.quad(0x1a0)), m.n(.01))));
        const auto radius = m.store(m.mul(functions.sine(special.quad(0x198)), m.n(256.)));
        for (unsigned index = 0; index < 3; ++index) {
            const auto phase = m.mul(m.i(index), m.q(0x400921fb54442d28ull));
            const auto angle = m.store(m.add(m.div(m.add(phase, phase), m.n(3.)), m.q(special.quad(0x190))));
            const auto x = m.store(m.add(m.n(32.), m.q(player.quad(0x90))));
            pool.options[index].quad(8, m.store(m.add(m.mul(functions.cosine(angle), m.q(radius)), m.q(x))));
            const auto y = m.store(m.add(m.n(16.), m.q(player.quad(0x98))));
            pool.options[index].quad(0x10, m.store(m.add(m.mul(functions.sine(angle), m.q(radius)), m.q(y))));
        }
    } else if (selector == 21) {
        const bool slow = key(3) != 0;
        for (std::int64_t index = 0; index < count; ++index) {
            auto& option = pool.options.at(static_cast<std::size_t>(index));
            ease(option, 0x40, option.quad(slow ? 0x30 : 0x20), 5.);
        }
    } else if (selector == 22) {
        const bool slow = key(3) != 0;
        for (std::int64_t index = 0; index < count; ++index) {
            auto& option = pool.options.at(static_cast<std::size_t>(index));
            const auto angle = m.store(m.add(m.mul(m.i(player.word(0xa4)), m.q(option.quad(0x28))), m.q(0x3ff921fb54442d28ull)));
            ease(option, 0x38, slow ? 0 : option.quad(0x18), 5.);
            polar(option, angle, option.quad(0x38));
        }
    }
}
}
