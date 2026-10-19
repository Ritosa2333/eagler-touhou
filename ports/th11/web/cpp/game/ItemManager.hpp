#pragma once
#include "AnmVm.hpp"
#include "Movement.hpp"
#include <memory>
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
    virtual ~ItemWorld()=default;
    virtual bool effect(Vec3,i32){return false;}
    virtual bool sound(i32,float){return false;}
    virtual bool collect(ItemState&,bool& convert_power_items){return false;}
    virtual bool rank_delta(i32){return false;}
};
class ItemManager {
public:
    static constexpr u32 ordinary_capacity=150,cancel_capacity=2048,capacity=ordinary_capacity+cancel_capacity;
    ItemManager(AnmResource&,AnmEnvironment&,ItemWorld&,u16 file_id=6);
    ItemState& at(u32 n){return storage[n];}
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
private:
    std::unique_ptr<ItemState[]> storage;
    AnmResource& resource;AnmEnvironment& animations;ItemWorld& world;u16 file_id;
    bool bind(ItemState&,i32 script);
    void move(ItemState&);
    void home(ItemState&);
};
}
