#pragma once
#include "SpellStatus.hpp"
#include "PracticeConfig.hpp"
#include "Types.hpp"
#include "MotionState.hpp"
#include "PositionInterpolation.hpp"
#include "BulletShooter.hpp"
#include "Rng.hpp"
#include "EnemySpawn.hpp"
#include "ScreenGridObject.hpp"
#include <array>
#include <vector>
#include <string>
namespace th15 {
enum class EnemyUpdateRule:i32 { none=0,inherit_nearby_motion=1,cancel_unmarked=2,offset_lasers=3,scale_near_player=4 };
enum class EnemyDamageRule:i32 { none=0,one_shot_register=1,animation_regions=2 };
using EnemyMotion=MotionState;
struct EnemyAnimationHost {
    virtual ~EnemyAnimationHost()=default;
    virtual bool size(u32 handle,Vec2& out)const noexcept=0;
    virtual bool change_direction(u32& handle,i32 resource,i32 script,i32 layer)=0;
};
enum class EnemyMovementResult { active, outside, animation_unavailable };
struct EnemyInterrupt {i32 life=-1,time=-1;std::string script,timeout_script;};
struct EnemyWorldState;
struct EnemyState {
    MotionState motion{},previous{};EnemyMotion absolute{},relative{};
    Vec2 bound_center{},bound_size{};
    PositionInterpolation absolute_position{},relative_position{};
    ScalarInterpolation absolute_angle{},absolute_speed{},relative_angle{},relative_speed{};
    Vec2Interpolation absolute_shape{},relative_shape{},absolute_wave{},relative_wave{};
    std::array<i32,4> integer_registers{};std::array<float,4> float_registers{},temporary_registers{};
    u32 flags=0;i32 animation_frame=0;u32 id=0,parent_id=0;
    Vec2 sprite_size{};
    i32 selected_animation_resource=0,animation_resource=0,animation_script=0,animation_base=0,animation_layer=1,movement_direction=0;
    i32 animation_rotation_mode=0;
    EnemyShooters shooters{};float minimum_bullet_distance_squared=0;
    Vec2 hitbox{},hurtbox{};float rotation_angle=0,damage_multiplier=0;
    std::array<u32,16> animation_handles{};
    std::array<Vec3,16> animation_offsets{};
    std::array<i32,16> animation_parents{};
    i32 life=0,initial_life=0,phase_life=0,life_budget=0,life_threshold=0;
    i32 primary_drop=0;std::array<i32,16> item_drops{};Vec2 drop_radius{};
    Timer age_timer{},collision_timer{},invulnerability_timer{};
    Timer lifetime_timer{},phase_timer{},damage_timer{0,0,0,0,0};
    i32 score=0,boss_slot=-1,hit_sound=-1,death_sound=0,death_animation=0,death_animation_resource=0,damage_limit=0;
    float slowdown=0;u32 lifecycle_flags=0;
    i32 chapter=0,chapter_contribution=0;u32 life_flags=0;
    i32 pending_damage=0,cumulative_damage=0,damage_flash_frames=0;
    i32 inactive_animation=0,normal_animation=0;Vec3 last_hit_position{};
    std::array<EnemyInterrupt,8> interrupts;std::string death_script;
    std::array<i32,2> script_control{};
    EnemyUpdateRule update_rule=EnemyUpdateRule::none;EnemyDamageRule damage_rule=EnemyDamageRule::none;i32 callback_state=0;
    EnemyDistortionState distortion;ScreenGridObject distortion_mesh;
    void initialize(const EnemySpawnRequest&,u32 identifier,i32 current_chapter);
    void finish_spawn()noexcept;
    void apply_damage(i32 amount)noexcept;
    void recompose()noexcept;
    EnemyMovementResult update_movement(float rate,const Vec3& background_delta,EnemyAnimationHost* animations=nullptr);
    const std::string* check_interrupt(EnemyWorldState& world)noexcept;
};
struct EnemyWorldState {
    const PracticeState* practice=nullptr;
    float* frame_rate=nullptr;float current_rate(float fallback)const noexcept{return frame_rate?*frame_rate:fallback;}
    Vec3 player_position{};EnemyState* boss=nullptr;std::vector<EnemyState*> enemies;
    std::array<u32,3> boss_ids{};u32 manager_flags=0;
    Rng* random=nullptr;
    i32 rank=0,difficulty=0,mode=0,submode=0,enemy_count=0;
    std::array<i32,3> counters{};u32 total=0;i32 state=0,spell_state=0,player_state=0;
    std::array<i32,4> integer_registers{};std::array<float,8> float_registers{};
    i32 counter1=0,counter3=0;
    i32 current_chapter=0,chapter_total=0,chapter_defeated=0,requested_chapter=0;u32 scene_flags=0;
    u32* scene_flags_owner=nullptr;i32* chapter_request_owner=nullptr;
    void request_chapter(i32 chapter)noexcept{scene_flags|=0x8000;requested_chapter=chapter;if(scene_flags_owner)*scene_flags_owner|=0x8000;if(chapter_request_owner)*chapter_request_owner=chapter;}
    // Standalone enemies own a local status; a battle binds the actual spell.
    PlayerSpellStatus local_spell;PlayerSpellStatus* spell_status=nullptr;
    u32& spell_flags()noexcept{return (spell_status?*spell_status:local_spell).flags;}
    const u32& spell_flags()const noexcept{return (spell_status?*spell_status:local_spell).flags;}
    i32& spell_elapsed()noexcept{return (spell_status?*spell_status:local_spell).frame;}
    const i32& spell_elapsed()const noexcept{return (spell_status?*spell_status:local_spell).frame;}
    i32& spell_bonus()noexcept{return (spell_status?*spell_status:local_spell).bonus;}
    const i32& spell_bonus()const noexcept{return (spell_status?*spell_status:local_spell).bonus;}
    i32 bomb_action=0,enemy_control=0;
    u32 mode_flags=0;i32 boss_seconds=0,boss_hundredths=0,game_state=0,boss_damage_gate=0;
    i32 shot_damage=0,other_damage=0,player_damage_state=0,damage_disabled=0;
    i32 active_count()const noexcept{u32 count=0;for(const auto* enemy:enemies)if(!(enemy->flags&0x31)&&enemy->collision_timer.current<=0)count++;return signed_bits(count);}
    EnemyState* find(u32 identifier)const noexcept{if(identifier)for(auto* enemy:enemies)if(enemy->id==identifier)return enemy;return nullptr;}
    // The original entity-pointer lookup retains the last visited candidate
    // when a nonzero identifier is absent; the existence query is exact.
    EnemyState* lookup(u32 identifier)const noexcept{if(!identifier)return nullptr;EnemyState* last=nullptr;for(auto* enemy:enemies){last=enemy;if(enemy->id==identifier)break;}return last;}
    EnemyState* boss_at(i32 slot)const noexcept{return slot>=0&&slot<3?lookup(boss_ids[slot]):nullptr;}
    void refresh_boss()noexcept{boss=boss_at(0);}
};
}
