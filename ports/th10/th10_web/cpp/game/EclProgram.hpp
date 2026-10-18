#pragma once
#include "EclContext.hpp"
namespace th10 {
struct EclResourceEnvironment;
struct EclSubroutine {const char* name; const u8* header;};
struct EclProgram {
    void* original_virtual_table;
    i32 file_count;
    i32 subroutine_count;
    u8* loaded_files[32];
    EclSubroutine* sorted_subroutines;
    u8 reserved_090[0x1008];
    void initialize(void* type_table) noexcept;
    i32 attach_file(u8* data,EclResourceEnvironment& environment);
    i32 load(const char* name,EclResourceEnvironment& environment);
    void release_files(EclResourceEnvironment& environment);
    void release_lookup(EclResourceEnvironment& environment,void* type_table);
    EclInstruction* find(const char* name) const noexcept;
};
static_assert(offsetof(EclProgram,sorted_subroutines)==0x8c);
static_assert(sizeof(EclProgram)==0x1098);
struct EclServices : EclGlobals {
#ifdef TH_ENABLE_THPRAC
    // F4 time lock (thprac_th10.cpp:527-547, EHOOK 0x44fb9f): return true to hold
    // the ECL sub-time for this context instead of adding the frame elapsed.
    // Implementations may clamp context.time before the caller skips the add.
    virtual bool hold_time(EclContext& context,float& elapsed){return false;}
#endif
    virtual i32 command(EclContext& context)=0;
    virtual void* allocate(u32 bytes)=0;
    virtual void release(void* memory)=0;
};
// The first slot is the original owner's virtual table, retained only while
// testing against original objects. Game callbacks are explicit EclServices.
struct EclOwner {
    void* original_virtual_table;
    EclContext* active;
    EclContext root;
    EclProgram* program;
    ListNode<EclContext> threads;
    void construct(void* type_table) noexcept;
    ListNode<EclContext>* find_thread(i32 id) noexcept;
    void cancel_secondary_threads() noexcept;
    void spawn_thread(i32 id,u32 skipped_arguments,EclServices& services);
    i32 update_threads(float elapsed,EclServices& services);
    void reset_threads(EclServices& services);
    void initialize_context() noexcept;
    void release_threads(EclServices& services);
    void select_subroutine(const char* name) noexcept;
};
static_assert(offsetof(EclOwner,program)==0x102c);
static_assert(offsetof(EclOwner,threads)==0x1030);
i32 call_subroutine(EclContext& target,EclContext& source,u32 skipped_arguments,EclServices& services);
}
