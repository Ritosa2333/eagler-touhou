#include "native-scheduler-clock.h"
#include <emscripten.h>
#include "cpu.h"
#include "mem.h"

THSchedulerClock thSchedulerClock = {0, 0, 0};

extern "C" EMSCRIPTEN_KEEPALIVE void* thSchedulerState() {
    return &thSchedulerClock;
}

// Read-only state addresses for comparing old and new scheduling of the same
// source build. These exports never execute guest code or modify lazy flags.
extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t thCoreState(unsigned int field) {
    switch (field) {
    case 0: return reinterpret_cast<uintptr_t>(MemBase);
    case 1: return MEM_TotalPages() * 4096;
    case 2: return reinterpret_cast<uintptr_t>(&cpu_regs);
    case 3: return sizeof(cpu_regs);
    case 4: return reinterpret_cast<uintptr_t>(&cpu);
    case 5: return sizeof(cpu);
    default: return 0;
    }
}

