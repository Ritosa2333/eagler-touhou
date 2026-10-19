#pragma once
#include "Types.hpp"
namespace th08 {
struct DamageRegion {
    Vec2 position;float radius,radius_growth;Vec2 dimensions,size_growth;float angle;
    i32 lifetime,cancel_item,hit_damage,damage_dealt,damage_limit,interval;u8 active,suppress_effect,padding[2];
    void reset(){std::memset(this,0,sizeof(*this));interval=1;}
};
static_assert(sizeof(DamageRegion)==64&&offsetof(DamageRegion,active)==60);
// Original two 192-entry region pools. Allocation replaces the final slot
// when full; the damage/cancellation update is owned by the player logic.
struct DamageRegions {
    DamageRegion damaging[192]{},cancelling[192]{};
    DamageRegion& rectangle(bool damage,const Vec2& position,float width,float height,i32 value,i32 lifetime);
    DamageRegion& circle(bool damage,const Vec2& position,float radius,float growth,i32 value,i32 lifetime);
    void update();
};
}
