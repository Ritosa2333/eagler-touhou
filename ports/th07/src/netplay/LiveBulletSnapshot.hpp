#pragma once

#include "BulletManager.hpp"
#include <eagler/netplay/PartitionedPoolJournal.hpp>
#include <array>
#include <cstddef>
#include <cstring>

namespace Netplay
{
using LiveBulletJournal = PartitionedPoolJournal<Bullet, 1024, 6>;

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
    // Normal + despawn may become active within this tick. The only spawn VM
    // that can execute is the one selected at spawn. Full-slot overwrite paths
    // explicitly Touch(AllParts) before changing any dormant VM or its padding.
    const std::uint32_t hot = (1u << 0) | (1u << 4) | (1u << 5);
    return state >= BULLET_SPAWNING_FAST && state <= BULLET_SPAWNING_SLOW
        ? hot | (1u << (state - 1)) : hot;
}

inline bool AnmPreviousUpdateIsByteNoop(const AnmVm &vm)
{
    // Match EVERY assignment in AnmVm::UpdatePrev. Bitwise comparisons retain
    // signed zero / NaN payloads; this is not approximate visual equivalence.
    return std::memcmp(&vm.prevRotation, &vm.rotation, sizeof(vm.rotation)) == 0 &&
        std::memcmp(&vm.prevScale, &vm.scale, sizeof(vm.scale)) == 0 &&
        std::memcmp(&vm.prevUvScrollPos, &vm.uvScrollPos, sizeof(vm.uvScrollPos)) == 0 &&
        std::memcmp(&vm.prevColor, &vm.color, sizeof(vm.color)) == 0 &&
        std::memcmp(&vm.prevColor2, &vm.color2, sizeof(vm.color2)) == 0 &&
        vm.prevUseColor2 == vm.useColor2 &&
        std::memcmp(&vm.prevPos, &vm.pos, sizeof(vm.pos)) == 0;
}

inline std::uint32_t LiveBulletMask(const Bullet &bullet, bool elideDormant)
{
    auto mask = LiveBulletMask(bullet.state);
    // While normal/spawning the despawn VM only receives UpdatePrev(), and
    // that write can be an exact byte no-op. Every transition to DESPAWN must
    // touch it before the first Draw/ExecuteScript; spawn and whole-slot clear
    // already touch the full slot. No animation or gameplay update is skipped.
    if (elideDormant && bullet.state != BULLET_DESPAWN &&
        AnmPreviousUpdateIsByteNoop(bullet.sprites.spriteSpawnEffectDonut))
        mask &= ~(1u << 4);
    return mask;
}
} // namespace Netplay
