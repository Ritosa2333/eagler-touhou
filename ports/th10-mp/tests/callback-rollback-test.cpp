// This unit links the real browser::Callbacks and UpdateChain implementations.
// The receiver fixture supplies only the journal owner. It NEVER overrides
// allocate_entry or clears callback storage on behalf of production.
// Application/AnimationEngine are not linked: their final MarkSimulated gate
// remains an integration obligation, not a claim made by this primitive suite.
#include "Callbacks.hpp"
#include "../web/cpp/multiplayer/RollbackPoolCapture.hpp"
#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if !defined(TH_NATIVE_PLATFORM) || !defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
#error This suite requires the actual native MP callback layout.
#endif

namespace {
using namespace th10;
using namespace th10::browser;
using Pool=decltype(std::declval<Callbacks&>().rollback_entries);
constexpr std::size_t Capacity=4096,Payload=sizeof(UpdateChainEntry),Stride=sizeof(Pool::Storage);
static_assert(sizeof(void*)==4&&Stride>=Payload);
static_assert(std::is_trivially_copyable_v<Pool>);
constexpr auto UpdateToken=callback_id::ScreenFadeUpdate,DrawToken=callback_id::ScreenFadeDraw;

struct Totals {
    std::uint64_t checks=0,failures=0,full_pool_comparisons=0,full_pool_mismatches=0;
    std::uint64_t preclear_observations=0,preclear_misses=0;
    std::uint64_t budget_cases=0,budget_rejections=0,budget_unexpected_successes=0;
    std::uint64_t budget_payload_changed=0,positive_control_failures=0;
} totals;
struct CaseResult {std::string name;bool packed=false;std::uint64_t checks=0,failures=0;};
std::vector<CaseResult> cases;
const char* current_case="initialization";

[[noreturn]] void broken_fixture(const char* expression,int line){
    std::fprintf(stderr,"callback rollback fixture: %s line %d: %s\n",current_case,line,expression);
    std::abort();
}
#define REQUIRE(expression) do { if(!(expression))broken_fixture(#expression,__LINE__); } while(false)
void expect(bool value,const char* expression,int line){
    ++totals.checks;if(value)return;++totals.failures;
    if(totals.failures<=12)
        std::fprintf(stderr,"callback rollback oracle: %s line %d: %s\n",current_case,line,expression);
}
#define EXPECT(expression) expect(bool(expression),#expression,__LINE__)

struct Owner {
    UpdateChainEntry* update=nullptr;UpdateChainEntry* draw=nullptr;
    u32 updates=0,draws=0;
};
struct Fixture final:CallbackReceiver {
    Callbacks callbacks;
    UpdateChain chain{};
    Owner owner{};
    multiplayer::RollbackPoolCapture<Capacity> capture;
    Netplay::RollbackJournal journal;
    bool packed=false,frame_open=false;
    std::uint64_t first_write_attempts=0,first_write_saved=0,first_write_failed=0;
    const void* expected_first=nullptr;
    std::array<unsigned char,Stride> expected_bytes{};
    bool first_seen=false,first_untouched=false;

