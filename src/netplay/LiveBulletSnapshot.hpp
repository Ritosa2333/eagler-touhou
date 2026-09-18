#pragma once

#include "BulletManager.hpp"
#include "PartitionedPoolJournal.hpp"

#include <array>
#include <cstddef>

namespace Netplay
{
using LiveBulletJournal = PartitionedPoolJournal<Bullet, 640, 6>;

inline std::array<LiveBulletJournal::Part, 6> LiveBulletParts()
{
    static_assert(offsetof(Bullet, sprites) == 0);
    static_assert(offsetof(BulletTypeSprites, spriteBullet) == 0);
    static_assert(offsetof(BulletTypeSprites, spriteSpawnEffectFast) == sizeof(AnmVm));
    static_assert(offsetof(BulletTypeSprites, spriteSpawnEffectNormal) == 2 * sizeof(AnmVm));
    static_assert(offsetof(BulletTypeSprites, spriteSpawnEffectSlow) == 3 * sizeof(AnmVm));
    static_assert(offsetof(BulletTypeSprites, spriteSpawnEffectDonut) == 4 * sizeof(AnmVm));
    return {{{0, sizeof(AnmVm)}, {sizeof(AnmVm), sizeof(AnmVm)},
             {2 * sizeof(AnmVm), sizeof(AnmVm)}, {3 * sizeof(AnmVm), sizeof(AnmVm)},
             {4 * sizeof(AnmVm), sizeof(AnmVm)},
             {5 * sizeof(AnmVm), sizeof(Bullet) - 5 * sizeof(AnmVm)}}};
}

inline std::uint32_t LiveBulletMask(std::uint16_t state)
{
    // Keep the first TH06 version deliberately conservative: normal VM,
    // despawn/donut VM and the complete gameplay tail are always rewindable.
    // Only the two unselected spawn VMs stay live until slot reuse overwrites
    // the whole Bullet, which must Touch(AllParts) first.
    const std::uint32_t hot = (1u << 0) | (1u << 4) | (1u << 5);
    return state >= BULLET_STATE_SPAWNING_FAST && state <= BULLET_STATE_SPAWNING_SLOW
        ? hot | (1u << (state - 1)) : hot;
}
} // namespace Netplay
