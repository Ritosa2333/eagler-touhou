// Test-only hook microbenchmark. Timing loops are deliberately restored to
// their raw code bodies after production instrumentation; see the driver audit.
// The journal backend and its capture path retain production instrumentation.
#include <eagler/netplay/WriteJournal.hpp>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>

namespace J = Netplay::WriteJournal;
namespace {
constexpr std::uint32_t Abi = 1, HeapBytes = 8u << 20, MaxSlots = 1u << 16;
constexpr std::uint32_t Stride = 64, Guard = 64, DataBytes = MaxSlots * Stride + 2 * Guard;
constexpr std::uint32_t HotSlots = 256, HotOperations = 1u << 20;
enum Kind { HeapDirty, HeapFirst, Outside, CrossDirty, CrossFirst, CrossFirstDirty, CrossLastDirty, Mixed };
enum Variant { Plain, Generic, Scalar };
struct Plan { std::uint32_t address, width; };
static_assert(sizeof(Plan) == 8);
struct __attribute__((packed, may_alias)) U16 { std::uint16_t value; };
struct __attribute__((packed, may_alias)) U32 { std::uint32_t value; };
struct __attribute__((packed, may_alias)) U64 { std::uint64_t value; };

std::uint8_t *gameData=nullptr, *hostData=nullptr, *hostBefore=nullptr;
std::uint8_t *heapBefore=nullptr, *heapExpected=nullptr, *hostExpected=nullptr;
std::uint8_t *targetBlocks=nullptr, *preloggedBlocks=nullptr;
std::uint8_t *heapBase=nullptr;
Plan* plan=nullptr;
std::uint32_t metadata[40]{}, frame=0, planSlots=0, wantedChecksum=0;
std::uint32_t expectedBlocks=0, expectedPrelogged=0, expectedCaptures=0, beforeHeapHash=0;
std::uint64_t beforeTouches=0, writtenBytes=0;
bool initialized=false, prepared=false;

std::uint32_t address(const void* pointer) {
    return std::uint32_t(reinterpret_cast<std::uintptr_t>(pointer));
}
std::uint32_t hash(const void* pointer, std::uint32_t bytes) {
    const auto* data=static_cast<const std::uint8_t*>(pointer);
    std::uint32_t result=2166136261u;
    for (std::uint32_t i=0;i<bytes;++i) result=(result^data[i])*16777619u;
    return result;
}
__attribute__((always_inline)) inline std::uint32_t datum(std::uint32_t seed, std::uint32_t i) {
    return (seed^i)*0x9e3779b1u+0x7f4a7c15u;
}
__attribute__((always_inline)) inline std::uint32_t fold(std::uint32_t checksum, std::uint32_t value, std::uint32_t i, std::uint32_t width) {
    return (checksum^(value+i+width))*16777619u;
}
void* hostAllocate(std::uint32_t bytes) {
    auto* value=std::malloc(bytes);
    assert(value && !J::OwnedByGameHeap(value));
    return value;
}
template<unsigned Width> __attribute__((always_inline)) inline void store(std::uint32_t at, std::uint32_t value) {
    auto* pointer=reinterpret_cast<void*>(std::uintptr_t(at));
    if constexpr (Width==1) *static_cast<volatile std::uint8_t*>(pointer)=std::uint8_t(value);
    if constexpr (Width==2) static_cast<volatile U16*>(pointer)->value=std::uint16_t(value);
    if constexpr (Width==4) static_cast<volatile U32*>(pointer)->value=value;
    if constexpr (Width==8 || Width==16) {
        const auto low=std::uint64_t(value)|(std::uint64_t(value^0xa5a5a5a5u)<<32);
        static_cast<volatile U64*>(pointer)->value=low;
        if constexpr (Width==16)
            reinterpret_cast<volatile U64*>(std::uintptr_t(at)+8)->value=~low;
    }
}
template<unsigned Width, unsigned Mode>
__attribute__((always_inline)) inline void one(std::uint32_t at, std::uint32_t value) {
    if constexpr (Mode==Generic) eagler_rb_touch(at,Width);
    if constexpr (Mode==Scalar) {
        if constexpr (Width==1) eagler_rb_touch1(at);
        if constexpr (Width==2) eagler_rb_touch2(at);
        if constexpr (Width==4) eagler_rb_touch4(at);
        if constexpr (Width==8) eagler_rb_touch8(at);
        if constexpr (Width==16) eagler_rb_touch16(at);
    }
    store<Width>(at,value);
}
template<unsigned Width, unsigned Mode>
__attribute__((always_inline)) inline std::uint32_t loop(std::uint32_t planAddress, std::uint32_t operations,
                                                        std::uint32_t seed, std::uint32_t mask) {
    const auto* entries=reinterpret_cast<const Plan*>(std::uintptr_t(planAddress));
    std::uint32_t checksum=2166136261u;
#pragma clang loop unroll(disable) vectorize(disable) interleave(disable)
    for (std::uint32_t i=0;i<operations;++i) {
        const auto entry=entries[i&mask];
        const auto value=datum(seed,i);
        if constexpr (Width) {
            one<Width,Mode>(entry.address,value);
            checksum=fold(checksum,value,i,Width);
        } else {
            switch(entry.width) {
            case 1: one<1,Mode>(entry.address,value); break;
            case 2: one<2,Mode>(entry.address,value); break;
            case 4: one<4,Mode>(entry.address,value); break;
            case 8: one<8,Mode>(entry.address,value); break;
            case 16: one<16,Mode>(entry.address,value); break;
            default: __builtin_trap();
            }
            checksum=fold(checksum,value,i,entry.width);
        }
    }
    return checksum;
}
// Independent byte-level oracle for the packed volatile stores above.
void referenceStore(std::uint8_t* to, unsigned width, std::uint32_t value) {
    for (unsigned j=0;j<width;++j) {
        const auto word=j<4 ? value : j<8 ? value^0xa5a5a5a5u :
                        j<12 ? ~value : ~(value^0xa5a5a5a5u);
        to[j]=std::uint8_t(word>>(8*(j&3)));
    }
}
void marks(std::uint32_t at, unsigned width, std::uint32_t& first, std::uint32_t& last) {
    assert(at>=address(heapBase) && at-address(heapBase)+width<=HeapBytes);
    first=(at-address(heapBase))/32;
    last=(at-address(heapBase)+width-1)/32;
}
} // namespace

