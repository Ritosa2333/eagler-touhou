#pragma once
#include "../../cpp/game/BulletExtras.hpp"
#include "../../cpp/game/BulletManager.hpp"
#include "../../cpp/game/BulletVisuals.hpp"
#include "../../cpp/game/AnmResource.hpp"
#include <memory>
#include <vector>
namespace bullet_test {
using namespace th09;
struct Event {u32 kind;i32 a,b;float x;};
struct Fixture:BulletFrameActions {
    Bullet bullet;Vec3 player;std::vector<Event> events;std::vector<BulletEmission> emissions;
    void play_sound(i32 sound,i32 pan)override{events.push_back({2,sound,pan,0});}
    void play_positioned_sound(i32 sound,float x)override{events.push_back({3,sound,0,x});}
    void change_type(Bullet& b,i32 type,i32 color)override{
        events.push_back({4,type,color,0});b.base_sprite=type*23+7;b.animation_height=float(8*(u32(color)%4+1));b.cull_width=16;b.cull_height=24;
    }
    bool emit_children(const BulletEmission& e)override{emissions.push_back(e);events.push_back({5,0,0,0});return true;}
    bool prepare(Bullet&,u32,i32,i32,u32)override{return false;}
    Bullet* first=nullptr;u32 test_frame=0;
    i32 index(const Bullet& b) const{return first?i32(&b-first):0;}
    bool advance_animation(Bullet& b,BulletAnimation animation)override{const i32 i=index(b);events.push_back({6,i,i32(animation),0});return (test_frame+u32(i)+u32(animation))%7==0;}
    i32 probe_attacks(Bullet& b)override{const i32 i=index(b);events.push_back({7,i,0,0});return (test_frame+u32(i))%3;}
    i32 collide_attacks(Bullet& b)override{const i32 i=index(b);events.push_back({8,i,0,0});return (test_frame+u32(i)*2)%3;}
    void collide_player(Bullet& b)override{events.push_back({9,index(b),0,0});}
};
struct ManagerFixture:Fixture {BulletManager manager;ManagerFixture(){first=manager.pool.data();}};
struct VisualFixture:ManagerFixture {
    AnmResource resource;Rng random;AnmExecutor executor{random};std::unique_ptr<BulletVisuals> visuals;
    bool load(const u8* data,u32 size){if(!resource.load(8,data,size))return false;visuals=std::make_unique<BulletVisuals>(resource.view(),executor);return visuals->initialize();}
    bool prepare(Bullet& b,u32 slot,i32 type,i32 color,u32 flags)override{return visuals&&visuals->prepare(b,slot,type,color,flags);}
    void change_type(Bullet& b,i32 type,i32 color)override{if(visuals)visuals->change_type(b,u32(index(b)),type,color);}
    bool advance_animation(Bullet& b,BulletAnimation kind)override{return visuals->advance(b,u32(index(b)),kind);}
};
struct Field {u32 original,offset,size;};
#define BF(original,field) {original,offsetof(Bullet,field),sizeof(Bullet::field)}
#define BN(original,section,type,field) {original,offsetof(Bullet,section)+offsetof(Bullet::type,field),sizeof(Bullet::type::field)}
inline const Field fields[]={
    BF(0xd4c,position),BF(0xd58,velocity),BF(0xd70,speed),BF(0xd74,acceleration),BF(0xd78,speed_delta),BF(0xd7c,direction),BF(0xd80,angular_velocity),BF(0xd84,turn_delta),
    BF(0x30,animation_height),BF(0xd48,base_sprite),BF(0xdbe,state),BF(0x10c0,sprite),BF(0x10c2,color),
    BF(0xdb4,active_extras),BF(0xdb8,available_extras),BF(0xdd4,extra_index),BF(0xdd0,transform_sound),BF(0xdd8,extras),BF(0xdb0,offscreen_grace),
    BN(0xf88,boost,Boost,time),BN(0xfa4,boost,Boost,counter),
    BN(0xfb4,linear,LinearAcceleration,time),BN(0xfc0,linear,LinearAcceleration,magnitude),BN(0xfc4,linear,LinearAcceleration,direction),BN(0xfc8,linear,LinearAcceleration,velocity),BN(0xfd4,linear,LinearAcceleration,duration),
    BN(0xfe0,polar,PolarAcceleration,time),BN(0xfec,polar,PolarAcceleration,magnitude),BN(0xff0,polar,PolarAcceleration,rotation),BN(0x1000,polar,PolarAcceleration,duration),
    BN(0x100c,turn,Turn,time),BN(0x1018,turn,Turn,speed),BN(0x101c,turn,Turn,angle),BN(0x102c,turn,Turn,duration),BN(0x1030,turn,Turn,repetitions),BN(0x1034,turn,Turn,count),
    BN(0x1044,bounce,Bounce,speed),BN(0x1058,bounce,Bounce,count),BN(0x105c,bounce,Bounce,limit),BF(0x1064,delay),BF(0x1090,wrap),
    BF(0xd88,lifetime),BF(0xd94,movement_time),BF(0xd38,hitbox),BF(0xdc0,outside_frames),BF(0xd46,draw_group),BF(0xdc3,near_attack),
    BF(0x10bc,collision_disabled),BF(0xdc4,cancel_pending),BF(0x224,has_body_script),
    BF(0xd44,template_flags),BF(0xd45,source_height),BF(0xdc2,spawn_active),BF(0x10bd,owner_flags),BF(0x10be,homing),BF(0xdcc,creation_reserved)
};
#undef BN
#undef BF
}
