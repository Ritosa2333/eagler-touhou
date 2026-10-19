#include "../web/cpp/multiplayer/HintOwnerPool.hpp"
#include "../web/cpp/multiplayer/RollbackPoolCapture.hpp"
#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <type_traits>

#ifndef TH_NATIVE_PLATFORM
#error This test requires TH_NATIVE_PLATFORM to check native callback contexts.
#endif

// Run with the title's wasm32 ABI. These are the real StageHints and
// UpdateChain methods; the fixture replaces only allocation and external I/O.
namespace {
using namespace th10;
using OwnerPool=multiplayer::HintOwnerPool;
using OwnerCapture=multiplayer::RollbackPoolCapture<1>;
using EntryPool=multiplayer::RollbackPool<sizeof(UpdateChainEntry),2>;
using EntryCapture=multiplayer::RollbackPoolCapture<2>;
using NodePool=multiplayer::RollbackPool<sizeof(StageHint),4>;
using NodeCapture=multiplayer::RollbackPoolCapture<4>;
constexpr std::size_t Payload=sizeof(StageHints);
constexpr std::size_t Stride=sizeof(OwnerPool::Storage);
constexpr std::size_t EntryStride=sizeof(EntryPool::Storage);
constexpr std::size_t NodeStride=sizeof(NodePool::Storage);
static_assert(sizeof(StageHints*)==4&&Payload==0x1a8);
static_assert(Stride>=Payload);

[[noreturn]] void fail(const char* expression,int line){
    std::fprintf(stderr,"hint owner lifetime: line %d: %s\n",line,expression);
    std::abort();
}
#define CHECK(expression) do { if(!(expression))fail(#expression,__LINE__); } while(false)

template<class T>
std::array<unsigned char,sizeof(T)> bytes_of(const T& value){
    static_assert(std::is_trivially_copyable_v<T>);
    std::array<unsigned char,sizeof(T)> bytes{};
    std::memcpy(bytes.data(),&value,sizeof(value));
    return bytes;
}

template<class Pool>
void seed(Pool& pool,unsigned char value){
    static_assert(std::is_trivially_copyable_v<Pool>);
    // Seed object, slot and metadata padding, then establish valid ownership.
    std::memset(&pool,value,sizeof(pool));
    std::memset(pool.occupied,0,sizeof(pool.occupied));
    pool.cursor=0;
}

template<std::size_t BlockSize,std::size_t Capacity>
void change_live_padding(multiplayer::RollbackPool<BlockSize,Capacity>& pool){
    constexpr auto storage=sizeof(typename multiplayer::RollbackPool<BlockSize,Capacity>::Storage);
    for(std::size_t slot=0;slot<Capacity;++slot)if(pool.active(slot)){
        auto* bytes=static_cast<unsigned char*>(pool.at(slot));
        for(std::size_t at=BlockSize;at<storage;++at)bytes[at]^=0x5a;
    }
}

void check_sentinels(const StageHints& hints,bool empty){
    const auto* self=reinterpret_cast<const StageHint*>(&hints);
    for(std::size_t index=0;index<8;++index){
        for(const auto* head:{&hints.pending[index],&hints.retained[index]}){
            CHECK(head->value==self&&head->previous==nullptr);
            if(empty)CHECK(head->next==nullptr);
        }
    }
}

struct Fixture final:StageHintsEnvironment,HintRecordingEnvironment,UpdateChainEnvironment {
    OwnerPool owners{};
    EntryPool entries{};
    NodePool nodes{};
    OwnerCapture owner_capture;
    EntryCapture entry_capture;
    NodeCapture node_capture;
    StageHints* live=nullptr;
    UpdateChain links{};
    UpdateChain* links_pointer=&links;
    multiplayer::TeamEconomy team_economy{};
    multiplayer::PilotEconomy pilot_economy{};
    GameEconomy economy{team_economy,pilot_economy};
    Player* player_pointer=nullptr;
    AnmRegistry animation_registry{};
    AnmFile* animation_file=nullptr;
    u8 hints_enabled=0,depth=0;
    float speed=1;
    const bool packed_scan;
    Netplay::RollbackJournal journal;
    bool frame_open=false,omit_owner_first_write=false;

