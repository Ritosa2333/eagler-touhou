#include "../web/cpp/multiplayer/SceneOwnerPools.hpp"
#include "../web/cpp/multiplayer/RollbackPoolCapture.hpp"
#include "../web/cpp/game/ProjectileSystems.hpp"
#include "../web/cpp/game/GameObjectResources.hpp"
#include "../web/cpp/game/CallbackNames.hpp"
#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <tuple>
#include <type_traits>

#ifndef TH_NATIVE_PLATFORM
#error This test requires the native callback context/owner layout.
#endif

// Link the real LaserManager and Bomb factories/shutdown plus UpdateChain.
// ProjectileSystems.cpp also contains the unrelated bullet-manager teardown;
// entering it would leave this fixture's declared scope.
namespace th10 {
void EnemyBullet::release(AnmAllocationEnvironment&){
    std::fputs("scene owner lifetime: unexpected bullet teardown\n",stderr);
    std::abort();
}
}

namespace {
using namespace th10;
using LaserPool=multiplayer::LaserOwnerPool;
using BombPool=multiplayer::BombOwnerPool;
constexpr std::size_t LaserPayload=sizeof(LaserManager),LaserStride=sizeof(LaserPool::Storage);
constexpr std::size_t BombPayload=sizeof(Bomb),BombStride=sizeof(BombPool::Storage);
static_assert(sizeof(void*)==4&&multiplayer::kMaxSeats==3);
static_assert(LaserStride>=LaserPayload&&BombStride>=BombPayload);
std::uint64_t checks=0;

[[noreturn]] void fail(const char* expression,int line){
    std::fprintf(stderr,"scene owner lifetime: line %d: %s\n",line,expression);
    std::abort();
}
#define CHECK(expression) do { ++checks; if(!(expression))fail(#expression,__LINE__); } while(false)

template<class T>
std::array<unsigned char,sizeof(T)> bytes_of(const T& value){
    static_assert(std::is_trivially_copyable_v<T>);
    std::array<unsigned char,sizeof(T)> bytes{};
    std::memcpy(bytes.data(),&value,sizeof(value));return bytes;
}
template<class Pool>
void seed(Pool& pool,unsigned char value){
    static_assert(std::is_trivially_copyable_v<Pool>);
    std::memset(&pool,value,sizeof(pool));
    std::memset(pool.occupied,0,sizeof(pool.occupied));pool.cursor=0;
}
template<std::size_t Payload,std::size_t Capacity>
void change_padding(multiplayer::RollbackPool<Payload,Capacity>& pool){
    using Storage=typename multiplayer::RollbackPool<Payload,Capacity>::Storage;
    for(std::size_t slot=0;slot<Capacity;++slot)if(pool.active(slot)){
        auto* bytes=static_cast<unsigned char*>(pool.at(slot));
        for(std::size_t at=Payload;at<sizeof(Storage);++at)bytes[at]^=0x5a;
    }
}
template<std::size_t Payload,std::size_t Capacity>
std::size_t active_count(const multiplayer::RollbackPool<Payload,Capacity>& pool){
    std::size_t count=0;for(std::size_t at=0;at<Capacity;++at)count+=pool.active(at);
    return count;
}

template<unsigned Seats>
struct Fixture final:UpdateChainEnvironment {
    static_assert(Seats==2||Seats==3);
    static constexpr unsigned EntryCount=2+Seats*2;
    using EntryPool=multiplayer::RollbackPool<sizeof(UpdateChainEntry),EntryCount>;
    LaserPool laser_pool{};
    BombPool bomb_pool{};
    EntryPool entry_pool{};
    multiplayer::RollbackPoolCapture<1> laser_capture;
    multiplayer::RollbackPoolCapture<multiplayer::kMaxSeats> bomb_capture;
    multiplayer::RollbackPoolCapture<EntryCount> entry_capture;
    LaserManager* live_laser=nullptr;
    std::array<Bomb*,multiplayer::kMaxSeats> live_bombs{};
    Bomb* actor_bomb=nullptr;
    EnemyBulletManager* unused_bullets=nullptr;
    ItemManager* unused_items=nullptr;
    GameEffects* unused_effects=nullptr;
    AnmFile animation_file{};
    UpdateChain links{};
    UpdateChain* links_pointer=&links;
    Netplay::RollbackJournal journal;
    u8 depth=0;
    float speed=1;
    bool packed=false,frame_open=false,omit_first_write=false;
    bool reject_laser=false,missing_file=false;
    int reject_bomb_seat=-1;
    unsigned resource_errors=0;

