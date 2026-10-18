#pragma once
#include "Types.hpp"
namespace th11 {
enum class CollisionKind:i32 {None=0,Hit=1,Graze=2};
struct PlayerCollisionResult {CollisionKind kind=CollisionKind::None;bool trigger_hit=false;};
struct PlayerCollision {
    Vec2 position{},minimum{},maximum{};float radius=0;
    i32 state=0,invincibility_frames=0;u32 flags=0;bool bomb_active=false;
    PlayerCollisionResult rectangle(Vec2 position,Vec2 size)const noexcept;
    PlayerCollisionResult circle(Vec2 position,float radius)const noexcept;
    PlayerCollisionResult laser(Vec2 origin,Vec2 player_half,float angle,float width,float length,bool probe=false)const noexcept;
private:
    PlayerCollisionResult hit()const noexcept;
};
}
