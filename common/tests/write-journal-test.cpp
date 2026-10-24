#include <eagler/netplay/WriteJournal.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <emscripten.h>

namespace J=Netplay::WriteJournal;
extern "C" {
std::size_t eagler_sim_used(void*);
std::size_t eagler_sim_test_used_uncached(void*);
std::size_t eagler_sim_test_used_scans();
void* eagler_sim_test_last_created_space();
}

static std::size_t checked_heap_usage(void* space,bool miss) {
    const auto scans=eagler_sim_test_used_scans();
    const auto expected=eagler_sim_test_used_uncached(space);
    assert(eagler_sim_used(space)==expected);
    assert(eagler_sim_test_used_scans()==scans+unsigned(miss));
    for(unsigned i=0;i<128;++i) {
        assert(eagler_sim_used(space)==expected);
        assert(J::Stats().heap_used_kib==expected/1024);
    }
    assert(eagler_sim_test_used_scans()==scans+unsigned(miss));
    return expected;
}

struct Object { std::uint32_t number=17; std::string text="a simulation-owned string longer than SSO"; };
struct State {
    std::uint32_t value=11;
    std::vector<std::uint32_t> values;
    std::unique_ptr<Object> object;
    std::array<std::uint8_t,47> tail{};
};
struct alignas(32) Root {
    std::uint64_t hostBefore=0x1122334455667788ull;
    State state;
    std::uint64_t hostAfter=0x8877665544332211ull;
} root;

static bool initialize(bool heapOnly=false) {
    if (!J::Initialize(heapOnly?nullptr:&root.state,heapOnly?0:sizeof(root.state),8*1024*1024)) return false;
    const auto* config=reinterpret_cast<const volatile std::uint32_t*>(
        std::uintptr_t(eagler_rb_config_address()));
    const auto* dirty=reinterpret_cast<const volatile std::uint32_t*>(
        std::uintptr_t(eagler_rb_dirty_config_address()));
    assert(dirty==config+5&&dirty[0]&&dirty[2]==J::DirtyGuardVersion);
    assert(eagler_rb_dirty_config_abi()==J::DirtyGuardVersion);
    { J::GameScope game;
      root.state.~State(); new(&root.state) State();
      root.state.values={1,2,3}; root.state.object=std::make_unique<Object>(); }
    assert(J::OwnedByGameHeap(root.state.values.data()));
    assert(J::OwnedByGameHeap(root.state.object.get()));
    return true;
}

static void trap_outside_memory() {
    // Read the live memory size after initialization/allocation. A volatile
    // store at its first inaccessible byte traps in Wasm itself, without
    // entering Emscripten's abort path or synthesizing a JavaScript exception.
    const auto end=std::uintptr_t(__builtin_wasm_memory_size(0))*65536;
    *reinterpret_cast<volatile std::uint8_t*>(end)=0x5a;
    assert(false&&"out-of-bounds Wasm store returned");
}

extern "C" EMSCRIPTEN_KEEPALIVE void eagler_write_journal_trap(int hostScope) {
    assert(hostScope==0||hostScope==1);
    assert(initialize());
    assert(J::BeginFrame(0));
    J::GameScope game;
    *reinterpret_cast<volatile std::uint32_t*>(&root.state.value)=91;
    *reinterpret_cast<volatile std::uint32_t*>(&root.state.object->number)=102;
    if(hostScope) { J::HostScope host; trap_outside_memory(); }
    else trap_outside_memory();
}

extern "C" EMSCRIPTEN_KEEPALIVE int eagler_write_journal_abandon(int hostScope) {
    const auto* config=reinterpret_cast<const volatile std::uint32_t*>(
        std::uintptr_t(eagler_rb_config_address()));
    // A raw Wasm trap unwinds past the C++ frame without running its scope
    // destructors. Both a tracked call and a suspended host call leave an open
    // transaction; the tracking flag alone cannot identify this condition.
    assert(J::FrameOpen()&&J::FrameCount()==1&&J::Ready()&&!J::Failed());
    assert(config[0]==std::uint32_t(!hostScope));
    assert(root.state.value==91&&root.state.object->number==102);
    assert(J::Stats().used_bytes>0);
    J::Clear();
    assert(!J::FrameOpen()&&J::FrameCount()==0&&config[0]==0);
    assert(J::Failed()&&!J::Ready());
    assert(std::strcmp(J::Error(),"Write journal abandoned an open frame")==0);
    assert(!J::BeginFrame(1));
    J::Clear();
    assert(J::Failed()&&!J::Ready()&&config[0]==0);
    assert(std::strcmp(J::Error(),"Write journal abandoned an open frame")==0);
    return 1;
}

