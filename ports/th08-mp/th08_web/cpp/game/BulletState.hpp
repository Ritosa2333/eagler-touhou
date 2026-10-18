#pragma once
#include "BulletTemplates.hpp"
#include "BulletEmission.hpp"
namespace th08 {
// TH08 layouts checked against the original allocation/creation routines.
// Field names also use the local MIT GensokyoClub/th08 reference snapshot.
struct BulletExtraState {Timer timer;float float_a=0,float_b=0;Vec3 vector;i32 integer_a=0,integer_b=0,integer_c=0;};
static_assert(sizeof(BulletExtraState)==0x2c);
struct BulletState {
    BulletTemplate sprites;Vec3 position,velocity,legacy_acceleration;
    float speed=0,legacy_float_a=0,change_speed=0,angle=0,legacy_float_b=0,change_angle=0;
    Timer since_fired,active_time;
    i32 legacy_integer=0,change_interval=0,change_count=0,change_limit=0,despawn_protection=0;
    u32 extra_flags=0,flags=0;i16 sprite_offset=0;u16 unknown_db6=0,state=0,despawn_counter=0;
    u8 unknown_dbc=0,grazed=0,unknown_dbe=0,padding_dbf=0;
    BulletState* next_in_layer=nullptr;i32 barrier_cooldown=0,transform_sound=0,current_extra=0;
    BulletExtra extras[18];BulletExtraState extra_state[7];u8 reisen_illusion=0,padding10b5[3]{};
};
static_assert(sizeof(BulletState)==0x10b8&&offsetof(BulletState,state)==0xdb8&&offsetof(BulletState,extras)==0xdd0);
struct LaserState {
    AnmVm animation[2];Vec3 position;float angle=0,start_offset=0,end_offset=0,length=0,width=0,width2=0,speed=0;
    i32 start=0,hitbox_start=0,duration=0,stop=0,hitbox_stop=0,in_use=0;Timer timer;u16 flags=0;i16 color=0;u8 state=0;i8 unknown599=0;u8 padding[2]{};
};
static_assert(sizeof(LaserState)==0x59c);
struct BulletManagerState {
    BulletTemplates templates;BulletState bullets[1537];LaserState lasers[256];
    i32 active_count=0,cancel_frames=0;Timer timer;i32 unknown_counter=0;const char* filename=nullptr;
    BulletState* layers[6]{};BulletState* next_slot=nullptr;i32 bonus_item=0;AnmLoaded* animation=nullptr;
    void reset()noexcept;
};
static_assert(sizeof(BulletManagerState)==0x6ba578&&offsetof(BulletManagerState,bullets)==0x1a880&&offsetof(BulletManagerState,next_slot)==0x6ba56c);
}
