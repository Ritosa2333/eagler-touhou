#pragma once
#include "MotionState.hpp"
#include "PositionInterpolation.hpp"
#include "Timer.hpp"
#include "Rng.hpp"
#include "EnemySpawn.hpp"
#include "SpellStatus.hpp"
#include "BulletShooter.hpp"
#include "ScreenGridObject.hpp"
#include <array>
#include <vector>
#include <string>
namespace th17 {
struct EnemyAnimationHost {
    virtual ~EnemyAnimationHost()=default;
    virtual bool size(u32 handle,Vec2& out)const noexcept=0;
    virtual bool change_direction(u32& handle,i32 resource,i32 script,i32 layer)=0;
};
enum class EnemyMovementResult { active, outside, animation_unavailable };
struct EnemyInterrupt {i32 life=-1,time=-1;std::string script,timeout_script;};
struct EnemyWorldState;
enum class EnemyUpdateRule:i32 {none=0,redirect_bullets=1};
struct EnemyState {
    MotionState motion{},previous{},absolute{},relative{};Vec2 bound_center{},bound_size{};
    PositionInterpolation absolute_position{},relative_position{};
    ScalarInterpolation absolute_angle{},absolute_speed{},relative_angle{},relative_speed{};
    Vec2Interpolation absolute_shape{},relative_shape{},absolute_wave{},relative_wave{};
    std::array<i32,4> integer_registers{};std::array<float,4> float_registers{},temporary_registers{};
    u32 flags=0,id=0,parent_id=0;i32 animation_frame=0;Timer age_timer{},collision_timer{};
    EnemyShooters shooters;float minimum_bullet_distance_squared=0;
    void recompose()noexcept;
    Vec2 sprite_size{};std::array<u32,16> animation_handles{};
    i32 selected_animation_resource=0,animation_resource=0,animation_script=0,animation_base=0,movement_direction=0,animation_rotation_mode=0,animation_layer=1;
    std::array<Vec3,16> animation_offsets{};std::array<i32,16> animation_parents{};
    Vec2 hitbox{},hurtbox{};float rotation_angle=0,damage_multiplier=0;
    i32 life=0,initial_life=0,phase_life=0,life_budget=0,life_threshold=0,primary_drop=0;
    std::array<i32,34> item_drops{};Vec2 drop_radius{};
    Timer lifetime_timer{},invulnerability_timer{},phase_timer{0,0,0,0,0};
    i32 score=0,boss_slot=-1,hit_sound=-1,death_sound=0,death_animation=0,death_animation_resource=0;
    i32 inactive_animation=0,normal_animation=0;std::array<EnemyInterrupt,8> interrupts;std::string death_script;
    i32 pending_damage=0,cumulative_damage=0,damage_flash_frames=0;Timer damage_timer{0,0,0,0,0};Vec3 last_hit_position{};
    float slowdown=0;u32 lifecycle_flags=0,life_flags=0;i32 chapter=0,chapter_contribution=0;
    EnemyUpdateRule update_rule=EnemyUpdateRule::none;i32 update_rule_state=0;
    EnemyDistortionState distortion;ScreenGridObject distortion_mesh;
    void initialize(const EnemySpawnRequest&,u32 identifier,i32 stage);
    void finish_spawn()noexcept;
    void apply_damage(i32 amount)noexcept;
    const std::string* check_timeout(EnemyWorldState&)noexcept;
    const std::string* check_life_interrupt(EnemyWorldState&)noexcept;
    EnemyMovementResult update_movement(float rate,const Vec3& background_delta,EnemyAnimationHost* animations=nullptr);
};
struct EnemyWorldState {
    Vec3 player_position{};EnemyState* boss=nullptr;std::vector<EnemyState*> enemies;Rng* random=nullptr;
    std::array<u32,3> boss_ids{};u32 manager_flags=0;
    i32 rank=0,difficulty=0,mode=0,submode=0,enemy_count=0;
    std::array<i32,3> counters{};std::array<i32*,3> shared_counters{};
    i32& counter(u32 index)noexcept{return shared_counters[index]?*shared_counters[index]:counters[index];}
    const i32& counter(u32 index)const noexcept{return shared_counters[index]?*shared_counters[index]:counters[index];}
    u32 total=0;i32 state=0,spell_state=0,player_state=0;
    std::array<i32,4> integer_registers{};std::array<float,8> float_registers{};
    i32 spell_id=-1,stage_number=0,counter3=0,roaring_kind=0;u32 roaring_flags=0;
    u32* shared_roaring_flags=nullptr;u32& roar_flags()noexcept{return shared_roaring_flags?*shared_roaring_flags:roaring_flags;}
    std::array<i32,20> graze_history{};i32 spirit_count=0,script_value_101=0;
    float* frame_rate=nullptr;float current_rate(float fallback)const noexcept{return frame_rate?*frame_rate:fallback;}
    i32 current_chapter=0,chapter_total=0,chapter_defeated=0,requested_chapter=0,enemy_control=0;u32 scene_flags=0,mode_flags=0;
    PlayerSpellStatus local_spell;PlayerSpellStatus* spell_status=nullptr;
    u32& spell_flags()noexcept{return (spell_status?*spell_status:local_spell).flags;}
    i32& spell_elapsed()noexcept{return (spell_status?*spell_status:local_spell).frame;}
    i32& spell_bonus()noexcept{return (spell_status?*spell_status:local_spell).bonus;}
    i32 player_damage_state=0,damage_disabled=0,shot_damage=0,other_damage=0;
    i32 boss_seconds=0,boss_hundredths=0,game_state=0,boss_damage_gate=0;float player_damage_scale=1;
    u32 game_frame_flags=0,player_behavior_flags=0;float last_death_angle=0;
    i32 active_count()const noexcept{u32 count=0;for(const auto* e:enemies)if(!(e->flags&0x31)&&e->collision_timer.current<=0)count++;return signed_bits(count);}
    EnemyState* find(u32 id)const noexcept{if(id)for(auto* e:enemies)if(e->id==id)return e;return nullptr;}
    EnemyState* lookup(u32 id)const noexcept{if(!id)return nullptr;EnemyState* last=nullptr;for(auto* e:enemies){last=e;if(e->id==id)break;}return last;}
    EnemyState* boss_at(i32 slot)const noexcept{return slot>=0&&slot<3?lookup(boss_ids[slot]):nullptr;}
    void refresh_boss()noexcept{boss=boss_at(0);}
    i32 recent_grazes()const noexcept{u32 a=0,b=0;for(u32 i=0;i<10;i++){a+=u32(graze_history[i*2]);b+=u32(graze_history[i*2+1]);}return signed_bits(a+b);}
};
}
