#pragma once
#include "EclOwner.hpp"
#include "Movement.hpp"
#include "Interpolation.hpp"
#include "Rng.hpp"
#include "BulletEmitter.hpp"
namespace th11 {
struct Enemy;
struct EnemyPhaseState;
class ScreenDeformation;
enum class EnemyTick:u32;
enum class EnemyDamage:u32;
enum class EnemyCollision:u32;
struct EnemyLink {Enemy* value;EnemyLink* next;EnemyLink* previous;};
struct EnemyInterrupt {i32 health,time;const char* health_script;const char* timeout_script;};
struct EnemyDrop {i32 primary,counts[12];Vec2 spread;};
static_assert(sizeof(EnemyDrop)==0x3c);
// Layout recovered from the TH11 constructor and script-variable accessors.
// Unreconstructed effect fields remain explicitly reserved here.
struct EnemyState {
    Movement previous,current,absolute,relative;
    Vec2 hitbox,collision_box;
    u32 animations[10];
    i32 animation_file,bound_animation_file,animation_script,base_animation,direction;
    u32 animation_options; i32 facing;
    i32 integers[4];float floats[4],extra_floats[4];
    Timer lifetime;EnemyLink manager_link;
    Vec3Interpolator absolute_position,relative_position;
    Vec2Interpolator absolute_angle,relative_angle,absolute_radius,relative_radius;
    Vec2Interpolator absolute_ellipse,relative_ellipse;
    BulletEmitter emitters[8];
    Vec3 emitter_offset[8],emitter_origin[8];
    Vec2 visual_size,clamp_center,clamp_size;
    i32 score,health,max_health,interrupt_health,scaled_health,health_baseline;
    u32 health_flags;
    EnemyDrop drops;
    i32 death_sound,death_animation,death_animation_file,damage_flash,damage_reserved;
    Timer damage_immunity,collision_immunity;
    u32 flags; i32 alternate_animation,saved_animation,boss_slot;float exclusion_distance;
    EnemyInterrupt interrupts[8];
    Enemy* script_owner;ScreenDeformation* deformation;
    float deformation_target,deformation_radius;u32 deformation_color;
    float deformation_phase_x,deformation_phase_y;
    EnemyTick tick_callback;EnemyDamage damage_callback;EnemyCollision collision_callback;
    void initialize(Enemy*,const float* rate)noexcept;
    void combine_movement()noexcept;
    i32 update_movement(const float* rate,const Vec3& camera_delta)noexcept;
    i32 take_damage(i32)noexcept;
    const char* check_interrupts(EnemyPhaseState&)noexcept;
};
struct Enemy {EclOwner script;EnemyState state;};
static_assert(offsetof(Enemy,state)==0x103c);
static_assert(offsetof(EnemyState,integers)==0x124);
static_assert(offsetof(EnemyState,lifetime)==0x154);
static_assert(offsetof(EnemyState,absolute_position)==0x174);
static_assert(offsetof(EnemyState,absolute_angle)==0x20c);
static_assert(offsetof(EnemyState,emitters)==0x374);
static_assert(offsetof(EnemyState,emitter_offset)==0x1414);
static_assert(offsetof(EnemyState,emitter_origin)==0x1474);
static_assert(offsetof(EnemyState,visual_size)==0x14d4);
static_assert(offsetof(EnemyState,health)==0x14f0);
static_assert(offsetof(EnemyState,flags)==0x1580);
static_assert(offsetof(EnemyState,script_owner)==0x1614);
static_assert(sizeof(EnemyState)==0x163c);
static_assert(sizeof(Enemy)==0x2678);
struct EnemyEnvironment {
    Rng random;float rate=1;Vec3 player_position,camera_delta;
    i32 rank=0,difficulty=0,character=0,subtype=0,shared_integers[3]{},enemy_count=0;
    EnemyState* boss=nullptr;
    i32 (*animation_sprite)(u32 animation,void* user)=nullptr;void* animation_user=nullptr;
    // ECL and ANM/bullets consume one original script RNG, in execution order.
    // Standalone comparison fixtures use random; the game supplies shared_random.
    Rng* shared_random=nullptr;
    Rng& rng()noexcept{return shared_random?*shared_random:random;}
};
struct EnemyGlobals:EclGlobals {
    EnemyState& enemy;EnemyEnvironment& environment;
    EnemyGlobals(EnemyState& e,EnemyEnvironment& w):enemy(e),environment(w){}
    i32 integer(i32)override;
    double floating(i32)override;
    i32* integer_reference(i32)override;
    float* float_reference(i32)override;
};
float aim_angle(const Vec3& origin,const Vec3& player)noexcept;
float position_distance(const Vec3&,const Vec3&)noexcept;
struct EnemyPhaseState {
    const float* rate; i32* countdown_seconds; i32* countdown_hundredths;
    u32* spell_flags; i32* spell_elapsed; i32* spell_bonus;
    i32* shared_spell_state; i32 special_active,character,subtype;
};
}
