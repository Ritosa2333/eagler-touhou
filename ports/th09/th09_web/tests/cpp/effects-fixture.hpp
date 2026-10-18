#pragma once
#include "../../cpp/game/EffectManager.hpp"
#include <vector>
namespace effects_test {
using namespace th09;
struct Fixture:EffectServices {
    Rng rng;EffectManager manager;EclVm enemy;AnmLoadedSprite sprite{};i32 sequence=0;
    std::vector<std::array<u32,8>> events;std::vector<BulletEmission> bullets;std::vector<u8> vertices;
    Fixture(i32 side,u32 size):EffectServices(rng),manager(*this,side,size,8){sprite.uvStart={.125f,.25f};sprite.uvEnd={.625f,.875f};}
    static u32 bits(float f){u32 n;std::memcpy(&n,&f,4);return n;}
    i32 index(const AnmVm& vm){for(u32 i=0;i<manager.actors.size();++i)if(manager.actors[i].animation.get()==&vm)return i32(i);return -1;}
    void start_animation(AnmVm& vm,i32 side,bool character,i32 script)override{events.push_back({0,u32(index(vm)),u32(side),u32(character),u32(script)});std::memset(&vm,0,sizeof(vm));vm.scriptIndex=i16(script);vm.visible=1;vm.color1.d3dColor=-1;vm.loadedSprite=&sprite;vm.intVar0=24;vm.scale={8,8};}
    bool advance_animation(AnmVm& vm)override{events.push_back({1,u32(index(vm))});return sequence>185;}
    void draw_animation(AnmVm& vm)override{events.push_back({2,u32(index(vm))});}
    void play_sound(i32 id,i32 pan)override{events.push_back({3,u32(id),u32(pan)});}
    void emit_bullets(i32 side,const BulletEmission& b)override{events.push_back({4,u32(side)});bullets.push_back(b);}
    EclVm* spawn_spirit(i32 side,i32 script,const Vec3& p)override{events.push_back({5,u32(side),u32(script),bits(p.x),bits(p.y),bits(p.z)});enemy.values.position=p;enemy.values.resolved_position={p.x+1.25f,p.y-2.5f,p.z};enemy.values.acceleration=.003f;enemy.values.flags=0x2020;enemy.values.direction=-.5f;return &enemy;}
    void fire_shots(i32 side,u32 set,i32 time)override{events.push_back({6,u32(side),set,u32(time)});}
    void begin_layer(i32 side,i32 layer)override{events.push_back({7,u32(side),u32(layer)});}
    template<class T>void draw(u32 mode,const T* p,u32 count){events.push_back({mode,count});const u8* bytes=reinterpret_cast<const u8*>(p);vertices.insert(vertices.end(),bytes,bytes+sizeof(T)*count);}
    void draw_color_fan(AnmVm&,const AttackColorVertex* p,u32 n)override{draw(8,p,n);}
    void draw_texture_strip(AnmVm&,const AttackTextureVertex* p,u32 n)override{draw(9,p,n);}
    void draw_additive_lines(const AttackColorVertex* p,u32 n)override{draw(10,p,n);}
    void draw_color_strip(AnmVm&,const AttackColorVertex* p,u32 n)override{draw(11,p,n);}
    void clear(){events.clear();bullets.clear();vertices.clear();}
};
struct Field {u32 original,offset,size;};
inline const Field fields[]={{12,offsetof(EffectActor,position),176},{0xc4,offsetof(EffectActor,active),8},{0xd0,offsetof(EffectActor,hidden),1}};
}
