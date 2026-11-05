#include "platform_window.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../runtime_core/worker.hpp"
#include <atomic>
#include <cstring>
#include <new>
#include <thread>

namespace th20::source::platform_window {
namespace {
using runtime::Worker;
Worker& worker(program_entry::GraphicsStatePrefix& g,unsigned index) {
    return *std::launder(reinterpret_cast<Worker*>(g.worker_storage[index]));
}
}
void initialize_window_global(WindowStatePrefix& w) {
    // Original CRT initializer zeroes the ENTIRE 0x2138 before calling its ctor.
    // All ctor writes are zero except this mode. This is not guessed zero state.
    std::memset(&w,0,sizeof(w));w.display_mode=2;
}
void initialize_graphics_global(GraphicsStatePrefix& g) {
    // Composition of 40aa10's memset and 4d8990's constructor. The two matrix
    // constructors 40bda0 perform no writes; all six viewport member ctors write
    // zeros. Their prior zero initialization is therefore required and retained.
    std::memset(&g,0,sizeof(g));
    platform::initialize_configuration(g.configuration);
    g.field_0b08=-2;g.field_0b0c=-2;g.field_0b10=-2;
    g.event_flags=0x880; // 4d88c0 preserves bits14..31 of the already-zero object
    ::new(static_cast<void*>(g.worker_storage[0])) Worker;
    ::new(static_cast<void*>(g.worker_storage[1])) Worker;
}
void destroy_graphics_global(GraphicsStatePrefix& g) {
    // 4d8da0 destroys +da0 then +d90. 40b980 first calls close_and_join,
    // then the genuine jthread destructor releases its stop state.
    worker(g,1).~Worker();worker(g,0).~Worker();
}
void sync_close_graphics_worker(GraphicsStatePrefix& g) {
    // The specimen DETACHES before its later join-if-joinable check. Preserve
    // that observable order rather than silently "fixing" the thread lifecycle.
    runtime::sync_close_worker(worker(g,0));
}
void join_graphics_worker(GraphicsStatePrefix& g,unsigned index) {runtime::join_worker(worker(g,index));}
}
namespace th20::source::program_entry {
WindowStatePrefix window_state;
GraphicsStatePrefix graphics_state;
std::uint32_t& graphics_event_flags=graphics_state.event_flags;
std::uint32_t& device_reset_countdown=graphics_state.reset_countdown;
std::uint32_t& render_value_005c5af8=graphics_state.render_value;
double& update_duration=graphics_state.update_duration;
namespace {
struct GlobalLifetime {
    GlobalLifetime() {
        // Keep the registry alive until both workers have been destroyed.
        runtime::shared_locks();
        platform_window::initialize_window_global(window_state);
        platform_window::initialize_graphics_global(graphics_state);
    }
    ~GlobalLifetime() {platform_window::destroy_graphics_global(graphics_state);}
} global_lifetime;
}
}
namespace th20::source::platform_window {
// Original 5b8890 is zero-initialized global HANDLE, assigned by 41c020.
HANDLE single_instance_mutex=nullptr;
}
namespace th20::source::program_entry::unrecovered {
void fn_004d9e30(GraphicsStatePrefix& g) {platform_window::sync_close_graphics_worker(g);}
}
