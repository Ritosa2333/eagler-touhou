#pragma once
#include "CharacterAttacks.hpp"
#include <memory>
namespace th09 {
enum class EffectKind:u8 {
    hit,bullet_transfer_left,bullet_transfer_right,spirit_transfer_left,spirit_transfer_right,spark,
    ring,focus_aura,ring_alternate,flash,reimu_field,slash_left,slash_right,
    protection_one,protection_two,protection_three,marisa_field,sakuya_field,youmu_field,player_death,
    hit_alternate,spark_alternate,burst,small_burst,mystia_field,tewi_field,reisen_field,
    lyrica_field,aya_field,medicine_field,cirno_field,komachi_field,yuuka_field,eiki_field,
    reisen_burst,lyrica_shots,merlin_field,lunasa_field
};
struct EffectActor;
struct EffectServices {
    Rng& random;FrameTiming timing;PlayfieldGeometry geometry[3];i32 coordinate_side=0,rank=0;
    Vec3 players[2];float player_angles[2]{},player_steps[2]{};i32 characters[2]{},spirit_counts[2]{};bool rewards_blocked=false;
    explicit EffectServices(Rng& r):random(r){}
    virtual ~EffectServices()=default;
    virtual void start_animation(AnmVm&,i32 side,bool character_resource,i32 script)=0;
    virtual bool advance_animation(AnmVm&)=0;
    virtual void draw_animation(AnmVm&)=0;
    virtual void play_sound(i32 id,i32 pan)=0;
    virtual void emit_bullets(i32 side,const BulletEmission&)=0;
    virtual EclVm* spawn_spirit(i32 side,i32 script,const Vec3&)=0;
    virtual void fire_shots(i32 side,u32 set,i32 time)=0;
    virtual void begin_layer(i32 side,i32 layer)=0;
    virtual void draw_color_fan(AnmVm&,const AttackColorVertex*,u32)=0;
    virtual void draw_color_strip(AnmVm&,const AttackColorVertex*,u32)=0;
    virtual void draw_texture_strip(AnmVm&,const AttackTextureVertex*,u32)=0;
    virtual void draw_additive_lines(const AttackColorVertex*,u32)=0;
};
struct EffectBurst {
    i32 phase=0;float radius=0;
    std::array<AttackColorVertex,33> fan{};
    std::array<std::array<AttackColorVertex,33>,4> rings{};
    std::array<std::array<float,33>,4> jitter{};
};
struct EffectActor {
    Vec3 position,arguments,velocity,acceleration,start_tangent,anchor,origin,destination,auxiliary;
    float auxiliary_scalar=0,radius=0,angle=0,spread=0,thickness=0;i32 segments=0,slot=0;
    float distortion=0,rotation=0,cycle=0;TransferParameters transfer;Timer time{0,0,0};u32 reserved=0;
    u8 active=0;EffectKind kind=EffectKind::hit;u8 flags0=0,flags1=0,layer=0,dirty=0,upper_layer=0,flags2=0,hidden=0;
    i32 side=0;std::unique_ptr<AnmVm> animation;std::vector<AttackColorVertex> colors;std::vector<AttackTextureVertex> texture;
    std::unique_ptr<EffectBurst> burst;
};
class EffectManager {
    EffectServices& services;
    bool initialize(EffectActor&,EffectKind,const Vec3&,const Vec3&,u32 color);
public:
    PoolCheckpoint* checkpoint=nullptr;
    void before_write(EffectActor& a){if(checkpoint)checkpoint->BeforeEffect(*this,&a-actors.data());}
    i32 side=0,count=0,frame=0;u32 cursor=0,capacity=0,reserved_slots=0;
    std::vector<EffectActor> actors;std::array<std::vector<EffectActor*>,3> draw_lists;
    EffectManager(EffectServices&,i32 side,u32 capacity,u32 reserved_slots);
    EffectActor* create(EffectKind,const Vec3&,const Vec3& extra={},i32 number=1,u32 color=0xffffffff);
    EffectActor* slotted(EffectKind,const Vec3&,u32 slot,u32 color=0xffffffff);
    EffectActor* get_slot(u32 slot){if(slot>=reserved_slots)return nullptr;auto& a=actors[capacity+slot];before_write(a);return &a;}
    void update(u32 game_flags,u32 first_field_flags);
    void draw(i32 layer);
    void clear();
};
bool initialize_effect(EffectActor&,EffectServices&);
bool update_effect(EffectActor&,EffectServices&);
bool draw_effect(EffectActor&,EffectServices&);
}
