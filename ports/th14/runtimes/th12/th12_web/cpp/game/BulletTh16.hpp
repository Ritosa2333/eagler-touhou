#pragma once
#include "BulletState.hpp"
#include "EnemyTh16Emitter.hpp"
#include "BulletLaunch.hpp"
namespace th11 {
struct Th16BulletAppearance {i32 script,sprites[64];float hitbox;i32 group,effect,secondary;};
static_assert(sizeof(Th16BulletAppearance)==0x114);
extern const Th16BulletAppearance th16_bullet_appearances[44];
extern const i32 th16_bullet_cancel_scripts[8];
bool bullet_th16_bind_metadata(BulletState&,const Th16BulletEmitter&,const BulletLaunch&,const float*)noexcept;
struct Th16BulletStepServices {
    virtual ~Th16BulletStepServices()=default;
    virtual i32 collision(BulletState&)=0;
    virtual i32 animation(AnmVm&)=0;
    virtual bool sound(i32){return false;}
    virtual bool positional_sound(i32,float){return false;}
    virtual Vec3 player_position()const{return {};}
    virtual Vec2 bounce_bounds()const{return {};}
    virtual bool fire(const Th16BulletEmitter&){return false;}
    virtual bool cancel(BulletState&){return false;}
    virtual bool change_appearance(BulletState&,i32,u32){return false;}
};
struct Th16BulletAppearanceServices {
    virtual ~Th16BulletAppearanceServices()=default;
    virtual bool reset_primary()=0;
    virtual bool bind_primary(i32)=0;
    virtual bool reset_secondary()=0;
    virtual bool interrupt_primary()=0;
};
bool bullet_th16_change_appearance(BulletState&,i32,u32,Th16BulletAppearanceServices&)noexcept;
void bullet_th16_clock(Timer&,float)noexcept;
i32 bullet_th16_step(BulletState&,float,Th16BulletStepServices&)noexcept;
i32 bullet_th16_start_transforms(BulletState&,const float*,i32* sound=nullptr,bool* positional=nullptr,const Vec3* player=nullptr,Th16BulletStepServices* services=nullptr)noexcept;
i32 bullet_th16_motion_one(BulletState&,float)noexcept;
i32 bullet_th16_motion_eight(BulletState&,float)noexcept;
i32 bullet_th16_motion_four(BulletState&,float)noexcept;
i32 bullet_th16_motion_target_speed(BulletState&,float)noexcept;
i32 bullet_th16_motion_sixteen(BulletState&,float,Th16BulletStepServices&)noexcept;
struct Th16CancelServices {
    virtual ~Th16CancelServices()=default;
    virtual i32 animation(AnmVm&)=0;
    virtual bool effect(const Vec3&,i32)=0;
    virtual bool sound(i32,float)=0;
    virtual bool reward(Vec3,u32){return false;}
};
bool bullet_th16_cancel_reward(Vec3,u32,u32&)noexcept;
bool bullet_th16_cancel(BulletState&,const float*,Th16CancelServices&,u32 rewards=0);
bool bullet_th16_circle_match(const BulletState&,const Vec3&,float,bool)noexcept;
struct Th16BulletCollisionServices {
    virtual ~Th16BulletCollisionServices()=default;
    virtual i32 probe(const BulletState&)=0;
    virtual bool effect(const Vec3&,i32,const Vec3& velocity)=0;
    virtual bool graze(const Vec3&)=0;
};
i32 bullet_th16_collision(BulletState&,float,Th16BulletCollisionServices&);
void bullet_th16_hit_effect_setup(AnmVm&,const Vec3&,const float*)noexcept;
struct Th16FireServices {
    virtual ~Th16FireServices()=default;
    virtual i32 spawn(const Th16BulletEmitter&,i32,i32,float)=0;
    virtual bool sound(i32,float)=0;
};
bool bullet_th16_fire(const Th16BulletEmitter&,const Vec3&,Th16FireServices&);
}
