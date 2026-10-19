#pragma once
#include "Types.hpp"
namespace th11 {
struct BulletTransform {float a,b;i32 c,d;u32 opcode; i32 concurrent;};
static_assert(sizeof(BulletTransform)==24);
struct BulletEmitter {
    i16 sprite,color;Vec3 position;float angle,spread,speed,slow_speed,radius;
    BulletTransform transforms[18];
    u8 reserved_1d4[0x24];
    i16 count,layers,aim;u16 reserved_1fe;
    u32 flags;i32 fire_sound,transform_sound,transform_start,reserved_210;
    void initialize()noexcept;
};
static_assert(offsetof(BulletEmitter,transforms)==0x24);
static_assert(offsetof(BulletEmitter,count)==0x1f8);
static_assert(sizeof(BulletEmitter)==0x214);
}
