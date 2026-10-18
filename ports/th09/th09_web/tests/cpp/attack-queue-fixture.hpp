#pragma once
#include "../../cpp/game/CharacterAttacks.hpp"
namespace attack_queue_test {
using namespace th09;
struct Fixture:CharacterAttackServices {
    Rng rng;std::array<AttackBehavior,27> behaviors=character_attack_behaviors();AttackQueue queue{behaviors,*this};
    std::vector<std::array<u32,16>> events;std::vector<BulletEmission> bullet_events;i32 collision_result=0;
    std::array<u8,3912> state_image{};u32 state_size=0;
    BulletManager bullet_managers[2];Bullet emitted;Vec2 effect_scales[2]{{1,1},{1,1}};
    std::vector<u8> draw_bytes;
    Fixture():CharacterAttackServices(rng){queue.limits[0]=queue.limits[1]=256;}
    static u32 bits(float f){u32 u;std::memcpy(&u,&f,4);return u;}
    i32 index(const AttackActor& a)const{return i32(&a-queue.actors.data());}
    void play_sound(i32 sound,i32 pan)override{events.push_back({0,u32(sound),u32(pan)});}
    void advance_animation(AnmVm& a)override{for(u32 i=0;i<AttackQueue::capacity;++i)for(u32 j=0;j<queue.actors[i].animations.size();++j)if(&a==&queue.actors[i].animations[j]){
        events.push_back({1,i,j,u32(i32(a.scriptIndex))});const auto& actor=queue.actors[i];if((actor.behavior==&behaviors[19]||actor.behavior==&behaviors[21])&&actor.time.current>330)a.visible=0;
    }}
    bool start_animation(AttackActor& a,u32 slot,i32 side,AttackAnimationResource resource,i32 script)override{
        events.push_back({2,u32(index(a)),slot,u32(side),u32(resource),u32(script)});std::memset(&a.animations[slot],0,sizeof(AnmVm));a.animations[slot].scriptIndex=i16(script);a.animations[slot].visible=1;return true;
    }
    i32 probe_cancellation(i32 side,const Vec3& p,const Vec3& box)override{events.push_back({3,u32(side),bits(p.x),bits(p.y),bits(p.z),bits(box.x),bits(box.y),bits(box.z)});return collision_result;}
    void collide_player(i32 side,const Vec3& p,float radius)override{events.push_back({4,u32(side),bits(p.x),bits(p.y),bits(p.z),bits(radius)});}
    void collide_player_box(i32 side,const Vec3& p,const Vec3& box)override{events.push_back({8,u32(side),bits(p.x),bits(p.y),bits(p.z),bits(box.x),bits(box.y),bits(box.z)});}
    void spawn_enemy(i32 side,const EnemySpawn& r,const EclLocals& locals)override{
        events.push_back({5,u32(side),u32(r.script),bits(r.position.x),bits(r.position.y),bits(r.position.z),u32(r.life),u32(r.item),u32(r.score),bits(locals.floats[0])});
    }
    void effect(i32 side,i32 type,const Vec3& p)override{events.push_back({6,u32(side),u32(type),bits(p.x),bits(p.y),bits(p.z)});}
    Bullet* emit_bullets(i32 side,bool secondary,const BulletEmission& b)override{events.push_back({7,u32(side),u32(secondary)});bullet_events.push_back(b);return &emitted;}
    BulletManager& bullet_manager(i32 side)override{return bullet_managers[side];}
    void scale_player_effect(i32 side,float amount)override{effect_scales[side].x*=amount;effect_scales[side].y*=amount;}
    void begin_attack_layer(i32 layer)override{events.push_back({12,u32(layer)});}
    void draw_animation(AnmVm& a)override{for(u32 i=0;i<AttackQueue::capacity;++i)for(u32 j=0;j<queue.actors[i].animations.size();++j)if(&a==&queue.actors[i].animations[j])events.push_back({13,i,j});}
    void draw(u32 kind,const void* vertices,u32 count,u32 stride){events.push_back({kind,count});const auto p=static_cast<const u8*>(vertices);draw_bytes.insert(draw_bytes.end(),p,p+count*stride);}
    void draw_color_fan(const AnmVm&,const AttackColorVertex* p,u32 count)override{draw(9,p,count,sizeof(*p));}
    void draw_texture_fan(const AnmVm&,const AttackTextureVertex* p,u32 count)override{draw(10,p,count,sizeof(*p));}
    void draw_additive_lines(const AttackColorVertex* p,u32 count)override{draw(11,p,count,sizeof(*p));}
    AttackActor* spawn_attack(i32 kind,i32 side,const Vec3& p,const Vec3* extra,const AttackActor* parent)override{return queue.create(kind,side,p,extra,parent);}
    void clear_events(){events.clear();bullet_events.clear();draw_bytes.clear();}
    const void* snapshot(u32 index){
        const auto& a=queue.actors[index];if(!a.state)return nullptr;state_image.fill(0);
        if(a.behavior==&behaviors[3]||a.behavior==&behaviors[4]){const auto& data=static_cast<SakuyaAttackState&>(*a.state);std::memcpy(state_image.data(),&data.phase,400);state_size=400;return state_image.data();}
        const auto& t=static_cast<TravelAttackState&>(*a.state);std::memcpy(state_image.data(),&t.phase,16);u32 shift=0;
        if(a.behavior==&behaviors[5]||a.behavior==&behaviors[10]){const auto& data=static_cast<CirnoAttackState&>(*a.state);std::memcpy(state_image.data()+16,&data.acceleration,12);shift=12;}
        else if(a.behavior==&behaviors[16]||a.behavior==&behaviors[23]){const auto& data=static_cast<TewiAttackState&>(*a.state);std::memcpy(state_image.data()+16,&data.horizontal_acceleration,4);shift=4;}
        std::memcpy(state_image.data()+16+shift,&t.origin,60);state_size=76+shift;
        if(a.behavior==&behaviors[17]){const auto& data=static_cast<AyaAttackState&>(*a.state);std::memcpy(state_image.data()+76,&data.variant,4);state_size=80;}
        if(a.behavior==&behaviors[20]){std::memcpy(state_image.data()+76,&t.speed,16);state_size=92;}
        if(a.behavior==&behaviors[18]||a.behavior==&behaviors[24]){const auto& data=static_cast<MedicineAttackState&>(*a.state);std::memcpy(state_image.data()+76,&data.movement_heading,268);state_size=344;}
        if((a.behavior>=&behaviors[6]&&a.behavior<=&behaviors[9])||a.behavior==&behaviors[11]||a.behavior==&behaviors[12]){
            const auto& data=static_cast<MystiaAttackState&>(*a.state);std::memcpy(state_image.data()+4,&data.alternate_sprite,16);std::memcpy(state_image.data()+20,&t.velocity,12);std::memcpy(state_image.data()+32,&t.origin,60);state_size=92;
        }
        if(a.behavior==&behaviors[14]||a.behavior==&behaviors[22]){const auto& data=static_cast<ReisenAttackState&>(*a.state);
            std::memcpy(state_image.data()+76,&data.draw_flag,8);std::memcpy(state_image.data()+84,data.fan.data(),660);std::memcpy(state_image.data()+744,data.rings.data(),2640);std::memcpy(state_image.data()+3384,data.jitter.data(),528);state_size=3912;
        }
        if(a.behavior==&behaviors[19]||a.behavior==&behaviors[21]){const auto& data=static_cast<FieldAttackState&>(*a.state);
            std::memcpy(state_image.data()+76,data.vertices.data(),924);std::memcpy(state_image.data()+1000,data.world.data(),396);std::memcpy(state_image.data()+1400,data.radii.data(),132);std::memcpy(state_image.data()+1532,data.radial_velocity.data(),128);
            std::memcpy(state_image.data()+1660,&data.uv_velocity,8);std::memcpy(state_image.data()+1668,&data.draw_flag,4);state_size=1672;
            if(a.behavior==&behaviors[21]){std::memcpy(state_image.data()+1672,&data.uv_angle,4);state_size=1676;}
        }
        return state_image.data();
    }
};
struct Field {u32 original,offset,size;};
#define AF(original,field) {original,offsetof(AttackActor,field),sizeof(AttackActor::field)}
inline const Field fields[]={AF(0,layer),AF(4,destination_side),AF(8,source_side),AF(12,active),AF(16,time),AF(32,position),AF(60,color)};
#undef AF
}
