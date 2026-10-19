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
public:
    // Derived, synchronous BulletSystem scan only. Never survives a tick or
    // rollback; all shapes and hit counters remain in the native region pool.
    class BarrierBatch {
        friend class PlayerCollision;
        PlayerCollision* owner;
        const BarrierBatch* previous=nullptr;
        u16 slots[192];u16 count=0;
    public:
        explicit BarrierBatch(PlayerCollision*);
        ~BarrierBatch();
        BarrierBatch(const BarrierBatch&)=delete;
        BarrierBatch& operator=(const BarrierBatch&)=delete;
    };
private:
    PlayerMovementState& movement;PlayerLifeState& life;PlayerLifeContext& context;
    DamageRegions& regions;i32& cancel_item;PlayerCollisionActions& actions;
    const BarrierBatch* barrier_batch=nullptr;
public:
    PlayerCollision(PlayerMovementState& m,PlayerLifeState& l,PlayerLifeContext& c,DamageRegions& r,i32& item,PlayerCollisionActions& a)
        :movement(m),life(l),context(c),regions(r),cancel_item(item),actions(a){}
    i32 barrier(const Vec2& position);
    i32 bullet(const Vec3& position,const Vec3& size,bool cancellation=true);
    i32 graze(const Vec3& position,const Vec3& size);
    bool item(const Vec3& position,const Vec3& size)const;
    i32 laser(const Vec2& center,const Vec2& size,const Vec3& origin,float angle,bool graze);
};
#ifdef TH_MULTIPLAYER_FIXTURES
bool fixture_collision_broadphase(bool enabled) noexcept;
const double* fixture_collision_broadphase_counts() noexcept;
bool fixture_collision_broadphase_probe() noexcept;
bool fixture_barrier_cache(bool enabled) noexcept;
void fixture_barrier_cache_audit(bool enabled) noexcept;
u32 fixture_barrier_cache_mismatches() noexcept;
#endif
}
