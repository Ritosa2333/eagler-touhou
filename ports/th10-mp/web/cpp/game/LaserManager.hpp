#pragma once
#include "BulletCancellation.hpp"
namespace th10 {
struct LaserBehaviorEnvironment;
struct ProjectileSystemsEnvironment;
struct UpdateChainEntry;
struct StraightLaserParameters {
    Vec3 position;
    float angle,target_length,initial_length,maximum_distance,width,speed;
    std::int16_t sprite_type,color;
    u32 flags;
    ProjectileCommand commands[18];
};
struct TimedLaserParameters {
    Vec3 position,velocity;
    float angle,angular_velocity,target_length,initial_length,width,growth_speed;
    i32 warning_frames,grow_frames,active_frames,shrink_frames;
    std::int16_t sprite_type,color;
    u32 flags;
    ProjectileCommand commands[18];
};
struct StraightLaser {
    EnemyLaser base;
    StraightLaserParameters parameters;
    AnmVm beam,tip;
    void initialize(u32 methods,const float* default_rate) noexcept;
    i32 start(const StraightLaserParameters& values,LaserBehaviorEnvironment& environment);
    i32 update(LaserBehaviorEnvironment& environment);
    i32 draw(LaserBehaviorEnvironment& environment);
    void process_commands(LaserBehaviorEnvironment& environment);
    void accelerate(LaserBehaviorEnvironment& environment);
    void turn(LaserBehaviorEnvironment& environment);
    void reflect(LaserBehaviorEnvironment& environment);
    i32 cancel_all(i32 convert,LaserBehaviorEnvironment& environment);
    i32 cancel_rectangle(const Vec3& center,const Vec3& size,i32 convert,LaserBehaviorEnvironment& environment);
    i32 cancel_circle(const Vec3& center,float radius,i32 convert,LaserBehaviorEnvironment& environment);
};
struct TimedLaser {
    EnemyLaser base;
    TimedLaserParameters parameters;
    AnmVm beam,tip;
    void initialize(u32 methods,const float* default_rate) noexcept;
    i32 start(const TimedLaserParameters& values,LaserBehaviorEnvironment& environment);
    i32 update(LaserBehaviorEnvironment& environment);
    i32 draw(LaserBehaviorEnvironment& environment);
    i32 cancel_all(i32 convert,LaserBehaviorEnvironment& environment);
    i32 cancel_rectangle(const Vec3& center,const Vec3& size,i32 convert,LaserBehaviorEnvironment& environment);
    i32 cancel_circle(const Vec3& center,float radius,i32 convert,LaserBehaviorEnvironment& environment);
};
static_assert(sizeof(StraightLaser)==0xd58);
static_assert(sizeof(TimedLaser)==0xd74);
struct LaserEnvironment {
    const float* default_rate;
    u32 straight_methods,timed_methods;
    virtual void* allocate(u32 size)=0;
    virtual void release(void* memory)=0;
    virtual void initialize_laser(EnemyLaser& laser,const void* parameters)=0;
    virtual i32 update_laser(EnemyLaser& laser)=0;
    virtual void draw_laser(EnemyLaser& laser)=0;
    virtual void destroy_laser(EnemyLaser& laser)=0;
    virtual void cancel_laser(EnemyLaser& laser,i32 convert_items)=0;
    virtual i32 cancel_rectangle(EnemyLaser& laser,const Vec3& center,const Vec3& size,i32 convert_items)=0;
    virtual i32 cancel_circle(EnemyLaser& laser,const Vec3& center,float radius,i32 convert_items)=0;
};
struct LaserManager {
    u32 manager_fields[2];
    UpdateChainEntry* update_handle;
    UpdateChainEntry* draw_handle;
    EnemyLaser sentinel;
    EnemyLaser* tail;
    i32 count;
    u32 last_id;
    Vec3 last_cancel_position,last_cancel_size;
    AnmFile* animation_file;
    void initialize(ProjectileSystemsEnvironment& environment) noexcept;
    i32 start(ProjectileSystemsEnvironment& environment);
    void shutdown(ProjectileSystemsEnvironment& environment);
    static LaserManager* create(ProjectileSystemsEnvironment& environment);
    EnemyLaser* find(u32 id) const noexcept;
    u32 create(i32 kind,const void* parameters,LaserEnvironment& environment);
    i32 update(LaserEnvironment& environment);
    void clear(LaserEnvironment& environment);
    i32 draw(LaserEnvironment& environment);
    i32 schedule_deletion() noexcept;
    i32 cancel_all(i32 convert_items,LaserEnvironment& environment);
    i32 cancel_rectangle(const Vec3& center,const Vec3& size,i32 convert_items,LaserEnvironment& environment);
    i32 cancel_circle(const Vec3& center,float radius,i32 convert_items,LaserEnvironment& environment);
    i32 intersections(const Vec3& center,float margin) const noexcept;
    i32 tick(u32 controller_flags,float& global_rate,LaserEnvironment& environment);
    i32 render(u32 controller_flags,LaserEnvironment& environment);
};
static_assert(offsetof(LaserManager,tail)==0x434);
static_assert(offsetof(LaserManager,last_cancel_position)==0x440);
static_assert(sizeof(LaserManager)==0x45c);
}
