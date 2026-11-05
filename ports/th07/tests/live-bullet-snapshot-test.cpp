#include "netplay/LiveBulletSnapshot.hpp"
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>

int main()
{
    const auto pool = std::make_unique<Bullet[]>(1024);
    const auto before = std::make_unique<unsigned char[]>(sizeof(Bullet) * 1024);
    const auto parts = Netplay::LiveBulletParts();
    // Include pointer/padding/matrix bytes: this is byte restoration, not a
    // canonical hash that could accidentally leave dormant state untested.
    std::memset(pool.get(), 0x5a, sizeof(Bullet) * 1024);
    for (unsigned i = 0; i < 1024; ++i) pool[i].state = 1 + i % 5;
    std::memcpy(before.get(), pool.get(), sizeof(Bullet) * 1024);
    Netplay::LiveBulletJournal journal;
    assert(journal.Reset(pool.get(), parts, 8));
    assert(journal.BeginFrame(0));
    std::array<std::uint32_t, 1024> masks{};
    for (unsigned i = 0; i < 1024; ++i) masks[i] = Netplay::LiveBulletMask(pool[i].state);
    assert(journal.CaptureMasks(masks));
    for (unsigned i = 0; i < 1024; ++i)
    {
        const auto mask = Netplay::LiveBulletMask(pool[i].state);
        assert(journal.Touch(i, mask));
        for (unsigned p = 0; p < 6; ++p)
            if (mask & (1u << p))
                std::memset(reinterpret_cast<unsigned char *>(&pool[i]) + parts[p].offset,
                            static_cast<int>(i & 255), parts[p].size);
    }
    const auto activeBytes = journal.BytesForFrame(0);
    assert(activeBytes < sizeof(Bullet) * 1024);
    assert(journal.EndFrame());
    assert(journal.BeginFrame(1, true));
    for (unsigned i = 0; i < 1024; i += 3)
    {
        assert(journal.Touch(i, Netplay::LiveBulletJournal::AllParts));
        std::memset(&pool[i], 0, sizeof(Bullet)); // Bomb or slot reuse.
    }
    assert(journal.EndFrame());
    assert(journal.BeginFrame(2));
    for (unsigned i = 0; i < 1024; ++i)
    {
        assert(journal.Touch(i, Netplay::LiveBulletJournal::AllParts));
        std::memset(&pool[i], 0xac, sizeof(Bullet));
    }
    assert(journal.EndFrame());
    std::uint32_t restored = 99;
    assert(journal.UndoTo(1, &restored) && restored == 0);
    assert(std::memcmp(pool.get(), before.get(), sizeof(Bullet) * 1024) == 0);
    std::cout << "PASS live Bullet byte oracle, active=" << activeBytes
              << " full=" << sizeof(Bullet) * 1024 << " copied="
              << journal.RestoreCopiedBytes() << " skipped=" << journal.RestoreSkippedBytes() << '\n';
}
