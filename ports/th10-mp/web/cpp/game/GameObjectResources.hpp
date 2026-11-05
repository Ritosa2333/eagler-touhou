#pragma once
#include "Item.hpp"
#include "Bomb.hpp"
#include "UpdateChain.hpp"
namespace th10 {
struct GameEffects {
    u32 flags,manager_state;
    UpdateChainEntry *update_entry,*draw_entry;
    AnmFile* animations;
    u32 reserved_014;
};
static_assert(sizeof(GameEffects)==0x18 && sizeof(Bomb)==0x48 && sizeof(ItemManager)==0x21cec0);
enum class GameObjectKind { Items,Bomb,Effects };
struct GameObjectResourceEnvironment {
    ItemManager** items;
    Bomb** bomb;
    GameEffects** effects;
    UpdateChain** chain;
    UpdateChainEnvironment* callbacks;
    CallbackToken item_update,item_draw,bomb_update,bomb_draw,effects_update,effects_draw;
    virtual void* allocate(u32 bytes)=0;
    virtual void delete_object(void* object)=0;
    virtual void release_geometry(void* geometry)=0;
    virtual AnmFile* load_effect_animations()=0;
    virtual void report_effect_error()=0;
};
struct GameObjectResources {
    GameObjectResourceEnvironment& environment;
    void initialize(ItemManager& items) const noexcept;
    void initialize(Bomb& bomb) const noexcept;
    void initialize(GameEffects& effects) const noexcept;
    i32 start(ItemManager& items);
    i32 start(Bomb& bomb);
    i32 start(GameEffects& effects);
    void shutdown(ItemManager& items);
    void shutdown(Bomb& bomb);
    void shutdown(GameEffects& effects);
    void* create(GameObjectKind kind);
};
}