extern "C" EMSCRIPTEN_KEEPALIVE int eagler_write_journal_test(int scenario) {
    if (scenario<0) return initialize()?1:0;
    assert(initialize(scenario==5||scenario==8||scenario>=12));
    if (scenario==0) {
        auto* original=root.state.object.get();
        const auto originalText=root.state.object->text;
        std::array<std::uint8_t,sizeof(State)> beforeRoot{};
        std::memcpy(beforeRoot.data(),&root.state,sizeof(State));
        assert(J::BeginFrame(10));
        const std::array<std::uint8_t,4> crossing{0x93,0xa7,0x18,0x4b};
        auto* rootBytes=reinterpret_cast<std::uint8_t*>(&root);
        const auto stateOffset=reinterpret_cast<std::uintptr_t>(&root.state)-reinterpret_cast<std::uintptr_t>(&root);
        std::memcpy(rootBytes+stateOffset-2,crossing.data(),crossing.size());
        std::memcpy(rootBytes+stateOffset+sizeof(State)-2,crossing.data(),crossing.size());
        { J::GameScope game;
          root.state.value=21;
          root.state.values.resize(4000,0x11223344);
          root.state.object->text.assign(400,'x');
          std::memset(root.state.tail.data()+1,0x75,45); }
        assert(J::EndFrame());
        const auto* firstVector=root.state.values.data();
        assert(J::BeginFrame(11));
        { J::GameScope game;
          root.state.object.reset();
          root.state.object=std::make_unique<Object>();
          root.state.object->number=99;
          root.state.values.assign(12000,0x55667788); }
        assert(J::EndFrame());
        root.hostBefore=0x1234; root.hostAfter=0x5678;
        assert(J::RestoreTo(10));
        assert(std::memcmp(beforeRoot.data(),&root.state,sizeof(State))==0);
        assert(root.state.value==11&&root.state.values==std::vector<std::uint32_t>({1,2,3}));
        assert(root.state.object.get()==original&&root.state.object->number==17);
        assert(root.state.object->text==originalText);
        assert(std::all_of(root.state.tail.begin(),root.state.tail.end(),[](auto v){return v==0;}));
        assert(root.hostBefore==0x1234&&root.hostAfter==0x5678);
        assert(J::FrameCount()==0);
        // Replaying the allocation must return the same address: restoring only
        // objects but not allocator free lists cannot pass this check.
        assert(J::BeginFrame(10));
        { J::GameScope game; root.state.values.resize(4000,0x11223344); }
        assert(root.state.values.data()==firstVector);
        assert(J::EndFrame()&&J::RestoreTo(10));
    } else if (scenario==1) {
        // Repeated branch/replay/discard traverses both the frame ring and the
        // 8-bit first-write generation wrap, including an empty checkpoint.
        for (std::uint32_t frame=0;frame<1100;++frame) {
            const auto before=root.state.value;
            assert(J::BeginFrame(frame));
            // Empty bulk writes must not disable tracking for later stores.
            eagler_rb_touch(std::uint32_t(reinterpret_cast<std::uintptr_t>(&root.state)),0);
            root.state.value=before+1;
            root.state.value=before+2;
            assert(J::EndFrame());
            assert(J::RestoreTo(frame));
            assert(root.state.value==before);
            assert(J::BeginFrame(frame)); root.state.value=before+1;
            assert(J::EndFrame()); J::DiscardBefore(frame+1);
            assert(J::FrameCount()==0&&J::Stats().used_bytes==0);
        }
        assert(root.state.value==1111);
        assert(J::BeginFrame(1100)&&J::EndFrame()&&J::RestoreTo(1100));
    } else if (scenario==2) {
        std::vector<int> host;
        assert(J::BeginFrame(0));
        { J::GameScope game;
          auto a=std::make_unique<Object>();
          assert(J::OwnedByGameHeap(a.get()));
          { J::SystemScope system; host.assign(1000,42); assert(!J::OwnedByGameHeap(host.data())); }
          auto b=std::make_unique<Object>();
          assert(J::OwnedByGameHeap(b.get()));
          root.state.object=std::move(b); }
        assert(J::EndFrame()&&J::RestoreTo(0));
        assert(host.size()==1000&&host[999]==42);
        assert(root.state.object->number==17);
    } else if (scenario==3) {
        assert(J::BeginFrame(0));root.state.value=100;assert(J::EndFrame());
        assert(J::BeginFrame(2));root.state.value=200;assert(J::EndFrame());
        // A missing exact target must fail before touching state/history.
        assert(!J::RestoreTo(1)&&J::Failed());
        assert(root.state.value==200&&J::FrameCount()==2);
        J::Clear();assert(!J::Ready()&&!J::BeginFrame(3));
    } else if (scenario==4) {
        std::uint8_t* buffer=nullptr;
        { J::GameScope game; buffer=static_cast<std::uint8_t*>(J::AllocateAligned(32,96)); }
        std::memset(buffer,0x32,96);
        assert(J::BeginFrame(0));
        buffer[0]=0x77; // Only the first block is already dirty.
        const std::uint64_t value=0x123456789abcdef0ull;
        std::memcpy(buffer+31,&value,sizeof(value));
        assert(J::EndFrame()&&J::RestoreTo(0));
        assert(std::all_of(buffer,buffer+96,[](auto byte){return byte==0x32;}));
        J::Release(buffer);
    } else if (scenario==5) {
        // TH15 tracks only its simulation heap. The external State and host
        // containers must retain their writes while heap scalars and bulk
        // mutations rewind, even when an untracked write precedes them.
        const auto* config=reinterpret_cast<const volatile std::uint32_t*>(
            std::uintptr_t(eagler_rb_config_address()));
        assert(config[3]==0&&config[4]==0);
        std::uint8_t* buffer=nullptr;
        { J::GameScope game; buffer=static_cast<std::uint8_t*>(J::AllocateAligned(32,4096)); }
        assert(J::OwnedByGameHeap(buffer));
        for (std::size_t i=0;i<4096;++i) buffer[i]=std::uint8_t(i*17+3);
        std::array<std::uint8_t,4096> before{};
        std::memcpy(before.data(),buffer,before.size());
        std::vector<std::uint8_t> host(128,0x44);
        assert(!J::OwnedByGameHeap(host.data()));
        assert(J::BeginFrame(0));
        root.state.value=91;host[0]=0x71;
        buffer[0]=0x77;
        const std::uint64_t scalar=0xfedcba9876543210ull;
        std::memcpy(buffer+31,&scalar,sizeof(scalar));
        root.state.object->number=101;root.state.values[1]=0xaabbccdd;
        host[64]=0x72;
        std::memmove(buffer+128,buffer+16,1024);
        std::memset(buffer+2048,0xb7,2048);
        assert(J::EndFrame()&&J::RestoreTo(0));
        assert(std::memcmp(buffer,before.data(),before.size())==0);
        assert(root.state.object->number==17&&root.state.values[1]==2);
        assert(root.state.value==91&&host[0]==0x71&&host[64]==0x72&&host[127]==0x44);
        assert(J::Ready()&&J::FrameCount()==0);
        J::Release(buffer);
    } else if (scenario==6) {
        const auto* config=reinterpret_cast<const volatile std::uint32_t*>(
            std::uintptr_t(eagler_rb_config_address()));
        std::vector<std::uint8_t> host;
        { J::HostScope idle; assert(config[0]==0&&!J::FrameOpen()); }
        assert(config[0]==0);
        assert(J::BeginFrame(0));
        { J::GameScope game;
          root.state.value=73;
          { J::HostScope outer;
            assert(config[0]==0&&J::FrameOpen());
            host.assign(4096,0x47);assert(!J::OwnedByGameHeap(host.data()));
            { J::HostScope inner;host[12]=0x85;assert(config[0]==0&&J::FrameOpen()); }
            assert(config[0]==0); }
          assert(config[0]==1&&J::FrameOpen());
          auto simulation=std::make_unique<Object>();assert(J::OwnedByGameHeap(simulation.get()));
          root.state.object=std::move(simulation);root.state.object->number=102;
          root.state.value=91; }
        assert(J::EndFrame()&&J::RestoreTo(0));
        assert(root.state.value==11&&root.state.object->number==17);
        assert(host.size()==4096&&host[0]==0x47&&host[12]==0x85);
        // A terminal failure inside a host operation cannot reactivate tracing
        // when its scope exits, even though the failed frame is still open.
        assert(J::BeginFrame(1));
        { J::HostScope hostOperation;assert(!J::BeginFrame(2));assert(J::Failed()&&config[0]==0); }
        assert(J::Failed()&&config[0]==0);
        const auto* firstError=J::Error();
        J::Clear();assert(!J::Ready()&&config[0]==0);
        assert(J::Error()==firstError);
    } else if (scenario==7) {
        // Probe every scalar width and alignment, with neither, either, or
        // both blocks already dirty. An inline first-write shortcut must not
        // skip a first mutation merely because the other block has a tag.
        const auto* config=reinterpret_cast<const volatile std::uint32_t*>(
            std::uintptr_t(eagler_rb_config_address()));
        const auto* dirty=reinterpret_cast<const volatile std::uint32_t*>(
            std::uintptr_t(eagler_rb_dirty_config_address()));
        const auto* tags=reinterpret_cast<const volatile std::uint8_t*>(std::uintptr_t(dirty[0]));
        std::uint8_t* buffer=nullptr;
        { J::GameScope game; buffer=static_cast<std::uint8_t*>(J::AllocateAligned(32,96)); }
        const auto index=(reinterpret_cast<std::uintptr_t>(buffer)-config[1])/32;
        std::array<std::uint8_t,96> before{};
        for (std::size_t i=0;i<before.size();++i) before[i]=std::uint8_t(i*19+7);
        std::memcpy(buffer,before.data(),before.size());
        const std::array<std::uint8_t,16> replacement{
            0xf1,0x31,0x82,0xa7,0x90,0x62,0xb3,0xc4,0x25,0xd6,0x47,0xe8,0x59,0x6a,0x7b,0x8c};
        std::uint32_t frame=0,previous=dirty[1],wraps=0;
        for (unsigned width: {1u,2u,4u,8u,16u}) for (unsigned offset=0;offset<32;++offset)
          for (unsigned already=0;already<4;++already,++frame) {
            assert(J::BeginFrame(frame));
            const auto generation=dirty[1];
            assert(config[0]==1&&generation>=1&&generation<=255&&generation!=previous);
            if (previous==255) { assert(generation==1);++wraps; }
            previous=generation;
            if (already&1) buffer[0]^=0x61;
            if (already&2) buffer[32]^=0x92;
            switch (width) {
                case 1: std::memcpy(buffer+offset,replacement.data(),1);break;
                case 2: std::memcpy(buffer+offset,replacement.data(),2);break;
                case 4: std::memcpy(buffer+offset,replacement.data(),4);break;
                case 8: std::memcpy(buffer+offset,replacement.data(),8);break;
                case 16: std::memcpy(buffer+offset,replacement.data(),16);break;
            }
            assert(tags[index]==generation);
            if ((already&2)||offset+width>32) assert(tags[index+1]==generation);
            assert(J::EndFrame()&&config[0]==0&&J::RestoreTo(frame));
            assert(std::memcmp(buffer,before.data(),before.size())==0);
            // Restoration keeps the generation monotonic; reopening a reused
            // frame number may never mistake its prior tags for a fresh save.
            assert(dirty[1]==generation);
            assert(J::BeginFrame(frame));
            assert(dirty[1]>=1&&dirty[1]<=255&&dirty[1]!=previous);
            if (previous==255) { assert(dirty[1]==1);++wraps; }
            previous=dirty[1];
            buffer[0]=0xa7;buffer[63]=0x85;
            assert(J::EndFrame()&&J::RestoreTo(frame));
            assert(std::memcmp(buffer,before.data(),before.size())==0);
          }
        assert(frame==640&&wraps>=5);
        J::Clear();assert(config[0]==0&&J::FrameCount()==0);
        assert(J::BeginFrame(0));buffer[33]=0x65;
        assert(J::EndFrame()&&J::RestoreTo(0));
        assert(std::memcmp(buffer,before.data(),before.size())==0);
        J::Release(buffer);
    } else if (scenario==8) {
        // Direct real-Wasm hook calls probe the reserved heap edges without
        // corrupting dlmalloc metadata or writing adjacent host allocations.
        // Every overlapping scalar must capture exactly the clipped edge block;
        // touching wholly outside it must neither save bytes nor disable tracing.
        const auto* config=reinterpret_cast<const volatile std::uint32_t*>(
            std::uintptr_t(eagler_rb_config_address()));
        const auto lo=config[1],hi=lo+config[2];
        assert(config[3]==0&&config[4]==0&&lo>16&&hi>lo&&hi<0xfffffff0u);
        struct Probe { std::uint32_t address,bytes,blocks; };
        const Probe probes[]{
            {lo-16,16,0},{lo-15,16,1},{lo-1,1,0},{lo-1,2,1},
            {lo,1,1},{lo,16,1},{hi-16,16,1},{hi-1,1,1},
            {hi-1,2,1},{hi-1,16,1},{hi,1,0},{hi,16,0},{hi+1,16,0},
            {0,16,0},{0xffffffffu,1,0},{0xfffffff8u,16,0},
            // Bulk and zero-length calls retain their previous full clipping.
            {lo-16,32,1},{hi-16,32,1},{lo-32,32,0},{hi,32,0},{lo,0,0},
        };
        std::uint32_t frame=0;
        for (const auto& probe:probes) {
            assert(J::BeginFrame(frame));
            eagler_rb_touch(probe.address,probe.bytes);
            assert(config[0]==1&&J::FrameOpen()&&!J::Failed());
            assert(J::EndFrame()&&J::LastFrameBlocks()==probe.blocks);
            assert(J::RestoreTo(frame)&&J::Ready()&&J::FrameCount()==0);
            ++frame;
        }
    } else if (scenario==9) {
        // The oracle always walks the real allocator. Count only scans made
        // through the cached query, including Stats(), so repeated diagnostics
        // cannot accidentally pass by rescanning or by estimating live bytes.
        auto* space=eagler_sim_test_last_created_space();assert(space);
        const auto initial=checked_heap_usage(space,true);
        void* anchor=nullptr;
        { J::GameScope game;anchor=J::Allocate(256); }
        const auto baseline=checked_heap_usage(space,false);assert(baseline>initial);
        { J::GameScope game;
          { J::SystemScope system;
            auto* host=J::Allocate(8192);auto* aligned=J::AllocateAligned(256,4096);
            assert(!J::OwnedByGameHeap(host)&&!J::OwnedByGameHeap(aligned));
            J::Release(host);J::Release(aligned); } }
        assert(checked_heap_usage(space,false)==baseline);

        assert(J::BeginFrame(10));
        void* first=nullptr;
        { J::GameScope game;first=J::Allocate(5000); }
        const auto firstUsed=checked_heap_usage(space,false);assert(firstUsed>baseline);
        std::memset(first,0x57,5000);
        assert(checked_heap_usage(space,false)==firstUsed);
        assert(J::EndFrame()&&J::RestoreTo(10));
        assert(checked_heap_usage(space,true)==baseline);

        assert(J::BeginFrame(20));
        { J::GameScope game;assert(J::Allocate(8192)); }
        assert(J::EndFrame());
        const auto outerUsed=checked_heap_usage(space,false);assert(outerUsed>baseline);
        assert(J::BeginFrame(21));
        { J::GameScope game;
          J::Release(anchor);assert(J::Allocate(16384));
          auto* aligned=J::AllocateAligned(4096,6000);
          assert(reinterpret_cast<std::uintptr_t>(aligned)%4096==0); }
        const auto nestedUsed=checked_heap_usage(space,false);assert(nestedUsed>outerUsed);
        assert(J::EndFrame()&&J::RestoreTo(21));
        assert(checked_heap_usage(space,true)==outerUsed);
        assert(J::BeginFrame(21));
        { J::GameScope game;assert(J::Allocate(24000)); }
        assert(checked_heap_usage(space,false)>outerUsed);
        assert(J::EndFrame()&&J::RestoreTo(20));
        assert(checked_heap_usage(space,true)==baseline);
        J::Clear();assert(checked_heap_usage(space,true)==baseline);

        assert(J::BeginFrame(30));
        void* retained=nullptr;
        { J::GameScope game;retained=J::Allocate(7000); }
        assert(J::EndFrame());
        const auto retainedUsed=checked_heap_usage(space,false);assert(retainedUsed>baseline);
        // Clear discards history; it does not rewind the live allocation.
        J::Clear();assert(checked_heap_usage(space,true)==retainedUsed);
        J::Release(retained);assert(checked_heap_usage(space,false)==baseline);
        assert(J::BeginFrame(31)&&J::EndFrame());
        J::DiscardBefore(32);assert(checked_heap_usage(space,false)==baseline);
        assert(J::BeginFrame(32));std::memset(anchor,0x65,256);
        assert(checked_heap_usage(space,false)==baseline);
        assert(J::EndFrame()&&J::RestoreTo(32));
        assert(checked_heap_usage(space,true)==baseline);
        J::Release(anchor);assert(checked_heap_usage(space,false)==initial);
        J::Clear();assert(checked_heap_usage(space,true)==initial);
    } else if (scenario==10||scenario==11) {
        auto* space=eagler_sim_test_last_created_space();assert(space);
        const auto initial=checked_heap_usage(space,true);
        assert(J::BeginFrame(100));
        void* allocation=nullptr;
        { J::GameScope game;allocation=J::Allocate(16384); }
        const auto current=checked_heap_usage(space,false);assert(current>initial);
        if(scenario==10) assert(J::EndFrame());
        // Missing target and open-boundary rejection both retain the live
        // allocation. Neither may retain an old cache across the failed call.
        assert(!J::RestoreTo(scenario==10?101:100)&&J::Failed());
        assert(checked_heap_usage(space,true)==current);
        assert(!J::RestoreTo(100));
        assert(checked_heap_usage(space,true)==current);
        J::Clear();assert(checked_heap_usage(space,true)==current);
        J::Release(allocation);assert(checked_heap_usage(space,false)==initial);
    } else if (scenario>=12&&scenario<=14) {
        // Fill the real bounded log without modifying allocator metadata. The
        // next frame's first allocator write must fail inside its wrapper,
        // while the usage cache was valid on entry to malloc/memalign/free.
        auto* space=eagler_sim_test_last_created_space();assert(space);
        const auto* config=reinterpret_cast<const volatile std::uint32_t*>(
            std::uintptr_t(eagler_rb_config_address()));
        void* allocation=nullptr;
        if (scenario==14) { J::GameScope game;allocation=J::Allocate(8192); }
        const auto before=checked_heap_usage(space,true);
        assert(config[2]==8*1024*1024&&config[3]==0&&config[4]==0);
        for (std::uint32_t frame=0;frame<4;++frame) {
            assert(J::BeginFrame(frame));
            J::Touch(reinterpret_cast<void*>(std::uintptr_t(config[1])),config[2]);
            assert(!J::Failed()&&J::EndFrame());
        }
        const auto full=J::Stats();
        assert(full.used_bytes==full.capacity_bytes&&full.frames==4);
        assert(checked_heap_usage(space,false)==before);
        assert(J::BeginFrame(4)&&J::Ready()&&J::FrameOpen()&&config[0]==1);
        const auto scans=eagler_sim_test_used_scans();
        if (scenario==14) J::Release(allocation);
        else {
            J::GameScope game;
            allocation=scenario==12?J::Allocate(8192):J::AllocateAligned(4096,6000);
            assert(allocation&&J::OwnedByGameHeap(allocation));
        }
        // Do not call Stats before the cold-query assertion: that would hide
        // accidental cache reactivation after Fail inside allocator writes.
        assert(J::Failed()&&!J::Ready()&&J::FrameOpen()&&config[0]==0);
        assert(std::strcmp(J::Error(),"Write journal capacity exhausted")==0);
        assert(eagler_sim_test_used_scans()==scans);
        const auto after=checked_heap_usage(space,true);
        assert(scenario==14?after<before:after>before);
        assert(!J::EndFrame()&&!J::FrameOpen()&&J::Failed());
        assert(checked_heap_usage(space,false)==after);
        J::Clear();assert(checked_heap_usage(space,true)==after);
        assert(J::Failed()&&!J::Ready()&&config[0]==0);
        assert(std::strcmp(J::Error(),"Write journal capacity exhausted")==0);
    } else return 0;
    std::printf("write-journal scenario %d passed\n",scenario);
    return 1;
}
