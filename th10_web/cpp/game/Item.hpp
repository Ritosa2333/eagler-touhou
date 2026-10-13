#pragma once
#include "AnmFile.hpp"
#include "AnmEnvironment.hpp"
#include "GameEconomy.hpp"
namespace th10 {
struct ItemFrameEnvironment;
struct ItemDrawEnvironment;
struct UpdateChainEntry;
enum class ItemUpdate { Skipped, Active, FullPower };
struct Item {
    AnmVm animation;
    Vec3 position,velocity;
    float motion_3c4;
    Timer timer;
    u32 timer_flags;
    i32 state,kind,sprite_kind;
    float attraction_speed;
    i32 value_variant;
    ItemUpdate update(ItemFrameEnvironment& environment);
    void draw(ItemDrawEnvironment& environment);
};
static_assert(sizeof(Item)==0x3f0);
static_assert(offsetof(Item,state)==0x3dc);
struct ItemEnvironment {
    AnmFile* animation_file;
    AnmEnvironment* animations;
    u32* started_animations;
    const float* default_rate;
    const std::int16_t* power;
    virtual void spawn_effect(const Vec3& position,i32 script)=0;
    void initialize_animation(Item& item,i32 script);
};
struct ItemRegion {
    Vec3 minimum,maximum;
    bool contains(const Vec3& point) const noexcept;
};
struct ItemFrameEnvironment : ItemEnvironment,EconomyEnvironment {
    GameEconomy* economy;
    const Vec3* player_position;
    const i32* player_state;
    const float* player_attraction_speed;
    const i32* auto_collect;
    const u32* input_keys;
    const ItemRegion* pickup_region;
    const ItemRegion* slow_region;
    const ItemRegion* fast_region;
#ifdef TH_ENABLE_THPRAC
    virtual void point_collected(bool) {}
#endif
    virtual void update_power_display(i32 whole,i32 fraction)=0;
    virtual void refresh_player_power()=0;
    virtual void popup(const Vec3& position,i32 value,u32 color)=0;
    virtual void play_sound(i32 sound,float horizontal_position)=0;
};
struct ItemDrawEnvironment {
    virtual void bind_item_sprite(AnmVm& vm,i32 sprite)=0;
    virtual void draw_animation(AnmVm& vm)=0;
    virtual bool presentation(const Item& item,Vec3& position)=0;
};
struct ItemManager {
    u32 flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    AnmFile* animation_file;
    Item regular[150];
    Item faith[2048];
    i32 active_count;
    i32 faith_cursor,faith_count;
    i32 spawn(const Vec3& position,i32 kind,u32 color,float angle,float speed,ItemEnvironment& environment);
    i32 update(ItemFrameEnvironment& environment);
    i32 convert_power(ItemEnvironment& environment);
    i32 draw(ItemDrawEnvironment& environment);
};
static_assert(offsetof(ItemManager,faith)==0x24eb4);
static_assert(offsetof(ItemManager,faith_cursor)==0x21ceb8);
}
