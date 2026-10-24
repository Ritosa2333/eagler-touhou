#pragma once

#include <cstddef>
#include <cstdint>

// The simulation owns one fixed mspace and an optional external root object.
// The post-link Wasm pass journals the first write to each 32-byte block,
// including allocator metadata. Host services and output queues stay outside
// these regions; irreversible effects are published by the confirmed driver.
namespace Netplay::WriteJournal {
constexpr std::uint32_t InstrumentationVersion = 0x52424a31;
// Optional in-Wasm guard ABI: 32-byte blocks, one byte per first-write
// generation. Consumers must check this export before reading its metadata.
constexpr std::uint32_t DirtyGuardVersion = 0x52424431;
// Optional shared scalar hooks: one u32 address argument, fixed byte widths.
// This is separate from the in-Wasm dirty-tag metadata ABI above.
constexpr std::uint32_t ScalarHookVersion = 1;
bool Initialize(void* root, std::size_t rootBytes,
                std::size_t heapBytes = std::size_t(192) << 20);
bool Ready();

class GameScope {
public:
    GameScope();
    ~GameScope();
    GameScope(const GameScope&) = delete;
    GameScope& operator=(const GameScope&) = delete;
private:
    int previous_;
};
class SystemScope {
public:
    SystemScope();
    ~SystemScope();
    SystemScope(const SystemScope&) = delete;
    SystemScope& operator=(const SystemScope&) = delete;
private:
    int previous_;
};

// For a reviewed, host-only call graph: it must not write or release game
// memory, or invoke game callbacks. Keep ordinary SystemScope for mixed code.
// Frame boundaries remain open; nested scopes restore their prior tracing state.
class HostScope {
public:
    HostScope();
    ~HostScope();
    HostScope(const HostScope&) = delete;
    HostScope& operator=(const HostScope&) = delete;
private:
    SystemScope allocation_;
    std::uint32_t previous_;
};

void* Allocate(std::size_t bytes);
void* AllocateZeroed(std::size_t count, std::size_t size);
void* AllocateAligned(std::size_t alignment, std::size_t bytes);
void Release(void* pointer);
bool OwnedByGameHeap(const void* pointer);

bool BeginFrame(std::uint32_t frame);
bool EndFrame();
bool RestoreTo(std::uint32_t frame);
void DiscardBefore(std::uint32_t frame);
void Clear();
bool FrameOpen();
std::size_t FrameCount();
bool Failed();
const char* Error();
// Explicit preimages are useful for native tests; Wasm writes call this through
// eagler_rb_touch after every original operand has been evaluated.
void Touch(void* address, std::size_t bytes);

struct Statistics {
    std::uint32_t frames=0, last_blocks=0, peak_blocks=0;
    std::uint32_t restores=0, restored_frames=0;
    std::uint64_t touches=0, used_bytes=0, capacity_bytes=0;
    std::uint32_t heap_used_kib=0, heap_capacity_kib=0;
};
Statistics Stats();
// Constant-time seal accounting. Stats() may walk allocator chunks on its first
// heap query or after restore/cache invalidation. Ordinary allocation/free
// update the exact cached chunk total without rescanning.
std::uint32_t LastFrameBlocks();
}

extern "C" {
void eagler_rb_touch(std::uint32_t address, std::uint32_t bytes);
// Each fixed-width hook has touch(address, width) semantics. A post-link pass
// must check ScalarHookVersion and exclude all six scalar ABI exports from
// instrumentation before selecting them; the generic hook remains the default.
std::uint32_t eagler_rb_scalar_abi();
void eagler_rb_touch1(std::uint32_t address);
void eagler_rb_touch2(std::uint32_t address);
void eagler_rb_touch4(std::uint32_t address);
void eagler_rb_touch8(std::uint32_t address);
void eagler_rb_touch16(std::uint32_t address);
// Five words: tracking enabled, heap address/bytes, external root address/bytes.
std::uint32_t eagler_rb_config_address();
// Three words: tag array address, current generation (1..255 while tracking),
// and DirtyGuardVersion. Heap tags come first; root writes use the full hook.
std::uint32_t eagler_rb_dirty_config_address();
std::uint32_t eagler_rb_dirty_config_abi();
std::uint32_t eagler_rb_instrumented();
}
