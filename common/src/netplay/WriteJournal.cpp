// Adapted from apeirs/th11, th11-rollback (GPL-3.0), whole-state journal.
// https://github.com/apeirs/th11/tree/324fb143e78560ae34f5930a19048e34035eb2f8
// Some consumers discover common sources by directory. Allocator replacement
// belongs only to products that explicitly enable this backend.
#if defined(EAGLER_WRITE_JOURNAL) && EAGLER_WRITE_JOURNAL
#include <eagler/netplay/WriteJournal.hpp>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <new>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
void* eagler_sim_create(void*, std::size_t);
void* eagler_sim_malloc(void*, std::size_t);
void* eagler_sim_memalign(void*, std::size_t, std::size_t);
void eagler_sim_free(void*, void*);
std::size_t eagler_sim_used(void*);
alignas(32) volatile std::uint32_t eagler_rb_config[8]{};
}

namespace Netplay::WriteJournal {
namespace {
constexpr std::size_t BlockBytes = 32, MaxFrames = 32, MaxEntries = std::size_t(1) << 20;
struct Entry { std::uintptr_t address; std::uint8_t bytes[BlockBytes]; };
struct Frame { std::uint32_t number=0; std::size_t first=0, count=0; };
void* heap = nullptr;
std::uintptr_t heapLo=0, heapHi=0, rootLo=0, rootHi=0, rootBlockLo=0;
std::size_t heapBlocks=0, rootBlocks=0;
int allocationScope=0;
bool ready=false, open=false, failed=false;
const char* error="";
std::uint8_t* tags=nullptr;
std::uint8_t tag=0;
Entry* entries=nullptr;
std::size_t entryHead=0, entryUsed=0, frameHead=0, frameCount=0;
Frame frames[MaxFrames];
Statistics statistics{};

bool Fail(const char* reason) {
    if (!failed) { failed=true; error=reason; }
    eagler_rb_config[0]=0;
    return false;
}
Frame& At(std::size_t i) { return frames[(frameHead+i)%MaxFrames]; }
Entry& Record(std::size_t i) { return entries[i%MaxEntries]; }
void* AllocateFailure() { Fail("Simulation heap exhausted"); std::abort(); }

void Capture(std::uintptr_t lo, std::uintptr_t hi, std::uintptr_t regionLo,
             std::uintptr_t regionHi, std::uintptr_t blockLo, std::size_t tagBase) {
    lo=std::max(lo,regionLo); hi=std::min(hi,regionHi);
    if (lo>=hi) return;
    auto& frame=At(frameCount-1);
    for (auto block=lo&~std::uintptr_t(BlockBytes-1); block<hi; block+=BlockBytes) {
        const auto index=tagBase+(block-blockLo)/BlockBytes;
        if (tags[index]==tag) continue;
        if (entryUsed==MaxEntries) { Fail("Write journal capacity exhausted"); return; }
        auto& saved=Record(frame.first+frame.count);
        saved.address=block;
        // External roots can start/end in the middle of a block. Neither read
        // nor restore an adjacent host byte, even for an unaligned wide store.
        const auto begin=std::max(block,regionLo), end=std::min(block+BlockBytes,regionHi);
        std::memcpy(saved.bytes+(begin-block),reinterpret_cast<const void*>(begin),end-begin);
        tags[index]=tag; ++frame.count; ++entryUsed;
    }
}
}

bool Initialize(void* root, std::size_t rootBytes, std::size_t heapBytes) {
    const auto address=reinterpret_cast<std::uintptr_t>(root);
    if (ready) return !failed && address==rootLo && rootBytes==rootHi-rootLo && heapBytes==heapHi-heapLo;
#ifdef __EMSCRIPTEN__
    if (eagler_rb_instrumented()!=InstrumentationVersion)
        return Fail("Write-journal Wasm instrumentation is missing or incompatible");
#endif
    if ((!root && rootBytes) || rootBytes>std::numeric_limits<std::uintptr_t>::max()-address ||
        heapBytes<1024*1024 || heapBytes%4096 || heapBytes>std::size_t(512)*1024*1024)
        return Fail("Invalid simulation region");
    auto* region=std::aligned_alloc(4096,heapBytes);
    if (!region) return Fail("Cannot reserve simulation heap");
    auto* space=eagler_sim_create(region,heapBytes);
    if (!space) { std::free(region); return Fail("Cannot initialize simulation allocator"); }
    const auto base=reinterpret_cast<std::uintptr_t>(region);
    const auto rootStart=address&~std::uintptr_t(BlockBytes-1);
    const auto rootCount=rootBytes?(address+rootBytes-rootStart+BlockBytes-1)/BlockBytes:0;
    auto* nextTags=static_cast<std::uint8_t*>(std::calloc(heapBytes/BlockBytes+rootCount,1));
    auto* nextEntries=static_cast<Entry*>(std::malloc(sizeof(Entry)*MaxEntries));
    if (!nextTags || !nextEntries) {
        std::free(nextTags); std::free(nextEntries); std::free(region);
        return Fail("Cannot reserve write journal");
    }
    heap=space; heapLo=base; heapHi=base+heapBytes;
    rootLo=address; rootHi=address+rootBytes; rootBlockLo=rootStart;
    heapBlocks=heapBytes/BlockBytes; rootBlocks=rootCount;
    tags=nextTags; entries=nextEntries;
    eagler_rb_config[0]=0;
    eagler_rb_config[1]=std::uint32_t(heapLo); eagler_rb_config[2]=std::uint32_t(heapBytes);
    eagler_rb_config[3]=std::uint32_t(rootLo); eagler_rb_config[4]=std::uint32_t(rootBytes);
    eagler_rb_config[5]=std::uint32_t(reinterpret_cast<std::uintptr_t>(tags));
    eagler_rb_config[6]=0; eagler_rb_config[7]=DirtyGuardVersion;
    statistics.capacity_bytes=sizeof(Entry)*MaxEntries;
    statistics.heap_capacity_kib=std::uint32_t(heapBytes/1024);
    ready=true;
    return true;
}
bool Ready() { return ready&&!failed; }
GameScope::GameScope():previous_(allocationScope) { if (ready) allocationScope=1; }
GameScope::~GameScope() { allocationScope=previous_; }
SystemScope::SystemScope():previous_(allocationScope) { allocationScope=0; }
SystemScope::~SystemScope() { allocationScope=previous_; }
HostScope::HostScope():previous_(eagler_rb_config[0]) { eagler_rb_config[0]=0; }
HostScope::~HostScope() { eagler_rb_config[0]=open&&!failed?previous_:0; }
bool OwnedByGameHeap(const void* pointer) {
    const auto address=reinterpret_cast<std::uintptr_t>(pointer);
    return address>=heapLo && address<heapHi;
}
void* Allocate(std::size_t bytes) {
    void* result=allocationScope?eagler_sim_malloc(heap,bytes?bytes:1):std::malloc(bytes?bytes:1);
    return result?result:AllocateFailure();
}
void* AllocateZeroed(std::size_t count, std::size_t size) {
    if (size && count>std::numeric_limits<std::size_t>::max()/size) return AllocateFailure();
    const auto bytes=count*size;
    auto* result=Allocate(bytes);
    std::memset(result,0,bytes);
    return result;
}
void* AllocateAligned(std::size_t alignment, std::size_t bytes) {
    alignment=std::max(alignment,sizeof(void*));
    if ((alignment&(alignment-1)) || bytes>std::numeric_limits<std::size_t>::max()-(alignment-1))
        return AllocateFailure();
    const auto rounded=((bytes?bytes:1)+alignment-1)&~(alignment-1);
    void* result=allocationScope?eagler_sim_memalign(heap,alignment,rounded):std::aligned_alloc(alignment,rounded);
    return result?result:AllocateFailure();
}
void Release(void* pointer) {
    if (OwnedByGameHeap(pointer)) eagler_sim_free(heap,pointer);
    else std::free(pointer);
}

bool BeginFrame(std::uint32_t frame) {
    if (!Ready()) return Fail("Write journal is not ready");
    if (open) return Fail("Write journal frame already open");
    if (frameCount && frame<=At(frameCount-1).number) return Fail("Write journal frame order rejected");
    if (frameCount==MaxFrames) return Fail("Write journal history exhausted");
    if (++tag==0) { std::memset(tags,0,heapBlocks+rootBlocks); tag=1; }
    // Publish only after a wrapped generation has been cleared, and before
    // tracing becomes observable by any instrumented simulation write.
    eagler_rb_config[6]=tag;
    auto& next=At(frameCount++);
    next.number=frame; next.first=(entryHead+entryUsed)%MaxEntries; next.count=0;
    open=true; eagler_rb_config[0]=1;
    return true;
}
bool EndFrame() {
    eagler_rb_config[0]=0;
    if (!open) return Fail("Write journal frame was not open");
    open=false;
    statistics.last_blocks=std::uint32_t(At(frameCount-1).count);
    statistics.peak_blocks=std::max(statistics.peak_blocks,statistics.last_blocks);
    ++statistics.frames;
    return !failed;
}
bool RestoreTo(std::uint32_t frame) {
    if (!Ready() || open) return Fail("Write journal restore is not at a closed boundary");
    bool found=false;
    for (std::size_t i=0;i<frameCount;++i) found=found||At(i).number==frame;
    if (!found) return Fail("Write journal has no exact target checkpoint");
    eagler_rb_config[0]=0;
    while (frameCount && At(frameCount-1).number>=frame) {
        const auto& log=At(frameCount-1);
        for (std::size_t i=log.count;i--;) {
            const auto& entry=Record(log.first+i);
            const bool inHeap=entry.address>=heapLo&&entry.address<heapHi;
            const auto lo=std::max(entry.address,inHeap?heapLo:rootLo);
            const auto hi=std::min(entry.address+BlockBytes,inHeap?heapHi:rootHi);
            std::memcpy(reinterpret_cast<void*>(lo),entry.bytes+(lo-entry.address),hi-lo);
        }
        entryUsed-=log.count; --frameCount; ++statistics.restored_frames;
    }
    ++statistics.restores;
    return true;
}
void DiscardBefore(std::uint32_t frame) {
    while (frameCount && !(open&&frameCount==1) && At(0).number<frame) {
        entryHead=(entryHead+At(0).count)%MaxEntries; entryUsed-=At(0).count;
        frameHead=(frameHead+1)%MaxFrames; --frameCount;
    }
}
void Clear() {
    if (open) Fail("Write journal abandoned an open frame");
    eagler_rb_config[0]=0;
    open=false; frameHead=frameCount=entryHead=entryUsed=0;
    // A failure or abandoned transaction remains terminal: Clear cannot turn
    // partial history into evidence that the simulation is safe to continue.
}
bool FrameOpen() { return open; }
std::size_t FrameCount() { return frameCount; }
bool Failed() { return failed; }
const char* Error() { return error; }
Statistics Stats() {
    auto result=statistics;
    result.used_bytes=entryUsed*sizeof(Entry);
    if (heap) result.heap_used_kib=std::uint32_t(eagler_sim_used(heap)/1024);
    return result;
}
std::uint32_t LastFrameBlocks() { return statistics.last_blocks; }
void Touch(void* pointer, std::size_t bytes) {
    if (!open || failed || !bytes) return;
    eagler_rb_config[0]=0;
    const auto lo=reinterpret_cast<std::uintptr_t>(pointer);
    const auto hi=bytes>std::numeric_limits<std::uintptr_t>::max()-lo?
        std::numeric_limits<std::uintptr_t>::max():lo+bytes;
    Capture(lo,hi,heapLo,heapHi,heapLo,0);
    if (!failed && rootBlocks) Capture(lo,hi,rootLo,rootHi,rootBlockLo,heapBlocks);
    ++statistics.touches;
    if (!failed) eagler_rb_config[0]=1;
}
}

