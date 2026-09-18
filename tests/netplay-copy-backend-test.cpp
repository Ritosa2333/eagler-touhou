#include "netplay/RollbackJournal.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/heap.h>
#endif

int main()
{
    for (bool fast : {false, true})
    {
        Netplay::RollbackJournal journal;
        assert(journal.Reset({4, 2 * 1024 * 1024, 32, fast}));
        std::vector<unsigned char> world(1024 * 1024 + 64);
        for (std::size_t i = 0; i < world.size(); ++i)
            world[i] = static_cast<unsigned char>((i * 71 + i / 911) & 255);
        const std::array<std::size_t, 8> sizes{1, 1023, 1024, 1025, 4095, 16384, 65539, 300001};
        for (unsigned round = 0; round < 30; ++round)
        {
            auto before = world;
            assert(journal.BeginFrame(round * 2));
            std::size_t offset = round % 7;
            for (auto size : sizes)
            {
                assert(journal.Touch(world.data() + offset, size));
                for (std::size_t byte = 0; byte < size; ++byte)
                    world[offset + byte] ^= static_cast<unsigned char>(round + 87);
                offset += size + 3;
            }
            assert(journal.EndFrame());
#ifdef __EMSCRIPTEN__
            if (fast && round == 0)
            {
                // The bridge must use the current heap view, not a cached view
                // detached when ALLOW_MEMORY_GROWTH replaces the backing buffer.
                const auto heap = emscripten_get_heap_size();
                assert(emscripten_resize_heap(heap + 32 * 1024 * 1024));
                assert(emscripten_get_heap_size() >= heap + 32 * 1024 * 1024);
            }
#endif
            assert(journal.BeginFrame(round * 2 + 1, true));
            assert(journal.Touch(world.data() + round % 7, 1));
            world[round % 7] ^= 55;
            assert(journal.EndFrame());
            assert(journal.UndoTo(round * 2 + 1));
            assert(world == before);
        }
    }
    std::puts("copy backends: PASS small/large/unaligned/growth/undo bytes");
}
