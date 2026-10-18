#pragma once
#include "EclProgram.hpp"
#include "Interpolation.hpp"
#include "Movement.hpp"
#include "BulletEmitter.hpp"
namespace th10 {
struct EnemyEnvironment;
struct EnemyPhaseState;
struct EnemyFrameEnvironment;
struct ItemDropEnvironment;
struct EnemyDropEnvironment;
struct EnemyProjectileEnvironment;
struct EnemyAnimationEnvironment;
struct Enemy;
struct EnemyManager;
struct EnemyManagerEnvironment;
struct EnemySpawnEnvironment;
struct EnemySceneEnvironment;
struct EnemyCommandEnvironment;
enum class EnemyMotionResult { Continue, AlreadyUpdated, Despawn };
struct EnemyInterrupt {i32 health,time;const char* health_subroutine;const char* time_subroutine;};
struct EnemyDrops {
    i32 kind; i32 counts[12]; Vec2 spread;
    void scatter(const Vec3& position,ItemDropEnvironment& environment);
    void release(const Vec3& position,ItemDropEnvironment& environment);
};
// Original enemy data follows the common script owner. Unnamed storage is
// retained until its gameplay role is recovered; it is not executable code.
struct EnemyState {
    Movement previous,current,absolute,relative;
    Vec2 hitbox;
    Vec2 collision_box;
    u32 animations[10];
    i32 animation_file,bound_animation_file,animation_script,base_animation,direction;
    i32 integer_variables[4];
    float float_variables[4];
    Timer lifetime;
    u32 lifetime_flags;
    ListNode<Enemy> manager_node;
    Vec3Interpolator absolute_position,relative_position;
    Vec2Interpolator absolute_angle,relative_angle,absolute_radius,relative_radius;
    BulletEmitter emitters[8];
    Vec3 emitter_offsets[8];
    Vec2 visual_size;
    Vec2 clamp_center,clamp_size;
    i32 score,health,maximum_health,health_to_interrupt;
    EnemyDrops drops;
    i32 death_sound,death_animation,death_animation_file;
    i32 flash_frames;
    u32 reserved_1418;
    Timer damage_immunity;
    u32 damage_immunity_flags;
    Timer collision_immunity;
    u32 collision_immunity_flags;
    u32 flags;
    i32 alternate_animation,saved_animation,boss_slot;
    float shot_exclusion_distance_squared;
    EnemyInterrupt interrupts[8];
    EclOwner* script_owner;
    void construct() noexcept;
    EnemyMotionResult advance_movement(EnemyEnvironment& environment) noexcept;
    bool movement_command(EclContext& context,EclGlobals& globals,EnemyEnvironment& environment);
    bool state_command(EclContext& context,EclGlobals& globals,EnemyFrameEnvironment& environment,i32& result);
    bool projectile_command(EclContext& context,EclGlobals& globals,EnemyProjectileEnvironment& environment);
    bool animation_command(EclContext& context,EclGlobals& globals,EnemyAnimationEnvironment& environment);
    bool spawn_command(EclContext& context,EclGlobals& globals,EnemySpawnEnvironment& environment);
    bool scene_command(EclContext& context,EclGlobals& globals,EnemySceneEnvironment& environment);
    i32 execute_command(EclContext& context,EclGlobals& globals,EnemyCommandEnvironment& environment);
    const char* check_interrupts(EnemyPhaseState& world) noexcept;
    i32 update(EnemyFrameEnvironment& environment);
    i32 destroy(EnemyDropEnvironment& environment);
};
struct Enemy {
    EclOwner script;
    EnemyState state;
    void initialize(EclProgram& program,const char* subroutine,const float* rate,void* type_table) noexcept;
    void shutdown(EnemyManager& manager,EnemyManagerEnvironment& environment);
};
static_assert(sizeof(Enemy)==0x2518);
static_assert(offsetof(Enemy,state)==0x103c);
static_assert(offsetof(EnemyState,integer_variables)==0xfc);
static_assert(offsetof(EnemyState,lifetime)==0x11c);
static_assert(offsetof(EnemyState,absolute_position)==0x13c);
static_assert(offsetof(EnemyState,absolute_angle)==0x1d4);
static_assert(offsetof(EnemyState,emitters)==0x2c4);
static_assert(offsetof(EnemyState,emitter_offsets)==0x1344);
static_assert(offsetof(EnemyState,visual_size)==0x13a4);
static_assert(offsetof(EnemyState,health)==0x13c0);
static_assert(offsetof(EnemyState,flags)==0x1444);
static_assert(offsetof(EnemyState,interrupts)==0x1458);
static_assert(offsetof(EnemyState,script_owner)==0x14d8);
}
