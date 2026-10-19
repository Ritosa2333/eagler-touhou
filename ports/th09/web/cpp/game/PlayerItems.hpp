#pragma once
#include "AnmLayout.hpp"
#include "AttackTransfer.hpp"
#include "PlayerCollision.hpp"
#include <array>
namespace th09 {
struct PlayerItem {i32 type=0;Vec3 position,velocity;i32 active=0;AnmVm animation;};
struct PlayerItemActions {
    virtual ~PlayerItemActions()=default;
    virtual void start_animation(AnmVm&,i32)=0;
    virtual void draw_animation(AnmVm&)=0;
    virtual void charge(float)=0;
    virtual TransferParameters* create_transfer(i32 effect,const Vec3&,const Vec3&,float delay)=0;
    virtual void combo(const Vec3&,i32 normal,i32 spirit,i32 character,i32 score)=0;
    virtual void play_sound(i32,i32)=0;
};
struct ItemContext {i32 state=0,side=0,rank=0,difficulty=0;float pickup_size=0;Vec3 player;Bounds pickup_bounds;PlayfieldGeometry geometry[2];};
class PlayerItems {
    Rng& random;PlayerItemActions& actions;
public:
    std::array<PlayerItem,4> items;Vec2 attraction;
    PlayerItems(Rng& r,PlayerItemActions& a):random(r),actions(a){}
    void spawn(i32 type,const Vec3&,bool scene_locked);
    void update(const ItemContext&);
    void draw(const PlayfieldGeometry&);
};
}
