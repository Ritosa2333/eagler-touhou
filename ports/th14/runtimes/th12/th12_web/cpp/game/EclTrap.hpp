#pragma once
#include <cstdio>
#include "Types.hpp"
namespace th11 {
// The ECL handlers stop the module when an instruction contradicts the world: a boss command with no
// boss, a reference the running thread cannot resolve, a slot past the end of an array, or a spawn
// pool index that is not there. `__builtin_trap` reaches the page as `RuntimeError: unreachable`
// and says none of that, so the site reports itself first.
//
// The command that was running is the other half: `report_ecl_underflow` in the interpreter needs
// the same pair, and EnemyScript.cpp sets it at the top of every command.
extern u32 g_ecl_last_opcode;

[[noreturn]] inline void ecl_trap(const char* file, int line) {
    std::fprintf(stderr, "[ecl] handler trap %s:%d command=0x%x\n", file, line, g_ecl_last_opcode);
    std::fflush(stderr);
    __builtin_trap();
}
}
