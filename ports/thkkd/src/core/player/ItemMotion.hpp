// 恢复 0x005e2c70..0x005e3368 的完整道具移动，保留吸附触发、逐槽输入读取与 double 写回顺序。
#pragma once
#include "core/player/ItemPool.hpp"
#include "core/math/GameMath.hpp"
#include "core/math/Random.hpp"

namespace kaikeidou {
struct ItemMotionContext { std::uint32_t slow_binding = 0; };
struct ItemMotionInput {
    virtual ~ItemMotionInput() = default;
    virtual std::uint32_t read_counter(std::uint32_t binding) = 0;
};
inline constexpr std::array<std::uint64_t, 8> item_absorption_radius_bits{{
    0x4018000000000000ull, 0x4020000000000000ull, 0x4020000000000000ull, 0x4018000000000000ull,
    0x4018000000000000ull, 0x4028000000000000ull, 0x4024000000000000ull, 0x4036000000000000ull}};
namespace item_motion_detail {
inline std::int64_t signed_word(std::uint32_t value) {
    return value >> 31 ? static_cast<std::int64_t>(value) - 0x100000000ll : value;
}
inline bool nonnegative_at_least(binary_float::Number left, binary_float::Number right) {
    if (!right.mantissa) return true;
    if (!left.mantissa) return false;
    return left.exponent > right.exponent || (left.exponent == right.exponent && left.mantissa >= right.mantissa);
}
inline std::uint64_t difference(std::uint64_t left, std::uint64_t right, unsigned precision) {
    auto second = binary_float::binary64(right); second.negative = !second.negative;
    return binary_float::to_binary64(binary_float::add(binary_float::binary64(left), second, precision));
}
inline void homing(Item& item, const ObjectRecord<240>& player, std::uint64_t speed, GameMath& functions, unsigned precision) {
    using namespace binary_float;
    const auto dx = difference(player.quad(0x90), item.quad(0x18), precision);
    const auto dy = difference(player.quad(0x98), item.quad(0x20), precision);
    const auto angle = to_binary64(functions.atan2(dy, dx));
    const auto horizontal = multiply(functions.cosine(angle), binary64(speed), precision);
    item.quad(0x18, to_binary64(add(horizontal, binary64(item.quad(0x18)), precision)));
    const auto vertical = multiply(functions.sine(angle), binary64(speed), precision);
    item.quad(0x20, to_binary64(add(vertical, binary64(item.quad(0x20)), precision)));
}
inline void free_motion(Item& item, unsigned precision) {
    using namespace binary_float;
    item.quad(0x18, to_binary64(add(binary64(item.quad(0x18)), binary64(item.quad(0x28)), precision)));
    const auto vertical = add(binary64(item.quad(0x30)), binary64(item.quad(0x38)), precision);
    item.quad(0x20, to_binary64(add(vertical, binary64(item.quad(0x20)), precision)));
    item.word(4, item.word(4) + 1u);
    if (signed_word(item.word(4)) < 40) {
        for (const auto offset : {0x28u, 0x30u}) item.quad(offset,
            to_binary64(multiply(binary64(item.quad(offset)), binary64(0x3fee666666666666ull), precision)));
    } else if (item.word(4) == 40) {
        item.quad(0x28, 0); item.quad(0x30, 0);
    }
    if (crt_sse2::real(item.quad(0x38)) <= 1.4) item.quad(0x38,
        to_binary64(add(binary64(item.quad(0x38)), binary64(0x3fb999999999999aull), precision)));
    if (crt_sse2::real(item.quad(0x20)) > 480.) item.word(0, 0);
}
}
inline void update_item_motion(ItemPool& pool, const ObjectRecord<240>& player, const ItemMotionContext& context,
    ItemMotionInput& input, GameMath& functions, RandomPrecision precision) {
    using namespace binary_float;
    const auto working = static_cast<unsigned>(precision);
    const bool respawn = player.word(0) == 2 && item_motion_detail::signed_word(player.word(4)) < 40;
    for (auto& item : pool.items) {
        if (item.word(0) == 0) continue;
        if (!respawn && item.word(0xc) == 0) {
            const auto threshold = item_motion_detail::signed_word(player.word(0x1c) + player.word(0x20));
            if (static_cast<double>(threshold) < crt_sse2::real(player.quad(0x98))) {
                const auto dx = binary64(item_motion_detail::difference(player.quad(0x90), item.quad(0x18), working));
                const auto dy = binary64(item_motion_detail::difference(player.quad(0x98), item.quad(0x20), working));
                const auto kind = item.word(8);
                if (kind >= item_absorption_radius_bits.size()) throw std::out_of_range("Item absorption kind outside recovered radius table");
                const auto radius = binary64(to_binary64(add(binary64(player.quad(0xd0)), binary64(item_absorption_radius_bits[kind]), working)));
                const auto slow = input.read_counter(context.slow_binding);
                const auto distance_squared = add(multiply(dx, dx, working), multiply(dy, dy, working), working);
                auto radius_squared = multiply(radius, radius, working);
                if (item_motion_detail::signed_word(slow) < 1) radius_squared = multiply(radius_squared, binary64(0x3fd3333333333333ull), working);
                if (item_motion_detail::nonnegative_at_least(radius_squared, distance_squared)) item.word(0xc, 2);
            } else if (crt_sse2::real(item.quad(0x20)) > 0.) item.word(0xc, 1);
        }
        if (item.word(0xc) == 2) item_motion_detail::homing(item, player, 0x4014666666666666ull, functions, working);
        else if (item.word(0xc) == 1) item_motion_detail::homing(item, player, 0x4020000000000000ull, functions, working);
        else item_motion_detail::free_motion(item, working);
    }
}
}
