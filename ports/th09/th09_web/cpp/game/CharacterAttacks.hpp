#pragma once
#include "AttackQueue.hpp"
#include "AttackTransfer.hpp"
#include "EnemyTimeline.hpp"
#include "BulletManager.hpp"
#include "SpriteGeometry.hpp"
namespace th09 {
enum class AttackAnimationResource {shared_effects,character_shots};
struct CharacterAttackServices:AttackServices {
    Rng& random;FrameTiming timing;PlayfieldGeometry geometry[2];Vec3 players[2];
    i32 rank=0,difficulty=0,attack_levels[2][2]{};
    explicit CharacterAttackServices(Rng& r):random(r){}
    virtual bool start_animation(AttackActor&,u32 slot,i32 side,AttackAnimationResource,i32 script)=0;
    virtual i32 probe_cancellation(i32 side,const Vec3&,const Vec3& extent)=0;
    virtual void collide_player(i32 side,const Vec3&,float radius)=0;
    virtual void collide_player_box(i32 side,const Vec3&,const Vec3& extent)=0;
    virtual void spawn_enemy(i32 side,const EnemySpawn&,const EclLocals&)=0;
    virtual void effect(i32 side,i32 type,const Vec3&)=0;
    virtual Bullet* emit_bullets(i32 side,bool secondary_pool,const BulletEmission&)=0;
    virtual BulletManager& bullet_manager(i32 side)=0;
    virtual void scale_player_effect(i32 side,float amount)=0;
    virtual void begin_attack_layer(i32 layer)=0;
    virtual void draw_animation(AnmVm&)=0;
    virtual void draw_color_fan(const AnmVm&,const AttackColorVertex*,u32 count)=0;
    virtual void draw_texture_fan(const AnmVm&,const AttackTextureVertex*,u32 count)=0;
    virtual void draw_additive_lines(const AttackColorVertex*,u32 count)=0;
    virtual AttackActor* spawn_attack(i32 kind,i32 side,const Vec3&,const Vec3* extra=nullptr,const AttackActor* parent=nullptr)=0;
};
struct TravelAttackState:CopyableAttackState<TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(phase,velocity,origin,destination,start_tangent,end_tangent,target,speed,heading,turn,emissions);return h.value;}

    i32 phase=0;Vec3 velocity,origin,destination,start_tangent,end_tangent,target;
    float speed=0,heading=0,turn=0;i32 emissions=0;
};
struct CirnoAttackState:CopyableAttackState<CirnoAttackState,TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(TravelAttackState::state_checksum());h.Add(acceleration);return h.value;}
Vec3 acceleration;};
struct TewiAttackState:CopyableAttackState<TewiAttackState,TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(TravelAttackState::state_checksum());h.Add(horizontal_acceleration);return h.value;}
float horizontal_acceleration=0;};
struct AyaAttackState:CopyableAttackState<AyaAttackState,TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(TravelAttackState::state_checksum());h.Add(variant);return h.value;}
u32 variant=0;};
struct SakuyaAttackState:CopyableAttackState<SakuyaAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(phase,velocity,history,parent);return h.value;}
i32 phase=0;Vec3 velocity;std::array<Vec3,32> history;const AttackActor* parent=nullptr;};
struct MedicineAttackState:CopyableAttackState<MedicineAttackState,TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(TravelAttackState::state_checksum());h.Add(movement_heading,movement_speed,has_bounced,history,directions);return h.value;}

    float movement_heading=0,movement_speed=0;i32 has_bounced=0;
    std::array<Vec3,16> history;std::array<float,16> directions{};
};
struct MystiaAttackState:CopyableAttackState<MystiaAttackState,TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(TravelAttackState::state_checksum());h.Add(alternate_sprite,flight_angle,volley_angle,angular_velocity);return h.value;}
i32 alternate_sprite=0;float flight_angle=0,volley_angle=0,angular_velocity=0;};
struct ReisenAttackState:CopyableAttackState<ReisenAttackState,TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(TravelAttackState::state_checksum());h.Add(draw_flag,radius,fan,rings,jitter);return h.value;}

    i32 draw_flag=0;float radius=4;
    std::array<AttackColorVertex,33> fan{};
    std::array<std::array<AttackColorVertex,33>,4> rings{};
    std::array<std::array<float,33>,4> jitter{};
};
struct FieldAttackState:CopyableAttackState<FieldAttackState,TravelAttackState> {
    u32 state_checksum()const{StateChecksum h;h.Add(TravelAttackState::state_checksum());h.Add(vertices,world,radii,radial_velocity,uv_velocity,draw_flag,uv_angle);return h.value;}

    std::array<AttackTextureVertex,33> vertices{};
    std::array<Vec3,33> world{};
    std::array<float,33> radii{},radial_velocity{};
    Vec2 uv_velocity;u32 draw_flag=0;float uv_angle=0;
};
// Each descriptor refers to a named game routine, with common travel math
// shared by the characters that use the same original transition.
std::array<AttackBehavior,27> character_attack_behaviors();
void draw_character_attacks(AttackQueue&,CharacterAttackServices&,bool cross_field);
}
