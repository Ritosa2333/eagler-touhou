#include "WorldState.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <numeric>

using namespace th09;
using namespace th09::multiplayer;
#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);std::abort();} } while(0)

// Exercise the actual WorldState first-write path without constructing game
// resources. The test has no production export or alternate capture algorithm.
namespace th09::multiplayer {
struct WorldStateEnemyCaptureTest {
    static void Begin(WorldState& state,EclVm* firstPrototype=nullptr,EclVm* firstPool=nullptr,
        EclVm* secondPrototype=nullptr,EclVm* secondPool=nullptr,bool hash=false){
        if(state.journal.IsFrameOpen())CHECK(state.journal.EndFrame());
        state.journal.DiscardBefore(1);
        state.ResetEnemyCapture();
        state.enemyRanges={{{firstPrototype,firstPool},{secondPrototype,secondPool}}};
        state.ownedBytes=0;state.hashEnabled=hash;state.hashParts.clear();state.digest={};
        state.good=state.journal.BeginFrame(0);CHECK(state.good);
    }
    static std::size_t Count(const WorldState& state){return state.enemyCount;}
    static std::size_t KnownCount(const WorldState& state){return state.enemySaved.count();}
    static EclVm* Owner(const WorldState& state,std::size_t at){CHECK(at<state.enemyCount);return state.enemies[at].first;}
    static std::size_t OwnedBytes(const WorldState& state){return state.ownedBytes;}
    static std::uintptr_t Storage(const WorldState& state){return reinterpret_cast<std::uintptr_t>(state.enemies.data());}
    static bool RestoreFirst(WorldState& state){CHECK(state.enemyCount);return state.enemies[0].second.Restore(*state.enemies[0].first);}
};
}
using Access=WorldStateEnemyCaptureTest;

struct Pool {
    EclVm before,prototype;
    std::array<EclVm,EnemyManager::capacity+1> slots;
    EclVm after;
};

template<class T>void append(std::vector<unsigned char>& bytes,const T& value){
    static_assert(std::is_trivially_copyable_v<T>);
    const auto* first=reinterpret_cast<const unsigned char*>(&value);
    bytes.insert(bytes.end(),first,first+sizeof(value));
}

// Compare every named POD block, including padding, and every complete owned
// context. Only owned allocation addresses are normalized to their identity;
// external pointers and the original context/locals relationships are retained.
std::vector<unsigned char> complete_bytes(const EclVm& e){
    std::vector<unsigned char> bytes;bytes.reserve(sizeof(EclVm)+4*sizeof(EclContext));
    EclVariables values;std::memcpy(&values,&e.values,sizeof(values));values.locals=nullptr;
    append(bytes,values);append(bytes,e.program);append(bytes,e.primary);
    int active=e.active==nullptr?-2:e.active==&e.primary?-1:-3;
    int locals=e.values.locals==nullptr?-2:e.values.locals==&e.primary.locals?-1:-3;
    for(unsigned i=0;i<4;++i){
        append(bytes,bool(e.asynchronous[i]));
        if(e.asynchronous[i]){
            append(bytes,*e.asynchronous[i]);
            if(e.active==e.asynchronous[i].get())active=int(i);
            if(e.values.locals==&e.asynchronous[i]->locals)locals=int(i);
        }
    }
    CHECK(active!=-3&&locals!=-3);append(bytes,active);append(bytes,locals);
#define BYTES(field) append(bytes,e.field)
    BYTES(generations);BYTES(active_slot);BYTES(scratch_depth);BYTES(behavior_flags);BYTES(difficulty_flags);
    BYTES(pending_interrupt);BYTES(interrupt_subroutines);BYTES(position_offset);BYTES(velocity);
    BYTES(movement);BYTES(emitter);BYTES(lasers);BYTES(animation);BYTES(status);BYTES(trail);
    BYTES(finished);BYTES(invalid);BYTES(failed_opcode);
#undef BYTES
    return bytes;
}

void seed(EclVm& e,unsigned value){
    e=EclVm{};e.behavior_flags=value%3?1:0;e.difficulty_flags=value*17;
    e.generations[1]=value;e.primary.locals.integers[0]=int(value+100);
    e.primary.stack[15].locals.floats[7]=float(value)+.25f;e.pending_interrupt=i16(value%300);
    e.position_offset={float(value),7,11};e.trail.history[95].position={9,float(value),13};
    // Defined POD bytes make the padding part of this fixture's exact oracle.
    std::memset(&e.animation,0,sizeof(e.animation));e.animation.layers[2].pendingInterrupt=int(value+19);
    if(value%7==0){
        e.asynchronous[1]=std::make_unique<EclContext>();
        e.asynchronous[1]->stack[15].locals.integers[7]=int(value+300);
        e.active=e.asynchronous[1].get();e.active_slot=1;
    }else if(value%3)e.active=&e.primary;
    if(value%5)e.bind_context();
}

