#pragma once
#include "BulletEmission.hpp"
#include "Timer.hpp"
namespace th09 {
struct Bullet {
    Vec3 position,velocity;
    float speed=0,acceleration=0,speed_delta=0,direction=0,angular_velocity=0,turn_delta=0;
    float animation_height=0,cull_width=0,cull_height=0;i32 base_sprite=0;
    i16 state=0,sprite=0,color=0;
    u32 active_extras=0,available_extras=0; i32 extra_index=0,transform_sound=-1;
    BulletExtra extras[18];
    i32 offscreen_grace=0;
    struct Boost {Timer time{0,0,0};i32 counter=0;} boost;
    struct LinearAcceleration {Timer time{0,0,0};float magnitude=0,direction=0;Vec3 velocity;i32 duration=0;} linear;
    struct PolarAcceleration {Timer time{0,0,0};float magnitude=0,rotation=0;i32 duration=0;} polar;
    struct Turn {Timer time{0,0,0};float speed=0,angle=0;i32 duration=0,repetitions=0,count=0;} turn;
    struct Bounce {float speed=0;i32 count=0,limit=0;} bounce;
    Timer delay{0,0,0},wrap{0,0,0};
    Timer lifetime{0,0,0},movement_time{0,0,0};
    Vec3 hitbox;
    i16 outside_frames=0;u8 draw_group=0,near_attack=0,collision_disabled=0,cancel_pending=0;
    u8 template_flags=0,source_height=0,spawn_active=0,owner_flags=0,homing=0;
    i32 creation_reserved=0;
    u32 has_body_script=0;i32 draw_next=-1;
    void clear_extras() noexcept {
        extra_index=0;available_extras=0;active_extras=0;for(auto& e:extras)e.flags=0;
    }
    BulletExtra& set_extra(u32 index,u32 flag,i32 mode=0) noexcept {
        auto& e=extras[index];e.flags=flag;e.mode=mode;available_extras|=flag;extra_index=0;return e;
    }
    bool intersects_field() const noexcept {
        return !(position.x+cull_width*.5f < -144.f || position.x-cull_width*.5f > 144.f || position.y+cull_height*.5f < 0.f || position.y-cull_height*.5f > 448.f);
    }
    void remove() noexcept {state=0;lifetime.reset();movement_time.reset();}
};
}