extern "C" {
EMSCRIPTEN_KEEPALIVE std::uint32_t eagler_rb_config_address() {
    return std::uint32_t(reinterpret_cast<std::uintptr_t>(&eagler_rb_config[0]));
}
EMSCRIPTEN_KEEPALIVE std::uint32_t eagler_rb_dirty_config_address() {
    return std::uint32_t(reinterpret_cast<std::uintptr_t>(&eagler_rb_config[5]));
}
EMSCRIPTEN_KEEPALIVE std::uint32_t eagler_rb_dirty_config_abi() {
    return Netplay::WriteJournal::DirtyGuardVersion;
}
EMSCRIPTEN_KEEPALIVE __attribute__((noinline)) std::uint32_t eagler_rb_instrumented() {
    // The post-link pass patches this static word as well as the export body,
    // so calls inlined by wasm-opt read the same instrumentation version.
    static volatile std::uint32_t marker=0;
    return marker;
}
// An exported cold entry keeps wasm-opt from folding the complete capture loop
// into the frequently called trampoline. Its caller has already disabled
// tracing; retaining this symbol also preserves the engine's small hot body.
EMSCRIPTEN_KEEPALIVE __attribute__((noinline)) void eagler_rb_capture(std::uint32_t address, std::uint32_t bytes) {
    Netplay::WriteJournal::Touch(reinterpret_cast<void*>(std::uintptr_t(address)),bytes);
}
EMSCRIPTEN_KEEPALIVE void eagler_rb_touch(std::uint32_t address, std::uint32_t bytes) {
    // This exported trampoline is excluded by the instrumentation pass. Disable
    // tracing before calling instrumented C++ helpers or writing their stack.
    if (!eagler_rb_config[0] || !bytes) return;
    using namespace Netplay::WriteJournal;
    const auto lo=std::uintptr_t(address);
    // Scalar stores usually revisit blocks already captured in this frame.
    // Keep that case entirely inside the excluded trampoline: no tracing
    // toggle, journal helper call or metadata write is needed. Test complete
    // containment before the general overlap calculation. Initialize reserves
    // at least 1 MiB, so subtracting a scalar width cannot underflow heap size.
    if (bytes<=16) {
        std::size_t first=0,last=0;
        bool contained=false;
        const auto heapOffset=lo-heapLo;
        if (heapOffset<=heapHi-heapLo-bytes) {
            first=heapOffset/BlockBytes;
            last=(heapOffset+bytes-1)/BlockBytes;
            contained=true;
        } else if (!rootBlocks) {
            // A heap-only scalar outside the fully contained fast path either
            // clips one heap edge or is entirely host memory. Use differences
            // to preserve overlap without computing a wrapping end address;
            // ordinary host/stack stores avoid the general/root checks below.
            if (heapOffset<heapHi-heapLo || (lo<heapLo && heapLo-lo<bytes)) {
                eagler_rb_config[0]=0;
                eagler_rb_capture(address,bytes);
            }
            return;
        } else if (lo>=rootLo && lo<rootHi && bytes<=rootHi-lo) {
            first=heapBlocks+(lo-rootBlockLo)/BlockBytes;
            last=heapBlocks+(lo+bytes-1-rootBlockLo)/BlockBytes;
            contained=true;
        }
        if (contained) {
            // Most scalar stores do not cross a block. Do not load the same
            // first-write tag twice, but still test both for unaligned stores.
            if (tags[first]==tag && (first==last || tags[last]==tag)) return;
            eagler_rb_config[0]=0;
            eagler_rb_capture(address,bytes);
            return;
        }
    }
    const auto hi=bytes>std::numeric_limits<std::uintptr_t>::max()-lo?
        std::numeric_limits<std::uintptr_t>::max():lo+bytes;
    // Bulk and region-crossing writes retain full clipping. A scalar starting
    // outside a region can still overlap its beginning and must be captured.
    if (!(lo<heapHi && hi>heapLo) && !(rootBlocks && lo<rootHi && hi>rootLo)) return;
    eagler_rb_config[0]=0;
    eagler_rb_capture(address,bytes);
}
}

