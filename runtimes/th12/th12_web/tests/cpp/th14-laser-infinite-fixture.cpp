#include "../../cpp/game/LaserState.hpp"
#include "../../cpp/game/EclContext.hpp"
#include "../../cpp/game/LaserManager.hpp"
#include <cstdlib>
using namespace th11;
#define API(name) extern "C" __attribute__((export_name(#name)))
namespace {
struct World:LaserWorld {
    std::vector<u32> deliveries;
    std::vector<std::vector<u32>> collisions;i32 hit=0;
    bool sound(i32,float,bool)override{return true;}
    bool boss_position(Vec3& p)override{p={7.5f,120.25f,-3.5f};return true;}
    bool cancel_effect(Vec3 p,i32 script)override{deliveries.insert(deliveries.end(),{u32(script),float_bits(p.x),float_bits(p.y),float_bits(p.z)});return true;}
    bool th14_curve_reward(Vec3 p,u32 rewards)override{if(rewards)deliveries.insert(deliveries.end(),{0x10000+rewards,float_bits(p.x),float_bits(p.y),float_bits(p.z)});return true;}
    i32 test(Vec3 p,float a,float w,float l,u32 probe){collisions.push_back({0,float_bits(p.x),float_bits(p.y),float_bits(p.z),float_bits(a),float_bits(w),float_bits(l),probe});return hit;}
    i32 collision(Vec3 p,float a,float w,float l)override{return test(p,a,w,l,0);}
    i32 warning_collision(Vec3 p,float a,float w,float l)override{return test(p,a,w,l,1);}
    bool th14_curve_graze(Vec3 p)override{collisions.push_back({1,float_bits(p.x),float_bits(p.y),float_bits(p.z)});return true;}
    i32 th14_infinite_cut(Th14LaserInfinite&,Vec3 p,Vec2 s,float angle,u32 rewards,bool skip)override{collisions.push_back({2,float_bits(p.x),float_bits(p.y),float_bits(p.z),float_bits(s.x),float_bits(s.y),float_bits(angle),rewards,u32(skip)});return 0;}
} world;
Th14LaserInfinite laser;
std::vector<std::array<u8,0x354>> fragments;
std::vector<std::array<u32,223>> factory_results;
std::array<u32,3> registry_result;
}
API(allocate) void* allocate(u32 n){return std::malloc(n);}
API(release) void release(void* p){std::free(p);}
API(initialize) void initialize(const u8* request,float rate){laser=Th14LaserInfinite{};world.rate=rate;laser.base.initialize(&world.rate);laser_th14_infinite_initialize_state(laser,request,world);}
API(tick) i32 tick(){const auto result=laser_th14_infinite_step(laser,world);if(!result)laser.base.lifetime.tick();return result;}
API(value) u32 value(u32 n){const auto& b=laser.base;const u32 values[]={u32(b.state),u32(b.type),float_bits(b.position.x),float_bits(b.position.y),float_bits(b.position.z),float_bits(b.angle),float_bits(b.length),float_bits(b.width),float_bits(b.speed),u32(b.id),u32(b.protection),u32(b.transform_index),b.active_transforms,u32(b.graze_timer.previous),u32(b.graze_timer.current),float_bits(b.graze_timer.fractional),u32(b.lifetime.previous),u32(b.lifetime.current),float_bits(b.lifetime.fractional)};return values[n];}
API(cancel) i32 cancel(i32 sprite,u32 length,u32 angle,u32 mode){laser=Th14LaserInfinite{};auto& b=laser.base;b.sprite=i16(sprite);b.color=7;b.angle=float_bits(angle);b.length=float_bits(length);b.position={-180.25f,7.5f,3.25f};b.state=2;b.protection=mode==2?3:0;world.deliveries.clear();return laser_th14_infinite_cancel(laser,mode!=0,mode==2,world);}
API(delivery_count) u32 delivery_count(){return world.deliveries.size()/4;}
API(delivery_word) u32 delivery_word(u32 n){return world.deliveries.at(n);}
API(circle) i32 circle(i32 sprite,u32 length,u32 angle,u32 radius,u32 mode){
    laser=Th14LaserInfinite{};auto& b=laser.base;b.sprite=i16(sprite);b.color=7;b.angle=float_bits(angle);b.length=float_bits(length);b.width=24;b.position={-180.25f,7.5f,3.25f};b.protection=mode==2?3:0;
    const float maximum=800;const i32 color=7;std::memcpy(laser.request+0x20,&maximum,4);std::memcpy(laser.request+0x50,&sprite,4);std::memcpy(laser.request+0x54,&color,4);world.deliveries.clear();
    return laser_th14_infinite_cut_circle(laser,{-140.25f,7.5f,0},float_bits(radius),mode==1?1u:0u,mode==2,fragments,world);
}
API(fragment_count) u32 fragment_count(){return fragments.size();}
API(rectangle) i32 rectangle(i32 sprite,u32 length,u32 angle,u32 width,u32 height,u32 rotation,u32 mode,u32 flags,u32 placement){
    laser=Th14LaserInfinite{};auto& b=laser.base;b.sprite=i16(sprite);b.color=7;b.angle=float_bits(angle);b.length=float_bits(length);b.width=24;b.position={-180.25f,7.5f,3.25f};b.protection=mode==2?3:0;
    const float maximum=800;const i32 color=7;std::memcpy(laser.request+0x20,&maximum,4);std::memcpy(laser.request+0x50,&sprite,4);std::memcpy(laser.request+0x54,&color,4);std::memcpy(laser.request+0x58,&flags,4);world.deliveries.clear();
    const Vec3 center=placement==1?Vec3{-180.25f,7.5f,0}:placement==2?Vec3{900,-500,0}:Vec3{-140.25f,7.5f,0};
    return laser_th14_infinite_cut_rectangle(laser,center,{float_bits(width),float_bits(height)},float_bits(rotation),mode==1?1u:0u,mode==2,fragments,world);
}
API(fragment_word) u32 fragment_word(u32 n,u32 j){u32 v;std::memcpy(&v,fragments.at(n).data()+4*j,4);return v;}
API(registry_rectangle) i32 registry_rectangle(i32 sprite,u32 length,u32 angle,u32 width,u32 height,u32 rotation,u32 mode,u32 flags,u32 placement){
    rectangle(sprite,length,angle,width,height,rotation,mode,flags,placement);laser.base.length=float_bits(length);world.deliveries.clear();factory_results.clear();
    AnmResource resource;AnmEnvironment animations;world.rate=1;
    resource.scripts.resize(300);for(auto& s:resource.scripts)s.bytes={2,0,8,0,0,0,0,0,255,255,0,0,0,0,0,0};
    LaserManager manager(resource,animations,world,6);auto* parent=new Th14LaserInfinite(laser);parent->base.type=1;parent->base.state=2;parent->base.rectangle_enabled=1;manager.append(parent->base);
    const Vec3 center=placement==1?Vec3{-180.25f,7.5f,0}:placement==2?Vec3{900,-500,0}:Vec3{-140.25f,7.5f,0};
    const i32 result=manager.cancel_rectangle_th14(center,{float_bits(width),float_bits(height)},float_bits(rotation),mode==1?1u:0u,false);
    registry_result={float_bits(parent->base.length),manager.active_count,u32(manager.last_error)};
    for(auto* node=parent->base.next;node;node=node->next){const auto& child=*reinterpret_cast<Th14LaserLine*>(node);std::array<u32,223> values{};std::memcpy(values.data(),child.request,0x354);
        const auto& b=child.base;const u32 state[]={u32(b.type),u32(b.state),float_bits(b.position.x),float_bits(b.position.y),float_bits(b.position.z),float_bits(b.angle),float_bits(b.length),float_bits(b.width),float_bits(b.speed),u32(b.id)};std::memcpy(values.data()+213,state,sizeof state);factory_results.push_back(values);
    }return result;
}
API(registry_value) u32 registry_value(u32 n){return registry_result.at(n);}
API(factory_count) u32 factory_count(){return factory_results.size();}
API(factory_word) u32 factory_word(u32 n,u32 k){return factory_results.at(n).at(k);}
API(collision) i32 collision(u32 length,u32 width,u32 angle,float rate,u32 age,i32 hit,u32 probe,u32 state){laser=Th14LaserInfinite{};world.rate=rate;world.player={17.25f,120.5f,3.25f};world.hit=hit;world.collisions.clear();auto& b=laser.base;b.state=i32(state);b.position={-22.25f,40.5f,3.25f};b.length=float_bits(length);b.width=float_bits(width);b.angle=float_bits(angle);b.graze_timer.set(i32(age),&world.rate);return laser_th14_infinite_collision(laser,world,probe);}
API(collision_timer) u32 collision_timer(u32 n){const auto& t=laser.base.graze_timer;const u32 values[]={u32(t.previous),u32(t.current),float_bits(t.fractional)};return values[n];}
API(collision_count) u32 collision_count(){return world.collisions.size();}
API(collision_size) u32 collision_size(u32 n){return world.collisions.at(n).size();}
API(collision_word) u32 collision_word(u32 n,u32 k){return world.collisions.at(n).at(k);}
