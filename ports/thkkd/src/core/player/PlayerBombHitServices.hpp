// 炸弹伤害复用射击命中服务；MarisaC的原row[7]读另从调用者保留的栈字提供。
#pragma once
#include "core/player/PlayerShotHit.hpp"
namespace kaikeidou {
struct PlayerBombHitServices : PlayerShotHitServices {
    virtual std::uint32_t overflow_row_word() = 0;
};
}