    explicit Fixture(bool scan=false,bool coalesce=false):UpdateChainEnvironment{},packed(scan){
        lock_depth=&depth;links.initialize();
        seed(laser_pool,0x91);seed(bomb_pool,0x43);seed(entry_pool,0x65);
        const auto bytes=sizeof(laser_pool)+sizeof(bomb_pool)+sizeof(entry_pool)+
                         sizeof(links)+sizeof(live_laser)+sizeof(live_bombs)+sizeof(actor_bomb)+64;
        CHECK(journal.Reset({14,bytes,64,true,coalesce}));
    }
    bool save(void* p,std::size_t bytes){return journal.Touch(p,bytes);}
    void begin(u32 frame,bool current=true){
        CHECK(!frame_open&&journal.BeginFrame(frame));
        const auto touch=[&](void* p,std::size_t n){return save(p,n);};
        CHECK(laser_capture.Capture(laser_pool,touch,packed));
        CHECK(bomb_capture.Capture(bomb_pool,touch,packed));
        CHECK(entry_capture.Capture(entry_pool,touch,packed));
        CHECK(save(&links,sizeof(links)));
        if(current){
            CHECK(save(&live_laser,sizeof(live_laser)));
            CHECK(save(&live_bombs,sizeof(live_bombs)));
            CHECK(save(&actor_bomb,sizeof(actor_bomb)));
        }
        if(live_laser)CHECK(laser_capture.TouchSlot(live_laser,LaserPayload,touch));
        for(auto* bomb:live_bombs)if(bomb)CHECK(bomb_capture.TouchSlot(bomb,BombPayload,touch));
        frame_open=true;
    }
    void end(){CHECK(frame_open&&journal.EndFrame());frame_open=false;CHECK(depth==0);}
    void clear_capture(){laser_capture.Clear();bomb_capture.Clear();entry_capture.Clear();}
    void undo(u32 frame){
        CHECK(!frame_open);u32 restored=~0u;
        CHECK(journal.UndoTo(frame,&restored)&&restored==frame);clear_capture();
    }
    void clear_history(){CHECK(!frame_open);journal.Clear();clear_capture();}

    UpdateChainEntry* allocate_entry() override{
        // Every callback slot is active at lifecycle checkpoints. Preserve
        // production allocation semantics: no invented dormant first-write hook.
        auto* result=static_cast<UpdateChainEntry*>(entry_pool.allocate(sizeof(UpdateChainEntry),true));
        CHECK(result);return result;
    }
    void release_entry(UpdateChainEntry* entry) override{CHECK(entry_pool.release(entry));}
    void enter_lock() override{}
    void leave_lock() override{}
    i32 invoke(CallbackToken token,void* owner) override{
        if(token==callback_id::LasersUpdate||token==callback_id::LasersDraw)CHECK(owner==live_laser);
        else{
            CHECK(token==callback_id::BombUpdate||token==callback_id::BombDraw);
            bool found=false;for(auto* bomb:live_bombs)found|=bomb&&bomb==owner;
            CHECK(found);
        }
        return 1;
    }

