#pragma once
#include <cstdint>
namespace th20::source::progress {
// Original CR record, constructed at 50af20. Fields still being recovered retain
// their exact byte representation; no unrelated substitute progress storage.
struct Profile { std::uint8_t bytes[0x7ae8]; };
static_assert(sizeof(Profile)==0x7ae8);
}
