#pragma once
#include "BulletCancellation.hpp"
#include "AnmEnvironment.hpp"
namespace th10 {
struct BulletEmitter;
struct ProjectileSystemsEnvironment;
struct UpdateChainEntry;
enum class BulletFeature : u32 {SpawnAcceleration,VectorAcceleration,AngularAcceleration,Turn,TurnToAngle,TurnAimed,Reflect,Homing,HorizontalWrap,VerticalWrap};
struct BulletFrameEnvironment : BulletEffectEnvironment {
    const u32* controller_flags;
    const Vec3* player_position;
    virtual void run_commands(EnemyBullet& bullet)=0;
    virtual void update_feature(EnemyBullet& bullet,BulletFeature feature)=0;
    virtual i32 collide_player(const Vec3& point,const Vec2& size)=0;
    virtual void play_sound(i32 sound,float x)=0;
    virtual void play_turn_sound(i32 sound)=0;
    virtual void submit(AnmVm& animation)=0;
    virtual bool presentation(const EnemyBullet& bullet,Vec3& position,float& angle)=0;
};
struct EnemyBulletManager {
    u32 manager_fields[2];
    UpdateChainEntry* update_handle;
    UpdateChainEntry* draw_handle;
    EnemyBullet* cursor;
    EnemyBullet* draw_heads[6];
    EnemyBullet* draw_tails[6];
    Vec3 last_cancel_position,last_cancel_size;
    i32 active_count;
    EnemyBullet pool[2001];
    AnmFile* animation_file;
    void initialize(ProjectileSystemsEnvironment& environment) noexcept;
    i32 start(ProjectileSystemsEnvironment& environment);
    void clear(ProjectileSystemsEnvironment& environment);
    void shutdown(ProjectileSystemsEnvironment& environment);
    static EnemyBulletManager* create(ProjectileSystemsEnvironment& environment);
    i32 spawn(const BulletEmitter& emitter,i32 index,i32 layer,float aim,BulletBehaviorEnvironment& environment);
    i32 cancel_rectangle(i32 convert_items,const u32* small_colors,const u32* medium_colors,const u32* large_colors,BulletEffectEnvironment& environment);
    void cancel_all(bool respect_protection,BulletEffectEnvironment& environment);
    i32 count_in_circle(const Vec3& center,float radius) const noexcept;
    i32 update(BulletFrameEnvironment& environment);
    i32 draw_layer(i32 layer,BulletFrameEnvironment& environment);
    i32 draw(BulletFrameEnvironment& environment);
    i32 tick(BulletFrameEnvironment& environment);
    i32 render(BulletFrameEnvironment& environment);
};
static_assert(offsetof(EnemyBulletManager,pool)==0x60);
static_assert(offsetof(EnemyBulletManager,animation_file)==0x3e0b50);
bool bullet_outside_playfield(const Vec3& point,float width,float height,bool extended_top) noexcept;
}
