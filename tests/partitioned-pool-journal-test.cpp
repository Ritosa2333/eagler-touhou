#include <eagler/netplay/PartitionedPoolJournal.hpp>
#include <array>
#include <cassert>
#include <cstring>
#include <deque>
#include <iostream>

struct Object { std::array<unsigned char, 128> bytes; };
using Journal = Netplay::PartitionedPoolJournal<Object, 17, 4>;
using Pool = std::array<Object, 17>;
constexpr std::array<Journal::Part, 4> parts{{{0, 8}, {8, 24}, {32, 32}, {64, 64}}};

int main()
{
    Pool pool{};
    Journal journal;
    assert(!journal.Reset(nullptr, parts, 7));
    auto invalid = parts; invalid[2].offset++;
    assert(!journal.Reset(pool.data(), invalid, 7));
    assert(journal.Reset(pool.data(), parts, 7));
    struct Saved { std::uint32_t first, last; Pool before; };
    std::deque<Saved> history;
    std::uint32_t random = 0x1e117u, frame = 0;
    auto next = [&]() { random ^= random << 13; random ^= random >> 17; random ^= random << 5; return random; };
    unsigned undos = 0;
    for (unsigned step = 0; step < 6000; ++step)
    {
        if (history.size() == 7) history.pop_front();
        Saved saved{frame, frame, pool};
        const unsigned length = 1 + next() % 4;
        for (unsigned tick = 0; tick < length; ++tick, ++frame)
        {
            assert(journal.BeginFrame(frame, tick != 0));
            std::array<std::uint32_t, 17> masks{};
            for (auto &mask : masks) mask = next() & Journal::AllParts;
            assert(journal.CaptureMasks(masks));
            for (unsigned write = 0; write < 40; ++write)
            {
                const auto slot = next() % 17, part = next() % 4;
                assert(journal.Touch(slot, 1u << part));
                assert(journal.Touch(slot, 1u << part));
                std::memset(pool[slot].bytes.data() + parts[part].offset,
                            static_cast<int>(next() & 255), parts[part].size);
            }
            assert(journal.EndFrame());
            saved.last = frame;
        }
        history.push_back(saved);
        assert(journal.BytesForFrame(saved.last) <= sizeof(Pool));
        if ((step % 3) == 0)
        {
            const auto index = next() % history.size();
            const Saved target = history[index];
            std::uint32_t restored = 0xffffffffu;
            assert(journal.UndoTo(target.last, &restored));
            assert(restored == target.first);
            assert(std::memcmp(pool.data(), target.before.data(), sizeof(Pool)) == 0);
            while (history.size() > index) history.pop_back();
            frame = restored;
            ++undos;
        }
        if (step % 11 == 0 && history.size() > 2)
        {
            const auto keep = history[1].first;
            journal.DiscardBefore(keep);
            while (!history.empty() && history.front().last < keep) history.pop_front();
        }
    }
    assert(!journal.Failed());
    // Parts first written only in a NEWER checkpoint must also be undone.
    assert(journal.Reset(pool.data(), parts, 3));
    const Pool original = pool;
    for (unsigned f = 0; f < 3; ++f)
    {
        assert(journal.BeginFrame(f));
        assert(journal.Touch(0, 1u << f));
        std::memset(pool[0].bytes.data() + parts[f].offset, 19 + f, parts[f].size);
        assert(journal.EndFrame());
    }
    assert(journal.UndoTo(0));
    assert(std::memcmp(pool.data(), original.data(), sizeof(Pool)) == 0);
    assert(!journal.UndoTo(0));
    std::cout << "PASS partitioned journal: 6000 checkpoints, " << undos
              << " exact-byte undos, extensions, ring reuse and later cold writes\n";
}
