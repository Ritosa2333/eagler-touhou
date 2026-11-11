"""Apply idempotent source edits to the isolated, pinned build tree."""
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / 'build/native-scheduler/source'
PINNED = ROOT / 'vendor-source/core-source/emulators-6bbf0925d221a7304ea8e0dda1e89b8dc01569b2'
DOSBOX = ROOT / 'vendor-source/core-source/dosbox-x-788fde99d3d451eab3621009186e5abe516f2b9c'

def write_changed(path, text):
    if not path.exists() or path.read_text() != text:
        path.write_text(text)

for name in ['native-scheduler-clock.cpp', 'native-scheduler-clock.h']:
    write_changed(SOURCE / 'native/jsdos' / name, (ROOT / 'tools' / name).read_text())

timer = (PINNED / 'native/jsdos/jsdos-timer.cpp').read_text()
timer = timer.replace('#include <jsdos-timer.h>', '#include <jsdos-timer.h>\n#ifdef TH_NATIVE_SCHEDULER\n#include "native-scheduler-clock.h"\n#endif')
timer = timer.replace('double now() {', '''double now() {
#ifdef TH_NATIVE_SCHEDULER
    if (thSchedulerClock.mode != 0) return thSchedulerClock.now;
#endif''')
write_changed(SOURCE / 'native/jsdos/jsdos-timer.cpp', timer)

sleep = (PINNED / 'native/jsdos/jsdos-asyncify.cpp').read_text()
sleep = sleep.replace('#include <cmath>', '#include <cmath>\n#include <cstdlib>\n#ifdef TH_NATIVE_SCHEDULER\n#include "native-scheduler-clock.h"\nextern "C" uintptr_t thCpuState(unsigned int);\n#endif')
sleep = sleep.replace('  if (asyncifyLockCount != 0) {', '''#ifdef TH_NATIVE_SCHEDULER
  if (reinterpret_cast<unsigned int*>(thCpuState(0))[3]) std::abort();
#endif
  if (asyncifyLockCount != 0) {''')
sleep = sleep.replace('  syncSleep(ms, nonSkippable);', '''#ifdef TH_NATIVE_SCHEDULER
  if (thSchedulerClock.mode == 2) {
    // Match the deterministic host scheduler's exact one-ms minimum, including
    // its non-skippable=false semantics. Never run past the input boundary.
    const double wake = thSchedulerClock.now + (nonSkippable && ms > 1 ? ms : 1);
    if (wake <= thSchedulerClock.deadline) {
      thSchedulerClock.now = wake;
      return;
    }
  }
#endif
  syncSleep(ms, nonSkippable);''')
write_changed(SOURCE / 'native/jsdos/jsdos-asyncify.cpp', sleep)

target = (PINNED / 'targets/dosbox-x-sdl2.cmake').read_text()
target = target.replace('"${NATIVE_DIR}/jsdos/jsdos-timer.cpp"', '"${NATIVE_DIR}/jsdos/jsdos-timer.cpp"\n        "${NATIVE_DIR}/jsdos/native-scheduler-clock.cpp"')
# Full PC-98 BIOS continuations; the upstream allow-list is insufficient.
target = target.replace('"-sASYNCIFY_WHITELIST=@${TARGETS_DIR}/dosbox-x-asyncify.txt"', '"-sASYNCIFY_STACK_SIZE=131072"')
target = target.replace('"-sASYNCIFY_STACK_SIZE=131072"', '"-sASYNCIFY_STACK_SIZE=131072"\n        "-O1"')
target = target.replace('"-sEXPORT_NAME=\'WDOSBOXXJSPI\'"', '"-sEXPORT_NAME=\'WDOSBOXXJSPI\'"\n        "-g3"')
target += '''
# The interpreter is CPU-bound; the package-wide -Oz setting inhibits inlining.
# Keep the change local to this translation unit, without unsafe math flags.
set_source_files_properties("${NATIVE_DIR}/dosbox-x/src/cpu/core_normal.cpp"
    PROPERTIES COMPILE_OPTIONS "-O3")
'''
write_changed(SOURCE / 'targets/dosbox-x-sdl2.cmake', target)

