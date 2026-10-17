#include "netplay/LiveBulletSnapshot.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>

int main()
{
    // Check the guard against the real UpdatePrev implementation, including
    // arbitrary object bytes. No float approximation, padding or NaN elision.
    std::uint32_t rng = 77135;
    auto next = [&]() { rng ^= rng << 13; rng ^= rng >> 17; rng ^= rng << 5; return rng; };
    for (unsigned test = 0; test < 3000; ++test)
    {
        AnmVm vm;
        auto *bytes = reinterpret_cast<unsigned char *>(&vm);
        for (std::size_t i = 0; i < sizeof(vm); ++i) bytes[i] = next() & 255;
        if (test % 3) vm.UpdatePrev();
        if (test % 3 == 2) reinterpret_cast<unsigned char *>(&vm.prevColor)[0] ^= 1u;
        unsigned char before[sizeof(AnmVm)];
        std::memcpy(before, &vm, sizeof(vm));
        const bool predictedNoop = Netplay::AnmPreviousUpdateIsByteNoop(vm);
        vm.UpdatePrev();
        assert(predictedNoop == (std::memcmp(before, &vm, sizeof(vm)) == 0));
    }
    AnmVm zero{};
    zero.scale.x = 0.0f; zero.UpdatePrev(); zero.prevScale.x = -0.0f;
    assert(!Netplay::AnmPreviousUpdateIsByteNoop(zero));

    const auto pool = std::make_unique<Bullet[]>(1024);
    const auto before = std::make_unique<unsigned char[]>(sizeof(Bullet) * 1024);
    std::array<std::uint32_t, 1024> masks{};
    for (unsigned i = 0; i < 1024; ++i)
    {
        std::memset(&pool[i], 0, sizeof(Bullet));
        pool[i].state = BULLET_NORMAL;
        auto &vm = pool[i].sprites.spriteSpawnEffectDonut;
        vm.rotation.x = static_cast<float>(i);
        vm.UpdatePrev();
        masks[i] = Netplay::LiveBulletMask(pool[i], true);
        assert(!(masks[i] & (1u << 4)));
    }
    std::memcpy(before.get(), pool.get(), sizeof(Bullet) * 1024);
    Netplay::LiveBulletJournal journal;
    assert(journal.Reset(pool.get(), Netplay::LiveBulletParts(), 4));
    assert(journal.BeginFrame(0));
    assert(journal.CaptureMasks(masks));
    const auto captured = journal.BytesForFrame(0);
    for (unsigned i = 0; i < 1024; ++i) pool[i].sprites.UpdateLivePrev(pool[i].state);
    assert(journal.EndFrame());
    assert(journal.BeginFrame(1, true));
    // Promotion must save the untouched dormant bytes BEFORE the first Draw.
    for (unsigned i = 0; i < 1024; i += 5)
    {
        assert(journal.Touch(i, 1u << 4));
        pool[i].state = BULLET_DESPAWN;
        pool[i].sprites.spriteSpawnEffectDonut.pos.x = 91.0f;
        pool[i].sprites.UpdateLivePrev(pool[i].state);
        assert(Netplay::LiveBulletMask(pool[i], true) & (1u << 4));
    }
    assert(journal.EndFrame());
    assert(journal.BeginFrame(2, true));
    assert(journal.Touch(2, Netplay::LiveBulletJournal::AllParts));
    std::memset(&pool[2], 0xc7, sizeof(Bullet)); // Whole-slot Bomb clear/reuse.
    assert(journal.EndFrame());
    std::uint32_t restored = 99;
    assert(journal.UndoTo(2, &restored) && restored == 0);
    assert(std::memcmp(before.get(), pool.get(), sizeof(Bullet) * 1024) == 0);
    std::cout << "PASS dormant VM: 3000 byte-noop guards, signed zero, promotion/clear; capture="
              << captured << " full=" << sizeof(Bullet) * 1024 << '\n';
}
