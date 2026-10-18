#include "netplay/CompactBulletSnapshot.hpp"

#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

static void CopyMatrices(Bullet &dst, const Bullet &src)
{
    AnmVm *d[] = {&dst.sprites.spriteBullet, &dst.sprites.spriteSpawnEffectFast,
                  &dst.sprites.spriteSpawnEffectNormal, &dst.sprites.spriteSpawnEffectSlow,
                  &dst.sprites.spriteSpawnEffectDonut};
    const AnmVm *s[] = {&src.sprites.spriteBullet, &src.sprites.spriteSpawnEffectFast,
                        &src.sprites.spriteSpawnEffectNormal, &src.sprites.spriteSpawnEffectSlow,
                        &src.sprites.spriteSpawnEffectDonut};
    for (unsigned i = 0; i < 5; ++i)
    {
        d[i]->baseTransformMatrix = s[i]->baseTransformMatrix;
        d[i]->matrix = s[i]->matrix;
        d[i]->worldTransformMatrix = s[i]->worldTransformMatrix;
        d[i]->uvMatrix = s[i]->uvMatrix;
    }
}

static void FillMatrices(Bullet &bullet, unsigned char value)
{
    AnmVm *vms[] = {&bullet.sprites.spriteBullet, &bullet.sprites.spriteSpawnEffectFast,
                    &bullet.sprites.spriteSpawnEffectNormal, &bullet.sprites.spriteSpawnEffectSlow,
                    &bullet.sprites.spriteSpawnEffectDonut};
    for (AnmVm *vm : vms)
        std::memset(&vm->baseTransformMatrix, value, sizeof(ZunMatrix) * 4);
}

int main()
{
    static_assert(Netplay::COMPACT_BULLET_BYTES < sizeof(Bullet));
    std::mt19937 rng(0x07c0ffeeu);
    std::vector<std::uint8_t> packed(Netplay::COMPACT_BULLET_BYTES);
    for (unsigned round = 0; round < 2000; ++round)
    {
        Bullet source{};
        Bullet restored{};
        auto *bytes = reinterpret_cast<unsigned char *>(&source);
        for (std::size_t i = 0; i < sizeof(Bullet); ++i)
            bytes[i] = static_cast<unsigned char>(rng());
        std::memset(&restored, 0x5a, sizeof(restored));
        assert(Netplay::PackCompactBullet(source, packed.data(), packed.size()));
        assert(Netplay::UnpackCompactBullet(restored, packed.data(), packed.size()));

        // Matrix bytes are intentionally not restored; once those presentation
        // bytes are normalized, every simulation/script byte must match exactly.
        CopyMatrices(restored, source);
        assert(std::memcmp(&source, &restored, sizeof(Bullet)) == 0);

        FillMatrices(restored, 0xa5);
        assert(Netplay::UnpackCompactBullet(restored, packed.data(), packed.size()));
        Bullet expected = source;
        FillMatrices(expected, 0xa5);
        assert(std::memcmp(&expected, &restored, sizeof(Bullet)) == 0);
    }
    std::printf("compact bullet snapshot: PASS 2000 byte-exact restores; %zu -> %zu bytes\n",
                sizeof(Bullet), Netplay::COMPACT_BULLET_BYTES);
}
