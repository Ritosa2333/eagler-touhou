#pragma once
#include "AnmLayout.hpp"
namespace th08 {
struct BulletExtra {
    float float_a=0,float_b=0;i32 integer_a=0,integer_b=0;u32 flags=0;i32 mode=0;
};
static_assert(sizeof(BulletExtra)==24);
struct BulletEmission {
    i16 sprite=0,color=0;Vec3 position;float angle=0,spread=0,speed=0,ending_speed=0;
    BulletExtra extras[18];
    struct LaserParameters {float start_offset,end_offset,length,width;i32 start,duration,stop,hitbox_start,hitbox_stop;};
    union {u32 reserved1d0[9]{};LaserParameters laser;};
    i16 count=0,layers=0,pattern=0,unknown1fa=0;u32 flags=0;
    i32 sound=0,transform_sound=0,extra_index=0;struct BulletTemplate* type=nullptr;
};
static_assert(sizeof(BulletEmission)==0x210&&offsetof(BulletEmission,count)==0x1f4);
struct BulletEmissionActions {
    virtual ~BulletEmissionActions()=default;
    virtual void emit(BulletEmission& parameters)=0;
    virtual void clear(i32 mode)=0;
};
struct LaserEmissionActions {virtual ~LaserEmissionActions()=default;virtual struct LaserState* laser(BulletEmission& parameters)=0;};
}