    explicit Fixture(unsigned char poison,bool scan=false):callbacks(*this),packed(scan){
        // Only the POD pool is poisoned, never a vtable, receiver or journal.
        std::memset(&pool(),poison,sizeof(Pool));
        std::memset(pool().occupied,0,sizeof(pool().occupied));pool().cursor=0;
        chain.initialize();
        callbacks.bind(UpdateToken,this,[](void*,void* p,i32){++static_cast<Owner*>(p)->updates;return 1;});
        callbacks.bind(DrawToken,this,[](void*,void* p,i32){++static_cast<Owner*>(p)->draws;return 1;});
        reset_journal(sizeof(Pool)+sizeof(chain)+sizeof(owner)+1024,Capacity+32);
    }
    Pool& pool(){return callbacks.rollback_entries;}
    const Pool& pool()const{return callbacks.rollback_entries;}
    void reset_journal(std::size_t bytes,std::size_t blocks){
        REQUIRE(!frame_open);
        REQUIRE(journal.Reset({4,bytes,blocks,true,true}));
        capture.Clear();first_write_attempts=first_write_saved=first_write_failed=0;
    }
    // The legacy header intentionally has no virtual allocation barrier.
    // Both modules compile this same body; only production decides to call it.
    void preserve_callback_entry(UpdateChainEntry* entry)
#ifndef TH10_CALLBACK_ROLLBACK_LEGACY
        override
#endif
    {
        if(!frame_open)return;
        if(entry==expected_first){
            first_seen=true;
            first_untouched=std::memcmp(entry,expected_bytes.data(),Stride)==0;
        }
        const bool saved=capture.TouchSlot(entry,sizeof(UpdateChainEntry),
            [&](void* p,std::size_t size){
                ++first_write_attempts;
                const bool result=journal.Touch(p,size);
                if(result)++first_write_saved;
                return result;
            });
        if(!saved)++first_write_failed;
    }
    void expect_first(std::size_t slot){
        REQUIRE(slot<Capacity);expected_first=pool().at(slot);
        std::memcpy(expected_bytes.data(),expected_first,Stride);
        first_seen=first_untouched=false;
    }
    void begin(u32 frame){
        REQUIRE(!frame_open&&journal.BeginFrame(frame));
        REQUIRE(capture.Capture(pool(),[&](void* p,std::size_t n){return journal.Touch(p,n);},packed));
        REQUIRE(journal.Touch(&chain,sizeof(chain)));
        REQUIRE(journal.Touch(&owner,sizeof(owner)));
        frame_open=true;first_write_attempts=first_write_saved=first_write_failed=0;
    }
    void end(){
        REQUIRE(frame_open&&!journal.Failed());
        REQUIRE(journal.EndFrame());frame_open=false;
        REQUIRE(callbacks.depth==0);
    }
    void undo(u32 frame){
        u32 restored=~u32(0);
        REQUIRE(!frame_open&&journal.UndoTo(frame,&restored)&&restored==frame);
        capture.Clear();expected_first=nullptr;
    }
    void create_pair(){
        REQUIRE(!owner.update&&!owner.draw);
        // Real add -> real allocate_entry -> real memset/initialize/insert.
        owner.update=chain.add(UpdateToken,&owner,14,false,true,callbacks);
        owner.draw=chain.add(DrawToken,&owner,49,true,true,callbacks);
        REQUIRE(owner.update&&owner.draw);
    }
    void run_pair(){
        REQUIRE(chain.run(false,callbacks)==1);
        REQUIRE(chain.run(true,callbacks)==1);
        REQUIRE(callbacks.depth==0);
    }
    void release_pair(){
        REQUIRE(owner.update&&owner.draw);
        chain.remove_locked(owner.update,callbacks);chain.remove_locked(owner.draw,callbacks);
        owner.update=owner.draw=nullptr;
        REQUIRE(callbacks.depth==0);
    }
    void fill_raw(std::size_t count){
        for(std::size_t n=0;n<count;++n)REQUIRE(callbacks.allocate_entry()!=nullptr);
    }
};

struct Snapshot {
    std::vector<unsigned char> pool;
    std::array<unsigned char,sizeof(UpdateChain)> chain{};
    std::array<unsigned char,sizeof(Owner)> owner{};
    Snapshot()=default;
    explicit Snapshot(const Fixture& f):pool(sizeof(Pool)){
        std::memcpy(pool.data(),&f.pool(),pool.size());
        std::memcpy(chain.data(),&f.chain,chain.size());
        std::memcpy(owner.data(),&f.owner,owner.size());
    }
};
void compare(const Fixture& f,const Snapshot& before){
    REQUIRE(before.pool.size()==sizeof(Pool));
    // The independent oracle includes inactive Storage and both padding kinds.
    // Metadata is also checked separately: a dormant-byte failure cannot be
    // mislabeled as allocator cursor/occupancy corruption.
    REQUIRE(std::memcmp(f.pool().occupied,before.pool.data()+offsetof(Pool,occupied),
                        sizeof(f.pool().occupied))==0);
    u32 cursor=0;std::memcpy(&cursor,before.pool.data()+offsetof(Pool,cursor),sizeof(cursor));
    REQUIRE(f.pool().cursor==cursor);
    ++totals.full_pool_comparisons;
    const bool same=std::memcmp(&f.pool(),before.pool.data(),sizeof(Pool))==0;
    if(!same)++totals.full_pool_mismatches;
    EXPECT(same);
    EXPECT(std::memcmp(&f.chain,before.chain.data(),before.chain.size())==0);
    EXPECT(std::memcmp(&f.owner,before.owner.data(),before.owner.size())==0);
}
void check_preclear(const Fixture& f){
    ++totals.preclear_observations;
    if(!f.first_seen||!f.first_untouched)++totals.preclear_misses;
    EXPECT(f.first_seen&&f.first_untouched);
}
template<class Function>
void run_case(const char* name,bool packed,Function function,bool positive=false){
    current_case=name;
    const auto before_checks=totals.checks,before_failures=totals.failures;
    function();
    const auto failures=totals.failures-before_failures;
    cases.push_back({name,packed,totals.checks-before_checks,failures});
    if(positive)totals.positive_control_failures+=failures;
}

void dormant(bool packed,unsigned char poison){
    auto holder=std::make_unique<Fixture>(poison,packed);auto& f=*holder;
    const Snapshot before(f);
    f.begin(0);f.expect_first(0);f.create_pair();f.run_pair();check_preclear(f);
    EXPECT(f.owner.updates==1&&f.owner.draws==1);
    f.end();f.undo(0);compare(f,before);
}

void active_reuse(bool packed){
    auto holder=std::make_unique<Fixture>(0x65,packed);auto& f=*holder;
    f.create_pair();auto* update=f.owner.update;auto* draw=f.owner.draw;
    // Reach cursor wrap through the real allocator, not a test-side cursor edit.
    f.fill_raw(Capacity-2);REQUIRE(f.pool().cursor==0);
    const Snapshot before(f);
    f.begin(0);f.release_pair();f.create_pair();
    EXPECT(f.owner.update==update&&f.owner.draw==draw);
    EXPECT(f.first_write_saved==0); // The initial active run already owns both.
    f.run_pair();f.end();f.undo(0);compare(f,before);
    const auto updates=f.owner.updates,draws=f.owner.draws;
    f.run_pair();EXPECT(f.owner.updates==updates+1&&f.owner.draws==draws+1);
}

void dormant_reuse(bool packed){
    auto holder=std::make_unique<Fixture>(0x91,packed);auto& f=*holder;
    f.fill_raw(Capacity-2);REQUIRE(f.pool().cursor==Capacity-2);
    const Snapshot before(f);
    f.begin(0);f.expect_first(Capacity-2);f.create_pair();check_preclear(f);
    auto* update=f.owner.update;auto* draw=f.owner.draw;
    f.release_pair();f.create_pair(); // Real wrap/scan reuses the two new slots.
    EXPECT(f.owner.update==update&&f.owner.draw==draw);
    EXPECT(f.first_write_saved==2);
    f.run_pair();f.end();f.undo(0);compare(f,before);
}

void history(bool packed){
    auto holder=std::make_unique<Fixture>(0x37,packed);auto& f=*holder;
    std::array<Snapshot,9> before{},after{};
    const auto simulate=[&](u32 frame){
        f.begin(frame);if(f.owner.update)f.release_pair();f.create_pair();f.run_pair();f.end();
    };
    for(u32 frame=0;frame<9;++frame){
        before[frame]=Snapshot(f);simulate(frame);after[frame]=Snapshot(f);
        if(frame%3==2){
            const auto target=frame-1;
            f.undo(target);compare(f,before[target]);
            for(u32 replay=target;replay<=frame;++replay){
                simulate(replay);compare(f,after[replay]);
            }
        }
        if(frame>2)f.journal.DiscardBefore(frame-2);
        EXPECT(f.journal.FrameCount()<=4);
    }
}

void budget(bool packed,bool block_limit){
    auto holder=std::make_unique<Fixture>(0xa7,packed);auto& f=*holder;
    const auto initial_bytes=sizeof(f.pool().occupied)+sizeof(f.pool().cursor)+
                             sizeof(f.chain)+sizeof(f.owner);
    f.reset_journal(block_limit?sizeof(Pool)+1024:initial_bytes+Stride-1,block_limit?3:32);
    f.begin(0);REQUIRE(f.journal.BlocksForFrame(0)==3);
    const Snapshot before(f);
    f.create_pair(); // Failure must not turn an allocated pointer into nullptr.
    EXPECT(f.owner.update&&f.owner.draw);
    const bool changed=std::memcmp(f.pool().blocks,before.pool.data()+offsetof(Pool,blocks),
                                   sizeof(f.pool().blocks))!=0;
    EXPECT(changed);if(changed)++totals.budget_payload_changed;
    EXPECT(f.first_write_failed>0);
    EXPECT(f.journal.Failed());
    const bool ended=f.journal.EndFrame();
    ++totals.budget_cases;
    if(!ended)++totals.budget_rejections;else ++totals.budget_unexpected_successes;
    EXPECT(!ended);
    // No further writes/undo after the failed transaction. This proves the
    // production allocator + common EndFrame boundary, NOT Application's
    // MarkSimulated gate. That integration is explicitly absent from this TU.
}

void without_capture(){
    auto holder=std::make_unique<Fixture>(0x5c);auto& f=*holder;
    auto* entry=f.callbacks.allocate_entry();REQUIRE(entry);
    const auto* bytes=reinterpret_cast<const unsigned char*>(entry);
    bool cleared=true,padding_retained=true;
    for(std::size_t i=0;i<Payload;++i)cleared=cleared&&bytes[i]==0;
    for(std::size_t i=Payload;i<Stride;++i)padding_retained=padding_retained&&bytes[i]==0x5c;
    EXPECT(cleared&&padding_retained);EXPECT(f.first_write_attempts==0);
    EXPECT(f.journal.FrameCount()==0&&!f.journal.IsFrameOpen());
    f.callbacks.release_entry(entry);
}

void exhausted_pool(){
    auto holder=std::make_unique<Fixture>(0);auto& f=*holder;
    f.fill_raw(Capacity);
    // Preserve the pre-existing raw allocator result. Do not pass this nullptr
    // to UpdateChain::add: its separate exhaustion handling is not changed.
    EXPECT(f.callbacks.allocate_entry()==nullptr);
    EXPECT(f.first_write_attempts==0&&f.journal.FrameCount()==0);
}
} // namespace