// Independent retained version of the old linear first-write rule. The owning
// copy remains the unchanged EnemyState implementation; the byte vector above
// also checks its complete result independently of WorldState's hash.
struct LinearOracle {
    struct Entry { EclVm* owner=nullptr;EnemyState state;std::vector<unsigned char> bytes;u32 fingerprint=0; };
    std::vector<Entry> entries;
    std::size_t count=0,comparisons=0,ownedBytes=0;
    StateChecksum digest;
    // EnemyState contains references to its own primary context. This oracle
    // must never relocate those objects after saving them.
    LinearOracle(){entries.reserve(265);}
    void Begin(){count=comparisons=ownedBytes=0;digest={};}
    bool Capture(EclVm& e){
        for(std::size_t i=0;i<count;++i){++comparisons;if(entries[i].owner==&e)return false;}
        if(count==entries.size())entries.emplace_back();auto& entry=entries[count++];
        entry.owner=&e;CHECK(entry.state.Save(e));entry.bytes=complete_bytes(e);
        entry.fingerprint=EnemyState::Fingerprint(e);digest.Add(entry.fingerprint);ownedBytes+=entry.state.Bytes();return true;
    }
};

std::vector<EclVm*> owners(Pool& first,Pool& second,EclVm& external){
    std::vector<EclVm*> all;
    for(auto* pool:{&first,&second}){
        all.push_back(&pool->prototype);for(auto& e:pool->slots)all.push_back(&e);
    }
    all.insert(all.end(),{&first.before,&first.after,&second.before,&second.after,&external});return all;
}

void capture(WorldState& state,LinearOracle& oracle,EclVm& e,unsigned changed){
    oracle.Capture(e);state.BeforeEnemy(e);CHECK(state.Good());
    CHECK(Access::Count(state)==oracle.count);
    CHECK(Access::Owner(state,oracle.count-1)==oracle.entries[oracle.count-1].owner);
    seed(e,changed); // Includes deleting and recreating asynchronous owners.
}

void verify(WorldState& state,LinearOracle& oracle,bool hash){
    CHECK(Access::Count(state)==oracle.count&&Access::OwnedBytes(state)==oracle.ownedBytes);
    for(std::size_t i=0;i<oracle.count;++i)CHECK(Access::Owner(state,i)==oracle.entries[i].owner);
    if(hash){
        CHECK(state.Fingerprint()==oracle.digest.value&&state.hashParts.size()==oracle.count);
        for(const auto& part:state.hashParts)CHECK(part.first=="dynamic enemy");
    }
    CHECK(state.Restore());
    for(std::size_t i=0;i<oracle.count;++i){
        auto& entry=oracle.entries[i];CHECK(complete_bytes(*entry.owner)==entry.bytes);
        seed(*entry.owner,unsigned(19000+i));CHECK(entry.state.Restore(*entry.owner));
        CHECK(complete_bytes(*entry.owner)==entry.bytes);
        CHECK(EnemyState::Fingerprint(*entry.owner)==entry.fingerprint);
    }
}