    struct Lasers final:ProjectileSystemsEnvironment {
        Fixture& f;
        explicit Lasers(Fixture& value):ProjectileSystemsEnvironment{},f(value){
            chain=&f.links;callbacks=&f;active_lasers=&f.live_laser;active_bullets=&f.unused_bullets;
            laser_update=callback_id::LasersUpdate;laser_draw=callback_id::LasersDraw;
        }
        AnmFile* load_bullet_animations() override{return f.missing_file?nullptr:&f.animation_file;}
        void missing_bullet_animations() override{++f.resource_errors;}
        void discard_file_animations(AnmFile*) override{CHECK(false);}
        void destroy_laser(EnemyLaser&) override{CHECK(false);} // Declared empty beam list.
        void* allocate_manager(u32 bytes) override{
            CHECK(bytes==LaserPayload);if(f.reject_laser)return nullptr;
            auto* result=f.laser_pool.allocate(bytes);if(!result)return nullptr;
            if(f.frame_open&&!f.omit_first_write)
                CHECK(f.laser_capture.TouchSlot(result,LaserStride,[&](void* p,std::size_t n){return f.save(p,n);}));
            return result;
        }
        void release_manager(void* p) override{CHECK(f.laser_pool.release(p));}
    };
    struct Bombs final:GameObjectResourceEnvironment {
        Fixture& f;unsigned seat;
        Bombs(Fixture& value,unsigned index):GameObjectResourceEnvironment{},f(value),seat(index){
            CHECK(seat<Seats);items=&f.unused_items;bomb=&f.live_bombs[seat];effects=&f.unused_effects;
            chain=&f.links_pointer;callbacks=&f;bomb_update=callback_id::BombUpdate;bomb_draw=callback_id::BombDraw;
        }
        void* allocate(u32 bytes) override{
            CHECK(bytes==BombPayload);if(static_cast<int>(seat)==f.reject_bomb_seat)return nullptr;
            auto* result=f.bomb_pool.allocate(bytes);if(!result)return nullptr;
            if(f.frame_open&&!f.omit_first_write)
                CHECK(f.bomb_capture.TouchSlot(result,BombStride,[&](void* p,std::size_t n){return f.save(p,n);}));
            return result;
        }
        void delete_object(void* p) override{CHECK(f.bomb_pool.release(p));}
        void release_geometry(void*) override{CHECK(false);}
        AnmFile* load_effect_animations() override{CHECK(false);return nullptr;}
        void report_effect_error() override{CHECK(false);}
    };

