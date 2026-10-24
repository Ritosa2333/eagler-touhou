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
    assert(initialize(scenario==5));
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
    } else return 0;
    std::printf("write-journal scenario %d passed\n",scenario);
    return 1;
}