int main(){
    auto first=std::make_unique<Pool>(),second=std::make_unique<Pool>();EclVm external;
    const auto all=owners(*first,*second,external);
    CHECK(all.size()==265);
    WorldState state;LinearOracle oracle;std::size_t cases=0,denseComparisons=0;
    // Calls before a world has registered its ranges keep the original
    // fallback, including repeat calls and owners adjacent to a known pool.
    state.BeforeEnemy(first->prototype);state.BeforeEnemy(first->prototype);
    CHECK(Access::Count(state)==1&&Access::KnownCount(state)==0);
    for(unsigned round=0;round<24;++round){
        const bool registered=round%6!=0,hash=round%3!=0,mixed=round%4==0;
        for(std::size_t i=0;i<all.size();++i)seed(*all[i],unsigned(i+round*300));
        Access::Begin(state,registered?&first->prototype:nullptr,registered?first->slots.data():nullptr,
            registered?&second->prototype:nullptr,registered?second->slots.data():nullptr,hash);oracle.Begin();
        // The old WorldState reserves 260 owning snapshots. Test its complete
        // supported dense inventory separately from additional unknown owners,
        // whose behavior is verified below with ample remaining capacity.
        std::vector<std::size_t> order;
        if(mixed)order={0,130,129,259,200,17,90,230,260,261,262,263,264};
        else {order.resize(260);std::iota(order.begin(),order.end(),0);}
        if(round%3==1)std::reverse(order.begin(),order.end());
        if(round%3==2)for(std::size_t i=0;i<order.size();++i)std::swap(order[i],order[(i*37+round*11)%order.size()]);
        // Cross-pool boss aliases arrive before either pool's ordinary walk.
        for(auto at:{std::size_t(200),std::size_t(129),std::size_t(130),std::size_t(0),std::size_t(259)})
            capture(state,oracle,*all[at],unsigned(6000+at+round));
        for(auto at:order){
            capture(state,oracle,*all[at],unsigned(7000+at+round));
            capture(state,oracle,*all[at],unsigned(8000+at+round));
        }
        CHECK(Access::KnownCount(state)==(registered?(mixed?8:260):0));
        if(round==1)denseComparisons=oracle.comparisons;
        verify(state,oracle,hash);++cases;
    }
    // Reuse all nine history records across both retirement and restoration.
    // New epochs may register the two pools in a different order; their saved
    // prefixes and identity bits must never leak from the old ring occupant.
    std::array<std::unique_ptr<WorldState>,9> ring;
    for(auto& slot:ring)slot=std::make_unique<WorldState>();
    for(unsigned frame=0;frame<90;++frame){
        auto& record=*ring[frame%ring.size()];const bool swapped=(frame/9)%2!=0;
        Pool& a=swapped?*second:*first;Pool& b=swapped?*first:*second;
        Access::Begin(record,&a.prototype,a.slots.data(),&b.prototype,b.slots.data(),true);oracle.Begin();
        for(auto at:{std::size_t(0),std::size_t(130),std::size_t(129),std::size_t(259),std::size_t(264)}){
            seed(*all[at],unsigned(frame*100+at));capture(record,oracle,*all[at],unsigned(frame*100+at+9000));
            capture(record,oracle,*all[at],unsigned(frame*100+at+10000));
        }
        CHECK(Access::KnownCount(record)==4);
        if(frame%3)verify(record,oracle,true);else CHECK(record.Seal());++cases;
    }
    // Mark-before-copy failure matches the old append-before-Save rule: a
    // transient ECL owner poisons the checkpoint; duplicates do not retry it.
    EclContext transient;
    for(bool registered:{false,true}){
        Access::Begin(state,registered?&first->prototype:nullptr,registered?first->slots.data():nullptr);
        first->prototype.active=&transient;state.BeforeEnemy(first->prototype);
        CHECK(!state.Good()&&Access::Count(state)==1);seed(first->prototype,42);
        state.BeforeEnemy(first->prototype);CHECK(!state.Good()&&Access::Count(state)==1);CHECK(!state.Restore());++cases;
    }
    // Preserve the discovered pre-existing boundary instead of silently making
    // >260 owners green: vector growth relocates EnemyState's self references.
    // With ranges unregistered, BeforeEnemy executes the exact old linear
    // fallback; registered ranges reproduce the same failure at the same size.
    // The first saved owner restores successfully until storage actually moves.
    for(bool registered:{false,true}){
        WorldState legacyBoundary;
        Access::Begin(legacyBoundary,registered?&first->prototype:nullptr,registered?first->slots.data():nullptr,
            registered?&second->prototype:nullptr,registered?second->slots.data():nullptr);
        const auto storage=Access::Storage(legacyBoundary);
        for(std::size_t i=0;i<all.size();++i){
            seed(*all[i],unsigned(i+1));legacyBoundary.BeforeEnemy(*all[i]);CHECK(legacyBoundary.Good());
            if(i==259){CHECK(Access::Storage(legacyBoundary)==storage);CHECK(Access::RestoreFirst(legacyBoundary));}
        }
        CHECK(Access::Count(legacyBoundary)==265&&Access::Storage(legacyBoundary)!=storage);
        CHECK(!Access::RestoreFirst(legacyBoundary));CHECK(!legacyBoundary.Restore());++cases;
    }
    std::puts("Retained legacy boundary: 265 owners relocate EnemyState; original linear fallback and registered bitmap both fail Restore after the same growth");
    std::printf("Enemy first-write: %zu complete owning-state/order/hash/fallback/ring and retained-boundary cases PASS; dense 260 known owners and separate 8 known + 5 fallback owners; dense old-linear pointer comparisons=%zu (structural count, not timing)\n",cases,denseComparisons);
    return 0;
}