    explicit Fixture(bool packed=false,bool coalesce=false)
        :StageHintsEnvironment{},HintRecordingEnvironment{},UpdateChainEnvironment{},packed_scan(packed){
        current=&live;
        StageHintsEnvironment::game=HintRecordingEnvironment::game=&economy;
        player=&player_pointer;registry=&animation_registry;text_animations=&animation_file;
        chain=&links_pointer;callbacks=this;
        update_callback=71;draw_callback=72;enabled=&hints_enabled;rate=&speed;
        lock_depth=&depth;
        links.initialize();
        seed(owners,0xa5);seed(entries,0x6d);seed(nodes,0x3b);
        const auto capacity=sizeof(owners)+sizeof(entries)+sizeof(nodes)+sizeof(links)+sizeof(live)+64;
        CHECK(journal.Reset({14,capacity,64,true,coalesce}));
    }

    bool save(void* address,std::size_t bytes){return journal.Touch(address,bytes);}
    void begin(u32 frame,bool capture_current=true){
        CHECK(!frame_open&&journal.BeginFrame(frame));
        const auto touch=[&](void* p,std::size_t bytes){return save(p,bytes);};
        CHECK(owner_capture.Capture(owners,touch,packed_scan));
        CHECK(entry_capture.Capture(entries,touch,packed_scan));
        CHECK(node_capture.Capture(nodes,touch,packed_scan));
        CHECK(save(&links,sizeof(links)));
        if(capture_current)CHECK(save(&live,sizeof(live)));
        // This duplicate payload request must route through slot coverage.
        if(live)CHECK(owner_capture.TouchSlot(live,Payload,touch));
        frame_open=true;
    }
    void end(){CHECK(frame_open&&journal.EndFrame());frame_open=false;CHECK(depth==0);}
    void undo(u32 frame){
        CHECK(!frame_open);
        u32 restored=~0u;
        CHECK(journal.UndoTo(frame,&restored)&&restored==frame);
        owner_capture.Clear();entry_capture.Clear();node_capture.Clear();
    }
    void clear_history(){
        CHECK(!frame_open);journal.Clear();
        owner_capture.Clear();entry_capture.Clear();node_capture.Clear();
        // Generation reset discards history, never live World-owned pools.
    }

    void* allocate(u32 bytes) override {
        if(bytes==Payload){
            auto* result=owners.allocate(bytes);
            if(!result)return nullptr;
            if(frame_open&&!omit_owner_first_write)
                CHECK(owner_capture.TouchSlot(result,Stride,[&](void* p,std::size_t n){return save(p,n);}));
            return result;
        }
        CHECK(bytes==sizeof(StageHint));
        auto* result=nodes.allocate(bytes);
        CHECK(result);
        if(frame_open)CHECK(node_capture.TouchSlot(result,NodeStride,[&](void* p,std::size_t n){return save(p,n);}));
        return result;
    }
    void release_object(void* object) override {
        if(owners.owns(object)){CHECK(owners.release(object));return;}
        CHECK(nodes.owns(object)&&nodes.release(object));
    }
    StageHint* allocate_hint() override {return static_cast<StageHint*>(allocate(sizeof(StageHint)));}
    UpdateChainEntry* allocate_entry() override {
        // Match the current production callback allocator. Both slots are
        // live at the start of lifecycle checkpoints and therefore captured.
        // This test does not invent a dormant callback allocation hook.
        auto* result=static_cast<UpdateChainEntry*>(entries.allocate(sizeof(UpdateChainEntry),true));
        CHECK(result);return result;
    }
    void release_entry(UpdateChainEntry* entry) override {CHECK(entries.release(entry));}
    void enter_lock() override {}
    void leave_lock() override {}
    i32 invoke(CallbackToken token,void* owner) override {
        CHECK((token==update_callback||token==draw_callback)&&owner==live);
        return 1;
    }

    // Disabled hints still install their real two UpdateChain jobs. Any
    // resource, animation or file operation would invalidate this fixture.
    void* allocate_bytes(u32) override {CHECK(false);return nullptr;}
    void free_bytes(void*) override {CHECK(false);}
    u8* read_file(const char*,u32&) override {CHECK(false);return nullptr;}
    u32 create_animation(AnmFile&,i32,const Vec3&) override {CHECK(false);return 0;}
    void draw_text(AnmVm&,i32,u32,const char*) override {CHECK(false);}
    void make_directory(const char*) override {CHECK(false);}
    bool begin_file(const char*) override {CHECK(false);return false;}
    void write_text(const char*,bool) override {CHECK(false);}
    void end_file() override {CHECK(false);}
    HintDate local_time() override {CHECK(false);return {};}

