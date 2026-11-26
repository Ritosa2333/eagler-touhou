// 原A72248起30槽炸弹对象，每槽112字节，未命名字段按原字节保留。
#pragma once
#include "core/shared/ObjectRecord.hpp"
#include <array>

namespace kaikeidou {
using PlayerBomb = ObjectRecord<112>;
struct PlayerBombPool {
    std::array<PlayerBomb, 30> bombs{};
};
inline constexpr std::array<std::uint32_t, 9> player_bomb_durations{{320, 280, 280, 280, 280, 300, 360, 340, 340}};
}
