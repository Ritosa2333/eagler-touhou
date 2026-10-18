#pragma once
#include "program_entry.hpp"

namespace th20::source::program_entry::unrecovered {
// Historical integration namespace: runtime_core, platform_services and
// platform_window now supply their recovered operations. Remaining domains
// stay deliberately undefined; use symbol audits rather than declaration counts.
// Each names a real dependency observed in the original call sites. Names describe the call's
// role where established; fn_VA names avoid pretending unknown semantics exist.
// Source-level signatures replace hidden ECX `this` arguments with references.
struct alignas(runtime::MemoryResource) RuntimeListenerStorage {
    std::byte bytes[sizeof(runtime::MemoryResource)];
}; // raw stack storage for the explicitly constructed/destroyed 4-byte PMR object
bool set_rounding_mode(std::uint32_t);                   // 0x0054a4e0, x87/SSE fenv helpers
void lock_registry_enable(LockRegistry&);                 // 0x0041ccc0
void lock_registry_disable(LockRegistry&);                // 0x0041ca30
void* allocate(std::uint32_t);                            // 0x0054275d
AllocationController* construct_allocations(void*);       // 0x0041f5b0
void destroy_allocations(AllocationController*, int);     // 0x00419320
void construct_listener(RuntimeListenerStorage&);         // 0x00418db0
void install_listener(RuntimeListenerStorage*);           // 0x0041e770 -> 0x00541562
void destroy_listener(RuntimeListenerStorage&);           // 0x00418e90
void log_append(LogBuffer&, const char*, ...);            // 0x00454150
void log_error(LogBuffer&, const char*, ...);              // 0x00454230
void log_restart(LogBuffer&);                            // 0x00419be0
void log_flush(LogBuffer&);                              // 0x00453220
int fn_0041c020(HINSTANCE);
void fn_004117a0(GraphicsStatePrefix&, HINSTANCE);
int fn_0041b4f0(WindowStatePrefix&);
int load_configuration(GraphicsStatePrefix&, const char*); // 0x004dc1c0
void fn_00420f80();
void fn_00421040();
void fn_0041ae70(HINSTANCE);
void fn_00416d20();
int fn_0041c320();
int create_window(WindowStatePrefix&, HINSTANCE);          // 0x0041ccf0
int fn_0041c3e0(WindowStatePrefix&);
FunctionController* make_function_controller(AllocationController*, const char*); // 0x004187a0
SpriteController* make_sprite_controller(AllocationController*, const char*);     // 0x00418830
void free_function_controller(AllocationController*, FunctionController*);         // 0x004186c0
void free_sprite_controller(AllocationController*, SpriteController*);             // 0x00418730
int fn_0041d0f0(GraphicsStatePrefix&);
double read_clock(WindowStatePrefix&);                   // 0x0041cb10, x87 double return
int fn_004de1f0();
void fn_0041e050(WindowStatePrefix&, int);
void fn_004dd840(GraphicsStatePrefix&);
void fn_0041dbe0(SpriteController*);
int fn_0041c730(int);
void fn_0041a2c0();
void fn_0041d9f0(SpriteController*);
void fn_004dbce0(GraphicsStatePrefix&);
void fn_004dbd70(GraphicsStatePrefix&);
void fn_004dd490(GraphicsStatePrefix&);
void fn_00426170(ThreadRegistry&);
void save_configuration(ConfigPrefix&);                 // 0x0041aa70

// The three frame schedulers share these still-unrecovered engine operations.
void reset_sprite_queue(SpriteController*);               // 0x004455c0
void select_viewport(GraphicsStatePrefix&, int);           // 0x0041dce0
void fn_004d9e30(GraphicsStatePrefix&);
void prepare_sprite_draw(SpriteController*);              // 0x00445a40
void fn_004dda60(GraphicsStatePrefix&);
void fn_004193e0(WindowStatePrefix&);
void fn_004199a0(WindowStatePrefix&);
void fn_00419a50(WindowStatePrefix&);
}