#define MICRO_LOOP(NAME, WIDTH, MODE) \
extern "C" EMSCRIPTEN_KEEPALIVE __attribute__((noinline)) \
std::uint32_t micro_loop_##NAME(std::uint32_t p,std::uint32_t n,std::uint32_t seed,std::uint32_t mask) { \
    return loop<WIDTH,MODE>(p,n,seed,mask); \
}
#define MICRO_WIDTH(WIDTH) \
MICRO_LOOP(plain_##WIDTH,WIDTH,Plain) \
MICRO_LOOP(generic_##WIDTH,WIDTH,Generic) \
MICRO_LOOP(scalar_##WIDTH,WIDTH,Scalar)
MICRO_WIDTH(1)
MICRO_WIDTH(2)
MICRO_WIDTH(4)
MICRO_WIDTH(8)
MICRO_WIDTH(16)
MICRO_LOOP(plain_mixed,0,Plain)
MICRO_LOOP(generic_mixed,0,Generic)
MICRO_LOOP(scalar_mixed,0,Scalar)

extern "C" EMSCRIPTEN_KEEPALIVE int micro_initialize() {
    if (initialized) return 1;
    // Raw Wasm must reject initialization; a separate raw instance is checked.
    if (!J::Initialize(nullptr,0,HeapBytes)) return 0;
    assert(eagler_rb_scalar_abi()==1);
    const auto* cfg=reinterpret_cast<const volatile std::uint32_t*>(
        std::uintptr_t(eagler_rb_config_address()));
    assert(cfg[2]==HeapBytes && cfg[3]==0 && cfg[4]==0 && cfg[0]==0);
    heapBase=reinterpret_cast<std::uint8_t*>(std::uintptr_t(cfg[1]));
    { J::GameScope scope;
      gameData=static_cast<std::uint8_t*>(J::AllocateAligned(Stride,DataBytes)); }
    assert(J::OwnedByGameHeap(gameData) && address(gameData)%Stride==0);
    hostData=static_cast<std::uint8_t*>(hostAllocate(DataBytes));
    hostBefore=static_cast<std::uint8_t*>(hostAllocate(DataBytes));
    hostExpected=static_cast<std::uint8_t*>(hostAllocate(DataBytes));
    heapBefore=static_cast<std::uint8_t*>(hostAllocate(HeapBytes));
    heapExpected=static_cast<std::uint8_t*>(hostAllocate(HeapBytes));
    targetBlocks=static_cast<std::uint8_t*>(hostAllocate(HeapBytes/32));
    preloggedBlocks=static_cast<std::uint8_t*>(hostAllocate(HeapBytes/32));
    plan=static_cast<Plan*>(hostAllocate(MaxSlots*sizeof(Plan)));
    for (std::uint32_t i=0;i<DataBytes;++i) {
        gameData[i]=std::uint8_t(i*17u+(i>>8)+3u);
        hostData[i]=std::uint8_t(i*29u+(i>>9)+11u);
    }
    std::memcpy(hostBefore,hostData,DataBytes);
    std::memcpy(heapBefore,heapBase,HeapBytes);
    beforeHeapHash=hash(heapBefore,HeapBytes);
    metadata[0]=Abi;
    initialized=true;
    return 1;
}

extern "C" EMSCRIPTEN_KEEPALIVE int micro_prepare(std::uint32_t kind,std::uint32_t width,
                                                 std::uint32_t variant,std::uint32_t seed) {
    assert(initialized && !prepared && !J::FrameOpen() && !J::FrameCount() && J::Ready());
    assert(kind<=Mixed && variant<=Scalar);
    assert((kind==Mixed && width==0) || (kind!=Mixed && (width==1||width==2||width==4||width==8||width==16)));
    assert(!(kind>=CrossDirty && kind<=CrossLastDirty && width==1));
    assert(std::memcmp(heapBase,heapBefore,HeapBytes)==0);
    std::memcpy(hostData,hostBefore,DataBytes);
    std::memcpy(heapExpected,heapBefore,HeapBytes);
    std::memcpy(hostExpected,hostBefore,DataBytes);
    std::memset(targetBlocks,0,HeapBytes/32);
    std::memset(preloggedBlocks,0,HeapBytes/32);
    std::memset(metadata+1,0,sizeof(metadata)-sizeof(metadata[0]));
    planSlots=(kind==HeapDirty || kind==Outside || kind==CrossDirty)?HotSlots:MaxSlots;
    const std::uint32_t operations=planSlots==HotSlots?HotOperations:planSlots;
    const unsigned widths[]={1,2,4,8,16};
    expectedBlocks=expectedPrelogged=expectedCaptures=0;
    wantedChecksum=2166136261u;
    writtenBytes=0;
    for (std::uint32_t i=0;i<planSlots;++i) {
        const auto w=kind==Mixed?widths[i%5]:width;
        auto path=kind==Mixed?i%7:kind;
        // A one-byte store cannot cross a block; mixed cases record this
        // conversion and still cover all crossing widths independently.
        if (w==1 && path>=CrossDirty) path=(path==CrossDirty?HeapDirty:HeapFirst);
        const bool outside=path==Outside;
        const bool crossing=path>=CrossDirty;
        const auto at=address(outside?hostData:gameData)+Guard+i*Stride+(crossing?31u:3u);
        plan[i]={at,w};
        ++metadata[30+path];
        if (outside) continue;
        std::uint32_t first=0,last=0;
        marks(at,w,first,last);
        targetBlocks[first]=targetBlocks[last]=1;
        if (variant==Plain || path==HeapDirty || path==CrossDirty)
            preloggedBlocks[first]=preloggedBlocks[last]=1;
        else if (path==CrossFirstDirty) preloggedBlocks[first]=1;
        else if (path==CrossLastDirty) preloggedBlocks[last]=1;
    }
    for (std::uint32_t i=0;i<HeapBytes/32;++i) {
        expectedBlocks+=targetBlocks[i]!=0;
        expectedPrelogged+=preloggedBlocks[i]!=0;
    }
    for (std::uint32_t i=0;i<planSlots;++i) {
        const auto entry=plan[i];
        if (!J::OwnedByGameHeap(reinterpret_cast<void*>(std::uintptr_t(entry.address)))) continue;
        std::uint32_t first=0,last=0;
        marks(entry.address,entry.width,first,last);
        if (!preloggedBlocks[first] || !preloggedBlocks[last]) ++expectedCaptures;
    }
    for (std::uint32_t i=0;i<operations;++i) {
        const auto w=plan[i&(planSlots-1)].width;
        wantedChecksum=fold(wantedChecksum,datum(seed,i),i,w);
        writtenBytes+=w;
    }
    // Slots never overlap, and each repeats at a power-of-two period. The
    // independent expected image uses each slot's final actual iteration.
    for (std::uint32_t i=0;i<planSlots;++i) {
        const auto entry=plan[i];
        const auto iteration=operations-planSlots+i;
        const bool game=J::OwnedByGameHeap(reinterpret_cast<void*>(std::uintptr_t(entry.address)));
        auto* expected=game?heapExpected:hostExpected;
        const auto offset=entry.address-(game?address(heapBase):address(hostData));
        referenceStore(expected+offset,entry.width,datum(seed,iteration));
    }
    ++frame;
    assert(J::BeginFrame(frame));
    for (std::uint32_t i=0;i<HeapBytes/32;++i)
        if (preloggedBlocks[i]) J::Touch(heapBase+i*32,32);
    assert(!J::Failed());
    beforeTouches=J::Stats().touches;
    metadata[1]=address(plan);
    metadata[2]=operations;
    metadata[3]=seed;
    metadata[4]=planSlots-1;
    metadata[5]=wantedChecksum;
    metadata[6]=expectedBlocks;
    metadata[7]=expectedPrelogged;
    metadata[8]=expectedCaptures;
    metadata[9]=address(heapBase);
    metadata[10]=HeapBytes;
    metadata[11]=address(gameData);
    metadata[12]=DataBytes;
    metadata[13]=address(hostData);
    metadata[14]=DataBytes;
    metadata[15]=std::uint32_t(writtenBytes);
    metadata[16]=std::uint32_t(writtenBytes>>32);
    metadata[17]=hash(plan,planSlots*sizeof(Plan));
    metadata[18]=beforeHeapHash;
    metadata[19]=kind;
    metadata[20]=width;
    metadata[21]=variant;
    metadata[22]=frame;
    metadata[23]=planSlots;
    prepared=true;
    return 1;
}

extern "C" EMSCRIPTEN_KEEPALIVE std::uint32_t micro_metadata(std::uint32_t index) {
    assert(index<40);
    return metadata[index];
}

extern "C" EMSCRIPTEN_KEEPALIVE int micro_finish(std::uint32_t observedChecksum) {
    assert(prepared && J::FrameOpen() && J::Ready());
    assert(observedChecksum==wantedChecksum);
    assert(J::EndFrame());
    const auto after=J::Stats();
    assert(after.touches>=beforeTouches && after.touches-beforeTouches==expectedCaptures);
    assert(J::LastFrameBlocks()==expectedBlocks);
    assert(std::memcmp(heapBase,heapExpected,HeapBytes)==0);
    assert(std::memcmp(hostData,hostExpected,DataBytes)==0);
    assert(hash(plan,planSlots*sizeof(Plan))==metadata[17]);
    metadata[24]=J::LastFrameBlocks();
    metadata[25]=std::uint32_t(after.touches-beforeTouches);
    metadata[26]=hash(heapBase,HeapBytes);
    metadata[27]=hash(hostData,DataBytes);
    assert(J::RestoreTo(frame));
    assert(J::Ready() && !J::FrameOpen() && J::FrameCount()==0);
    // This compares every byte, including allocator metadata, unused heap,
    // the 64-byte guards around the payload, and unwritten slot bytes.
    assert(std::memcmp(heapBase,heapBefore,HeapBytes)==0);
    assert(std::memcmp(hostData,hostExpected,DataBytes)==0);
    metadata[28]=hash(heapBase,HeapBytes);
    metadata[29]=1;
    prepared=false;
    return 1;
}
