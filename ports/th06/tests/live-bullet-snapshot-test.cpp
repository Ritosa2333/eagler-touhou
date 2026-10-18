#include "netplay/LiveBulletSnapshot.hpp"

#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>

// This byte oracle never advances game timers; satisfy the real AnmVm/Bullet
// constructors without pulling Supervisor timing into a storage unit test.
void ZunTimer::Initialize()
{
    current = 0;
    previous = -1;
    subFrame = 0.0f;
}

int main()
{
    constexpr unsigned Count = 640;
    const auto pool = std::make_unique<Bullet[]>(Count);
    const auto before = std::make_unique<unsigned char[]>(sizeof(Bullet) * Count);
    const auto parts = Netplay::LiveBulletParts();

    // Pointer, padding and presentation bytes all participate: rollback is an
    // exact byte restore, not merely equality of the canonical gameplay hash.
    std::memset(pool.get(), 0x5a, sizeof(Bullet) * Count);
    for (unsigned i = 0; i < Count; ++i)
        pool[i].state = static_cast<u16>(BULLET_STATE_FIRED + i % 5);
    std::memcpy(before.get(), pool.get(), sizeof(Bullet) * Count);

    Netplay::LiveBulletJournal journal;
    assert(journal.Reset(pool.get(), parts, 8));
    assert(journal.BeginFrame(0));
    std::array<std::uint32_t, Count> masks{};
    for (unsigned i = 0; i < Count; ++i)
        masks[i] = Netplay::LiveBulletMask(pool[i].state);
    assert(journal.CaptureMasks(masks));
    for (unsigned i = 0; i < Count; ++i)
    {
        const auto mask = masks[i];
        assert(journal.Touch(i, mask));
        for (unsigned part = 0; part < 6; ++part)
            if (mask & (1u << part))
                std::memset(reinterpret_cast<unsigned char *>(&pool[i]) + parts[part].offset,
                            static_cast<int>(i & 255), parts[part].size);
    }
    const auto activeBytes = journal.BytesForFrame(0);
    assert(activeBytes < sizeof(Bullet) * Count);
    assert(journal.EndFrame());

    assert(journal.BeginFrame(1, true));
    for (unsigned i = 0; i < Count; i += 3)
    {
        assert(journal.Touch(i, Netplay::LiveBulletJournal::AllParts));
        std::memset(&pool[i], 0, sizeof(Bullet)); // clear/reuse/Bomb-like writer
    }
    assert(journal.EndFrame());

    assert(journal.BeginFrame(2));
    for (unsigned i = 0; i < Count; ++i)
    {
        assert(journal.Touch(i, Netplay::LiveBulletJournal::AllParts));
        std::memset(&pool[i], 0xac, sizeof(Bullet));
    }
    assert(journal.EndFrame());

    std::uint32_t restored = 99;
    assert(journal.UndoTo(1, &restored) && restored == 0);
    assert(std::memcmp(pool.get(), before.get(), sizeof(Bullet) * Count) == 0);
    std::cout << "TH06 live Bullet byte oracle: PASS active=" << activeBytes
              << " full=" << sizeof(Bullet) * Count << " copied="
              << journal.RestoreCopiedBytes() << " skipped="
              << journal.RestoreSkippedBytes() << '\n';
}
