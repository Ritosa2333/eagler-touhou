#include "../web/cpp/multiplayer/SceneOwnerPools.hpp"
#include "../web/cpp/multiplayer/RollbackPoolCapture.hpp"
#include "../web/cpp/game/GameSessionResources.hpp"
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
#include <memory>
#include <tuple>
#include <type_traits>
#include <vector>

#ifndef TH_NATIVE_PLATFORM
#error This test requires the native callback context/owner layout.
#endif

// Real GameSessionResources (including every loader phase and retained-stage
// shutdown), GameSession timer initialization, GameObjectResources and native
// UpdateChain are linked. Other SessionObject factories are explicit external
// dependencies: their callbacks are not added to this four-entry chain. Replay,
// ItemManager and EnemyManager use complete real storage because the native
// retained path accesses their payloads. This is not their lifecycle test.
namespace {
using namespace th10;
using SessionPool=multiplayer::SessionOwnerPool;
using EffectPool=multiplayer::GameEffectOwnerPool;
constexpr std::size_t SessionPayload=sizeof(GameSession),SessionStride=sizeof(SessionPool::Storage);
constexpr std::size_t EffectPayload=sizeof(GameEffects),EffectStride=sizeof(EffectPool::Storage);
constexpr unsigned ObjectCount=static_cast<unsigned>(SessionObject::Count);
constexpr unsigned index(SessionObject kind){return static_cast<unsigned>(kind);}
static_assert(sizeof(void*)==4&&SessionStride>=SessionPayload&&EffectStride>=EffectPayload);
std::uint64_t checks=0;
[[noreturn]] void fail(const char* expression,int line){
    std::fprintf(stderr,"session/effect owner lifetime: line %d: %s\n",line,expression);std::abort();
}
#define CHECK(expression) do { ++checks; if(!(expression))fail(#expression,__LINE__); } while(false)

template<class T> std::array<unsigned char,sizeof(T)> bytes_of(const T& value){
    static_assert(std::is_trivially_copyable_v<T>);
    std::array<unsigned char,sizeof(T)> bytes{};std::memcpy(bytes.data(),&value,sizeof(value));return bytes;
}
template<class Pool> void seed(Pool& pool,unsigned char value){
    static_assert(std::is_trivially_copyable_v<Pool>);
    std::memset(&pool,value,sizeof(pool));std::memset(pool.occupied,0,sizeof(pool.occupied));pool.cursor=0;
}
template<std::size_t Payload,std::size_t Capacity>
std::size_t active_count(const multiplayer::RollbackPool<Payload,Capacity>& pool){
    std::size_t result=0;for(std::size_t i=0;i<Capacity;++i)result+=pool.active(i);return result;
}
template<std::size_t Payload,std::size_t Capacity>
void change_padding(multiplayer::RollbackPool<Payload,Capacity>& pool){
    using Storage=typename multiplayer::RollbackPool<Payload,Capacity>::Storage;
    for(std::size_t i=0;i<Capacity;++i)if(pool.active(i)){
        auto* p=static_cast<unsigned char*>(pool.at(i));
        for(std::size_t at=Payload;at<sizeof(Storage);++at)p[at]^=0x5a;
    }
}
bool all_zero(const void* p,std::size_t size){
    const auto* bytes=static_cast<const unsigned char*>(p);
    for(std::size_t i=0;i<size;++i)if(bytes[i])return false;return true;
}

struct Fixture final:UpdateChainEnvironment {
    using EntryPool=multiplayer::RollbackPool<sizeof(UpdateChainEntry),4>;
    SessionPool session_pool{};EffectPool effect_pool{};EntryPool entry_pool{};
    multiplayer::RollbackPoolCapture<1> session_capture,effect_capture;
    multiplayer::RollbackPoolCapture<4> entry_capture;
    struct Actors {
        GameSession* session=nullptr;GameEffects* effects=nullptr;
        std::array<GameSystemCallbacks*,ObjectCount> other{};
    } actors;
    struct Values {
        multiplayer::TeamEconomy team{};multiplayer::PilotEconomy pilot{};
        i32 new_game=0,practice_lives=0,pending_screen=3;
        i32 drawing_resource=-1,updating_resource=-1,pending_upload=0;
        i32 loader_stop=0,loader_running=0,menu_state=1;
        u32 engine_flags=0,display_flags=0,background_color=0,loading_animation=0;
        double recorded_time=3,rendered_time=5;float rate=1;
    } values;
    struct External {
        std::array<GameSystemCallbacks,ObjectCount> headers{};
        Replay replay{};EnemyManager enemies{};
        std::array<UpdateChainEntry,6> retained_entries{};
    } external;
    // ItemManager is large. Keep both its storage and byte oracle off the WASI
    // stack; full capture here isolates the owner's lifecycle from item policy.
    std::unique_ptr<ItemManager> retained_items{new ItemManager{}};
    GameEconomy game{values.team,values.pilot};
    AnmFile animation_file{};StageConfiguration stage_configuration{};
    const StageConfiguration* current_stage=&stage_configuration;
    ScoreData* unused_scores=nullptr;ItemManager* unused_items=nullptr;Bomb* unused_bomb=nullptr;
    std::array<u8,52> configuration{};
    UpdateChain links{};UpdateChain* links_pointer=&links;u8 depth=0;
    Netplay::RollbackJournal journal;
    bool packed=false,frame_open=false,omit_first_write=false;
    bool reject_session=false,reject_effect=false,missing_file=false;
    unsigned evictions=0,loads_started=0,loads_finished=0,load_failures=0;
    unsigned resource_errors=0,score_saves=0,retained_gui_discards=0,replay_prepares=0;
    std::array<unsigned,ObjectCount> creates{},destroys{};