# Retain the already shipped, instruction-budget-equivalent idle fusion in the
# source build. It only reads directly mapped RAM and leaves final iterations
# to reconstruct the interpreter and lazy-flags scratch state.
cpu = (DOSBOX / 'src/cpu/core_normal.cpp').read_text()
idle = '''
static Bitu TH_IdleTriplets() {
    if (CPU_Cycles <= 9 || cpu.pmode || cpu.code.big || (reg_flags & 0x20100)) return 0;
    const HostPt code = get_tlb_read(113269);
    if (!code || host_readd(code + 113269) != 0x3b2ab2a1 ||
        host_readd(code + 113273) != 0x72539006 || host_readb(code + 113277) != 0xf7) return 0;
    const PhysPt dsBase = SegBase(ds);
    if (dsBase > 0x90000) return 0;
    const PhysPt a = dsBase + 0x2ab2, b = dsBase + 0x5390;
    const HostPt ra = get_tlb_read(a), rb = get_tlb_read(b);
    if (!ra || !rb || host_readw(ra + a) >= host_readw(rb + b)) return 0;
    const Bitu skipped = ((CPU_Cycles - 6) / 3) * 3;
    CPU_Cycles -= skipped;
    return skipped;
}

'''
cpu = cpu.replace('#ifdef JSDOS_NO_RECURSION\nBits CPU_Core_Normal_Run_Impl(void);', idle + '#ifdef JSDOS_NO_RECURSION\nBits CPU_Core_Normal_Run_Impl(void);')
cpu = cpu.replace('\t\tcycle_count++;', '\t\tif (core.cseip == 113269) cycle_count += TH_IdleTriplets();\n\t\tcycle_count++;')

# Same-source synchronous lane with an instrumented dispatcher and fallback.
# A one-instruction fallback keeps the full *remaining* instruction budget, so
# REP, I/O delay, STI shadow and early decode_end retain their original rules.
# It returns internally before the next instruction without FillFlags().
body_start = cpu.index('\tif (CPU_Cycles <= 0)', cpu.index('Bits CPU_Core_Normal_Run_Impl'))
body_end = cpu.index('\nBits CPU_Core_Normal_Trap_Run', body_start)
body = cpu[body_start:body_end]
full = body.replace('\twhile (CPU_Cycles-->0) {', '''\tbool executed = false;
\twhile (CPU_Cycles-->0) {
        if (single && executed) { ++CPU_Cycles; return TH_CPU_MORE; }
        executed = true;''')
fast = body.replace('\twhile (CPU_Cycles-->0) {', '''\twhile (CPU_Cycles-->0) {
        if (!TH_CanSyncInstruction()) { ++CPU_Cycles; return TH_CPU_FALLBACK; }''')
# The guard rejects 0F before execution. Do not compile unreachable SSE/MMX /
# protected-mode decoder cases into the synchronous switch: huge WASM function
# bodies stay in baseline tier and lose most of the benefit of removing Asyncify.
fast = fast.replace('\t\t#include "core_normal/prefix_0f.h"', '')
fast = fast.replace('\t\t#include "core_normal/prefix_66_0f.h"', '')
sync_macros = '''
#include "native-cpu-helpers.h"
#undef LoadMb
#undef LoadMw
#undef LoadMd
#undef SaveMb
#undef SaveMw
#undef SaveMd
#define LoadMb TH_ReadB
#define LoadMw TH_ReadW
#define LoadMd TH_ReadD
#define SaveMb TH_WriteB
#define SaveMw TH_WriteW
#define SaveMd TH_WriteD
#define Fetchb TH_Fetchb
#define Fetchw TH_Fetchw
#define Fetchd TH_Fetchd
#define Fetchbs TH_Fetchbs
#define Fetchws TH_Fetchws
#define Fetchds TH_Fetchds
#undef Push_16
#undef Push_32
#undef Pop_16
#undef Pop_32
#define Push_16 TH_Push16
#define Push_32 TH_Push32
#define Pop_16 TH_Pop16
#define Pop_32 TH_Pop32
'''
ea = (DOSBOX / 'src/cpu/core_normal/table_ea.h').read_text()
ea = ea[ea.index('static PhysPt EA_16_00_n'):ea.index('static uint32_t SIBZero')]
ea = ea.replace('EA_16_', 'TH_EA_16_')
sync_macros += ea
sync_macros += 'static GetEAHandler thSyncEATable[192] = {\n'
for mode in ['00', '40', '80']:
    for _ in range(8):
        sync_macros += ','.join('TH_EA_16_' + mode[0] + str(i) + '_n' for i in range(8)) + ',\n'