int main(){
    for(bool packed:{false,true}){
        run_case("cold-inactive",packed,[&]{dormant(packed,0);});
        run_case("poisoned-inactive",packed,[&]{dormant(packed,0xd3);});
        run_case("active-release-reuse",packed,[&]{active_reuse(packed);},true);
        run_case("inactive-create-release-reuse",packed,[&]{dormant_reuse(packed);});
        run_case("history-ring-replay",packed,[&]{history(packed);});
        run_case("byte-budget-failure",packed,[&]{budget(packed,false);});
        run_case("block-budget-failure",packed,[&]{budget(packed,true);});
    }
    run_case("outside-capture",false,without_capture,true);
    run_case("exhausted-pool-existing-result",false,exhausted_pool,true);
#ifdef TH10_CALLBACK_ROLLBACK_LEGACY
    constexpr bool legacy=true;
#else
    constexpr bool legacy=false;
#endif
    std::printf("{\"schema\":1,\"suite\":\"callback-rollback\",\"complete\":true,"
        "\"legacy\":%s,\"passed\":%s,\"checks\":%llu,\"failures\":%llu,"
        "\"payloadBytes\":%zu,\"storageStride\":%zu,\"capacity\":%zu,"
        "\"fullPoolComparisons\":%llu,\"fullPoolMismatches\":%llu,"
        "\"preclearObservations\":%llu,\"preclearMisses\":%llu,"
        "\"budgetCases\":%llu,\"budgetRejections\":%llu,\"budgetUnexpectedSuccesses\":%llu,"
        "\"budgetPayloadChanged\":%llu,\"positiveControlFailures\":%llu,"
        "\"applicationLinked\":false,\"applicationMarkSimulatedCovered\":false,\"cases\":[",
        legacy?"true":"false",totals.failures?"false":"true",
        static_cast<unsigned long long>(totals.checks),static_cast<unsigned long long>(totals.failures),
        Payload,Stride,Capacity,
        static_cast<unsigned long long>(totals.full_pool_comparisons),
        static_cast<unsigned long long>(totals.full_pool_mismatches),
        static_cast<unsigned long long>(totals.preclear_observations),
        static_cast<unsigned long long>(totals.preclear_misses),
        static_cast<unsigned long long>(totals.budget_cases),
        static_cast<unsigned long long>(totals.budget_rejections),
        static_cast<unsigned long long>(totals.budget_unexpected_successes),
        static_cast<unsigned long long>(totals.budget_payload_changed),
        static_cast<unsigned long long>(totals.positive_control_failures));
    for(std::size_t i=0;i<cases.size();++i){
        const auto& value=cases[i];
        std::printf("%s{\"name\":\"%s\",\"packed\":%s,\"checks\":%llu,\"failures\":%llu}",
            i?",":"",value.name.c_str(),value.packed?"true":"false",
            static_cast<unsigned long long>(value.checks),static_cast<unsigned long long>(value.failures));
    }
    std::puts("]}");
    return totals.failures?1:0;
}
