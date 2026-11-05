#pragma once
#include "EclStack.hpp"
namespace th10 {
struct EclOwner;
struct EclServices;
struct EclInstruction {
    i32 time;
    u16 opcode;
    u16 length;
    u16 references;
    u8 difficulty;
    u8 parameter_count;
    u32 stack_adjustment;
    u32 argument(u32 index) const noexcept {
        u32 result; std::memcpy(&result, reinterpret_cast<const u8*>(this) + 16 + index * 4, 4); return result;
    }
    bool is_reference(u32 index) const noexcept { return (references & (1u << (index & 31))) != 0; }
};
static_assert(sizeof(EclInstruction) == 16);
struct EclGlobals {
    virtual i32 integer(i32 variable) = 0;
    virtual Extended floating(i32 variable) = 0;
    virtual i32* integer_reference(i32 variable) = 0;
    virtual float* float_reference(i32 variable) = 0;
};
struct EclContext {
    float time;
    EclInstruction* instruction;
    EclStack stack;
    i32 thread_id;
    EclOwner* owner;
    u32 state_1018;
    u32 difficulty;
    u32 flags;
    i32 integer_argument(u32 index, EclGlobals& globals);
    Extended float_argument(u32 index, EclGlobals& globals);
    i32 resolve_integer(u32 index, i32 value, EclGlobals& globals);
    Extended resolve_float(u32 index, float value, EclGlobals& globals);
    i32* integer_reference(u32 index, EclGlobals& globals);
    float* float_reference(u32 index, EclGlobals& globals);
    i32 update(float elapsed, EclServices& services);
};
static_assert(offsetof(EclContext, stack) == 8);
static_assert(offsetof(EclContext, owner) == 0x1014);
}
