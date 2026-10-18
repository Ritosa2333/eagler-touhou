#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <intrin.h>
#include "bridge.hpp"
#include "native_core.hpp"

static_assert(sizeof(void*) == 4, "This bridge is the PE32/x86 game ABI");
namespace r = th20::recovered;

// The .def file supplies undecorated, stable names for the PE import table.
extern "C" {
Th20BridgeStats g_th20_bridge_stats = {
    th20_bridge_magic, 1, sizeof(Th20BridgeStats), 1, 0,
    th20_clock_pointer_rva, {0, 0, 0, 0, 0, 0, 0}
};
}

namespace {
inline void count(unsigned index) noexcept {
    _InterlockedIncrement(&g_th20_bridge_stats.counts[index]);
}
inline const float* clock_rate() noexcept {
    // The original global contains a pointer, and the rate is its first float.
    // Resolve it for each call: the game owns and can replace the clock object.
    return *reinterpret_cast<const float* const*>(
        g_th20_bridge_stats.main_module_base + th20_clock_pointer_rva);
}
}

extern "C" std::uint32_t __cdecl th20_lcg_step(std::uint32_t state) noexcept {
    count(0);
    return r::lcg_step(state);
}
extern "C" std::uint32_t __fastcall th20_lcg_next(std::uint32_t* state, void*) noexcept {
    count(1);
    return r::lcg_next(*state);
}
extern "C" r::Timer* __fastcall th20_timer_reset(r::Timer* self, void*) noexcept {
    count(2);
    r::timer_reset(*self);
    return self;
}
extern "C" r::Timer* __fastcall th20_timer_mode(r::Timer* self, void*, std::uint32_t mode) noexcept {
    count(3);
    r::timer_mode(*self, mode);
    return self;
}
extern "C" r::Timer* __fastcall th20_timer_set(r::Timer* self, void*, std::int32_t value) noexcept {
    count(4);
    r::timer_set(*self, value);
    return self;
}
extern "C" r::Timer* __fastcall th20_timer_add(r::Timer* self, void*, float delta) noexcept {
    count(5);
    r::timer_add(*self, delta, clock_rate());
    return self;
}
extern "C" std::int32_t __fastcall th20_timer_tick(r::Timer* self, void*) noexcept {
    count(6);
    return r::timer_tick(*self, clock_rate());
}

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        g_th20_bridge_stats.main_module_base =
            reinterpret_cast<std::uint32_t>(GetModuleHandleW(nullptr));
    }
    return TRUE;
}
