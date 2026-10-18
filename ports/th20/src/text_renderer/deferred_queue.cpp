#include "deferred_queue.hpp"
#include "../runtime_core/runtime_core.hpp"
namespace th20::source::text {
std::pmr::list<std::function<void()>> deferred_tasks;
// pmr::list size is MSVC x86 STL layout evidence; binding on the Windows
// build only.
#ifndef TH_SDL3
static_assert(sizeof(deferred_tasks)==12);
#endif
void enqueue_deferred_task(std::function<void()> function) {deferred_tasks.push_back(std::move(function));}
void process_one_deferred_task() {
    if(deferred_tasks.empty())return;
    std::lock_guard lock(runtime::shared_locks().slot(18));
    std::lock_guard nested(runtime::shared_locks().slot(19));
    std::function<void()> function=deferred_tasks.front();deferred_tasks.pop_front();function();
}
}