sync_macros += '};\n#undef EALookupTable\n#define EALookupTable thSyncEATable\n'
string = (DOSBOX / 'src/cpu/core_normal/string.h').read_text()
string = string[string.index('void DoString(STRING_OP_NORMAL type)'):]
string = string.replace('void DoString(', 'static void TH_DoString(')
start_io = string.index('\t\t\t\tcase R_OUTSB:')
end_io = string.index('\t\t\t\tcase R_STOSB:', start_io)
string = string[:start_io] + string[end_io:]
sync_macros += string + '\n#undef DoString\n#define DoString TH_DoString\n'
prefix = cpu[:cpu.index('#ifdef JSDOS_NO_RECURSION\nBits CPU_Core_Normal_Run_Impl')]
prefix += '\n#include <emscripten.h>\n#include "native-cpu-preflight.h"\n'
dispatch = '''
static Bits TH_CPU_Full(bool single);
static __attribute__((noinline)) Bits TH_CPU_Synchronous();

Bits CPU_Core_Normal_Run(void) {
    if (!thCpuControl[0]) return TH_CPU_Full(false);
    for (;;) {
        if (!TH_CpuEligible() || !TH_CpuMemorySafe()) return TH_CPU_Full(false);
        for (unsigned int seg = 0; seg < 6; ++seg)
            if (Segs.phys[seg] > 0xffff0) return TH_CPU_Full(false);
        ++thCpuControl[3];
        const Bitu before = cycle_count;
        Bits result = TH_CPU_Synchronous();
        thCpuControl[1] += cycle_count - before;
        --thCpuControl[3];
        if (result != TH_CPU_FALLBACK) return result;
        ++thCpuControl[2];
        result = TH_CPU_Full(true);
        if (result != TH_CPU_MORE) return result;
    }
}

extern "C" EMSCRIPTEN_KEEPALIVE uintptr_t thCpuState(unsigned int field) {
    switch (field) {
    case 0: return reinterpret_cast<uintptr_t>(thCpuControl);
    case 1: return reinterpret_cast<uintptr_t>(&lflags);
    case 2: return sizeof(lflags);
    case 3: return reinterpret_cast<uintptr_t>(&core);
    case 4: return sizeof(core);
    case 5: return reinterpret_cast<uintptr_t>(&CPU_Cycles);
    case 6: return reinterpret_cast<uintptr_t>(&cycle_count);
    case 7: return reinterpret_cast<uintptr_t>(&Segs);
    case 8: return sizeof(Segs);
    case 9: return reinterpret_cast<uintptr_t>(&last_prefix);
    case 10: return reinterpret_cast<uintptr_t>(&last_ea86_offset);
    default: return 0;
    }
}

extern "C" EMSCRIPTEN_KEEPALIVE Bits thCpuRun(Bits budget) {
    CPU_Cycles = budget;
    return CPU_Core_Normal_Run();
}

static Bits TH_CPU_Full(bool single) {
'''
cpu = prefix + dispatch + full + sync_macros + '\nstatic __attribute__((noinline)) Bits TH_CPU_Synchronous() {\n' + fast + cpu[body_end:]
write_changed(SOURCE / 'native/dosbox-x/src/cpu/core_normal.cpp', cpu)
write_changed(SOURCE / 'native/dosbox-x/src/cpu/native-cpu-preflight.h',
              (ROOT / 'tools/native-cpu-preflight.h').read_text())
write_changed(SOURCE / 'native/dosbox-x/src/cpu/native-cpu-helpers.h',
              (ROOT / 'tools/native-cpu-helpers.h').read_text())

write_changed(SOURCE / 'native/dosbox-x/include/native-cpu-memory.h',
              (ROOT / 'tools/native-cpu-memory.h').read_text())

# Handler identity certifies this specific PC-98 map, not arbitrary MMIO.
paging = (DOSBOX / 'src/cpu/paging.cpp').read_text()
paging = paging.replace('#include "paging.h"', '#include "paging.h"\n#include "native-cpu-memory.h"')
for signature in ['void PAGING_InitTLB(void) {', 'void PAGING_ClearTLB(void) {',
                  'void PAGING_UnlinkPages(PageNum lin_page,PageNum pages) {',
                  'void PAGING_MapPage(PageNum lin_page,PageNum phys_page) {']:
    assert signature in paging, signature
    paging = paging.replace(signature, signature + '\n    TH_CpuMemoryChanged();')
