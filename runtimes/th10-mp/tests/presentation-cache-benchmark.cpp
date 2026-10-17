// Fixture-free cache microbenchmark. It does not measure game FPS or a phone.
#ifndef TH10_CACHE_HEADER
#define TH10_CACHE_HEADER "../th10_web/cpp/multiplayer/PresentationCache.hpp"
#endif
#include TH10_CACHE_HEADER
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>

struct Key {
    std::uintptr_t address = 0;
    static std::uint64_t comparisons;
    bool operator==(const Key& other) const noexcept {
        ++comparisons;
        return address == other.address;
    }
};
std::uint64_t Key::comparisons = 0;
struct Hash { std::size_t operator()(Key key) const noexcept { return key.address; } };
struct Sample { std::array<std::uint32_t,32> words{}; };
using Cache = th10::multiplayer::PresentationCache<Key,Sample,4096,Hash>;

int main() {
    for (const auto counts : {std::array<int,2>{256,64},{1200,1200},{3500,1200},{4096,1200}}) {
        for (int trial = 0; trial < 5; ++trial) {
            Cache cache;
            Sample sample{};
            Key::comparisons = 0;
            std::uint64_t accepted = 0, rejected = 0, checksum = 0;
            const auto begin = std::chrono::steady_clock::now();
            for (int frame = 0; frame < 6; ++frame) {
                cache.clear();
                for (int i = 0; i < counts[0]; ++i) {
                    sample.words[0] = std::uint32_t(i + frame);
                    const auto result = cache.try_emplace({0x100000u + std::uintptr_t(i) * 0x3ac}, sample);
                    assert(result.inserted);++accepted;
                }
                // Repeated first-sample attempts also occur during a rollback
                // burst before the next forward snapshot clears the cache.
                for (int replay = 0; replay < 12; ++replay) {
                    for (int i = 0; i < counts[0]; ++i) {
                        const auto result = cache.try_emplace({0x100000u + std::uintptr_t(i) * 0x3ac}, sample);
                        assert(result.value);checksum += result.value->words[0];
                    }
                    for (int i = 0; i < counts[1]; ++i) {
                        sample.words[0] = std::uint32_t(i);
                        const auto result = cache.try_emplace({0x1000000u + std::uintptr_t(i) * 0x7f0}, sample);
                        if (result.inserted) ++accepted;
                        if (result.overflow) ++rejected;
                        if (result.value) checksum += result.value->words[0];
                    }
                }
            }
            const double millis = std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now() - begin).count();
            std::printf("{\"registry\":%d,\"embedded\":%d,\"trial\":%d,\"ms\":%.6f,\"comparisons\":%llu,\"accepted\":%llu,\"rejected\":%llu,\"checksum\":%llu,\"cacheBytes\":%zu}\n",
                counts[0],counts[1],trial,millis,(unsigned long long)Key::comparisons,
                (unsigned long long)accepted,(unsigned long long)rejected,(unsigned long long)checksum,sizeof(Cache));
        }
    }
}
