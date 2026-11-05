#include "Rng.hpp"
namespace th09 {
u16 Rng::next16() noexcept {
    const u16 mixed=u16((seed^0x9630u)-0x6553u);
    seed=u16((mixed<<2)|(mixed>>14));
    ++calls;
    return seed;
}
u32 Rng::next32() noexcept {
    const u32 high=next16();
    return (high<<16)|next16();
}
}
