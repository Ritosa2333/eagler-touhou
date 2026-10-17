#pragma once
#include <cstdint>

// Public PE export layout. All fields are little-endian uint32_t, including
// counters read from another process. Function order matches the patched RVAs.
struct Th20BridgeStats {
    std::uint32_t magic;
    std::uint32_t version;
    std::uint32_t size;
    std::uint32_t source_status;
    std::uint32_t main_module_base;
    std::uint32_t clock_pointer_rva;
    volatile long counts[7];
};
static_assert(sizeof(Th20BridgeStats) == 52);
constexpr std::uint32_t th20_bridge_magic = 0x42303254;
constexpr std::uint32_t th20_clock_pointer_rva = 0x1aefe0;
