#pragma once
#include "Types.hpp"
namespace th15 {
enum class PlayerContact:i32 { none=0,hit=1,graze=2 };
struct PlayerDamageHost {virtual ~PlayerDamageHost()=default;virtual void hit()=0;};
struct PlayerCollision {
    Vec2 position{},hitbox_min{},hitbox_max{};
    float radius=0,size_multiplier=1;bool enlarged=false,bomb_active=false;
    i32 state=0,invulnerability=0;
    Vec2 laser_half_size{};
    PlayerContact rectangle(const Vec2& position,const Vec2& size,bool graze_only,PlayerDamageHost* damage=nullptr)const;
    PlayerContact circle(const Vec2& position,float bullet_radius,bool graze_only,PlayerDamageHost* damage=nullptr)const;
    PlayerContact laser(const Vec2& origin,float angle,float length,float width,bool graze_only,PlayerDamageHost* damage=nullptr)const;
private:
    PlayerContact contact(bool graze_only,PlayerDamageHost* damage)const;
};
}
