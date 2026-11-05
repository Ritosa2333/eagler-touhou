#pragma once
#include "EclContext.hpp"
namespace th11 {
struct EclServices:EclGlobals {
    // 0 handled; -1 wait without advancing; -2 unavailable game system.
    virtual i32 command(EclContext&)=0;
    virtual void* allocate(u32)=0;
    virtual void release(void*)=0;
};
struct EclThread {EclContext* value;EclThread* next;EclThread* previous;};
struct EclOwner {
    void* original_virtual_table=nullptr;EclContext* active=nullptr;EclContext root;
    EclProgram* program=nullptr;EclThread threads{};
    void initialize_context()noexcept;
    EclThread* find_thread(i32)noexcept;
    void cancel_secondary_threads()noexcept;
    void spawn_thread(i32,u32 skipped,EclServices&);
    i32 update_threads(float,EclServices&);
    void release_threads(EclServices&);
    void reset_threads(EclServices&);
    void select_subroutine(const char*)noexcept;
};
static_assert(offsetof(EclOwner,program)==0x102c);
static_assert(offsetof(EclOwner,threads)==0x1030);
i32 call_subroutine(EclContext& target,EclContext& source,u32 skipped,EclServices&);
}
