// 依据 0x006604c0 恢复移动、动画标志与边界处理；输入计数由平台层或 Replay 提供。
#pragma once
#include "core/math/BinaryFloat.hpp"
#include <array>
#include <cstdint>
#include <cstring>

namespace kaikeidou {
struct PlayerMovement {
    std::uint32_t state = 0;
    std::uint32_t state_timer = 0;
    std::uint32_t sprite = 0;
    std::uint64_t x = 0, y = 0;
    std::uint64_t fast_straight = 0, fast_diagonal = 0;
    std::uint64_t slow_straight = 0, slow_diagonal = 0;
};
struct MovementInput {
    // 原输入层返回按住的帧数；四项依次为左、右、上、下。
    std::array<std::uint32_t, 4> directions{};
    std::uint32_t slow = 0;
};
inline void update_movement(PlayerMovement& player, MovementInput input, unsigned precision) {
    if (player.state == 1) return;
    auto& left = input.directions[0];
    auto& right = input.directions[1];
    auto& up = input.directions[2];
    auto& down = input.directions[3];
    if (left && right) left = right = 0;
    if (up && down) up = down = 0;
    const auto positive = [](std::uint32_t value) { return value && !(value >> 31); };
    if (positive(left)) player.sprite += 8u;
    else if (positive(right)) player.sprite += 4u;
    const auto horizontal = left ? -1 : (right ? 1 : 0);
    const auto vertical = up ? -1 : (down ? 1 : 0);
    const bool diagonal = horizontal && vertical;
    const auto speed = positive(input.slow) ? (diagonal ? player.slow_diagonal : player.slow_straight) :
                                             (diagonal ? player.fast_diagonal : player.fast_straight);
    const auto move = [precision](std::uint64_t position, std::uint64_t delta, int direction) {
        if (direction < 0) delta ^= std::uint64_t(1) << 63;
        return binary_float::to_binary64(binary_float::add(
            binary_float::binary64(position), binary_float::binary64(delta), precision));
    };
    if (horizontal) player.x = move(player.x, speed, horizontal);
    if (vertical) player.y = move(player.y, speed, vertical);
    const auto real = [](std::uint64_t bits) { double value; std::memcpy(&value, &bits, 8); return value; };
    if (real(player.x) < 8.) player.x = 0x4020000000000000ull;
    else if (real(player.x) > 376.) player.x = 0x4077800000000000ull;
    if (real(player.y) < 16.) player.y = 0x4030000000000000ull;
    else if (real(player.y) > 432. && (player.state != 2 ||
        (!(player.state_timer >> 31) && player.state_timer > 39))) player.y = 0x407b000000000000ull;
}
}
