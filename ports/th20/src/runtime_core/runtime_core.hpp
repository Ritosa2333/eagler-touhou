#pragma once
#include <array>
#include <cstdarg>
#include <cstdint>
#include <functional>
#include <memory_resource>
#include <mutex>
#include <string>
#include "scheduler.hpp"

namespace th20::source::runtime {
// Original 0x452d80/0x452e00: 22 recursive locks, followed by 22 tracked
// nesting bytes and one enabled byte. Standard C++ mutexes implement the actual
// lock behavior; no original CRT machine code is used.
class LockRegistry {
public:
    static constexpr std::size_t count = 22;
    std::recursive_mutex& slot(std::size_t index) { return locks_.at(index); }
    void enable() noexcept { enabled_ = true; }                 // 0x41ccc0
    void disable() noexcept { enabled_ = false; }               // 0x41ca30
    bool enabled() const noexcept { return enabled_; }
    void enter_tracked(std::size_t);                           // 0x412550
    void leave_tracked(std::size_t);                           // 0x412750
    std::uint8_t depth(std::size_t index) const { return depth_.at(index); }
    scheduler::Environment scheduler_environment() {
        return scheduler::Environment(slot(0), slot(1), enabled_, depth_[0]);
    }
private:
    std::array<std::recursive_mutex, count> locks_;
    std::array<std::uint8_t, count> depth_{};
    bool enabled_ = false;
};
LockRegistry& shared_locks() noexcept;

// The object previously mislabeled a runtime listener is a polymorphic memory
// resource: vtable 0x56c920 = destructor, aligned allocate/deallocate, is_equal.
class MemoryResource final : public std::pmr::memory_resource {
public:
    MemoryResource() = default;                                // 0x418db0
    ~MemoryResource() override = default;                      // 0x418e90
private:
    void* do_allocate(std::size_t, std::size_t) override;        // 0x41f6d0
    void do_deallocate(void*, std::size_t, std::size_t) override; // 0x41f760
    bool do_is_equal(const std::pmr::memory_resource&) const noexcept override; // 0x41c9f0
};
struct AllocationController {
    std::uint32_t reserved = 0;
    MemoryResource resource;
};
static_assert(sizeof(void*) == 4, "Original runtime object layouts require x86");
static_assert(sizeof(MemoryResource) == 4 && sizeof(AllocationController) == 8);
AllocationController* create_allocation_controller();         // 0x41f5b0 composition
void destroy_allocation_controller(AllocationController*);    // 0x419320
std::pmr::memory_resource* set_default_resource(std::pmr::memory_resource*) noexcept; // 0x41e770/0x541562
void* allocate_bytes(std::size_t) noexcept;                    // 0x41f610
void release_bytes(void*) noexcept;                           // 0x41f670/0x40d840
void* allocate_array(std::size_t);                            // 0x40d8c0 successful allocation path

// Original +0 allocator resource, +4 SSO bytes, +0x14 length, +0x18 capacity,
// +0x1c error flag: this is pmr::string, not a wide string or stream.
struct Log {
    std::pmr::string text;
    std::uint8_t error = 0;
};
// MSVC x86 STL layout evidence (pmr::string is 28 bytes there, 16 under
// libc++/wasm32). Binding on the Windows build; TH_SDL3 recompiles everything
// consistently with libc++ layouts.
#ifndef TH_SDL3
static_assert(sizeof(std::pmr::string) == 28 && sizeof(Log) == 32);
#endif
void clear_log(Log&) noexcept;                                // 0x419be0/0x41c0e0
void append_log(Log&, const char*);                           // 0x4530a0/0x454310
void log_vprintf(Log&, bool error, const char*, std::va_list); // 0x454150/0x454230
void log_printf(Log&, const char*, ...);
void log_error(Log&, const char*, ...);
void finish_log(Log&);                                        // 0x453220: actual CP932 MessageBoxW on errors

scheduler::State* create_scheduler();                         // 0x4187a0
// 0x4186c0 -> 0x418e40 -> 0x4125b0. Renderer flush is a required integration
// operation, passed explicitly until the owning renderer source is linked.
void destroy_scheduler(scheduler::State*, scheduler::Environment&,
                       const std::function<void()>& renderer_flush);
}
