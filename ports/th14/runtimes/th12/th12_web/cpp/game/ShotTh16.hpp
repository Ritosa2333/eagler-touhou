#pragma once
#include "ShtResource.hpp"
#include "Movement.hpp"
#include "Timer.hpp"
#include "Rng.hpp"
#if PORT_TITLE == 16
namespace th11 {
struct ShotPlayer;
struct Th16ShotTarget {Vec3 position{};u32 flags=0;};
struct Th16ShotTargets {
    virtual ~Th16ShotTargets()=default;
    virtual bool present()const=0;
    virtual u32 nearest(Vec3,float)=0;
    virtual bool valid(u32)=0;
    virtual Th16ShotTarget target(u32)=0;
};
struct Th16ShotSpawn {
    u32 flags=0,packed=0;
    Timer timer;
    Movement movement;
    i32 state=0,damage=0;
    Vec2 hitbox{};
    u32 animation=0,damage_source=0,target_id=0;
    i32 hit_this_frame=0,hit_last_frame=0;
};
struct Th16DamageSource {std::array<u32,37> words{};};
struct Th16DamageSources {
    std::array<Th16DamageSource,257> records{};
    i32 cursor=0;
    u32 rectangle(Vec3,float angle,Vec2,i32 duration,i32 damage);
    u32 circle(Vec3,float radius,float growth,i32 duration,i32 damage);
    bool update(const float* rate);
};
// Retail444e10 through4451ef, before animation and damage-source allocation.
bool shot_th16_spawn_prepare(Th16ShotSpawn&,const ShotSpec&,u32 packed,const ShotPlayer&,const Vec3&,Rng&,std::array<u32,16>& lasers,const float* rate);
u32 shot_th16_spawn_damage(const Th16ShotSpawn&,u32 shot_index,Th16DamageSources&);
struct Th16ShotDelivery {
    virtual ~Th16ShotDelivery()=default;
    virtual u32 create(bool season,i32 script)=0;
    virtual bool orient(u32,float raw_angle)=0;
    virtual bool position(u32,Vec3)=0;
    virtual void erase(u32)=0;
    // Zero succeeds; positive values are the retail callback's rejection.
    // Negative values report a callback which has not been implemented.
    virtual i32 callback(Th16ShotSpawn&,const ShotSpec&,i32)=0;
    virtual bool sound(i32,float)=0;
};
// Return0 on delivery, -1 for a retail callback rejection, -2 for a host failure.
i32 shot_th16_spawn(Th16ShotSpawn&,const ShotSpec&,u32 packed,u32 index,i32 frame,const ShotPlayer&,Vec3,Rng&,std::array<u32,16>&,Th16DamageSources&,const float*,Th16ShotDelivery&);
bool shot_th16_spawn_callback(Th16ShotSpawn&,const ShotSpec&,Th16DamageSources&);
void shot_th16_homing(Th16ShotSpawn&,Th16ShotTargets&);
struct Th16ShotUpdateDelivery {
    virtual ~Th16ShotUpdateDelivery()=default;
    virtual i32 update_callback(Th16ShotSpawn&,const ShotSpec&)=0;
    virtual bool find(u32,u32& flags2)=0;
    virtual bool quad(u32,std::array<Vec3,4>&)=0;
    virtual void erase(u32)=0;
    virtual void position(u32,Vec3)=0;
    virtual void orient(u32,float)=0;
    Vec2 play_origin{};
};
// One occupied slot of complete4456d0. Nonzero callback results skip the slot.
bool shot_th16_update(Th16ShotSpawn&,const ShotSpec&,Th16DamageSources&,const float*,Th16ShotUpdateDelivery&);
struct Th16DamageQuery {Vec3 position{};const Vec2* size=nullptr;float radius=0,angle=0;bool advanced=false,probe=false;u32 stamp=0;i32 cap=0;};
struct Th16DamageDelivery {
    virtual ~Th16DamageDelivery()=default;
    virtual bool bomb(const Th16DamageQuery&,i32&)=0;
    virtual bool overlaps(const Th16DamageSource&,const Th16DamageQuery&)=0;
    virtual bool hit(u32 id,Th16DamageSource&,const Th16DamageQuery&,i32&)=0;
    virtual bool score(i32)=0;
};
bool shot_th16_collect(Th16DamageSources&,const Th16DamageQuery&,Th16DamageDelivery&,i32&,bool& bomb,Vec3* last_hit);
bool shot_th16_circle_rectangle(Vec3,Vec2,float,Vec3,float)noexcept;
bool shot_th16_rectangles(Vec3,Vec2,float,Vec3,Vec2,float)noexcept;
bool shot_th16_overlaps(const Th16DamageSource&,const Th16DamageQuery&)noexcept;
struct AnmVm;
bool shot_th16_default_hit(Th16ShotSpawn&,Th16DamageSources&,AnmVm&,i32&);
struct Th16BeamEffects {i32 pan=0;std::array<i32,2> interrupts{};u32 count=0;bool pan_active=false,stop=false;};
bool shot_th16_beam_update(Th16ShotSpawn&,const ShotSpec&,const ShotPlayer&,Th16DamageSource&,AnmVm*,bool firing,u32& cache,Th16BeamEffects&);
struct Th16BeamHit {Vec3 position{},travel{};float angle=0;i32 damage=0;bool effect=false,interrupt=false;};
bool shot_th16_beam_intersection(Vec3,Vec3,Vec2,float,float,Vec2&)noexcept;
bool shot_th16_beam_hit(Th16ShotSpawn&,Th16DamageSource&,const Th16DamageQuery&,Th16BeamHit&);
void shot_th16_beam_effect_setup(AnmVm&,const Th16BeamHit&,Vec3,const float*);
float shot_th16_freeze_angle(const Th16ShotSpawn&,Rng&);
void shot_th16_freeze_finish(AnmVm&,Rng&,const float*);
}
#endif
