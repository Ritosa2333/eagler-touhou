#include "BulletManager.hpp"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <random>

// BulletTypeSprites owns real AnmVm instances whose timers call Initialize in
// their constructors. This test never advances a timer, so keep the fixture
// independent from Supervisor timing by providing the exact initializer only.
void ZunTimer::Initialize()
{
    current = 0;
    previous = -1;
    subFrame = 0.0f;
}

static AnmVm &Visible(BulletTypeSprites &s, BulletState state)
{
    if (state == BULLET_STATE_SPAWNING_FAST) return s.spriteSpawnEffectFast;
    if (state == BULLET_STATE_SPAWNING_NORMAL) return s.spriteSpawnEffectNormal;
    if (state == BULLET_STATE_SPAWNING_SLOW) return s.spriteSpawnEffectSlow;
    if (state == BULLET_STATE_DESPAWNING) return s.spriteSpawnEffectDonut;
    return s.spriteBullet;
}

static void Mutate(AnmVm &vm, unsigned seed)
{
    vm.rotation = ZunVec3(float(seed % 17), float(seed % 13), float(seed % 31));
    vm.scaleX = float(seed % 43);
    vm.scaleY = float(seed % 47);
    vm.uvScrollPos.x = float(seed % 53);
    vm.uvScrollPos.y = float(seed % 59);
    vm.color = seed * 113;
    vm.pos = ZunVec3(float(seed % 67), float(seed % 71), float(seed % 73));
}

int main()
{
    std::mt19937 rng(71341);
    for (unsigned run = 0; run < 3000; ++run)
    {
        BulletTypeSprites original{}, optimized{};
        BulletState state = static_cast<BulletState>(1 + rng() % 4);
        for (BulletState s : {BULLET_STATE_FIRED, BULLET_STATE_SPAWNING_FAST,
             BULLET_STATE_SPAWNING_NORMAL, BULLET_STATE_SPAWNING_SLOW, BULLET_STATE_DESPAWNING})
            Mutate(Visible(original, s), rng());
        original.UpdatePrev(); optimized = original;
        for (unsigned tick = 0; tick < 100; ++tick)
        {
            original.UpdatePrev(); optimized.UpdateLivePrev(state);
            // Spawn can complete or a collision/Bomb can start despawn in
            // this tick, after publishing endpoints but before the next draw.
            if (state >= BULLET_STATE_SPAWNING_FAST && state <= BULLET_STATE_SPAWNING_SLOW && rng() % 7 == 0)
                state = BULLET_STATE_FIRED;
            if (rng() % 23 == 0) state = BULLET_STATE_DESPAWNING;
            const auto value = rng();
            Mutate(Visible(original, state), value);
            Mutate(Visible(optimized, state), value);
            assert(std::memcmp(&Visible(original, state), &Visible(optimized, state), sizeof(AnmVm)) == 0);
        }
    }
    std::puts("live bullet presentation: PASS 300000 real-AnmVm observable-state comparisons");
}
