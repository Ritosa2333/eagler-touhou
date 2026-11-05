#pragma once
#include "../../cpp/game/EclOwner.hpp"
#include <cstdlib>
#include <vector>
namespace th11::test {
struct EclGlobalsFixture: EclServices {
    i32 ints[64]{};float floats[64]{};
    i32 command_result=0;u32 commands=0,allocations=0,releases=0;
    struct CommandEvent{i32 thread;float time;u32 opcode;};std::vector<CommandEvent> events;
    i32 command(EclContext& c)override{++commands;events.push_back({c.thread_id,c.time,c.instruction->opcode});return command_result;}
    void* allocate(u32 n)override{++allocations;auto* p=std::malloc(n);if(p)std::memset(p,0,n);return p;}
    void release(void* p)override{++releases;std::free(p);}
    i32 integer(i32 id)override{return ints[u32(-i64(id))%64];}
    double floating(i32 id)override{return floats[u32(-i64(id))%64];}
    i32* integer_reference(i32 id)override{return ints+u32(-i64(id))%64;}
    float* float_reference(i32 id)override{return floats+u32(-i64(id))%64;}
};
}
