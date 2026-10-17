#include "Rng.hpp"
namespace th08 {
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
Extended Rng::unit() noexcept {
    return Extended::from_int64(next32())/number(4294967296.0f);
}
Extended Rng::signed_unit() noexcept {
    return Extended::from_int64(next32())/number(2147483648.0f)-number(1.0f);
}
}
