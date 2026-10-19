#pragma once
#include "enemy-manager-fixture.hpp"
namespace enemy_frame_test {
using namespace th09;
struct Fixture:enemy_manager_test::Fixture,EnemyFrameActions {
    EnemyPlayerState player;EnemyFrameSettings settings;AnmLoadedSprite sprite{};
    std::vector<std::array<u32,12>> frame_events;
    struct Shot {i32 total=0,primary=0,secondary=0;} shots[2];u32 shot_index=0;
    Fixture(){manager.player=&player;manager.frame_actions=this;sprite.widthPx=32;sprite.heightPx=24;}
    i32 index(const EclVm& e)const{return i32(&e-manager.enemies.data());}
    void event(u32 type,i32 arg,const Vec3& p){frame_events.push_back({type,u32(arg),bits(p.x),bits(p.y),bits(p.z)});}
    void attack_position(i32 side,const Vec3& p)override{event(0,side,p);}
    bool advance_animation(AnmVm& anm)override{
        for(u32 n=0;n<EnemyManager::capacity;++n)for(u32 layer=0;layer<3;++layer)if(&manager.enemies[n].animation.layers[layer]==&anm){frame_events.push_back({1,n,layer,u32(i32(anm.scriptIndex)),u32(anm.color1.d3dColor)});}
        anm.color1.d3dColor^=0x010203;return anm.scriptIndex==7;
    }
    void body_collision(const Vec3& p,const Vec3& extent)override{frame_events.push_back({2,bits(p.x),bits(p.y),bits(p.z),bits(extent.x),bits(extent.y),bits(extent.z)});}
    i32 shot_damage(const Vec3& p,const Vec3& extent,i32& primary,i32& token,i32& secondary)override{
        frame_events.push_back({3,bits(p.x),bits(p.y),bits(p.z),bits(extent.x),bits(extent.y),bits(extent.z),u32(token)});
        const auto& s=shots[shot_index++%2];primary=s.primary;secondary=s.secondary;++token;return s.total;
    }
    void add_score(i32 amount)override{frame_events.push_back({4,u32(amount)});}
    void effect(i32 type,const Vec3& p)override{event(5,type,p);}
    void play_positioned_sound(i32 id,float x)override{frame_events.push_back({6,u32(id),bits(x)});}
    void sound_pan(i32 id,i32 pan)override{frame_events.push_back({7,u32(id),u32(pan)});}
    void boss_position(i32 id,const Vec3& p)override{event(8,id,p);}
    void boss_state(i32 id,u32 state)override{frame_events.push_back({9,u32(id),state});}
    void release_attached_effects(EclVm& e)override{frame_events.push_back({10,u32(index(e)),u32(e.status.attached_effect_count)});e.status.attached_effect_count=0;}
    void update_attached_effects(EclVm& e)override{frame_events.push_back({11,u32(index(e)),u32(e.status.attached_effect_count)});}
    bool enemy_death(EclVm& e,i32 source)override{frame_events.push_back({12,u32(index(e)),u32(source)});return true;}
    void finish_match()override{frame_events.push_back({13});}
    bool start_animation(EclVm& vm,u32 slot,bool alt,i32 script)override{
        frame_events.push_back({14,u32(index(vm)),slot,u32(alt),u32(script)});return enemy_manager_test::Fixture::start_animation(vm,slot,alt,script);
    }
    bool emit(const BulletEmission& shot)override{frame_events.push_back({15});return enemy_manager_test::Fixture::emit(shot);}
    bool emit_capture_bullets(const BulletEmission& shot)override{return emit(shot);}
    void clear_events(){frame_events.clear();events.clear();animation_events.clear();shot_index=0;}
};
struct Field {u32 base,original,offset,size;};
#define MF(original,field) {0,original,offsetof(EnemyManager,field),sizeof(EnemyManager::field)}
#define PF(original,field) {1,original,offsetof(EnemyPlayerState,field),sizeof(EnemyPlayerState::field)}
inline const Field fields[]={
    MF(0x2ac3ac,alive),MF(0x2ac3b0,normal_alive),MF(0x2ac3b4,attack_alive),MF(0x2ac3b8,spirit_alive),MF(0x2ac44c,capture_lifetime),
    MF(0x2ac3c8,focus_time),MF(0x2ac404,frame_time),MF(0x2ac3d4,pattern_index),PF(0,state),PF(0x364,focus),PF(0x30364,nearest_position),PF(0x303c8,protection)
};
#undef MF
#undef PF
inline const ecl_test::ExtraField extras[]={
    {0x2d8c,offsetof(EclVm,velocity),12},{0x33e8,offsetof(EclVm,trail)+offsetof(EnemyTrail,history),sizeof(EnemyTrail::history)},
    {0x1f8,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+offsetof(AnmVm,color1),12},
    {0x49c,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+sizeof(AnmVm)+offsetof(AnmVm,color1),12},
    {0x740,offsetof(EclVm,animation)+offsetof(EnemyAnimation,layers)+sizeof(AnmVm)*2+offsetof(AnmVm,color1),12}
};
}