    StageHints* create(){
        CHECK(live==nullptr);
        auto* result=StageHints::create(*this);
        CHECK(result&&result==live&&owners.active(0));
        check_sentinels(*live,true);
        return result;
    }
    void destroy(){
        CHECK(live&&owners.active(0));
        auto* retired=live;
        retired->shutdown(*this);
        release_object(retired);
        CHECK(live==nullptr&&!owners.active(0));
        CHECK(links.update.node.next==nullptr&&links.draw.node.next==nullptr);
        for(std::size_t index=0;index<2;++index)CHECK(!entries.active(index));
        for(std::size_t index=0;index<4;++index)CHECK(!nodes.active(index));
        CHECK(depth==0);
    }
};

struct Snapshot {
    std::array<unsigned char,sizeof(OwnerPool)> owners;
    std::array<unsigned char,sizeof(EntryPool)> entries;
    std::array<unsigned char,sizeof(NodePool)> nodes;
    std::array<unsigned char,sizeof(UpdateChain)> links;
    StageHints* current;
    u8 depth;
};
Snapshot snapshot(const Fixture& fixture){
    return {bytes_of(fixture.owners),bytes_of(fixture.entries),bytes_of(fixture.nodes),
            bytes_of(fixture.links),fixture.live,fixture.depth};
}
bool same(const Fixture& fixture,const Snapshot& expected){
    return bytes_of(fixture.owners)==expected.owners&&bytes_of(fixture.entries)==expected.entries&&
           bytes_of(fixture.nodes)==expected.nodes&&bytes_of(fixture.links)==expected.links&&
           fixture.live==expected.current&&fixture.depth==expected.depth;
}

void check_graph(Fixture& fixture){
    auto* owner=fixture.live;
    CHECK(owner&&fixture.owners.owns(owner)&&fixture.owners.active(0)&&fixture.owners.cursor==0);
    check_sentinels(*owner,false);
    std::array<bool,4> seen{};
    for(std::size_t index=0;index<8;++index){
        for(auto* head:{&owner->pending[index],&owner->retained[index]}){
            auto* previous=head;
            for(auto* node=head->next;node;node=node->next){
                CHECK(fixture.nodes.owns(node->value));
                const auto slot=(reinterpret_cast<std::uintptr_t>(node->value)-
                                 reinterpret_cast<std::uintptr_t>(fixture.nodes.blocks))/NodeStride;
                CHECK(slot<seen.size()&&!seen[slot]&&fixture.nodes.active(slot));
                seen[slot]=true;
                CHECK(&node->value->node==node&&node->previous==previous);
                previous=node;
            }
        }
    }
    for(std::size_t index=0;index<seen.size();++index)CHECK(seen[index]==fixture.nodes.active(index));
    UpdateChainEntry* jobs[2]={owner->update_entry,owner->draw_entry};
    UpdateChainEntry* sentinels[2]={&fixture.links.update,&fixture.links.draw};
    for(std::size_t index=0;index<2;++index){
        auto* entry=jobs[index];auto* sentinel=sentinels[index];
        CHECK(entry&&fixture.entries.owns(entry)&&fixture.entries.active(index));
        CHECK(entry->owner==owner&&entry->callback==(index?fixture.draw_callback:fixture.update_callback));
        CHECK(sentinel->node.value==sentinel&&sentinel->node.next==&entry->node);
        CHECK(entry->node.value==entry&&entry->node.previous==&sentinel->node&&entry->node.next==nullptr);
        const auto resolved=fixture.resolve(entry->callback);
        CHECK(entry->function.context==static_cast<UpdateChainEnvironment*>(&fixture));
        CHECK(entry->function.context!=entry->owner&&entry->function.function==resolved.function);
        CHECK(entry->function.parameter==resolved.parameter&&entry->function(entry->owner)==1);
        CHECK(entry->notification.context==nullptr&&entry->notification.function==nullptr);
    }
    CHECK(fixture.depth==0);
}

void populate(Fixture& fixture,i32 serial){
    fixture.economy.stage=1;fixture.economy.section=2;
    auto* recorded=fixture.live->record("retained owner",Vec3{float(serial),12,0},fixture);
    CHECK(recorded&&recorded->node.previous==&fixture.live->retained[1]);
    auto* pending=static_cast<StageHint*>(fixture.allocate(sizeof(StageHint)));
    pending->initialize();pending->delay=20;
    pending->node.insert_after(fixture.live->pending[2]);
    CHECK(pending->node.previous==&fixture.live->pending[2]);
    fixture.live->frames=serial;fixture.live->section_frames=serial+3;
    fixture.live->manager_state=static_cast<u32>(serial);
    fixture.live->positions[3]={float(serial),7,0};fixture.live->widths[3]=32;
}

void allocation_contract(){
    OwnerPool pool{};seed(pool,0xa5);
    const auto dormant=bytes_of(pool.blocks[0]);
    CHECK(!pool.allocate(0)&&!pool.allocate(Payload+1));
    auto* address=pool.allocate(Payload);
    CHECK(address==pool.at(0)&&bytes_of(pool.blocks[0])==dormant);
    CHECK(pool.active(0)&&pool.cursor==0&&!pool.allocate(Payload));
    unsigned char foreign[Stride]{};
    CHECK(!pool.owns(nullptr)&&!pool.owns(foreign));
    CHECK(!pool.owns(static_cast<unsigned char*>(address)+1));
    CHECK(!pool.owns(static_cast<void*>(pool.blocks+1)));
    CHECK(!pool.release(nullptr)&&!pool.release(foreign));
    CHECK(!pool.release(static_cast<unsigned char*>(address)+1));
    CHECK(pool.active(0)&&pool.release(address)&&!pool.release(address));
    StageHints* current=nullptr;
    for(unsigned generation=0;generation<32;++generation){
        auto* owner=static_cast<StageHints*>(pool.allocate(Payload));
        CHECK(owner==address);
        owner->initialize(&current);CHECK(current==owner);
        check_sentinels(*owner,true);CHECK(owner->frames==0);
        owner->frames=41;
        CHECK(!pool.allocate(Payload)&&pool.release(owner));
        current=nullptr;
    }
}

void lifecycle(bool packed,bool coalesce){
    Fixture fixture(packed,coalesce);
    auto* stable=fixture.create();populate(fixture,17);check_graph(fixture);
    const auto initial=snapshot(fixture);

    // Destruction without recreation must restore current, occupancy, full
    // owner bytes, node links and both callback owners from a null state.
    fixture.begin(0);fixture.destroy();fixture.end();fixture.undo(0);
    CHECK(same(fixture,initial));check_graph(fixture);

    // Exercise retirement and same-slot reconstruction over the actual
    // rollback depth, then rewind all twelve generations as one operation.
    for(u32 frame=0;frame<12;++frame){
        fixture.begin(frame);
        fixture.destroy();CHECK(fixture.create()==stable);
        populate(fixture,static_cast<i32>(frame+100));
        change_live_padding(fixture.owners);change_live_padding(fixture.entries);
        change_live_padding(fixture.nodes);
        check_graph(fixture);fixture.end();
    }
    CHECK(fixture.journal.FrameCount()==12);
    fixture.undo(0);CHECK(same(fixture,initial));check_graph(fixture);

    // Discarded future coverage must not survive into a new checkpoint.
    fixture.begin(0);fixture.destroy();CHECK(fixture.create()==stable);
    populate(fixture,222);fixture.end();fixture.undo(0);
    CHECK(same(fixture,initial));check_graph(fixture);

    // stop_session clears history before shutdown; BeginNextRun can clear
    // history after new scene construction. Neither operation clears pools.
    for(unsigned generation=0;generation<8;++generation){
        fixture.clear_history();fixture.destroy();CHECK(fixture.create()==stable);
        fixture.clear_history();
        CHECK(fixture.live==stable&&fixture.owners.active(0));
        check_graph(fixture);
    }
    fixture.destroy();
}

void dormant_first_write(bool packed,bool omit){
    Fixture fixture(packed);
    const auto initial=snapshot(fixture);
    fixture.omit_owner_first_write=omit;
    fixture.begin(0);
    auto* owner=static_cast<StageHints*>(fixture.allocate(Payload));
    CHECK(owner==fixture.owners.at(0));
    // Only initialize is needed to test a dormant manager's first write.
    // No new dormant callback entry is allocated in this independent case.
    owner->initialize(&fixture.live);check_sentinels(*owner,true);
    change_live_padding(fixture.owners);
    fixture.end();fixture.undo(0);
    CHECK(fixture.live==nullptr&&!fixture.owners.active(0));
    CHECK(bytes_of(fixture.entries)==initial.entries&&bytes_of(fixture.links)==initial.links);
    if(omit)CHECK(bytes_of(fixture.owners)!=initial.owners&&!same(fixture,initial));
    else CHECK(same(fixture,initial));
}

void missing_current_negative(bool packed){
    Fixture fixture(packed);fixture.create();populate(fixture,17);
    const auto initial=snapshot(fixture);
    fixture.begin(0,false);fixture.destroy();fixture.end();fixture.undo(0);
    CHECK(fixture.live==nullptr&&fixture.owners.active(0)&&!same(fixture,initial));
    CHECK(bytes_of(fixture.owners)==initial.owners&&bytes_of(fixture.entries)==initial.entries);
    CHECK(bytes_of(fixture.nodes)==initial.nodes&&bytes_of(fixture.links)==initial.links);
}

void payload_stride_and_failures(bool packed){
    OwnerPool pool{};seed(pool,0xa5);
    auto* owner=static_cast<StageHints*>(pool.allocate(Payload));
    StageHints* current=nullptr;owner->initialize(&current);
    OwnerCapture capture;
    Netplay::RollbackJournal journal;
    CHECK(journal.Reset({2,sizeof(pool)+32,16,true,true})&&journal.BeginFrame(0));
    const auto touch=[&](void* p,std::size_t bytes){return journal.Touch(p,bytes);};
    CHECK(capture.Capture(pool,touch,packed));
    const auto blocks=journal.BlocksForFrame(0);
    CHECK(journal.BytesForFrame(0)==sizeof(pool.occupied)+sizeof(pool.cursor)+Stride);
    CHECK(capture.OwnsSlot(owner,Payload)&&capture.OwnsSlot(owner,Stride));
    CHECK(!capture.OwnsSlot(static_cast<unsigned char*>(pool.at(0))+1,Payload));
    CHECK(!capture.OwnsSlot(owner,Payload-1)&&!capture.OwnsSlot(owner,Stride+1));
    CHECK(capture.TouchSlot(owner,Payload,touch)&&capture.TouchSlot(owner,Stride,touch));
    CHECK(journal.BlocksForFrame(0)==blocks);
    // A direct payload record overlaps the aligned live-slot record. Keep
    // strict rejection as a negative control; never mask these owner bytes.
    if constexpr(Stride>Payload)CHECK(!journal.Touch(owner,Payload)&&journal.Failed());
    else CHECK(!journal.Touch(owner,Payload-1)&&journal.Failed());

    OwnerPool dormant{};seed(dormant,0x79);
    OwnerCapture retry;
    CHECK(retry.Capture(dormant,[](void*,std::size_t){return true;},packed));
    auto* slot=dormant.allocate(Payload);
    unsigned attempts=0;
    CHECK(!retry.TouchSlot(slot,Payload,[&](void* p,std::size_t bytes){
        ++attempts;CHECK(p==slot&&bytes==Stride);return false;
    }));
    CHECK(retry.TouchSlot(slot,Stride,[&](void* p,std::size_t bytes){
        ++attempts;CHECK(p==slot&&bytes==Stride);return true;
    }));
    CHECK(retry.TouchSlot(slot,Payload,[&](void*,std::size_t){++attempts;return false;}));
    CHECK(attempts==2); // A failed first write must not mark the slot covered.

    if constexpr(Stride>Payload){
        const auto before=bytes_of(pool.blocks[0]);
        Netplay::RollbackJournal payload_only;
        CHECK(payload_only.Reset({1,Stride,4})&&payload_only.BeginFrame(0));
        CHECK(payload_only.Touch(owner,Payload));
        owner->frames=999;change_live_padding(pool);
        CHECK(payload_only.EndFrame()&&payload_only.UndoTo(0));
        CHECK(std::memcmp(pool.at(0),before.data(),Payload)==0);
        CHECK(bytes_of(pool.blocks[0])!=before); // Payload-only undo lost padding.
    }
}
} // namespace

int main(){
    allocation_contract();
    for(unsigned mode=0;mode<2;++mode){
        const bool packed=mode!=0;
        lifecycle(packed,false);lifecycle(packed,true);
        dormant_first_write(packed,false);dormant_first_write(packed,true);
        missing_current_negative(packed);payload_stride_and_failures(packed);
    }
    std::printf("hint owner lifetime: PASS real StageHints/UpdateChain; payload=%zu stride=%zu callback_stride=%zu; stable restart address, 16 self sentinels, node links, native context/owner separation, full 12-frame undo, generation reset, capacity/release errors, scalar/packed and negative controls\n",
                Payload,Stride,EntryStride);
}
