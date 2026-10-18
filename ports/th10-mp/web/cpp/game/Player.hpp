#pragma once
#include "AnmFile.hpp"
#include "Movement.hpp"
#include "AnmRegistry.hpp"
#include "GameEconomy.hpp"
#include "UpdateChain.hpp"
namespace th10 {
struct PlayerCollisionEnvironment;
struct PlayerDamageEnvironment;
struct PlayerLifecycleEnvironment;
struct PlayerMovementEnvironment;
struct PlayerShootingEnvironment;
struct PlayerFrameEnvironment;
struct PlayerOptionsEnvironment;
struct PlayerProfileEnvironment;
struct PlayerDrawEnvironment;
struct Enemy;
struct PracticeState;
struct PlayerShotDefinition {
    std::int8_t fire_interval,fire_offset;
    std::int16_t damage;
    Vec2 offset,hitbox;
    float angle,speed;
    std::int8_t option,type;
    std::int16_t animation,hit_animation,sound;
    u32 on_initialize,on_update,on_draw,on_hit;
};
static_assert(sizeof(PlayerShotDefinition)==0x34);
struct PlayerShot {
    Timer timer;
    u32 timer_flags;
    Movement motion;
    i32 state;
    u32 animation,secondary_animation;
    Enemy* homing_target;
    i32 collided,first_collision;
    PlayerShotDefinition* definition;
};
static_assert(sizeof(PlayerShot)==0x5c);
static_assert(offsetof(PlayerShot,definition)==0x58);
struct PlayerOption {
    i32 active;
    u8 reserved_004[0x30];
    struct FixedPoint {i32 x,y;};
    FixedPoint target,position,offset,focused_offset;
    u8 reserved_054[0x14];
    u32 animations[2];
    u8 reserved_070[0x14];
    i32 previous_focus,index;
    i32 snap_next;
    u32 on_update,on_draw;
};
static_assert(sizeof(PlayerOption)==0x98);
struct DamageArea {
    float radius,radial_speed,angle;
    float angular_velocity;
    Vec2 size;
    Movement motion;
    Timer timer;
    u32 timer_flags;
    i32 damage,total_damage,damage_limit,interval;
    u32 flags;
};
static_assert(sizeof(DamageArea)==0x6c);
struct PlayerShotPattern {PlayerShotDefinition* definitions;u32 reserved;};
struct PlayerCallbackTables {const u32* initialize;const u32* update;const u32* draw;const u32* hit;};
struct PlayerProfile {
    u16 reserved_000,pattern_count;
    float collision_size,item_attraction_speed,pickup_size;
    float fast_speed,slow_speed,fast_diagonal,slow_diagonal;
    Vec3 fast_options[10],slow_options[10];
    PlayerShotPattern patterns[10];
    void resolve(const PlayerCallbackTables& callbacks);
};
struct PlayerProfileEnvironment {
    PlayerCallbackTables callbacks;
    virtual PlayerProfile* load(const char* name)=0;
};
static_assert(offsetof(PlayerProfile,patterns)==0x110);
struct PlayerBounds {Vec3 minimum,maximum;};
// Verified prefix of the original player object. Fields past the item regions
// and the still-unidentified ranges are retained until their systems are recovered.
struct Player {
    u32 flags,manager_state;
    UpdateChainEntry* update_entry;
    UpdateChainEntry* draw_entry;
    AnmFile* animation_file;
    AnmVm animation;
    Vec3 position;
    PlayerOption::FixedPoint fixed_position;
    i32 fast_speed,slow_speed,fast_diagonal,slow_diagonal;
    u8 reserved_3e4[0xc];
    PlayerOption::FixedPoint velocity;
    u8 reserved_3f8[0xc];
    PlayerBounds collision_bounds;
    Vec3 hitbox_half_size;
    Vec3 fast_pickup_size,slow_pickup_size,death_position;
    PlayerOption::FixedPoint input_velocity;
    i32 direction;
    i32 state;
    PlayerProfile* profile;
    Timer fire_timer;
    u32 fire_timer_flags;
    Timer state_timer;
    u32 state_timer_flags;
    Timer focus_timer;
    u32 focus_timer_flags;
    PlayerShot shots[128];
    u32 focus_animation;
    PlayerOption options[4];
    i32 option_count;
    Enemy* target;
    u8 target_seen,reserved_3509[3];
    DamageArea damage_areas[32];
    u8 reserved_428c[0x68];
    i32 active_lasers[5];
    i32 option_follow_speed;
    Timer invulnerability;
    u32 invulnerability_flags;
    u32 reserved_4320;
    PlayerBounds pickup_bounds,slow_pickup_bounds,fast_pickup_bounds;
    PlayerOption::FixedPoint position_history[33];
    i32 focused;

    Extended angle_from(const Vec3& point) const noexcept;
    Extended angle_towards(const Vec3& point) const noexcept;
    i32 collide_rectangle(const Vec3& center,const Vec2& size,PlayerCollisionEnvironment& environment);
    i32 collide_laser(const Vec3& origin,float angle,float width,float length,PlayerCollisionEnvironment& environment);
    DamageArea* emit_circle(const Vec3& position,float radius,float growth,i32 frames,i32 damage,const float* default_rate);
    i32 damage(const Vec3& center,const Vec2& size,PlayerDamageEnvironment& environment,u32* hit_count=nullptr);
    void hit(PlayerLifecycleEnvironment& environment);
    void die(PlayerLifecycleEnvironment& environment);
    void set_position(PlayerOption::FixedPoint position) noexcept;
    void update_position_history() noexcept;
    i32 move(PlayerMovementEnvironment& environment);
    i32 spawn_shot(PlayerShotDefinition& definition,i32 frame,PlayerShootingEnvironment& environment);
    i32 fire_pattern(i32 frame,PlayerShootingEnvironment& environment);
    i32 update_firing(PlayerShootingEnvironment& environment);
    i32 update_shots(PlayerShootingEnvironment& environment);
    i32 update(PlayerFrameEnvironment& environment);
    void reconfigure_options(PlayerOptionsEnvironment& environment);
    void update_trailing_option(PlayerOption& option);
    void update_anchored_option(PlayerOption& option,AnmRegistry& registry);
    void initialize_homing_shot(PlayerShot& shot);
    void update_homing_shot(PlayerShot& shot);
    void update_option_laser(PlayerShot& shot);
    void forget_enemy(Enemy* enemy) noexcept;
    i32 load_profile(const char* name,PlayerProfileEnvironment& environment);
    i32 draw(PlayerDrawEnvironment& environment);
};
static_assert(offsetof(Player,position)==0x3c0);
static_assert(offsetof(Player,state)==0x458);
static_assert(offsetof(Player,shots)==0x49c);
static_assert(offsetof(Player,options)==0x32a0);
static_assert(offsetof(Player,damage_areas)==0x350c);
static_assert(offsetof(Player,invulnerability)==0x430c);
static_assert(offsetof(Player,pickup_bounds)==0x4324);
static_assert(offsetof(Player,focused)==0x4474);
struct PlayerCollisionEnvironment {
    bool dialogue_active;
#ifdef TH_ENABLE_THPRAC
    const PracticeState* practice=nullptr;
#endif
    virtual void hit(Player& player)=0;
};
struct PlayerDamageEnvironment {
    AnmRegistry* registry;
    GameEconomy* economy;
    const float* default_rate;
    virtual i32 shot_hit(Player& player,PlayerShot& shot,const Vec3& position)=0;
    virtual u32 create_animation(AnmFile& file,i32 script,u32 tag)=0;
    virtual i32 bomb_damage(const Vec3& position)=0;
};
}
