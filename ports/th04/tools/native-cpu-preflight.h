// Synchronous interpreter lane for the pinned PC-98 configuration. The memory
// map is certified when it changes; per-instruction guards only reject paths
// which can suspend, change address width, or leave the certified address range.
static const Bits TH_CPU_MORE = -2147483647;
static const Bits TH_CPU_FALLBACK = -2147483646;
static unsigned int thCpuControl[4] = {1, 0, 0, 0};
extern bool TH_CpuMemorySafe();

static INLINE bool TH_CpuEligible() {
    if (cpu.pmode || paging.enabled || cpu.code.big || cpu.stack.big ||
        (reg_flags & 0x20100) ||
        cpu.stack.mask != 0xffff || cpu.idt.GetBase() > 0x10fbff) return false;
    return true;
}

static INLINE bool TH_CanSyncInstruction() {
    // Eligible instructions cannot change these CPU modes. I/O/control-register
    // fallback returns to the outer dispatcher, which checks eligibility again.
    if (reg_eip > 0xffff) return false;
    const PhysPt start = SegBase(cs) + reg_eip;
    // No handler read side effects while inspecting a prefix/fetch window.
    if (start > 0x10ffe0 || !get_tlb_read(start) || !get_tlb_read(start + 16)) return false;
    PhysPt ip = start;
    for (unsigned int count = 0; count < 8; ++count) {
        const uint8_t op = host_readb(get_tlb_read(ip) + ip); ++ip;
        switch (op) {
        case 0x26: case 0x2e: case 0x36: case 0x3e: case 0x64: case 0x65:
        case 0x66: case 0xf2: case 0xf3: continue;
        case 0x0f: case 0x67:
        case 0x6c: case 0x6d: case 0x6e: case 0x6f:
        case 0xe4: case 0xe5: case 0xe6: case 0xe7:
        case 0xec: case 0xed: case 0xee: case 0xef:
        case 0xd8: case 0xd9: case 0xda: case 0xdb:
        case 0xdc: case 0xdd: case 0xde: case 0xdf:
        case 0xc8: // ENTER's temporary stack indices can wrap outside real RAM
        case 0xf0: // LOCK's separate validation can read ahead
            return false;
        default: return true;
        }
    }
    return false;
}
