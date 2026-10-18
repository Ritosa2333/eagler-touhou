#include "Rng.hpp"
namespace th10 {
u16 Rng::next_word() noexcept {
    const u16 mixed = static_cast<u16>((seed ^ 0x9630u) + 0x9aadu);
    seed = static_cast<u16>((mixed << 2) | (mixed >> 14));
    ++calls;
    return seed;
}
// 0x44b9e0 / 0x44ba30. The first generated word is the high word.
u32 Rng::next_u32() noexcept {
    const u32 high = next_word();
    return (high << 16) | next_word();
}
// 0x43cb80. A zero bound consumes no random numbers.
u32 Rng::bounded(u32 exclusive_maximum) noexcept {
    return exclusive_maximum ? next_u32() % exclusive_maximum : 0;
}
static Extended unsigned_number(u32 value) noexcept {
    i32 signed_value; std::memcpy(&signed_value, &value, sizeof(value));
    auto result = Extended::from_int(signed_value);
    if (signed_value < 0) result = result + number(4294967296.0f);
    return result;
}
// 0x44bb20 / 0x44bb90. Preserve rounding after unsigned conversion.
Extended Rng::unit() noexcept { return unsigned_number(next_u32()) * number(0x1p-32f); }
Extended Rng::signed_unit() noexcept { return unsigned_number(next_u32()) * number(0x1p-31f) - number(1.0f); }
}
