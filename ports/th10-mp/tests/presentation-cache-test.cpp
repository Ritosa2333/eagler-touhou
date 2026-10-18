#include "../th10_web/cpp/multiplayer/PresentationCache.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <array>
#include <cstdint>
#include <unordered_map>

namespace {

struct Sample {
    int value = 0;
};

struct ConstantHash {
    std::size_t operator()(int) const noexcept { return 0; }
};

struct CountedKey {
    std::uintptr_t address = 0;
    static std::size_t comparisons;
    bool operator==(const CountedKey& other) const noexcept {
        ++comparisons;
        return address == other.address;
    }
};
std::size_t CountedKey::comparisons = 0;
struct AddressHash {
    std::size_t operator()(CountedKey key) const noexcept { return key.address; }
};
struct Hash32 {
    std::uint32_t operator()(int key) const noexcept { return std::uint32_t(key); }
};

void dense_misses() {
    using Dense = th10::multiplayer::PresentationCache<CountedKey,Sample,4096,AddressHash>;
    Dense cache;
    // Same 32-bit strides as TH10's registry AnmVm and EnemyBullet pools.
    for (std::size_t i = 0; i < 4096; ++i)
        assert(cache.try_emplace({0x100000 + i * 0x3ac},{int(i)}).inserted);
    CountedKey::comparisons = 0;
    for (std::size_t i = 0; i < 1200; ++i) {
        CountedKey key{0x1000000 + i * 0x7f0};
        assert(cache.find(key) == nullptr);
        assert(cache.try_emplace(key,{0}).overflow);
    }
    // Deterministic work bound for this source-derived dense address set,
    // not a wall-clock threshold or a universal bound on adversarial hashes.
    assert(CountedKey::comparisons < 24000);
    assert(cache.size() == 4096 && cache.overflow_count() == 1200);
    for (std::size_t i = 0; i < 4096; ++i)
        assert(cache.find({0x100000 + i * 0x3ac})->value == int(i));
}

void lazy_samples() {
    th10::multiplayer::PresentationCache<int,Sample,1,Hash32> cache;
    int made = 0;
    auto factory = [&]() { return Sample{++made}; };
    assert(cache.try_emplace_with(1,factory).inserted);
    assert(!cache.try_emplace_with(1,factory).inserted);
    assert(cache.try_emplace_with(2,factory).overflow);
    assert(made == 1 && cache.find(1)->value == 1);
    cache.clear();
    assert(cache.find(1) == nullptr);
    assert(cache.try_emplace_with(1,factory).value->value == 2);
    assert(made == 2);
}

void parity_and_lifetimes() {
    // Non-power-of-two capacity and intentionally worst-case collisions.
    th10::multiplayer::PresentationCache<int,Sample,7,ConstantHash> cache;
    std::unordered_map<int,int> expected;
    std::uint32_t random = 0x721fc9;
    std::size_t overflow = 0;
    for (int operation = 0; operation < 50000; ++operation) {
        random = random * 1664525u + 1013904223u;
        if ((random & 127u) == 0) {
            cache.clear();expected.clear();overflow = 0;
        }
        const int key = int((random >> 8) % 20);
        const auto old = expected.find(key);
        const bool inserted = old == expected.end() && expected.size() < 7;
        const bool full = old == expected.end() && expected.size() == 7;
        if (inserted) expected.emplace(key,operation);
        if (full) ++overflow;
        const auto result = cache.try_emplace(key,{operation});
        assert(result.inserted == inserted && result.overflow == full);
        assert(cache.size() == expected.size() && cache.overflow_count() == overflow);
        for (int lookup = 0; lookup < 20; ++lookup) {
            const auto value = expected.find(lookup);
            const auto* found = static_cast<const decltype(cache)&>(cache).find(lookup);
            assert((found == nullptr) == (value == expected.end()));
            if (found) assert(found->value == value->second);
        }
    }
    // Reuse of the same storage address after clear is a new incarnation.
    int owner = 0;
    th10::multiplayer::PresentationCache<const int*,Sample,3> pointers;
    auto* first = pointers.try_emplace(&owner,{11}).value;
    std::array<int,2> others{};
    pointers.try_emplace(&others[0],{12});
    pointers.try_emplace(&others[1],{13});
    assert(first == pointers.find(&owner) && first->value == 11);
    pointers.reset();
    assert(pointers.find(&owner) == nullptr);
    assert(pointers.try_emplace(&owner,{99}).value->value == 99);
}

} // namespace

int main() {
    dense_misses();
    lazy_samples();
    parity_and_lifetimes();
    using Cache = th10::multiplayer::PresentationCache<int,Sample,4,ConstantHash>;
    Cache cache;

    const auto first = cache.try_emplace(1,{10});
    assert(first.inserted && !first.overflow && first.value->value == 10);
    const auto duplicate = cache.try_emplace(1,{99});
    assert(!duplicate.inserted && !duplicate.overflow && duplicate.value->value == 10);

    // Every key hashes to the same bucket, so this exercises linear probing.
    for (int key = 2; key <= 4; ++key) {
        const auto result = cache.try_emplace(key,{key * 10});
        assert(result.inserted && !result.overflow);
    }
    assert(cache.size() == cache.capacity());
    assert(cache.find(1)->value == 10 && cache.find(4)->value == 40);

    const auto full = cache.try_emplace(5,{50});
    assert(!full.inserted && full.overflow && full.value == nullptr);
    assert(cache.overflow_count() == 1);
    // A full cache still finds duplicate keys and retains their first value.
    const auto fullDuplicate = cache.try_emplace(4,{999});
    assert(!fullDuplicate.inserted && !fullDuplicate.overflow);
    assert(fullDuplicate.value->value == 40 && cache.overflow_count() == 1);

    const auto old_generation = cache.generation();
    cache.clear();
    assert(cache.generation() != old_generation && cache.size() == 0);
    assert(cache.overflow_count() == 0 && cache.find(1) == nullptr);
    const auto reused = cache.try_emplace(5,{500});
    assert(reused.inserted && reused.value->value == 500);

    cache.reset();
    assert(cache.size() == 0 && cache.find(5) == nullptr);

#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    std::puts("presentation cache MP syntax and behavior: PASS");
#else
    std::puts("presentation cache SP syntax and behavior: PASS");
#endif
}
