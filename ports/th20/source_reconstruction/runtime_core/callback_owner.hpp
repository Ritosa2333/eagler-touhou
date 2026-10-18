#pragma once
#include "runtime_core.hpp"

namespace th20::source::runtime {
// Original 0x41fd20: a 16-byte polymorphic base, flags=2 and two null
// scheduler links. Its destructor 0x41fdf0 changes only the vptr. The input
// manager and other recovered engine owners must share this actual C++ type.
class CallbackOwner {
public:
    virtual ~CallbackOwner() = default;
    virtual void enable_callbacks() { // 0x421680 -> 0x4216a0
        if(update_node) scheduler::enable(*update_node);
        if(draw_node) scheduler::enable(*draw_node);
    }
    virtual void disable_callbacks() { // 0x421760 -> 0x421780
        if(update_node) scheduler::disable(*update_node);
        if(draw_node) scheduler::disable(*draw_node);
    }
    std::uint32_t flags=2;
    scheduler::Node* update_node=nullptr;
    scheduler::Node* draw_node=nullptr;
};
static_assert(sizeof(CallbackOwner)==16);
inline void retire_callback_owner(CallbackOwner* value) { // 0x4217c0/0x41f7c0
    if(!value) return;
    // Explicit virtual destruction performs the most-derived recovered
    // teardown before taking allocation slot 1, matching the original order.
    value->~CallbackOwner();
    std::lock_guard<std::recursive_mutex> lock(shared_locks().slot(1));
    ::operator delete(value);
}
}