paging += '''
static unsigned int thMemoryGeneration = 1, thMemoryCertified = 0;
static bool thMemorySafe = false;
void TH_CpuMemoryChanged() { ++thMemoryGeneration; thMemoryCertified = 0; }
static bool TH_KnownHandler(PageHandler* handler) {
    return handler == &init_page_handler || TH_CpuMemoryHandlerSafe(handler) ||
           TH_CpuVideoHandlerSafe(handler);
}
bool TH_CpuMemorySafe() {
    if (paging.enabled || !IS_PC98_ARCH) return false;
    if (thMemoryCertified == thMemoryGeneration) return thMemorySafe;
    thMemorySafe = false;
    for (PageNum page = 0; page < 0x110; ++page) {
        PageNum physical = page;
        PAGING_MakePhysPage(physical);
        if (!TH_KnownHandler(MEM_GetPageHandler(physical)) ||
            !TH_KnownHandler(paging.tlb.readhandler[page]) ||
            !TH_KnownHandler(paging.tlb.writehandler[page])) {
            thMemoryCertified = thMemoryGeneration;
            return false;
        }
    }
    thMemorySafe = true; thMemoryCertified = thMemoryGeneration;
    return true;
}
'''
write_changed(SOURCE / 'native/dosbox-x/src/cpu/paging.cpp', paging)

memory = (DOSBOX / 'src/hardware/memory.cpp').read_text()
memory = '#include "native-cpu-memory.h"\n' + memory
import re
memory = re.sub(r'(void MEM_(?:RegisterHandler|InvalidateCachedHandler|SetPageHandler|ResetPageHandler_\w+)\([^\n]*\) \{)',
                r'\1\n    TH_CpuMemoryChanged();', memory)
memory += '''
bool TH_CpuMemoryHandlerSafe(PageHandler* handler) {
    return handler == &ram_page_handler || handler == &rom_page_handler ||
           handler == &rom_page_alias_handler || handler == &unmapped_page_handler ||
           handler == &illegal_page_handler;
}
'''
write_changed(SOURCE / 'native/dosbox-x/src/hardware/memory.cpp', memory)
video = (DOSBOX / 'src/hardware/vga_memory.cpp').read_text()
video = '#include "native-cpu-memory.h"\n' + video
video += '''
bool TH_CpuVideoHandlerSafe(PageHandler* handler) {
    return handler == &vgaph.pc98 || handler == &vgaph.pc98_text ||
           handler == &vgaph.pc98_cg;
}
'''
write_changed(SOURCE / 'native/dosbox-x/src/hardware/vga_memory.cpp', video)

keyboard = (DOSBOX / 'src/hardware/keyboard.cpp').read_text()
keyboard = keyboard.replace('if (!keyb.repeat.wait) KEYBOARD_AddKey(keyb.repeat.key,true);',
    'if (!keyb.repeat.wait && (keyb.repeat.key < 340 || keyb.repeat.key > 347)) KEYBOARD_AddKey(keyb.repeat.key,true);')
write_changed(SOURCE / 'native/dosbox-x/src/hardware/keyboard.cpp', keyboard)

# Submit the native 32-bit SDL surface directly when the matching candidate
# presenter is installed. Other sizes/pitches/presenters retain the RGB bridge.
worker = (PINNED / 'src/dos/dosbox/cpp/worker-protocol.cpp').read_text()
worker += '''
#ifdef TH_NATIVE_SCHEDULER
EM_JS(int, thSubmitNativePixels, (void* pixels, int width, int height, int bgra), {
    if (!Module.nativeFramePresentation || !Module.framePresenter ||
        !Module.framePresenter.submitNative) return 0;
    return Module.framePresenter.submitNative(pixels, width, height, !!bgra) ? 1 : 0;
});
bool TH_ClientNativeFrame(void* pixels, bool bgra, int width, int height, int pitch) {
    if (!pixels || width <= 0 || height <= 0 || width != frameWidth ||
        height != frameHeight || pitch != width * 4) return false;
    return thSubmitNativePixels(pixels, width, height, bgra);
}
#endif
'''
write_changed(SOURCE / 'src/dos/dosbox/cpp/worker-protocol.cpp', worker)
gfx = (PINNED / 'native/jsdos/dosbox-x/jsdos-x-main.cpp').read_text()
gfx = '''#ifdef TH_NATIVE_SCHEDULER
extern bool TH_ClientNativeFrame(void*, bool, int, int, int);
#endif
''' + gfx
frame_guard = '''    if (!sdl.surface || !sdl.surface->pixels || sdl.surface->format->BytesPerPixel != 4)
        return;
'''
assert gfx.count(frame_guard) == 1
gfx = gfx.replace(frame_guard, frame_guard + '''
#ifdef TH_NATIVE_SCHEDULER
    if (TH_ClientNativeFrame(sdl.surface->pixels,
            vga.mode == M_LIN16 || vga.mode == M_LIN24 || vga.mode == M_LIN32,
            sdl.surface->w, sdl.surface->h, sdl.surface->pitch)) return;
#endif
''')
write_changed(SOURCE / 'native/jsdos/dosbox-x/jsdos-x-main.cpp', gfx)
print('Patched native clock, frame deadline and guarded synchronous CPU lane')
