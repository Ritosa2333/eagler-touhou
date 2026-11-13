#pragma once
#include "AnmVm.hpp"
#include "Movement.hpp"
#if PORT_TITLE == 14
#include "GameEconomy.hpp"
#endif
#include <memory>
namespace th11 {
struct EnemyDrop;
#if PORT_TITLE == 16
struct Th16DropServices {virtual ~Th16DropServices()=default;virtual bool spawn(i32,Vec3,float,float)=0;};
bool item_th16_scatter(EnemyDrop&,Vec3,Rng&,Th16DropServices&);
bool item_th16_drop(EnemyDrop&,Vec3,Rng&,Th16DropServices&);
#endif
class AnmRenderer;
struct ItemState {
    AnmVm vm;Vec3 position,velocity;float reserved_44c;
    Timer lifetime;i32 state,type,display_type;float attraction_speed;i32 delay;
#if PORT_TITLE == 14
    ItemState* th14_free_next;u32 th14_generation;AnmVm th14_indicator;
#endif
#if PORT_TITLE == 16
    ItemState* th16_free_next;u32 th16_generation;i32 th16_force_attract;
    float th16_angle;AnmVm th16_indicator;
#endif
};
#if PORT_TITLE != 14 && PORT_TITLE != 16
static_assert(sizeof(ItemState)==0x478+kAnmVmExtensionBytes);
#endif
static_assert(offsetof(ItemState,state)==0x464+kAnmVmExtensionBytes);
struct ItemBounds {float left=1,top=1,right=-1,bottom=-1;bool contains(Vec3 p)const noexcept{return left<=p.x&&top<=p.y&&p.x<=right&&p.y<=bottom;}};
struct ItemPlayer {
    Vec3 position{0,400,0};i32 state=1;float attraction_speed=8;
    ItemBounds pickup,focused_attract,unfocused_attract;bool focused=false,force_attract=false;
    i32 communication=0,power=0,max_power=400;
#if PORT_TITLE == 14
    i32 character=0;bool bomb_attract=false;
#endif
#if PORT_TITLE == 16
    i32 character=0;bool bomb_attract=false,season_attract=false,spell_attract=false;
#endif
};
struct ItemWorld {
    virtual ~ItemWorld()=default;
    virtual bool effect(Vec3,i32){return false;}
    virtual bool sound(i32,float){return false;}
    virtual bool collect(ItemState&,bool& convert_power_items){return false;}
    virtual bool rank_delta(i32){return false;}
#if PORT_TITLE == 16
    virtual bool item_th16_collect(ItemState&){return false;}
    virtual bool item_th16_spawn_notice(Vec3,i32){return false;}
    i32 th16_drop_counter=0;
    virtual i32& item_th16_drop_count(){return th16_drop_counter;}
#endif
#if PORT_TITLE == 14
    i32 th14_drop_counter=0;
    Th14ItemCombo th14_combo;
    virtual i32& item_th14_drop_counter(){return th14_drop_counter;}
    virtual Th14ItemCombo& item_th14_combo_state(){return th14_combo;}
    virtual bool item_th14_combo_settle(){return false;}
    virtual bool item_th14_spawn_notice(Vec3,i32){return false;}
#endif
};
class ItemManager {
public:
    static constexpr u32 ordinary_capacity=kPortTitle==14||kPortTitle==16?600:150,cancel_capacity=kPortTitle==14||kPortTitle==16?4096:2048,capacity=ordinary_capacity+cancel_capacity;
    ItemManager(AnmResource&,AnmEnvironment&,ItemWorld&,u16 file_id=6);
    ItemState& at(u32 n){return storage[n];}
    const ItemState& at(u32 n)const{return storage[n];}
    ItemPlayer player;u32 cancel_cursor=0,cancel_spawn_count=0,active_count=0;i32 last_error=0;
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
#if PORT_TITLE == 16
    i32 spawn_ordinary_th16(i32,Vec3,float,float,i32 force_attract=0);
    bool drop_th16(EnemyDrop&,Vec3);
    i32 spawn_cancel_th16(i32,Vec3,float,float,i32 force_attract=0);
    void release_th16(ItemState&)noexcept;
    u32 th16_spawn_count=0,th16_cancel_count=0,th16_generation=0;
    i32 th16_season=0;
    i32 th16_attract_timer=0;float th16_slowdown=1;
    i32 th16_free_slot()const noexcept{return th16_cancel_free?i32(th16_cancel_free-storage.get()):-1;}
    i32 th16_ordinary_slot()const noexcept{return th16_ordinary_free?i32(th16_ordinary_free-storage.get()):-1;}
#endif
#if PORT_TITLE == 14
    // 41ae00 reward type 1: the caller owns its on-screen counter.
    bool cancel_reward_th14(Vec3);
    bool cancel_reward_th14(Vec3,u32 rewards,u32 reward_count,u32 spell_flags);
    i32 spawn_cancel_th14(i32,Vec3,float,float);
    i32 spawn_ordinary_th14(i32,Vec3,float,float,i32 delay=0);
    bool scatter_th14(EnemyDrop&,Vec3);
    void release_th14(ItemState&)noexcept;
    u32 th14_spawn_count=0,th14_cancel_count=0,th14_generation=0;
    i32 th14_last_collect_type=-1;
    i32 th14_free_slot()const noexcept{return th14_cancel_free?i32(th14_cancel_free-storage.get()):-1;}
    i32 th14_ordinary_slot()const noexcept{return th14_ordinary_free?i32(th14_ordinary_free-storage.get()):-1;}
#endif
private:
#if PORT_TITLE == 14 && defined(TH14_CPP_STAGE_RESOURCE_HANDSHAKE)
    friend class GameBattle;
    ItemManager(AnmResource&,AnmEnvironment&,ItemWorld&,u16 file_id,bool defer_storage);
    void initialize_storage();
#endif
    std::unique_ptr<ItemState[]> storage;
    AnmResource& resource;AnmEnvironment& animations;ItemWorld& world;u16 file_id;
    bool bind(ItemState&,i32 script);
    void move(ItemState&);
    void home(ItemState&);
#if PORT_TITLE == 16
    ItemState* th16_cancel_free=nullptr;
    ItemState* th16_ordinary_free=nullptr;
    bool activate_th16(ItemState&);
    bool update_item_th16(ItemState&);
#endif
#if PORT_TITLE == 14
    ItemState* th14_cancel_free=nullptr;ItemState* th14_ordinary_free=nullptr;
    bool update_th14();bool draw_th14(AnmRenderer&);
#endif
};
}
