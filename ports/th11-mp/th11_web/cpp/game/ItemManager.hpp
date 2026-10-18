#pragma once
#include "AnmVm.hpp"
#include "Movement.hpp"
#include <memory>
#include <array>
#ifdef TH11_MULTIPLAYER
#include "ActiveSlotIndex.hpp"
namespace Netplay {class RollbackJournal;}
#endif
namespace th11 {
struct EnemyDrop;
class AnmRenderer;
struct ItemState {
    AnmVm vm;Vec3 position,velocity;float reserved_44c;
    Timer lifetime;i32 state,type,display_type;float attraction_speed;i32 delay;
};
static_assert(sizeof(ItemState)==0x478);
static_assert(offsetof(ItemState,state)==0x464);
struct ItemBounds {float left=1,top=1,right=-1,bottom=-1;bool contains(Vec3 p)const noexcept{return left<=p.x&&top<=p.y&&p.x<=right&&p.y<=bottom;}};
struct ItemPlayer {
    Vec3 position{0,400,0};i32 state=1;float attraction_speed=8;
    ItemBounds pickup,focused_attract,unfocused_attract;bool focused=false,force_attract=false;
    i32 communication=0,power=0,max_power=400;
};
struct ItemWorld {
#ifdef TH11_MULTIPLAYER
    virtual bool multiplayer_world()const{return false;}
    virtual bool multiplayer_item_player(ItemState&,ItemPlayer&){return false;}
    virtual bool multiplayer_power_full()const{return false;}
    virtual unsigned multiplayer_item_copies(i32)const{return 1;}
#endif
    virtual ~ItemWorld()=default;
    virtual bool effect(Vec3,i32){return false;}
    virtual bool sound(i32,float){return false;}
    virtual bool collect(ItemState&,bool& convert_power_items){return false;}
    virtual bool rank_delta(i32){return false;}
};
class ItemManager {
public:
    #ifdef TH11_MULTIPLAYER
    static constexpr u32 ordinary_capacity=450;
#else
    static constexpr u32 ordinary_capacity=150;
#endif
    static constexpr u32 cancel_capacity=2048,capacity=ordinary_capacity+cancel_capacity;
    ItemManager(AnmResource&,AnmEnvironment&,ItemWorld&,u16 file_id=6);
    ItemState& at(u32 n){
#ifdef TH11_MULTIPLAYER
        preserve_rollback_slots(n,1);
        activity.invalidate(); // a returned mutable slot may be changed externally
        exposed->remember(n);
#endif
        return storage[n];
    }
    const ItemState& at(u32 n)const noexcept{return storage[n];}
    u32 index_of(const ItemState& item)const noexcept{return u32(&item-storage.get());}
#ifdef TH11_MULTIPLAYER
    // Requires a fresh checkpoint, not extendPrevious. Owns only pool bytes
    // and explicit manager metadata. The caller owns the
    // journal transaction, shared ANM/RNG, world effects and resource lifetime.
    // Detach before undo/destruction; a failed capture is not a no-write promise.
    bool BeginRollbackFrame(Netplay::RollbackJournal&);
    void EndRollbackFrame()noexcept;
    bool RollbackFrameActive()const noexcept{return rollback_journal!=nullptr;}
#endif
    ItemPlayer player;u32 cancel_cursor=0,cancel_spawn_count=0,active_count=0;i32 last_error=0;
#ifdef TH11_MULTIPLAYER
    std::array<i32,capacity> multiplayer_targets{};
    i32 multiplayer_current_target=-1;
    bool multiplayer_spawning_transfer=false;
    std::array<u8,ordinary_capacity> multiplayer_fan{};
    bool spawn_transfer(i32,Vec3,unsigned);
#endif
    // 0 success/full pool, -2 unavailable resource or world dependency.
    i32 spawn(i32 type,Vec3 position,u32 color=0xffffffff,float angle=-1.5707963705062866f,float speed=2.2f);
    bool update();
    void reset()noexcept;
    bool draw(AnmRenderer&);
    bool activate(ItemState&);
    void attract_all();
    bool convert_power_items();
    bool scatter(EnemyDrop&,Vec3);
    bool drop(EnemyDrop&,Vec3);
private:
    u32 next_active(u32 first){
#ifdef TH11_MULTIPLAYER
        if(exposed->count>64){while(first<capacity&&!storage[first].state)++first;return first;}
        activity.ensure([&](u32 i){return storage[i].state!=0;});
        if(exposed->count)exposed->refresh(activity,[&](u32 i){return storage[i].state!=0;});
        return activity.next(first);
#else
        return first;
#endif
    }
    void set_state(ItemState& item,i32 state){item.state=state;
#ifdef TH11_MULTIPLAYER
        const auto address=reinterpret_cast<uintptr_t>(&item),base=reinterpret_cast<uintptr_t>(storage.get());
        if(address>=base&&address-base<sizeof(ItemState)*capacity&&(address-base)%sizeof(ItemState)==0)
            activity.set(u32((address-base)/sizeof(ItemState)),state!=0);
#endif
    }
    std::unique_ptr<ItemState[]> storage;
    AnmResource& resource;AnmEnvironment& animations;ItemWorld& world;u16 file_id;
    i32 spawn_single(i32,Vec3,u32,float,float,ItemState** out=nullptr);
    bool bind(ItemState&,i32 script);
    void move(ItemState&);
    void home(ItemState&);
#ifdef TH11_MULTIPLAYER
    ActiveSlotIndex<capacity> activity;
    std::unique_ptr<ExposedSlotIndex<capacity>> exposed=std::make_unique<ExposedSlotIndex<capacity>>();
    Netplay::RollbackJournal* rollback_journal=nullptr;
    std::array<bool,capacity> rollback_captured{};
    bool preserve_rollback_slots(u32 first,u32 count)noexcept;
    void preserve_rollback_slot(ItemState&)noexcept;
#endif
};
}
