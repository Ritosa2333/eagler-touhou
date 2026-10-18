#pragma once

#include "BulletManager.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace Netplay
{
// Bullet rendering owns four 4x4 matrices inside every AnmVm. They are rebuilt
// by SetActiveSprite / the 3D presentation helpers and are never read by the
// TH07 bullet simulation, collision or canonical gameplay hash. Keep every
// other byte verbatim so script VM state and pointer identity remain exact.
constexpr std::size_t COMPACT_BULLET_BYTES =
    sizeof(Bullet) - 5 * 4 * sizeof(ZunMatrix);

inline void RebuildCompactBulletPresentationVm(AnmVm &vm)
{
    // The omitted matrices are derived presentation caches. Recreate the same
    // static basis SetActiveSprite() owns, then force the next Draw() to apply
    // the restored scale/rotation. ExecuteScript/gameplay never consume these
    // matrix values.
    vm.baseTransformMatrix.Identity();
    vm.uvMatrix.Identity();
    if (vm.sprite)
    {
        vm.baseTransformMatrix.m[0][0] = vm.sprite->widthPx / 256.0f;
        vm.baseTransformMatrix.m[1][1] = vm.sprite->heightPx / 256.0f;
        vm.uvMatrix.m[0][0] = vm.sprite->widthPx / vm.sprite->textureWidth * vm.sprite->cols;
        vm.uvMatrix.m[1][1] = vm.sprite->heightPx / vm.sprite->textureHeight * vm.sprite->rows;
    }
    vm.matrix = vm.baseTransformMatrix;
    vm.worldTransformMatrix = vm.baseTransformMatrix;
    if (vm.skipTransform == 0)
    {
        vm.worldTransformMatrix.m[0][0] *= vm.scale.x;
        vm.worldTransformMatrix.m[1][1] *= vm.scale.y;
        ZunMatrix rotation;
        if (vm.rotation.x != 0.0f)
        {
            rotation.RotateX(vm.rotation.x);
            vm.worldTransformMatrix *= rotation;
        }
        if (vm.rotation.y != 0.0f)
        {
            rotation.RotateY(vm.rotation.y);
            vm.worldTransformMatrix *= rotation;
        }
        if (vm.rotation.z != 0.0f)
        {
            rotation.RotateZ(vm.rotation.z);
            vm.worldTransformMatrix *= rotation;
        }
    }
}

inline bool PackCompactBullet(const Bullet &bullet, std::uint8_t *out, std::size_t capacity)
{
    if (!out || capacity < COMPACT_BULLET_BYTES)
        return false;
    const std::uint8_t *cursor = reinterpret_cast<const std::uint8_t *>(&bullet);
    const std::uint8_t *const end = cursor + sizeof(Bullet);
    const AnmVm *vms[] = {
        &bullet.sprites.spriteBullet,
        &bullet.sprites.spriteSpawnEffectFast,
        &bullet.sprites.spriteSpawnEffectNormal,
        &bullet.sprites.spriteSpawnEffectSlow,
        &bullet.sprites.spriteSpawnEffectDonut,
    };
    std::size_t offset = 0;
    for (const AnmVm *vm : vms)
    {
        const auto *holeBegin = reinterpret_cast<const std::uint8_t *>(&vm->baseTransformMatrix);
        const auto *holeEnd = reinterpret_cast<const std::uint8_t *>(&vm->uvMatrix) + sizeof(vm->uvMatrix);
        if (holeBegin < cursor || holeEnd < holeBegin || holeEnd > end)
            return false;
        const std::size_t bytes = static_cast<std::size_t>(holeBegin - cursor);
        if (offset + bytes > capacity)
            return false;
        std::memcpy(out + offset, cursor, bytes);
        offset += bytes;
        cursor = holeEnd;
    }
    const std::size_t tail = static_cast<std::size_t>(end - cursor);
    if (offset + tail != COMPACT_BULLET_BYTES || offset + tail > capacity)
        return false;
    std::memcpy(out + offset, cursor, tail);
    return true;
}

inline bool UnpackCompactBullet(Bullet &bullet, const std::uint8_t *data, std::size_t size)
{
    if (!data || size < COMPACT_BULLET_BYTES)
        return false;
    std::uint8_t *cursor = reinterpret_cast<std::uint8_t *>(&bullet);
    std::uint8_t *const end = cursor + sizeof(Bullet);
    AnmVm *vms[] = {
        &bullet.sprites.spriteBullet,
        &bullet.sprites.spriteSpawnEffectFast,
        &bullet.sprites.spriteSpawnEffectNormal,
        &bullet.sprites.spriteSpawnEffectSlow,
        &bullet.sprites.spriteSpawnEffectDonut,
    };
    std::size_t offset = 0;
    for (AnmVm *vm : vms)
    {
        auto *holeBegin = reinterpret_cast<std::uint8_t *>(&vm->baseTransformMatrix);
        auto *holeEnd = reinterpret_cast<std::uint8_t *>(&vm->uvMatrix) + sizeof(vm->uvMatrix);
        if (holeBegin < cursor || holeEnd < holeBegin || holeEnd > end)
            return false;
        const std::size_t bytes = static_cast<std::size_t>(holeBegin - cursor);
        if (offset + bytes > size)
            return false;
        std::memcpy(cursor, data + offset, bytes);
        offset += bytes;
        cursor = holeEnd;
    }
    const std::size_t tail = static_cast<std::size_t>(end - cursor);
    if (offset + tail != COMPACT_BULLET_BYTES || offset + tail > size)
        return false;
    std::memcpy(cursor, data + offset, tail);
    return true;
}

inline void RebuildCompactBulletPresentation(Bullet &bullet)
{
    AnmVm *vms[] = {
        &bullet.sprites.spriteBullet,
        &bullet.sprites.spriteSpawnEffectFast,
        &bullet.sprites.spriteSpawnEffectNormal,
        &bullet.sprites.spriteSpawnEffectSlow,
        &bullet.sprites.spriteSpawnEffectDonut,
    };
    for (AnmVm *vm : vms)
        RebuildCompactBulletPresentationVm(*vm);
}
} // namespace Netplay