    explicit Fixture(bool scan=false,bool coalesce=false):UpdateChainEnvironment{},packed(scan){
        lock_depth=&depth;links.initialize();seed(session_pool,0x91);seed(effect_pool,0x43);seed(entry_pool,0x65);
        stage_configuration.resources[0]=17;game.stage=1;game.reserved_040=1;
        for(unsigned i=0;i<configuration.size();++i)configuration[i]=static_cast<u8>(i+1);
        const auto bytes=sizeof(session_pool)+sizeof(effect_pool)+sizeof(entry_pool)+sizeof(links)+
                         sizeof(actors)+sizeof(values)+sizeof(external)+sizeof(ItemManager)+512;
        CHECK(journal.Reset({14,bytes,64,true,coalesce}));
    }
    bool save(void* p,std::size_t size){return journal.Touch(p,size);}
    void begin(u32 frame,bool capture_actors=true){
        CHECK(!frame_open&&journal.BeginFrame(frame));
        const auto touch=[&](void* p,std::size_t n){return save(p,n);};
        CHECK(session_capture.Capture(session_pool,touch,packed));
        CHECK(effect_capture.Capture(effect_pool,touch,packed));
        CHECK(entry_capture.Capture(entry_pool,touch,packed));CHECK(save(&links,sizeof(links)));
        if(capture_actors)CHECK(save(&actors,sizeof(actors)));
        CHECK(save(&values,sizeof(values)));CHECK(save(&external,sizeof(external)));
        CHECK(save(retained_items.get(),sizeof(ItemManager)));
        if(actors.session)CHECK(session_capture.TouchSlot(actors.session,SessionPayload,touch));
        if(actors.effects)CHECK(effect_capture.TouchSlot(actors.effects,EffectPayload,touch));
        frame_open=true;
    }
    void end(){CHECK(frame_open&&journal.EndFrame());frame_open=false;CHECK(depth==0);}
    void clear_capture(){session_capture.Clear();effect_capture.Clear();entry_capture.Clear();}
    void undo(u32 frame){u32 restored=~0u;CHECK(!frame_open&&journal.UndoTo(frame,&restored)&&restored==frame);clear_capture();}
    void clear_history(){CHECK(!frame_open);journal.Clear();clear_capture();}
    UpdateChainEntry* allocate_entry() override{
        // All four entries are active at lifecycle checkpoints. Do not invent
        // a dormant callback first-write hook that production does not have.
        auto* entry=static_cast<UpdateChainEntry*>(entry_pool.allocate(sizeof(UpdateChainEntry),true));
        CHECK(entry);return entry;
    }
    void release_entry(UpdateChainEntry* entry) override{CHECK(entry_pool.release(entry));}
    void enter_lock() override{}
    void leave_lock() override{}
    i32 invoke(CallbackToken token,void* owner) override{
        if(token==callback_id::SessionUpdate||token==callback_id::SessionDraw)CHECK(owner==actors.session);
        else{CHECK(token==callback_id::EffectsUpdate||token==callback_id::EffectsDraw);CHECK(owner==actors.effects);}
        return 1;
    }
    struct Effects final:GameObjectResourceEnvironment {
        Fixture& f;
        explicit Effects(Fixture& value):GameObjectResourceEnvironment{},f(value){
            items=&f.unused_items;bomb=&f.unused_bomb;effects=&f.actors.effects;
            chain=&f.links_pointer;callbacks=&f;effects_update=callback_id::EffectsUpdate;effects_draw=callback_id::EffectsDraw;
        }
        void* allocate(u32 size) override{
            CHECK(size==EffectPayload);if(f.reject_effect)return nullptr;
            auto* result=f.effect_pool.allocate(size);if(!result)return nullptr;
            if(f.frame_open&&!f.omit_first_write)
                CHECK(f.effect_capture.TouchSlot(result,EffectStride,[&](void* p,std::size_t n){return f.save(p,n);}));
            return result;
        }
        void delete_object(void* p) override{CHECK(f.effect_pool.release(p));}
        void release_geometry(void*) override{CHECK(false);}
        AnmFile* load_effect_animations() override{return f.missing_file?nullptr:&f.animation_file;}
        void report_effect_error() override{++f.resource_errors;}
    };
    struct Sessions final:GameSessionResourceEnvironment {
        Fixture& f;
        explicit Sessions(Fixture& value):GameSessionResourceEnvironment{},f(value){
            game=&f.game;scores=&f.unused_scores;current=&f.actors.session;
            for(unsigned i=0;i<ObjectCount;++i)objects[i]=&f.actors.other[i];
            objects[index(SessionObject::Effects)]=reinterpret_cast<GameSystemCallbacks**>(&f.actors.effects);
            current_stage=&f.current_stage;configuration=f.configuration.data();
            new_game=&f.values.new_game;practice_lives=&f.values.practice_lives;
            engine_flags=&f.values.engine_flags;display_flags=&f.values.display_flags;pending_screen=&f.values.pending_screen;
            drawing_resource=&f.values.drawing_resource;updating_resource=&f.values.updating_resource;pending_upload=&f.values.pending_upload;
            loader_stop_requested=&f.values.loader_stop;loader_running=&f.values.loader_running;menu_state=&f.values.menu_state;
            background_color=&f.values.background_color;loading_animation=&f.values.loading_animation;
            recorded_time=&f.values.recorded_time;rendered_time=&f.values.rendered_time;rate=&f.values.rate;
            chain=&f.links_pointer;callbacks=&f;update_callback=callback_id::SessionUpdate;draw_callback=callback_id::SessionDraw;loader_callback=0x417c70;
        }
        GameSession* allocate_session() override{
            if(f.reject_session)return nullptr;auto* result=f.session_pool.allocate(SessionPayload);if(!result)return nullptr;
            if(f.frame_open&&!f.omit_first_write)
                CHECK(f.session_capture.TouchSlot(result,SessionStride,[&](void* p,std::size_t n){return f.save(p,n);}));
            return static_cast<GameSession*>(result);
        }
        void release_session(GameSession* p) override{CHECK(f.session_pool.release(p));}
        void evict_graphics_resources() override{++f.evictions;}
        void begin_loading(CallbackToken token) override{CHECK(token==loader_callback);++f.loads_started;f.values.loader_stop=0;f.values.loader_running=0;}
        void sleep(u32) override{CHECK(false);} // Declared resource/upload barriers are ready.
        bool create_object(SessionObject kind,i32 mode) override{
            CHECK(mode==0);++f.creates[index(kind)];
            if(kind==SessionObject::Effects){Effects env(f);return GameObjectResources{env}.create(GameObjectKind::Effects)!=nullptr;}
            auto* header=&f.external.headers[index(kind)];
            if(kind==SessionObject::Stage){
                // A retained background coexists with the fresh one. Both are
                // external headers here; their real resource lifetime is out of scope.
                auto* previous=object(SessionObject::PreviousStage);
                if(header==previous)header=&f.external.headers[index(SessionObject::PreviousStage)];
            }else if(kind==SessionObject::Replay){
                auto& replay=f.external.replay;replay.draw_entry=&f.external.retained_entries[4];
                replay.end_frame_entry=&f.external.retained_entries[5];
                replay.draw_entry->flags=replay.end_frame_entry->flags=3;
                header=reinterpret_cast<GameSystemCallbacks*>(&replay);
            }else if(kind==SessionObject::Items){header=reinterpret_cast<GameSystemCallbacks*>(f.retained_items.get());}
            else if(kind==SessionObject::Enemies){header=reinterpret_cast<GameSystemCallbacks*>(&f.external.enemies);}
            else if(kind==SessionObject::Results||kind==SessionObject::Bullets){
                const auto offset=kind==SessionObject::Bullets?0:2;
                header->update=&f.external.retained_entries[offset];header->draw=&f.external.retained_entries[offset+1];
                header->update->flags=header->draw->flags=3;
            }
            object(kind)=header;return true;
        }
        void destroy_object(SessionObject kind,GameSystemCallbacks* object_pointer) override{
            CHECK(object(kind)==object_pointer);++f.destroys[index(kind)];
            if(kind==SessionObject::Effects){
                auto* old=f.actors.effects;CHECK(reinterpret_cast<GameSystemCallbacks*>(old)==object_pointer);
                Effects env(f);GameObjectResources{env}.shutdown(*old);CHECK(!f.actors.effects);env.delete_object(old);
            }else object(kind)=nullptr;
        }
        void prepare_replay() override{++f.replay_prepares;}
        void reload_gui() override{}
        void discard_gui_stage() override{++f.retained_gui_discards;}
        void clear_enemies() override{f.external.enemies.head=f.external.enemies.tail=nullptr;}
        void stop_music() override{}
        void load_music(i32 slot,const char*) override{CHECK(slot==0||slot==1);}
        void music_command(i32 command) override{CHECK(command==3||command==4);}
        void finish_loading(bool success) override{if(success)++f.loads_finished;else ++f.load_failures;}
        void save_score() override{++f.score_saves;}
        u32 create_loading_animation(const Vec3& position) override{CHECK(position.z==0);return 73;}
    };
    bool create_session(bool load=true){
        Sessions env(*this);auto* value=GameSessionResources::create(0,env);if(!value)return false;
        return !load||GameSessionResources{*value,env}.load()==0;
    }
    void retire(i32 destination=3){
        CHECK(actors.session);values.pending_screen=destination;auto* old=actors.session;Sessions env(*this);
        GameSessionResources{*old,env}.shutdown();CHECK(!actors.session&&!actors.effects);env.release_session(old);
    }
    std::vector<unsigned char> item_bytes() const{
        std::vector<unsigned char> bytes(sizeof(ItemManager));std::memcpy(bytes.data(),retained_items.get(),bytes.size());return bytes;
    }
};

auto snapshot(const Fixture& f){
    return std::make_tuple(bytes_of(f.session_pool),bytes_of(f.effect_pool),bytes_of(f.entry_pool),bytes_of(f.links),
                           bytes_of(f.actors),bytes_of(f.values),bytes_of(f.external),f.item_bytes());
}
void check_entry(Fixture& f,UpdateChainEntry* entry,void* owner,CallbackToken token,i32 priority,bool enabled){
    CHECK(entry&&f.entry_pool.owns(entry)&&entry->owner==owner&&entry->callback==token);
    CHECK(entry->priority==priority&&entry->flags==(enabled?3u:1u));
    CHECK(entry->node.value==entry&&entry->initialize_callback==0&&entry->notify_callback==0);
    const auto resolved=f.resolve(token);
    CHECK(entry->function.context==static_cast<UpdateChainEnvironment*>(&f)&&entry->function.context!=owner);
    CHECK(entry->function.function==resolved.function&&entry->function.parameter==resolved.parameter&&entry->function(owner)==1);
    CHECK(!entry->notification.context&&!entry->notification.function);
}
void check_order(UpdateChainEntry& sentinel,UpdateChainEntry* first,UpdateChainEntry* second){
    CHECK(sentinel.node.value==&sentinel&&!sentinel.node.previous&&sentinel.node.next==&first->node);
    CHECK(first->node.previous==&sentinel.node&&first->node.next==&second->node);
    CHECK(second->node.previous==&first->node&&!second->node.next);
}
void check_graph(Fixture& f){
    auto* session=f.actors.session;auto* effects=f.actors.effects;
    CHECK(session&&effects&&f.session_pool.owns(session)&&f.effect_pool.owns(effects));
    CHECK(active_count(f.session_pool)==1&&active_count(f.effect_pool)==1&&active_count(f.entry_pool)==4);
    CHECK(session->stage_identifier==17&&session->elapsed.rate==&f.values.rate&&(session->timer_flags&1));
    CHECK(!(session->session_flags&4)&&effects->animations==&f.animation_file);
    check_entry(f,session->update_entry,session,callback_id::SessionUpdate,10,true);
    check_entry(f,session->draw_entry,session,callback_id::SessionDraw,4,true);
    check_entry(f,effects->update_entry,effects,callback_id::EffectsUpdate,23,false);
    check_entry(f,effects->draw_entry,effects,callback_id::EffectsDraw,32,false);
    check_order(f.links.update,session->update_entry,effects->update_entry);
    check_order(f.links.draw,session->draw_entry,effects->draw_entry);CHECK(f.depth==0);
}
void populate(Fixture& f,u32 serial){
    f.actors.session->elapsed.current=static_cast<i32>(serial);f.actors.session->configuration[7]=static_cast<u8>(serial);
    f.actors.effects->reserved_014=serial;f.game.score=static_cast<i32>(serial);f.game.stage_frames=serial;
    std::memset(f.retained_items->regular,static_cast<int>(serial|1),sizeof(f.retained_items->regular)+sizeof(f.retained_items->faith));
}

void lifecycle(bool packed,bool coalesce,i32 destination){
    Fixture f(packed,coalesce);CHECK(f.create_session());populate(f,17);check_graph(f);
    const auto before=snapshot(f);auto* stable_session=f.actors.session;auto* stable_effects=f.actors.effects;
    f.begin(0);f.retire(destination);f.end();f.undo(0);CHECK(snapshot(f)==before);check_graph(f);
    for(u32 frame=0;frame<12;++frame){
        f.begin(frame);f.retire(destination);CHECK(f.create_session());
        CHECK(f.actors.session==stable_session&&f.actors.effects==stable_effects);
        populate(f,100+frame);change_padding(f.session_pool);change_padding(f.effect_pool);change_padding(f.entry_pool);
        check_graph(f);f.end();
    }
    CHECK(f.journal.FrameCount()==12);f.undo(0);CHECK(snapshot(f)==before);check_graph(f);
    for(unsigned generation=0;generation<8;++generation){
        // The World-owned pools survive both generation/history resets.
        f.clear_history();f.retire();CHECK(f.create_session());f.clear_history();
        CHECK(f.actors.session==stable_session&&f.actors.effects==stable_effects);check_graph(f);
    }
    f.retire();CHECK(!active_count(f.session_pool)&&!active_count(f.effect_pool)&&!active_count(f.entry_pool));
}

template<class Pool> void allocation_contract(std::size_t payload){
    Pool pool{};seed(pool,0xa5);CHECK(!pool.allocate(0)&&!pool.allocate(payload+1));
    const auto before=bytes_of(pool.blocks);auto* owner=pool.allocate(payload);
    CHECK(owner==pool.at(0)&&bytes_of(pool.blocks)==before&&!pool.allocate(payload));
    CHECK(!pool.release(nullptr)&&!pool.release(static_cast<unsigned char*>(owner)+1));
    CHECK(pool.release(owner)&&!pool.release(owner));CHECK(pool.allocate(payload)==owner&&pool.release(owner));
}
void failures_and_saved_pointer(){
    {
        Fixture f;const auto before=snapshot(f);f.reject_session=true;
        CHECK(!f.create_session()&&snapshot(f)==before&&f.evictions==1&&!f.loads_started);
        f.reject_session=false;CHECK(f.create_session());const auto live=snapshot(f);
        CHECK(!f.create_session()&&snapshot(f)==live); // Full one-slot pool; native factory evicts before checking null.
        Fixture::Effects effects(f);CHECK(!GameObjectResources{effects}.create(GameObjectKind::Effects)&&snapshot(f)==live);
        auto* old_effect=f.actors.effects;GameObjectResources{effects}.shutdown(*old_effect);CHECK(!f.actors.effects);
        CHECK(!f.effect_pool.release(f.actors.effects)&&active_count(f.effect_pool)==1);effects.delete_object(old_effect);
        auto* old_session=f.actors.session;Fixture::Sessions sessions(f);GameSessionResources{*old_session,sessions}.shutdown();
        CHECK(!f.actors.session&&!f.session_pool.release(f.actors.session)&&active_count(f.session_pool)==1);
        sessions.release_session(old_session);CHECK(!active_count(f.entry_pool)&&!active_count(f.session_pool)&&!active_count(f.effect_pool));
    }
    for(bool missing:{false,true}){
        Fixture f;f.missing_file=missing;f.reject_effect=!missing;CHECK(!f.create_session());
        CHECK(f.actors.session&&!f.actors.effects&&(f.actors.session->session_flags&8));
        CHECK(f.load_failures==1&&f.resource_errors==(missing?1u:0u));
        CHECK(active_count(f.session_pool)==1&&!active_count(f.effect_pool)&&active_count(f.entry_pool)==2);
        CHECK(f.actors.session->update_entry->flags==3&&f.actors.session->draw_entry->flags==3);
        f.retire();f.missing_file=f.reject_effect=false;CHECK(f.create_session());check_graph(f);f.retire();
    }
}
void retained_native_path(){
    Fixture f;CHECK(f.create_session());populate(f,33);
    auto* session=f.actors.session;auto* effects=f.actors.effects;
    const auto retained=f.actors.other;const auto effect_creates=f.creates[index(SessionObject::Effects)];
    const auto item_creates=f.creates[index(SessionObject::Items)];
    f.retire(11);CHECK(f.retained_gui_discards==1&&f.destroys[index(SessionObject::Effects)]==1);
    for(auto kind:{SessionObject::Replay,SessionObject::Gui,SessionObject::Player,SessionObject::Bullets,
                   SessionObject::Items,SessionObject::Lasers,SessionObject::Results,SessionObject::TextOverlay,SessionObject::ScorePopups})
        CHECK(f.actors.other[index(kind)]==retained[index(kind)]&&!f.destroys[index(kind)]);
    CHECK(f.actors.other[index(SessionObject::PreviousStage)]==retained[index(SessionObject::Stage)]);
    for(const auto& entry:f.external.retained_entries)CHECK(entry.flags==1);
    CHECK(all_zero(f.retained_items->regular,sizeof(f.retained_items->regular)+sizeof(f.retained_items->faith)));
    CHECK(f.create_session()&&f.actors.session==session&&f.actors.effects==effects);check_graph(f);
    CHECK(f.replay_prepares==1&&f.creates[index(SessionObject::Effects)]==effect_creates+1);
    CHECK(f.creates[index(SessionObject::Items)]==item_creates);
    CHECK(f.actors.other[index(SessionObject::Stage)]!=f.actors.other[index(SessionObject::PreviousStage)]);
    f.retire();
}
void dormant_first_write(bool packed,bool omit){
    Fixture f(packed);const auto before=snapshot(f);f.omit_first_write=omit;f.begin(0);
    CHECK(f.create_session(false));Fixture::Effects effects(f);
    auto* value=static_cast<GameEffects*>(effects.allocate(EffectPayload));CHECK(value);
    GameObjectResources{effects}.initialize(*value);
    // Real owner initializers only: no dormant callback allocation is exercised
    // or silently repaired by a test-only first-write callback hook.
    change_padding(f.session_pool);change_padding(f.effect_pool);f.end();f.undo(0);
    CHECK(!f.actors.session&&!f.actors.effects&&!active_count(f.session_pool)&&!active_count(f.effect_pool));
    CHECK(bytes_of(f.entry_pool)==std::get<2>(before)&&bytes_of(f.links)==std::get<3>(before));
    if(omit)CHECK(snapshot(f)!=before);else CHECK(snapshot(f)==before);
}
void missing_actor_negative(bool packed){
    Fixture f(packed);CHECK(f.create_session());populate(f,17);const auto before=snapshot(f);
    f.begin(0,false);f.retire();f.end();f.undo(0);
    CHECK(!f.actors.session&&!f.actors.effects&&snapshot(f)!=before);
    CHECK(bytes_of(f.session_pool)==std::get<0>(before)&&bytes_of(f.effect_pool)==std::get<1>(before));
    CHECK(bytes_of(f.entry_pool)==std::get<2>(before)&&bytes_of(f.links)==std::get<3>(before));
}
} // namespace

int main(){
    allocation_contract<SessionPool>(SessionPayload);allocation_contract<EffectPool>(EffectPayload);
    failures_and_saved_pointer();retained_native_path();
    for(bool packed:{false,true}){
        dormant_first_write(packed,false);dormant_first_write(packed,true);missing_actor_negative(packed);
        for(bool coalesce:{false,true})for(i32 destination:{3,10,11})lifecycle(packed,coalesce,destination);
    }
    std::printf("session/effect owner lifetime: PASS (%llu checks); real GameSessionResources/GameSession/GameObjectResources/UpdateChain; "
                "session payload=%zu stride=%zu; effects payload=%zu stride=%zu; callback stride=%zu; "
                "complete loader, native restart/retained paths, full 12-frame undo, generation reset, "
                "allocation/resource failure and saved-pointer/first-write/current-pointer negative controls\n",
                static_cast<unsigned long long>(checks),SessionPayload,SessionStride,EffectPayload,EffectStride,
                sizeof(Fixture::EntryPool::Storage));
    return 0;
}
