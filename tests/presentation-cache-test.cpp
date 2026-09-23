#include "../th10_web/cpp/multiplayer/PresentationCache.hpp"

#include <cassert>
#include <cstddef>
#include <cstdio>

namespace {

struct Sample {
    int value = 0;
};

struct ConstantHash {
    std::size_t operator()(int) const noexcept { return 0; }
};

} // namespace

int main() {
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
