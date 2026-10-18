#include "runtime_core.hpp"
#include <atomic>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <thread>

namespace r = th20::source::runtime;
namespace s = th20::source::scheduler;
namespace {
void require(bool condition, const char* message) { if (!condition) throw std::runtime_error(message); }
}
int main() {
    try {
        auto& locks = r::shared_locks();
        require(!locks.enabled(), "Registry must initialize disabled");
        locks.enter_tracked(5); require(locks.depth(5) == 0, "Disabled tracked lock changed depth");
        locks.enable(); locks.enter_tracked(5); locks.enter_tracked(5);
        require(locks.depth(5) == 2, "Recursive tracked depth mismatch");
        std::atomic<bool> excluded{false};
        std::thread contender([&] { const bool acquired = locks.slot(5).try_lock(); excluded = !acquired; if (acquired) locks.slot(5).unlock(); });
        contender.join(); require(excluded, "Registry does not exclude a second thread");
        locks.leave_tracked(5); locks.leave_tracked(5); locks.disable();
        require(locks.depth(5) == 0, "Tracked depth did not balance");
        auto* controller = r::create_allocation_controller();
        require(controller->reserved == 0, "Allocation controller state differs");
        auto* previous = r::set_default_resource(&controller->resource);
        require(std::pmr::get_default_resource() == &controller->resource, "Default resource not installed");
        for (std::size_t alignment : {4u, 8u, 16u, 64u, 256u, 4096u}) {
            for (std::size_t size : {0u, 1u, 7u, 1024u, 8193u}) {
                auto* memory = controller->resource.allocate(size, alignment);
                require(memory && reinterpret_cast<std::uintptr_t>(memory) % alignment == 0, "Aligned resource allocation differs");
                if (size) std::memset(memory, 0x5a, size);
                controller->resource.deallocate(memory, size, alignment);
            }
        }
        require(controller->resource.is_equal(*std::pmr::new_delete_resource()), "Original resource equality always returns true");
        for (std::size_t size : {0u, 1u, 31u, 8192u}) {
            auto* memory = r::allocate_bytes(size); require(memory != nullptr, "Raw malloc failed");
            if (size) std::memset(memory, 0x31, size);
            r::release_bytes(memory);
        }
        errno = 0; require(r::allocate_bytes(0xffffffffu) == nullptr && errno == ENOMEM, "Oversized raw allocation contract differs");
        {
            r::Log log;
            require(log.text.get_allocator().resource() == &controller->resource, "Log pmr resource differs");
            r::log_printf(log, "%s %d/%08x\r\n", "runtime", -7, 0x1234);
            r::log_error(log, "%s", "error");
            require(log.text == "runtime -7/00001234\r\nerror" && log.error == 1, "Narrow formatted append failed");
            r::clear_log(log); require(log.text.empty() && log.error == 1, "Clear must preserve error flag");
            log.error = 0; r::log_printf(log, "test"); r::finish_log(log);
            require(log.text == "test---------------------------------------------------------- \r\n", "Finish separator differs");
        }
        r::set_default_resource(previous);
        r::destroy_allocation_controller(controller);
        unsigned callbacks = 0, flushes = 0;
        auto* state = r::create_scheduler(); auto environment = locks.scheduler_environment();
        locks.enable();
        require(&environment.chain_mutex == &locks.slot(0) && &environment.allocation_mutex == &locks.slot(1), "Scheduler created separate runtime locks");
        auto* node = s::register_callback(*state, environment, 3, +[](void*) -> std::int32_t { return 1; }, &callbacks, false, true);
        s::set_shutdown_callback(*node, +[](void* p) -> std::int32_t { ++*static_cast<unsigned*>(p); return 1; });
        r::destroy_scheduler(state, environment, [&] { ++flushes; require(callbacks == 0, "Renderer flush order changed"); });
        require(flushes == 1 && callbacks == 1, "Scheduler teardown did not execute real dependencies");
        require(locks.depth(0) == 0, "Scheduler tracking is not shared with registry");
        locks.disable();
        std::cout << "Runtime core source tests passed: shared recursive locks, PMR allocation, narrow logging, scheduler lifetime\n";
        return 0;
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
