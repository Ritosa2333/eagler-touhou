#pragma once
#include "EnemyCommands.hpp"
namespace th11 {
#ifdef TH11_MULTIPLAYER
class EclRollbackAllocator;
struct EnemyRollbackEntry;
#endif
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
#ifdef TH11_MULTIPLAYER
private:
    friend struct EnemyRollbackEntry;
    // Bound once by the stable Entry constructor; never snapshotted or rebound.
    EclRollbackAllocator* rollback_allocator_=nullptr;
#endif
};
}