// Linked explicitly by write-journal products; other games retain their normal
// allocator. Free routes by address, independent of the current allocation scope.
void* operator new(std::size_t n) { return Netplay::WriteJournal::Allocate(n); }
void* operator new[](std::size_t n) { return Netplay::WriteJournal::Allocate(n); }
void* operator new(std::size_t n,const std::nothrow_t&) noexcept { return Netplay::WriteJournal::Allocate(n); }
void* operator new[](std::size_t n,const std::nothrow_t&) noexcept { return Netplay::WriteJournal::Allocate(n); }
void* operator new(std::size_t n,std::align_val_t a) { return Netplay::WriteJournal::AllocateAligned(std::size_t(a),n); }
void* operator new[](std::size_t n,std::align_val_t a) { return Netplay::WriteJournal::AllocateAligned(std::size_t(a),n); }
void operator delete(void* p) noexcept { Netplay::WriteJournal::Release(p); }
void operator delete[](void* p) noexcept { Netplay::WriteJournal::Release(p); }
void operator delete(void* p,std::size_t) noexcept { Netplay::WriteJournal::Release(p); }
void operator delete[](void* p,std::size_t) noexcept { Netplay::WriteJournal::Release(p); }
void operator delete(void* p,std::align_val_t) noexcept { Netplay::WriteJournal::Release(p); }
void operator delete[](void* p,std::align_val_t) noexcept { Netplay::WriteJournal::Release(p); }
void operator delete(void* p,std::size_t,std::align_val_t) noexcept { Netplay::WriteJournal::Release(p); }
void operator delete[](void* p,std::size_t,std::align_val_t) noexcept { Netplay::WriteJournal::Release(p); }
#endif // EAGLER_WRITE_JOURNAL