    bool create_lasers(){Lasers env(*this);return LaserManager::create(env)!=nullptr;}
    bool create_bombs(){
        // Native equal-priority insert is before its predecessor: create seats
        // in reverse order, preserving ascending callback order.
        for(unsigned offset=0;offset<Seats;++offset){
            Bombs env(*this,Seats-1-offset);
            if(!GameObjectResources{env}.create(GameObjectKind::Bomb)){destroy_bombs();return false;}
        }
        actor_bomb=live_bombs[0];return true;
    }
    void destroy_lasers(){
        if(!live_laser)return;auto* old=live_laser;Lasers env(*this);
        old->shutdown(env);CHECK(live_laser==nullptr);env.release_manager(old);
    }
    void destroy_bombs(){
        for(unsigned seat=0;seat<Seats;++seat){
            auto* old=live_bombs[seat];if(!old)continue;
            Bombs env(*this,seat);GameObjectResources{env}.shutdown(*old);
            CHECK(live_bombs[seat]==nullptr);env.delete_object(old);
        }
        actor_bomb=nullptr;
    }
    void create(bool retain_lasers=false){
        if(retain_lasers)CHECK(live_laser);else CHECK(create_lasers());
        CHECK(create_bombs());
    }
    void retire(bool retain_lasers=false){
        // GameSessionResources destroys Lasers before the unconditional Bombs.
        if(!retain_lasers)destroy_lasers();destroy_bombs();
    }
};

template<unsigned Seats>
auto snapshot(const Fixture<Seats>& f){
    return std::make_tuple(bytes_of(f.laser_pool),bytes_of(f.bomb_pool),bytes_of(f.entry_pool),
                           bytes_of(f.links),f.live_laser,f.live_bombs,f.actor_bomb);
}
template<unsigned Seats>
void check_entry(Fixture<Seats>& f,UpdateChainEntry* entry,void* owner,CallbackToken token,
                 i32 priority,bool enabled){
    CHECK(entry&&f.entry_pool.owns(entry)&&entry->owner==owner&&entry->callback==token);
    CHECK(entry->priority==priority&&entry->flags==(enabled?3u:1u));
    CHECK(entry->node.value==entry&&entry->initialize_callback==0&&entry->notify_callback==0);
    const auto resolved=f.resolve(token);
    CHECK(entry->function.context==static_cast<UpdateChainEnvironment*>(&f));
    CHECK(entry->function.context!=owner&&entry->function.function==resolved.function);
    CHECK(entry->function.parameter==resolved.parameter&&entry->function(owner)==1);
    CHECK(entry->notification.context==nullptr&&entry->notification.function==nullptr);
}
template<std::size_t Count>
void check_order(UpdateChainEntry& sentinel,const std::array<UpdateChainEntry*,Count>& expected){
    CHECK(sentinel.node.value==&sentinel&&sentinel.node.previous==nullptr);
    auto* previous=&sentinel.node;auto* node=previous->next;
    for(auto* entry:expected){
        CHECK(node==&entry->node&&node->previous==previous);
        previous=node;node=node->next;
    }
    CHECK(node==nullptr);
}
template<unsigned Seats>
void check_graph(Fixture<Seats>& f){
    auto* laser=f.live_laser;
    CHECK(laser&&f.laser_pool.owns(laser)&&active_count(f.laser_pool)==1);
    CHECK(laser->tail==&laser->sentinel&&laser->sentinel.next==nullptr&&laser->count==0);
    CHECK(laser->animation_file==&f.animation_file);
    check_entry(f,laser->update_handle,laser,callback_id::LasersUpdate,19,false);
    check_entry(f,laser->draw_handle,laser,callback_id::LasersDraw,27,false);
    std::array<UpdateChainEntry*,Seats+1> update{},draw{};
    draw[0]=laser->draw_handle;update[Seats]=laser->update_handle;
    for(unsigned seat=0;seat<Seats;++seat){
        auto* bomb=f.live_bombs[seat];CHECK(bomb&&f.bomb_pool.owns(bomb));
        for(unsigned other=0;other<seat;++other)CHECK(bomb!=f.live_bombs[other]);
        check_entry(f,bomb->update_entry,bomb,callback_id::BombUpdate,17,true);
        check_entry(f,bomb->draw_entry,bomb,callback_id::BombDraw,34,true);
        update[seat]=bomb->update_entry;draw[seat+1]=bomb->draw_entry;
    }
    for(unsigned seat=Seats;seat<f.live_bombs.size();++seat)CHECK(f.live_bombs[seat]==nullptr);
    CHECK(f.actor_bomb==f.live_bombs[0]&&active_count(f.bomb_pool)==Seats);
    CHECK(active_count(f.entry_pool)==Fixture<Seats>::EntryCount);
    check_order(f.links.update,update);check_order(f.links.draw,draw);CHECK(f.depth==0);
}
template<unsigned Seats>
void populate(Fixture<Seats>& f,i32 serial){
    f.live_laser->last_id=static_cast<u32>(serial);
    f.live_laser->last_cancel_position={static_cast<float>(serial),2,3};
    for(unsigned seat=0;seat<Seats;++seat){
        auto& bomb=*f.live_bombs[seat];bomb.active=static_cast<i32>(seat&1);
        bomb.timer.current=serial+static_cast<i32>(seat);bomb.timer.rate=&f.speed;
        bomb.position={static_cast<float>(serial),static_cast<float>(seat),0};
        bomb.radius=123;bomb.mode=2;
    }
}

template<unsigned Seats>
void lifecycle(bool packed,bool coalesce,bool retain_lasers){
    Fixture<Seats> f(packed,coalesce);f.create();populate(f,17);check_graph(f);
    const auto before=snapshot(f);auto* stable_laser=f.live_laser;
    f.begin(0);f.retire(retain_lasers);f.end();f.undo(0);
    CHECK(snapshot(f)==before);check_graph(f);
    for(u32 frame=0;frame<12;++frame){
        f.begin(frame);f.retire(retain_lasers);f.create(retain_lasers);
        CHECK(f.live_laser==stable_laser);
        populate(f,static_cast<i32>(100+frame));
        change_padding(f.laser_pool);change_padding(f.bomb_pool);change_padding(f.entry_pool);
        check_graph(f);f.end();
    }
    CHECK(f.journal.FrameCount()==12);f.undo(0);CHECK(snapshot(f)==before);check_graph(f);
    f.begin(0);f.retire(retain_lasers);f.create(retain_lasers);populate(f,222);f.end();f.undo(0);
    CHECK(snapshot(f)==before);check_graph(f);
    // stop_session clears history before shutdown; BeginNextRun clears it
    // after the next graph exists. Neither reset clears World-owned pools.
    bool rotated=false;
    for(unsigned generation=0;generation<8;++generation){
        auto* previous=f.live_bombs[0];
        f.clear_history();f.retire();f.create();f.clear_history();
        rotated|=previous!=f.live_bombs[0];
        CHECK(f.live_laser==stable_laser);check_graph(f);
    }
    if constexpr(Seats==2)CHECK(rotated); // Three slots need not pin one seat forever.
    f.retire();CHECK(!active_count(f.laser_pool)&&!active_count(f.bomb_pool)&&!active_count(f.entry_pool));
}

void allocation_contract(){
    LaserPool lasers{};BombPool bombs{};seed(lasers,0xa5);seed(bombs,0x6b);
    CHECK(!lasers.allocate(0)&&!lasers.allocate(LaserPayload+1));
    const auto laser_bytes=bytes_of(lasers.blocks);
    auto* laser=lasers.allocate(LaserPayload);
    CHECK(laser==lasers.at(0)&&bytes_of(lasers.blocks)==laser_bytes&&!lasers.allocate(LaserPayload));
    CHECK(!lasers.release(nullptr)&&!lasers.release(static_cast<unsigned char*>(laser)+1));
    CHECK(lasers.release(laser)&&!lasers.release(laser));
    CHECK(lasers.allocate(LaserPayload)==laser&&lasers.release(laser));
    CHECK(!bombs.allocate(0)&&!bombs.allocate(BombPayload+1));
    std::array<void*,multiplayer::kMaxSeats> slots{};
    const auto bomb_bytes=bytes_of(bombs.blocks);
    for(auto& slot:slots){slot=bombs.allocate(BombPayload);CHECK(slot&&bombs.owns(slot));}
    CHECK(!bombs.allocate(BombPayload)&&bytes_of(bombs.blocks)==bomb_bytes);
    for(auto* slot:slots)CHECK(bombs.release(slot)&&!bombs.release(slot));
}

void failures_and_saved_pointer(){
    {
        Fixture<3> f;
        const auto empty=snapshot(f);f.reject_laser=true;
        CHECK(!f.create_lasers()&&snapshot(f)==empty);f.reject_laser=false;
        f.missing_file=true;CHECK(!f.create_lasers()&&f.resource_errors==1);
        CHECK(!f.live_laser&&!active_count(f.laser_pool)&&!active_count(f.entry_pool));
        f.missing_file=false;CHECK(f.create_lasers());
        const auto live=snapshot(f);CHECK(!f.create_lasers()&&snapshot(f)==live);
        CHECK(f.create_bombs());check_graph(f);
        auto* old=f.live_bombs[0];typename Fixture<3>::Bombs bombs(f,0);
        GameObjectResources{bombs}.shutdown(*old);
        CHECK(f.live_bombs[0]==nullptr);
        // The old production order did free(pilot.bomb) here, hence free(null).
        // Releasing the cleared current pointer must fail and retain the slot.
        CHECK(!f.bomb_pool.release(f.live_bombs[0])&&active_count(f.bomb_pool)==3);
        bombs.delete_object(old);CHECK(active_count(f.bomb_pool)==2);
        f.destroy_bombs();
        auto* old_laser=f.live_laser;typename Fixture<3>::Lasers lasers(f);
        old_laser->shutdown(lasers);CHECK(f.live_laser==nullptr);
        CHECK(!f.laser_pool.release(f.live_laser)&&active_count(f.laser_pool)==1);
        lasers.release_manager(old_laser);
        CHECK(!active_count(f.laser_pool)&&!active_count(f.bomb_pool)&&!active_count(f.entry_pool));
    }
    for(int seat:{2,1,0}){
        Fixture<3> f;CHECK(f.create_lasers());f.reject_bomb_seat=seat;
        CHECK(!f.create_bombs()&&!f.actor_bomb&&!active_count(f.bomb_pool));
        for(auto* bomb:f.live_bombs)CHECK(bomb==nullptr);
        CHECK(active_count(f.entry_pool)==2); // Only the retained laser callbacks.
        f.reject_bomb_seat=-1;CHECK(f.create_bombs());check_graph(f);f.retire();
        CHECK(!active_count(f.bomb_pool)&&!active_count(f.entry_pool));
    }
}

void dormant_first_write(bool packed,bool omit){
    Fixture<2> f(packed);const auto before=snapshot(f);f.omit_first_write=omit;f.begin(0);
    typename Fixture<2>::Lasers lasers(f);
    auto* laser=static_cast<LaserManager*>(lasers.allocate_manager(LaserPayload));CHECK(laser);
    laser->initialize(lasers);
    typename Fixture<2>::Bombs bombs(f,0);
    auto* bomb=static_cast<Bomb*>(bombs.allocate(BombPayload));CHECK(bomb);
    GameObjectResources{bombs}.initialize(*bomb);f.actor_bomb=bomb;
    // Initialize owners only: no dormant callback allocation is covered up by
    // a test-only capture hook. Full factory lifecycles begin with live entries.
    change_padding(f.laser_pool);change_padding(f.bomb_pool);
    f.end();f.undo(0);
    CHECK(!f.live_laser&&!f.actor_bomb&&!active_count(f.laser_pool)&&!active_count(f.bomb_pool));
    CHECK(bytes_of(f.entry_pool)==std::get<2>(before)&&bytes_of(f.links)==std::get<3>(before));
    if(omit)CHECK(snapshot(f)!=before);else CHECK(snapshot(f)==before);
}

void missing_current_negative(bool packed){
    Fixture<2> f(packed);f.create();populate(f,17);const auto before=snapshot(f);
    f.begin(0,false);f.retire();f.end();f.undo(0);
    CHECK(!f.live_laser&&!f.actor_bomb&&snapshot(f)!=before);
    for(auto* bomb:f.live_bombs)CHECK(bomb==nullptr);
    CHECK(bytes_of(f.laser_pool)==std::get<0>(before)&&bytes_of(f.bomb_pool)==std::get<1>(before));
    CHECK(bytes_of(f.entry_pool)==std::get<2>(before)&&bytes_of(f.links)==std::get<3>(before));
}
} // namespace

int main(){
    allocation_contract();failures_and_saved_pointer();
    for(bool packed:{false,true}){
        dormant_first_write(packed,false);dormant_first_write(packed,true);missing_current_negative(packed);
        for(bool coalesce:{false,true})for(bool retained:{false,true}){
            lifecycle<2>(packed,coalesce,retained);lifecycle<3>(packed,coalesce,retained);
        }
    }
    std::printf("scene owner lifetime: PASS (%llu checks); real LaserManager/Bomb/UpdateChain; "
                "laser payload=%zu stride=%zu; bomb payload=%zu stride=%zu; callback stride=%zu; "
                "2/3 seats, reverse creation, retained stages, full 12-frame undo, generation reset, "
                "allocation/resource failures, saved-pointer and first-write negative controls\n",
                static_cast<unsigned long long>(checks),LaserPayload,LaserStride,BombPayload,BombStride,
                sizeof(Fixture<3>::EntryPool::Storage));
    return 0;
}
