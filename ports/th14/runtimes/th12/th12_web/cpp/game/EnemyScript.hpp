#pragma once
#include "EnemyCommands.hpp"
namespace th11 {
struct EnemyScriptError {
    const EclInstruction* instruction=nullptr;i32 thread_id=0;float time=0;u32 opcode=0;
};
// Game-system adapter for the original ECL interpreter. Unimplemented commands
// preserve their instruction and stop the owner with -2 for a visible diagnostic.
class EnemyScriptServices final:public EclServices {
public:
    EnemyGlobals globals;EnemyCommandEnvironment& commands;EnemyScriptError error;
    EnemyScriptServices(EnemyState& e,EnemyEnvironment& w,EnemyCommandEnvironment& c):globals(e,w),commands(c){}
    i32 update(float elapsed);
    i32 command(EclContext&)override;
    i32 integer(i32 id)override{return globals.integer(id);}
    double floating(i32 id)override{return globals.floating(id);}
    i32* integer_reference(i32 id)override{return globals.integer_reference(id);}
    float* float_reference(i32 id)override{return globals.float_reference(id);}
    void* allocate(u32)override;
    void release(void*)override;
    float scalar_rate()const override{return globals.environment.rate;}
};
// Every extended command the scripts hand to the dispatcher, in execution order, kept as a ring. trace_point
// in Trace.hpp only covers the interpreter's own opcodes (call, jump, spawn, return), so which extended
// opcode a script actually reached was unobservable - and that is precisely the question when a script is
// expected to trigger something and does not. ecl_trace_total is monotonic; ecl_trace_at takes an absolute
// index and returns -1 for anything already overwritten. Diagnostic only, one dword write per command.
constexpr u32 kEclTraceCapacity=256;
u32 ecl_trace_total()noexcept;
i32 ecl_trace_at(u32 index)noexcept;
// The other half of the same question. update() returns -2 as soon as a refusal has been recorded, so a script
// that stops mid-loop leaves an owner that still exists and is still counted, and the two causes look identical
// from outside: the manager may have stopped calling update, or update may be called and refuse every time.
// These separate them, and carry the opcode that was refused so the culprit is named rather than inferred.
u32 ecl_update_total()noexcept;
u32 ecl_error_opcode()noexcept;
float ecl_error_time()noexcept;
// Whole-run execution count per extended opcode. The ring above only holds the tail, so it cannot answer
// whether an opcode that ends a stage section ran at all over a whole stage; this can, and a count of one
// reads very differently from a count of thousands.
u32 ecl_opcode_count(u32 opcode)noexcept;
}
