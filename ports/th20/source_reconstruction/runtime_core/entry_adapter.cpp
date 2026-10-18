#include "runtime_core.hpp"
#include "../program_entry/unrecovered_dependencies.hpp"
#include <new>

namespace th20::source::program_entry {
// These exact BSS/default initializers are established by original globals and
// 0x401210/0x401240. No unrecovered graphics/window state is defined here.
AllocationController* allocations = nullptr;
FunctionController* function_controller = nullptr;
LogBuffer log_buffer;
LockRegistry& lock_registry = runtime::shared_locks();
scheduler::Environment scheduler_environment = lock_registry.scheduler_environment();
namespace unrecovered {
void lock_registry_enable(LockRegistry& locks) { locks.enable(); }
void lock_registry_disable(LockRegistry& locks) { locks.disable(); }
void* allocate(std::uint32_t size) { return ::operator new(size); }
AllocationController* construct_allocations(void* memory) { return new (memory) AllocationController; }
void destroy_allocations(AllocationController* controller, int flags) {
    if (!controller) return;
    controller->~AllocationController();
    if (flags & 1) ::operator delete(controller);
}
void construct_listener(RuntimeListenerStorage& storage) { new (storage.bytes) runtime::MemoryResource; }
void install_listener(RuntimeListenerStorage* storage) {
    runtime::set_default_resource(storage ? std::launder(reinterpret_cast<runtime::MemoryResource*>(storage->bytes)) : nullptr);
}
void destroy_listener(RuntimeListenerStorage& storage) {
    std::launder(reinterpret_cast<runtime::MemoryResource*>(storage.bytes))->~MemoryResource();
}
void log_append(LogBuffer& log, const char* format, ...) {
    std::va_list args; va_start(args, format); runtime::log_vprintf(log, false, format, args); va_end(args);
}
void log_error(LogBuffer& log, const char* format, ...) {
    std::va_list args; va_start(args, format); runtime::log_vprintf(log, true, format, args); va_end(args);
}
void log_restart(LogBuffer& log) { runtime::clear_log(log); }
void log_flush(LogBuffer& log) { runtime::finish_log(log); }
FunctionController* make_function_controller(AllocationController*, const char*) { return runtime::create_scheduler(); }
void free_function_controller(AllocationController*, FunctionController* controller) {
    runtime::destroy_scheduler(controller, scheduler_environment, [] { fn_004d9e30(graphics_state); });
}
}
}
