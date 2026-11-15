#pragma once
#include "Types.hpp"
namespace th11 {
// Native TH14 0x380-byte emitter at enemy +598. TH12's 0x214-byte
// descriptor cannot hold this title's transform fields or 32-bit sprite IDs.
struct Th14BulletTransform {
    float a,b,c,d;
    i32 p,q,r,s;
    u32 opcode;
    i32 concurrent;
    u32 script;
};
static_assert(sizeof(Th14BulletTransform)==0x2c);
struct Th14BulletEmitter {
    i32 sprite,color;
    Vec3 position;
    float angle,spread,speed,slow_speed,radius;
    Th14BulletTransform transforms[18];
    u32 values_340[9];
    i16 count,layers,aim;u16 reserved_36a;
    u32 flags;
    i32 fire_sound,transform_sound,transform_start,reserved_37c;
};
static_assert(offsetof(Th14BulletEmitter,transforms)==0x28);
static_assert(offsetof(Th14BulletEmitter,count)==0x364);
static_assert(sizeof(Th14BulletEmitter)==0x380);
}
