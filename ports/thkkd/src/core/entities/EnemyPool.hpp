// 从 0x00643ad0 恢复普通敌人分配，复用槽位时保留未被原函数写入的字段。
#pragma once
#include "core/shared/ObjectRecord.hpp"

namespace kaikeidou {
using Enemy = ObjectRecord<0xf8>;
struct EnemyPool {
    std::array<Enemy, 80> enemies{};
    std::uint32_t next_id = 0;
};
inline std::int32_t allocate_enemy(EnemyPool& pool) {
    for (std::size_t i = 0; i < pool.enemies.size(); ++i) {
        auto& enemy = pool.enemies[i];
        if (enemy.word(0)) continue;
        enemy.word(0, 1); enemy.word(0xc, 0); enemy.word(4, 0);
        enemy.word(0x18, 60); enemy.word(0x14, 60); enemy.word(0x1c, 1);
        enemy.word(0x24, pool.next_id); enemy.word(0x28, 0);
        ++pool.next_id;
        if (pool.next_id == 1000) pool.next_id = 0;
        return static_cast<std::int32_t>(i);
    }
    return -1;
}
}
