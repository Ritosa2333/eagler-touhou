#include "BulletManager.hpp"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <random>

static AnmVm &Visible(BulletTypeSprites &s, BulletState state)
{
    if (state == BULLET_SPAWNING_FAST) return s.spriteSpawnEffectFast;
    if (state == BULLET_SPAWNING_NORMAL) return s.spriteSpawnEffectNormal;
    if (state == BULLET_SPAWNING_SLOW) return s.spriteSpawnEffectSlow;
    if (state == BULLET_DESPAWN) return s.spriteSpawnEffectDonut;
    return s.spriteBullet;
}

static void Mutate(AnmVm &vm, unsigned seed)
{
    vm.rotation = {float(seed % 17), float(seed % 13), float(seed % 31)};
    vm.scale = {float(seed % 43), float(seed % 47)};
    vm.uvScrollPos = {float(seed % 53), float(seed % 59)};
    vm.color.color = seed * 113;
    vm.color2.color = seed * 217;
    vm.useColor2 = seed & 1;
    vm.pos = {float(seed % 67), float(seed % 71), float(seed % 73)};
}

int main()
{
    std::mt19937 rng(71341);
    for (unsigned run = 0; run < 3000; ++run)
    {
        BulletTypeSprites original{}, optimized{};
        BulletState state = static_cast<BulletState>(1 + rng() % 4);
        for (BulletState s : {BULLET_NORMAL, BULLET_SPAWNING_FAST,
             BULLET_SPAWNING_NORMAL, BULLET_SPAWNING_SLOW, BULLET_DESPAWN})
            Mutate(Visible(original, s), rng());
        original.UpdatePrev(); optimized = original;
        for (unsigned tick = 0; tick < 100; ++tick)
        {
            original.UpdatePrev(); optimized.UpdateLivePrev(state);
            // Spawn can complete or a collision/Bomb can start despawn in
            // this tick, after publishing endpoints but before the next draw.
            if (state >= BULLET_SPAWNING_FAST && state <= BULLET_SPAWNING_SLOW && rng() % 7 == 0)
                state = BULLET_NORMAL;
            if (rng() % 23 == 0) state = BULLET_DESPAWN;
            const auto value = rng();
            Mutate(Visible(original, state), value);
            Mutate(Visible(optimized, state), value);
            assert(std::memcmp(&Visible(original, state), &Visible(optimized, state), sizeof(AnmVm)) == 0);
        }
    }
    std::puts("live bullet presentation: PASS 300000 real-AnmVm observable-state comparisons");
}
