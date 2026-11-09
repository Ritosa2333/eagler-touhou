#pragma once
#include "ItemState.hpp"
#include "AnmManager.hpp"
#include "EffectManager.hpp"
#include "AnmRenderer.hpp"
#include "ItemCollection.hpp"
#include "PlayerBounds.hpp"
#include <functional>
namespace th17 {
struct ItemSpawnContext {Rng& game_random;Vec3 player{};std::function<bool(i32,const Vec3&,float)> spirit;};
struct ItemFrameContext {PlayerMotion& player;PlayerBounds& bounds;ItemCollection& collection;Rng& visual_random;float rate=1,attraction_speed=5;i32 character=0,player_state=0,bomb_state=0,bomb_frame=0;u32 held=0;bool hud_collect=false,active=true;};
class ItemManager {
public:
    struct Slot {ItemState state;std::unique_ptr<AnmVm> body,arrow;bool arrow_ended=false;};
    static constexpr u32 ordinary_count=600,cancel_count=4096,pool_size=ordinary_count+cancel_count;
private:
    AnmManager& animations;EffectManager& effects;i32 resource;
    std::array<Slot,pool_size> slots;
    // The untouched part of each free list is a contiguous index range.
    // Released slots remain a stack above that range, in release order.
    std::array<u32,2> unused_free{ordinary_count,cancel_count};
    std::array<std::vector<u32>,2> recycled;
    std::array<u32,(pool_size+31)/32> used{};
    struct CachedVisual {
        std::weak_ptr<const AnmVm> idle;
        std::weak_ptr<const AnmInterpolators> curves;
    };
    struct IdleVisuals {CachedVisual body,arrow;};
    std::unique_ptr<std::array<IdleVisuals,pool_size>> idle_visuals;
    void touch(u32 index){used[index/32]|=u32(1)<<(index%32);if(idle_visuals)(*idle_visuals)[index]={};}
    bool appearance(Slot&,i32 kind);bool birth_effect(const ItemState&);
public:
    i32 requests=0,cancel_density=0,cancel_stamp=0,active_count=0;float motion_scale=1;std::string error;
    std::function<bool(i32)> sound;
    std::function<bool(i32)> immediate_sound;
    ItemManager(AnmManager&,EffectManager&,i32 resource);
    ~ItemManager();
    bool reset();
    Slot* find(u32 id){if(!id||id>pool_size)return nullptr;touch(id-1);return &slots[id-1];}
    const Slot* find(u32 id)const{return id&&id<=pool_size?&slots[id-1]:nullptr;}
    u32 next_used(u32 start)const noexcept;
    u32 spawn(i32 kind,const Vec3&,float angle,float speed,i32 delay=0,i32 sound=-1,ItemSpawnContext* context=nullptr);
    bool release(u32 id);bool falling(u32 id);
    bool update(ItemFrameContext&,const std::array<ItemFrameContext*,2>& partners={});
    bool draw(AnmRenderer&);
    u32 free_head(bool cancel)const noexcept{
        if(!recycled[cancel].empty())return recycled[cancel].back()+1;
        return unused_free[cancel]?(cancel?ordinary_count:0)+unused_free[cancel]:0;
    }
    void save_rollback(multiplayer::FrameImage&);
};
}
