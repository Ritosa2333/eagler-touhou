#pragma once
#include <cstdint>
#include <random>
namespace PracticeRuntime {
// Purple's Windows default_random_engine is MSVC mt19937. Preserve MSVC STL's
// uniform_int_distribution(0,640), whose range reduction differs from libc++.
// Reduction follows MSVC _Rng_from_urng_v2 (Apache-2.0 WITH LLVM-exception).
inline int PurpleDollsRandomInsert(std::uint16_t seed) {
    std::mt19937 engine(seed);
    constexpr std::uint32_t range=641;
    std::uint64_t product=std::uint64_t(std::uint32_t(engine()))*range;
    std::uint32_t remainder=std::uint32_t(product);
    if(remainder<range) {
        constexpr std::uint32_t threshold=(std::uint32_t(0)-range)%range;
        while(remainder<threshold) {
            product=std::uint64_t(std::uint32_t(engine()))*range;
            remainder=std::uint32_t(product);
        }
    }
    return int(product>>32);
}
}
