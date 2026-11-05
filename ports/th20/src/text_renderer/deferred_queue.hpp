#pragma once
#include <functional>
#include <list>
#include <memory_resource>
namespace th20::source::text {
// Actual allocator-bearing list initialized by401060 at5b66e0.
extern std::pmr::list<std::function<void()>> deferred_tasks;
void enqueue_deferred_task(std::function<void()>); //4162f0, caller must hold producer lock19
void process_one_deferred_task();                 //416140
}
