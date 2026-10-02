#include "../th10_web/cpp/multiplayer/RollbackPoolCapture.hpp"
#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

// No title ABI, retail resources, WASI, or 32-bit headers are required. Keep
// this synthetic payload deliberately misaligned to exercise Storage padding.
namespace {
constexpr std::size_t Payload = 37, Capacity = 64;
using Pool = th10::multiplayer::RollbackPool<Payload,Capacity>;
using Capture = th10::multiplayer::RollbackPoolCapture<Capacity>;
constexpr std::size_t Stride = sizeof(Pool::Storage);
static_assert(Stride >= Payload);

[[noreturn]] void fail(const char* expression,int line){
    std::fprintf(stderr,"rollback pool capture: line %d: %s\n",line,expression);
    std::abort();
}
#define CHECK(expression) do { if(!(expression))fail(#expression,__LINE__); } while(false)

std::uint32_t random_state = 0x683dfa21u;
std::uint32_t random_word(){
    random_state = random_state * 1664525u + 1013904223u;
    return random_state;
}

void seed(Pool& pool){
    auto* bytes=reinterpret_cast<unsigned char*>(pool.blocks);
    for(std::size_t i=0;i<sizeof(pool.blocks);++i)
        bytes[i]=static_cast<unsigned char>(random_word()>>24);
    std::memset(pool.occupied,0,sizeof(pool.occupied));
    pool.cursor=0;
}

bool same(const Pool& left,const Pool& right){
    return std::memcmp(left.blocks,right.blocks,sizeof(left.blocks))==0&&
           std::memcmp(left.occupied,right.occupied,sizeof(left.occupied))==0&&
           left.cursor==right.cursor;
}

void fill(Pool& pool,std::size_t slot,unsigned char value){
    // Mutate even alignment padding: exact-byte undo must preserve it too.
    std::memset(pool.at(slot),value,Stride);
}

void configure(Netplay::RollbackJournal& journal,bool coalesce=true){
    CHECK(journal.Reset({14,sizeof(Pool)+Stride,Capacity+8,true,coalesce}));
}

struct ScalarCapture {
    std::array<bool,Capacity> captured{};
    bool capture(Pool& pool,Netplay::RollbackJournal& journal){
        captured.fill(false);
        if(!journal.Touch(pool.occupied,sizeof(pool.occupied))||
           !journal.Touch(&pool.cursor,sizeof(pool.cursor)))return false;
        for(std::size_t i=0;i<Capacity;++i)if(pool.active(i)){
            if(!journal.Touch(pool.at(i),Stride))return false;
            captured[i]=true;
        }
        return true;
    }
    bool touch(Pool& pool,std::size_t slot,Netplay::RollbackJournal& journal){
        if(captured[slot])return true;
        if(!journal.Touch(pool.at(slot),Stride))return false;
        captured[slot]=true;
        return true;
    }
};

void dense_reuse_and_padding(){
    Pool pool{};seed(pool);
    std::memset(pool.occupied,1,sizeof(pool.occupied));
    const Pool before=pool;
    Netplay::RollbackJournal journal;configure(journal);
    Capture capture;
    const auto save=[&](void* address,std::size_t bytes){return journal.Touch(address,bytes);};
    CHECK(journal.BeginFrame(0));
    CHECK(capture.Capture(pool,save));
    CHECK(journal.BlocksForFrame(0)==3);
    CHECK(journal.BytesForFrame(0)==sizeof(pool.occupied)+sizeof(pool.cursor)+sizeof(pool.blocks));
    for(std::size_t i=0;i<Capacity;++i){
        CHECK(capture.OwnsSlot(pool.at(i),Payload));
        CHECK(capture.OwnsSlot(pool.at(i),Stride));
        fill(pool,i,static_cast<unsigned char>(i+91));
        CHECK(pool.release(pool.at(i)));
        pool.cursor=static_cast<std::uint32_t>(i);
        CHECK(pool.allocate(Payload,true)==pool.at(i));
        // The original live run owns the checkpoint-start bytes, even after
        // a zeroing allocation and a later whole-slot preservation request.
        CHECK(capture.TouchSlot(pool.at(i),i%2?Payload:Stride,save));
        fill(pool,i,static_cast<unsigned char>(i+121));
    }
    CHECK(journal.BlocksForFrame(0)==3);
    CHECK(journal.EndFrame());
    CHECK(journal.UndoTo(0));
    CHECK(same(pool,before));
}

void sparse_dormant_and_clear(){
    Pool pool{};seed(pool);
    for(const auto i:{1u,2u,5u,8u,9u})pool.occupied[i]=1;
    const Pool before=pool;
    Netplay::RollbackJournal journal;configure(journal);
    Capture capture;
    const auto save=[&](void* address,std::size_t bytes){return journal.Touch(address,bytes);};
    CHECK(journal.BeginFrame(0));
    CHECK(capture.Capture(pool,save));
    CHECK(journal.BlocksForFrame(0)==5); // metadata plus three live runs
    CHECK(journal.BytesForFrame(0)==sizeof(pool.occupied)+sizeof(pool.cursor)+5*Stride);
    pool.cursor=0;
    void* dormant=pool.allocate(Payload,false);
    CHECK(dormant==pool.at(0));
    // Match StageHint's nonzeroing allocation followed by a prewrite touch.
    CHECK(capture.TouchSlot(dormant,Stride,save));
    CHECK(journal.BlocksForFrame(0)==6);
    CHECK(journal.BytesForFrame(0)==sizeof(pool.occupied)+sizeof(pool.cursor)+6*Stride);
    fill(pool,0,0xa7);
    CHECK(capture.TouchSlot(dormant,Payload,save));
    CHECK(journal.BlocksForFrame(0)==6);
    CHECK(pool.release(dormant));pool.cursor=0;
    CHECK(pool.allocate(Payload,true)==dormant);
    CHECK(capture.TouchSlot(dormant,Payload,save));
    fill(pool,0,0x21);
    for(std::size_t i=0;i<Capacity;++i)if(pool.active(i))fill(pool,i,0x49);
    CHECK(journal.EndFrame());CHECK(journal.UndoTo(0));CHECK(same(pool,before));

    // Clearing coverage between histories must forget old live-run ownership.
    capture.Clear();
    CHECK(!capture.OwnsSlot(pool.at(1),Payload));
    CHECK(journal.BeginFrame(0));
    CHECK(capture.Capture(pool,save));
    CHECK(capture.TouchSlot(pool.at(0),Payload,save));
    fill(pool,0,0x8b);
    CHECK(journal.EndFrame());CHECK(journal.UndoTo(0));CHECK(same(pool,before));
}

void invalid_ownership(){
    Pool pool{};seed(pool);
    Capture capture;
    CHECK(!capture.OwnsSlot(pool.at(0),Payload));
    CHECK(capture.Capture(pool,[](void*,std::size_t){return true;}));
    unsigned char outside[Stride]{};
    CHECK(!capture.OwnsSlot(nullptr,Payload));
    CHECK(!capture.OwnsSlot(outside,Payload));
    CHECK(!capture.OwnsSlot(reinterpret_cast<unsigned char*>(pool.at(0))+1,Payload));
    CHECK(!capture.OwnsSlot(static_cast<void*>(pool.blocks+Capacity),Payload));
    CHECK(!capture.OwnsSlot(pool.occupied,Payload));
    CHECK(!capture.OwnsSlot(&pool.cursor,Payload));
    CHECK(!capture.OwnsSlot(pool.at(0),0));
    CHECK(!capture.OwnsSlot(pool.at(0),Payload-1));
    CHECK(!capture.OwnsSlot(pool.at(0),Stride+1));
    CHECK(capture.OwnsSlot(pool.at(Capacity-1),Payload));
    CHECK(capture.OwnsSlot(pool.at(Capacity-1),Stride));
    unsigned forwarded=0;
    const auto foreign=[&](void* address,std::size_t bytes){
        ++forwarded;CHECK(address==outside);CHECK(bytes==Payload-1);return false;
    };
    CHECK(!capture.TouchSlot(outside,Payload-1,foreign));CHECK(forwarded==1);
    Pool replacement{};seed(replacement);
    CHECK(capture.Capture(replacement,[](void*,std::size_t){return true;}));
    CHECK(!capture.OwnsSlot(pool.at(0),Payload));
    CHECK(capture.OwnsSlot(replacement.at(0),Payload));
    capture.Clear();CHECK(!capture.OwnsSlot(pool.at(0),Stride));
    CHECK(!capture.OwnsSlot(replacement.at(0),Stride));
}

void dialogue_duplicate_subrange(){
    // RollbackState also captures the GUI's dialogue pointer separately. With
    // adjacent live dialogue slots that duplicate MUST use capture routing;
    // sending it directly to the strict journal would overlap the live run.
    using DialoguePool=th10::multiplayer::RollbackPool<144,16>;
    DialoguePool pool{};pool.occupied[2]=pool.occupied[3]=1;
    th10::multiplayer::RollbackPoolCapture<16> capture;
    Netplay::RollbackJournal journal;
    CHECK(journal.Reset({2,sizeof(pool),32,true,true}));
    CHECK(journal.BeginFrame(0));
    const auto save=[&](void* address,std::size_t bytes){return journal.Touch(address,bytes);};
    CHECK(capture.Capture(pool,save));
    CHECK(capture.TouchSlot(pool.at(2),144,save));
    CHECK(capture.TouchSlot(pool.at(3),144,save));
    CHECK(journal.BlocksForFrame(0)==3);
    CHECK(!journal.Touch(pool.at(2),144));CHECK(journal.Failed());
}

void failed_capture_and_retry(){
    Pool pool{};seed(pool);pool.occupied[3]=pool.occupied[4]=1;
    Capture capture;
    unsigned calls=0;
    CHECK(!capture.Capture(pool,[&](void*,std::size_t){return ++calls<3;}));
    CHECK(calls==3); // metadata saved, live-run capture rejected
    calls=0;
    const auto reject=[&](void*,std::size_t){++calls;return false;};
    CHECK(!capture.TouchSlot(pool.at(3),Payload,reject));
    CHECK(!capture.TouchSlot(pool.at(3),Stride,reject));
    CHECK(calls==2); // a failed run/slot must not be marked captured
    const auto accept=[&](void* address,std::size_t bytes){
        ++calls;CHECK(address==pool.at(3));CHECK(bytes==Stride);return true;
    };
    CHECK(capture.TouchSlot(pool.at(3),Payload,accept));
    CHECK(capture.TouchSlot(pool.at(3),Stride,accept));CHECK(calls==3);

    Netplay::RollbackJournal journal;
    CHECK(journal.Reset({2,sizeof(pool.occupied)+sizeof(pool.cursor)+Stride,Capacity+8}));
    CHECK(journal.BeginFrame(0));
    capture.Clear();
    CHECK(!capture.Capture(pool,[&](void* address,std::size_t bytes){return journal.Touch(address,bytes);}));
    CHECK(journal.Failed());
    CHECK(!journal.EndFrame());
}

struct Pair {
    Pool batched{},scalar{};
    Capture capture;
    ScalarCapture oracle;
    Netplay::RollbackJournal journal,reference;
    explicit Pair(bool coalesce){seed(batched);scalar=batched;configure(journal,coalesce);configure(reference,coalesce);}
    void begin(std::uint32_t frame){
        CHECK(journal.BeginFrame(frame));CHECK(reference.BeginFrame(frame));
        CHECK(capture.Capture(batched,[&](void* address,std::size_t bytes){return journal.Touch(address,bytes);}));
        CHECK(oracle.capture(scalar,reference));
        CHECK(journal.BytesForFrame(frame)==reference.BytesForFrame(frame));
    }
    void touch(std::size_t slot){
        const auto bytes=slot%2?Payload:Stride;
        CHECK(capture.TouchSlot(batched.at(slot),bytes,[&](void* address,std::size_t size){return journal.Touch(address,size);}));
        CHECK(oracle.touch(scalar,slot,reference));
    }
    void mutate(){
        for(std::size_t i=0;i<Capacity;++i)if(batched.active(i)){
            const auto value=static_cast<unsigned char>(random_word()>>24);
            fill(batched,i,value);fill(scalar,i,value);
            if((random_word()&3u)==0){CHECK(batched.release(batched.at(i)));CHECK(scalar.release(scalar.at(i)));}
        }
        for(unsigned event=0;event<19;++event){
            const auto slot=random_word()%Capacity;
            // Explicitly preserve every dormant first write. This intentionally
            // does not claim that an unhooked allocate(...,true) saves old bytes.
            touch(slot);
            if(batched.active(slot)){CHECK(batched.release(batched.at(slot)));CHECK(scalar.release(scalar.at(slot)));}
            batched.cursor=scalar.cursor=slot;
            const bool zero=(random_word()&1u)!=0;
            CHECK(batched.allocate(Payload,zero)==batched.at(slot));
            CHECK(scalar.allocate(Payload,zero)==scalar.at(slot));
            const auto value=static_cast<unsigned char>(random_word()>>24);
            fill(batched,slot,value);fill(scalar,slot,value);
            touch(slot); // repeated/after-write request must retain first bytes
        }
        CHECK(same(batched,scalar));
    }
    void end(std::uint32_t frame){
        CHECK(journal.BytesForFrame(frame)==reference.BytesForFrame(frame));
        CHECK(journal.EndFrame());CHECK(reference.EndFrame());
    }
    void undo(std::uint32_t frame,const Pool& expected){
        std::uint32_t got=~0u,wanted=~0u;
        CHECK(journal.UndoTo(frame,&got));CHECK(reference.UndoTo(frame,&wanted));
        CHECK(got==frame&&wanted==frame);CHECK(same(batched,expected));CHECK(same(scalar,expected));
    }
};

void changing_runs_history_and_continuation(bool coalesce){
    Pair pair(coalesce);
    for(std::size_t i=0;i<Capacity;++i)pair.batched.occupied[i]=pair.scalar.occupied[i]=i%3!=0;
    const Pool original=pair.batched;
    std::vector<Pool> history;
    for(std::uint32_t frame=0;frame<12;++frame){
        history.push_back(pair.batched);pair.begin(frame);pair.mutate();pair.end(frame);
    }
    CHECK(pair.journal.FrameCount()==12);pair.undo(0,original);
    CHECK(pair.journal.FrameCount()==0);
    // Replay after rewind must reset discarded-future coverage, then continue
    // through ring eviction and confirmation under changing live-run shapes.
    history.clear();
    for(std::uint32_t frame=0;frame<70;++frame){
        history.push_back(pair.batched);pair.begin(frame);pair.mutate();pair.end(frame);
        if(frame==29){pair.journal.DiscardBefore(24);pair.reference.DiscardBefore(24);}
    }
    CHECK(pair.journal.FrameCount()==14);pair.undo(58,history[58]);
    for(std::uint32_t frame=58;frame<71;++frame){
        const Pool before=pair.batched;pair.begin(frame);pair.mutate();pair.end(frame);
        if(frame==65){pair.undo(frame,before);pair.begin(frame);pair.mutate();pair.end(frame);}
    }
    CHECK(!pair.journal.Failed()&&!pair.reference.Failed());
}

void elided_frame_then_capture(){
    Pair pair(true);
    pair.batched.occupied[1]=pair.scalar.occupied[1]=1;
    pair.begin(0);pair.mutate();pair.end(0);
    pair.journal.DiscardBefore(1);pair.reference.DiscardBefore(1);
    // An elided frame never opens a journal and must never call TouchSlot.
    // Match the RollbackState early return, mutating the two worlds directly.
    for(std::size_t i=0;i<Capacity;++i){
        pair.batched.occupied[i]=pair.scalar.occupied[i]=i%2==0;
        fill(pair.batched,i,static_cast<unsigned char>(i));fill(pair.scalar,i,static_cast<unsigned char>(i));
    }
    pair.batched.cursor=pair.scalar.cursor=17;
    CHECK(pair.journal.FrameCount()==0&&!pair.journal.IsFrameOpen());
    const Pool after_elided=pair.batched;
    pair.begin(2);pair.mutate();pair.end(2);pair.undo(2,after_elided);
}
} // namespace

int main(){
    dense_reuse_and_padding();sparse_dormant_and_clear();invalid_ownership();
    dialogue_duplicate_subrange();failed_capture_and_retry();changing_runs_history_and_continuation(false);
    changing_runs_history_and_continuation(true);elided_frame_then_capture();
    std::printf("rollback pool capture: PASS payload=%zu stride=%zu; dense/sparse padding, dormant first writes, reuse, 12-frame undo, ring/continuation, elision and failures\n",Payload,Stride);
}
