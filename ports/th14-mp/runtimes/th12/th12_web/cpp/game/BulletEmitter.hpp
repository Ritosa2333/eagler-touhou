#pragma once
#include "Types.hpp"
namespace th11 {
struct BulletTransform {float a,b;i32 c,d;u32 opcode; i32 concurrent;};
static_assert(sizeof(BulletTransform)==24);
struct BulletEmitter {
    i16 sprite,color;Vec3 position;float angle,spread,speed,slow_speed,radius;
    BulletTransform transforms[18];
    // 0x258 (Sffff) stores its fourth float here: the first word of the block that runs from the end of
    // the transform array at 0x1d4 to count at 0x1f8. Nothing has been found reading it yet, so it is
    // named after its offset rather than guessed at, the same convention the state commands use.
    float value_1d4;
    u8 reserved_1d8[0x0c];
    // 0x259 (iii) reads i(0) as the emitter index and stores i(1) through i(5) here, one dword each. The handler in
    // th12.exe 0x4186cd strides the index by 0x214 and writes +0x670 through +0x680 relative to the enemy, and the
    // dispatcher's emitter base is +0x48c, so those are 0x1e4, 0x1e8, 0x1ec, 0x1f0 and 0x1f4 - the block between the
    // transform array and count, which was a single reserved run until the five offsets were measured. The offsets
    // are the same in every emitter, so the name is the offset until something is found reading them.
    // The original writes them with a dword move, and the last one, 0x1f4, is four bytes before count.
    u32 values_1e4[5];
    i16 count,layers,aim;u16 reserved_1fe;
    u32 flags;i32 fire_sound,transform_sound,transform_start,reserved_210;
    void initialize()noexcept;
};
static_assert(offsetof(BulletEmitter,transforms)==0x24);
static_assert(offsetof(BulletEmitter,value_1d4)==0x1d4);
static_assert(offsetof(BulletEmitter,values_1e4)==0x1e4);
static_assert(offsetof(BulletEmitter,count)==0x1f8);
static_assert(sizeof(BulletEmitter)==0x214);
}
