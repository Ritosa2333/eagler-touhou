#pragma once
#include "PlayerLife.hpp"
#include "DamageRegions.hpp"
namespace th08 {
struct PlayerCollisionActions {
    virtual ~PlayerCollisionActions()=default;
    virtual void randomize_integrity()=0;
    virtual void die()=0;
    virtual void graze(const Vec3& position,bool laser)=0;
    // thprac F1 (upstream patches the hit judgment at 0x44abda): an enabled
    // invincibility cheat makes every hit check pass without killing.
    virtual bool invincible()=0;
};
// Original 00449ff0, 0044a230/360/470/5a0/6a0. cancel_item is the shared
// result consumed by the bullet manager after a cancellation hit.
class PlayerCollision {
    PlayerMovementState& movement;PlayerLifeState& life;PlayerLifeContext& context;
    DamageRegions& regions;i32& cancel_item;PlayerCollisionActions& actions;
public:
    PlayerCollision(PlayerMovementState& m,PlayerLifeState& l,PlayerLifeContext& c,DamageRegions& r,i32& item,PlayerCollisionActions& a)
        :movement(m),life(l),context(c),regions(r),cancel_item(item),actions(a){}
    i32 barrier(const Vec2& position);
    i32 bullet(const Vec3& position,const Vec3& size,bool cancellation=true);
    i32 graze(const Vec3& position,const Vec3& size);
    bool item(const Vec3& position,const Vec3& size)const;
    i32 laser(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze);
};
}
