// 原玩家弹对象池：0x00a87be0 为 20 个 24 字节槽，0x00a79090 为 80 个 56 字节槽。
#pragma once
#include "core/shared/ObjectRecord.hpp"

namespace kaikeidou {
struct PlayerShotPools {
    std::array<ObjectRecord<24>, 20> common{};
    std::array<ObjectRecord<56>, 80> weapon{};
};
}
