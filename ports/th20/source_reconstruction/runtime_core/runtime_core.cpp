#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "runtime_core.hpp"
#include <cstdio>
#include <cstdio>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <new>
#include <stdexcept>

namespace th20::source::runtime {
LockRegistry& shared_locks() noexcept { static LockRegistry registry; return registry; }
void LockRegistry::enter_tracked(std::size_t index) {
    if (enabled_) { slot(index).lock(); ++depth_.at(index); }
}
void LockRegistry::leave_tracked(std::size_t index) {
    if (enabled_) { --depth_.at(index); slot(index).unlock(); }
}
void* MemoryResource::do_allocate(std::size_t bytes, std::size_t alignment) {
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(1));
    return ::operator new(bytes, std::align_val_t(alignment));
}
void MemoryResource::do_deallocate(void* memory, std::size_t, std::size_t alignment) {
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(1));
    ::operator delete(memory, std::align_val_t(alignment));
}
bool MemoryResource::do_is_equal(const std::pmr::memory_resource&) const noexcept { return true; }
AllocationController* create_allocation_controller() { return new AllocationController; }
void destroy_allocation_controller(AllocationController* controller) { delete controller; }
std::pmr::memory_resource* set_default_resource(std::pmr::memory_resource* resource) noexcept {
    return std::pmr::set_default_resource(resource);
}
void* allocate_bytes(std::size_t bytes) noexcept {
    // 0x5584c0 uses max(size,1), rejects >0xffffffe0 and uses the CRT heap.
    // The current MSVC CRT supplies the same malloc/new-handler contract.
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(1));
    if (bytes > 0xffffffe0u) { errno = ENOMEM; return nullptr; }
    return std::malloc(bytes ? bytes : 1);
}
void release_bytes(void* memory) noexcept {
    if (!memory) return;
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(1));
    std::free(memory);
}
void* allocate_array(std::size_t bytes) {
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(1));
    return ::operator new[](bytes);
}
void clear_log(Log& log) noexcept { log.text.clear(); } // Original does not clear error flag.
void append_log(Log& log, const char* text) { log.text.append(text); }
void log_vprintf(Log& log, bool error, const char* format, std::va_list arguments) {
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(3));
    char buffer[1024]{};
    // Despite the Ghidra _vswprintf_s label, 0x54887b writes single bytes;
    // actual format literals are CP932 byte strings and the buffer is 0x400.
    ::vsprintf_s(buffer, sizeof(buffer), format, arguments);
    append_log(log, buffer);
    if (error) log.error = 1;
}
void log_printf(Log& log, const char* format, ...) {
    std::va_list arguments; va_start(arguments, format);
    log_vprintf(log, false, format, arguments); va_end(arguments);
}
void log_error(Log& log, const char* format, ...) {
    std::va_list arguments; va_start(arguments, format);
    log_vprintf(log, true, format, arguments); va_end(arguments);
}
void finish_log(Log& log) {
    if (log.text.empty()) return;
    log_printf(log, "---------------------------------------------------------- \r\n");
    if (log.error) {
#ifdef TH_SDL3
        // The error log is the whole channel on the browser runtime (stderr
        // reaches the page console); there is no modal dialog to block on.
        std::fprintf(stderr, "%s\n", log.text.c_str());
#else
        wchar_t buffer[8000];
        const auto converted = MultiByteToWideChar(932, 0, log.text.c_str(), -1, buffer, 8000);
        if (!converted) throw std::runtime_error("Log text exceeds the original 8000-character CP932 display buffer");
        MessageBoxW(nullptr, buffer, L"log", MB_ICONERROR);
#endif
    }
}
scheduler::State* create_scheduler() {
    auto* state = new scheduler::State;
    scheduler::initialize_state(*state);
    return state;
}
void destroy_scheduler(scheduler::State* state, scheduler::Environment& environment,
                       const std::function<void()>& renderer_flush) {
    if (!state) return;
    if (!renderer_flush) throw std::invalid_argument("Renderer flush dependency is required for scheduler teardown");
    renderer_flush();
    scheduler::shutdown_chains(*state, environment);
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(1));
    delete state;
}
}
