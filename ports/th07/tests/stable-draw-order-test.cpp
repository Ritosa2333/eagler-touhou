#include "graphics/StableDrawOrder.hpp"
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <random>

struct Item { bool blend; std::uint8_t collision; std::int32_t texture; };
int main()
{
    static Graphics::StableDrawOrder<Item, 1024> sorter;
    std::array<Item, 1024> items{};
    std::array<Item *, 1024> actual{}, expected{};
    std::mt19937 random(71350);
    auto oldCompare = [](Item *a, Item *b) {
        if (a->blend != b->blend) return a->blend < b->blend;
        if (a->collision != b->collision) return a->collision < b->collision;
        return a->texture < b->texture;
    };
    for (unsigned run = 0; run < 6000; ++run)
    {
        const std::size_t count = run % 4 == 0 ? 1024 : random() % 1025;
        for (std::size_t i = 0; i < count; ++i)
        {
            items[i] = {bool(random() & 1), static_cast<std::uint8_t>(random() % 8),
                        static_cast<std::int32_t>(random() % 17) - 1};
            if (run % 5 == 0) items[i] = {false, 0, -1};
            if (run % 7 == 0) items[i].texture = static_cast<std::int32_t>(random());
            actual[i] = expected[i] = &items[i];
        }
        std::stable_sort(expected.begin(), expected.begin() + count, oldCompare);
        assert(sorter.Sort(actual.data(), count, [](Item *i) {
            return decltype(sorter)::BulletKey(i->blend, i->collision, i->texture);
        }));
        assert(std::equal(actual.begin(), actual.begin() + count, expected.begin()));
    }
    assert(!sorter.Sort(actual.data(), 1025, [](Item *) { return std::uint64_t(0); }));
    std::puts("stable draw order: PASS 6000 exact stable-reference comparisons");
}
