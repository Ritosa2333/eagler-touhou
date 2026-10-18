#pragma once
#include "ItemState.hpp"
#include "AnmManager.hpp"
#include "EffectManager.hpp"
#include "ItemCollection.hpp"
#include "PlayerBounds.hpp"
#include "AnmRenderer.hpp"
namespace th15 {
struct ItemFrameContext {PlayerMotion& player;const PlayerBounds& bounds;ItemScoreState& score;ItemCollection& collection;Rng& visual_random;i32 player_state=1,character=0,bomb_state=0,bomb_frame=0;bool hud_collect=false;u32 held=0;float attraction_speed=5,rate=1;};
class ItemManager {
    friend class ItemCheckpoint;
public:
    struct Slot {ItemState state;std::unique_ptr<AnmVm> body,arrow;bool arrow_ended=false;};
    static constexpr u32 ordinary_count=600,cancel_count=4096,pool_size=ordinary_count+cancel_count;
private:
    AnmManager& animations;EffectManager& effects;i32 resource;std::array<Slot,pool_size> slots;std::array<std::vector<u32>,2> free_slots;
    bool appearance(Slot&,i32 kind);bool piece_effect(const ItemState&);
public:
    i32 requests=0,cancel_density=0,cancel_stamp=0,alternating_pieces=0,active_count=0;float motion_scale=1;std::string error;
    i32* alternating_counter=nullptr;
    std::function<bool(i32)> sound;
    std::function<bool(i32)> immediate_sound;
    ItemManager(AnmManager&,EffectManager&,i32 resource);
    ~ItemManager();
    bool reset();
    Slot* find(u32 id){return id>0&&id<=pool_size?&slots[id-1]:nullptr;}
    u32 spawn(i32 kind,const Vec3&,float angle,float speed);
    bool release(u32 id);bool falling(u32 id);
    bool update(ItemFrameContext&);
    bool draw(AnmRenderer&);
    u32 free_head(bool cancel)const noexcept{return free_slots[cancel].empty()?0:free_slots[cancel].back()+1;}
};
}
