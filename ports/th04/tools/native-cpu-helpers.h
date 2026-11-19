// Used exclusively after the synchronous lane's address-map certificate.
// Byte/word/dword operations retain the original TLB and device-handler semantics.
#include <cstdlib>
static INLINE void TH_RequireLane() {
    // Also prevents the optimiser from merging this entry with the general
    // paging-aware helpers used by the instrumented interpreter.
    if (!thCpuControl[3]) std::abort();
}
static INLINE uint8_t TH_ReadB(PhysPt address) {
    TH_RequireLane();
    HostPt p = get_tlb_read(address);
    return p ? host_readb(p + address) : get_tlb_readhandler(address)->readb(address);
}
static INLINE void TH_WriteB(PhysPt address, uint8_t value) {
    TH_RequireLane();
    HostPt p = get_tlb_write(address);
    if (p) host_writeb(p + address, value);
    else get_tlb_writehandler(address)->writeb(address, value);
}
static INLINE uint16_t TH_ReadW(PhysPt address) {
    TH_RequireLane();
    if ((address & 0xfff) == 0xfff) {
        uint16_t value = TH_ReadB(address); value |= uint16_t(TH_ReadB(address + 1)) << 8;
        return value;
    }
    HostPt p = get_tlb_read(address);
    return p ? host_readw(p + address) : get_tlb_readhandler(address)->readw(address);
}
static INLINE void TH_WriteW(PhysPt address, uint16_t value) {
    TH_RequireLane();
    if ((address & 0xfff) == 0xfff) {
        TH_WriteB(address, uint8_t(value)); TH_WriteB(address + 1, uint8_t(value >> 8)); return;
    }
    HostPt p = get_tlb_write(address);
    if (p) host_writew(p + address, value);
    else get_tlb_writehandler(address)->writew(address, value);
}
static INLINE uint32_t TH_ReadD(PhysPt address) {
    TH_RequireLane();
    if ((address & 0xfff) >= 0xffd) {
        uint32_t value = TH_ReadB(address); value |= uint32_t(TH_ReadB(address+1)) << 8;
        value |= uint32_t(TH_ReadB(address+2)) << 16; value |= uint32_t(TH_ReadB(address+3)) << 24;
        return value;
    }
    HostPt p = get_tlb_read(address);
    return p ? host_readd(p + address) : get_tlb_readhandler(address)->readd(address);
}
static INLINE void TH_WriteD(PhysPt address, uint32_t value) {
    TH_RequireLane();
    if ((address & 0xfff) >= 0xffd) {
        for (unsigned int i=0;i<4;++i) TH_WriteB(address+i,uint8_t(value>>(8*i))); return;
    }
    HostPt p = get_tlb_write(address);
    if (p) host_writed(p + address, value);
    else get_tlb_writehandler(address)->writed(address, value);
}
static INLINE uint8_t TH_Fetchb() {
    const uint8_t value = host_readb(get_tlb_read(core.cseip) + core.cseip);
    ++core.cseip; return value;
}
static INLINE uint16_t TH_Fetchw() {
    uint16_t value = TH_Fetchb(); value |= uint16_t(TH_Fetchb()) << 8; return value;
}
static INLINE uint32_t TH_Fetchd() {
    uint32_t value = TH_Fetchw(); value |= uint32_t(TH_Fetchw()) << 16; return value;
}
static INLINE int8_t TH_Fetchbs() { return int8_t(TH_Fetchb()); }
static INLINE int16_t TH_Fetchws() { return int16_t(TH_Fetchw()); }
static INLINE int32_t TH_Fetchds() { return int32_t(TH_Fetchd()); }
static INLINE void TH_Push16(uint16_t value) {
    uint32_t next = (reg_esp & cpu.stack.notmask) | ((reg_esp - 2) & cpu.stack.mask);
    TH_WriteW(SegPhys(ss) + (next & cpu.stack.mask), value); reg_esp = next;
}
static INLINE void TH_Push32(uint32_t value) {
    uint32_t next = (reg_esp & cpu.stack.notmask) | ((reg_esp - 4) & cpu.stack.mask);
    TH_WriteD(SegPhys(ss) + (next & cpu.stack.mask), value); reg_esp = next;
}
static INLINE uint16_t TH_Pop16() {
    uint16_t value = TH_ReadW(SegPhys(ss) + (reg_esp & cpu.stack.mask));
    reg_esp = (reg_esp & cpu.stack.notmask) | ((reg_esp + 2) & cpu.stack.mask); return value;
}
static INLINE uint32_t TH_Pop32() {
    uint32_t value = TH_ReadD(SegPhys(ss) + (reg_esp & cpu.stack.mask));
    reg_esp = (reg_esp & cpu.stack.notmask) | ((reg_esp + 4) & cpu.stack.mask); return value;
}
