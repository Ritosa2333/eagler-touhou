#include "../../cpp/game/LaserManager.hpp"
#include "../../cpp/game/EclContext.hpp"
#include <cstdlib>
using namespace th11;
namespace {
struct World:LaserWorld {std::vector<u32> effects,rewards;std::vector<std::vector<u32>> collisions;i32 hit=0;
 i32 test(Vec3 p,float a,float w,float l,u32 probe){collisions.push_back({0,float_bits(p.x),float_bits(p.y),float_bits(p.z),float_bits(a),float_bits(w),float_bits(l),probe});return hit;}
 i32 collision(Vec3 p,float a,float w,float l)override{return test(p,a,w,l,0);}
 i32 warning_collision(Vec3 p,float a,float w,float l)override{return test(p,a,w,l,1);}
 bool th14_curve_graze(Vec3 p)override{collisions.push_back({1,float_bits(p.x),float_bits(p.y),float_bits(p.z)});return true;}
 i32 th14_curve_cut(Th14LaserCurve&,Vec3 p,Vec2 s,float angle,u32 kind,bool skip)override{collisions.push_back({2,float_bits(p.x),float_bits(p.y),float_bits(p.z),float_bits(s.x),float_bits(s.y),float_bits(angle),kind,u32(skip)});return 0;}
bool cancel_effect(Vec3 p,i32 script)override{effects.insert(effects.end(),{u32(script),float_bits(p.x),float_bits(p.y),float_bits(p.z),0x200,0});return true;}bool th14_curve_reward(Vec3 p,u32 kind)override{rewards.insert(rewards.end(),{kind,float_bits(p.x),float_bits(p.y),float_bits(p.z)});return true;}} world;
AnmResource resource;AnmEnvironment animations;
std::vector<u32> result;
std::vector<std::vector<u32>> fragment_results;
}
extern "C" {
__attribute__((export_name("allocate"))) void* allocate(u32 n){return std::malloc(n);}
__attribute__((export_name("release"))) void release(void* p){std::free(p);}
__attribute__((export_name("run"))) u32 run(const u32* p,u32 count,i32 color,i32 state,i32 protection,u32 mode){
    LaserManager manager(resource,animations,world,7);auto* curve=new Th14LaserCurve{};auto& b=curve->base;b.initialize(&world.rate);b.type=2;b.state=state;b.protection=protection;b.offscreen_timer.set(7,&world.rate);b.offscreen_timer.fractional=7.25f;
    curve->path.samples.resize(count);for(u32 j=0;j<count;++j)curve->path.samples[j].position={float_bits(p[j*8]),float_bits(p[j*8+1]),float_bits(p[j*8+2])};std::memcpy(curve->path.request+0x1c,&color,4);manager.append(b);world.effects.clear();
    const bool ok=mode==2?manager.reset_th14():manager.cancel_object(b,true,mode==1);
    result={u32(b.state),u32(b.protection),u32(b.offscreen_timer.previous),u32(b.offscreen_timer.current),float_bits(b.offscreen_timer.fractional)};return ok;
}
__attribute__((export_name("value"))) u32 value(u32 n){return result.at(n);}
__attribute__((export_name("effect_count"))) u32 effect_count(){return world.effects.size()/6;}
__attribute__((export_name("effect_word"))) u32 effect_word(u32 n){return world.effects.at(n);}
__attribute__((export_name("rectangle"))) i32 rectangle(const u32* p,u32 count,const u8* request,u32 x,u32 y,u32 z,u32 width,u32 height,u32 angle,float rate,u32 protection){
 world.rate=rate;world.effects.clear();world.rewards.clear();fragment_results.clear();resource.scripts.resize(300);for(auto& s:resource.scripts)s.bytes={2,0,8,0,0,0,0,0,255,255,0,0,0,0,0,0};
 LaserManager manager(resource,animations,world,7);auto* curve=new Th14LaserCurve{};auto& b=curve->base;b.initialize(&world.rate);b.type=b.state=2;b.rectangle_enabled=1;b.protection=protection;curve->native_flags=1;std::memcpy(curve->path.request,request,0x354);curve->path.samples.resize(count);curve->path.nodes.resize(1);curve->path.nodes[0].end=0;
 for(u32 j=0;j<count;++j){auto& s=curve->path.samples[j];s.position={float_bits(p[j*8]),float_bits(p[j*8+1]),float_bits(p[j*8+2])};s.auxiliary={float_bits(p[j*8+3]),float_bits(p[j*8+4]),float_bits(p[j*8+5])};s.angle=float_bits(p[j*8+6]);s.speed=float_bits(p[j*8+7]);}
 curve->curve_timer={29,30,30.5f,&manager.rate,1};manager.append(b);const i32 selected=manager.cancel_rectangle_th14({float_bits(x),float_bits(y),float_bits(z)},{float_bits(width),float_bits(height)},float_bits(angle),1);
 result={curve->native_flags&2,u32(curve->path.samples.size()),u32(curve->curve_timer.previous),u32(curve->curve_timer.current),float_bits(curve->curve_timer.fractional),manager.active_count};
 for(auto* child=b.next;child;child=child->next){const auto& c=*reinterpret_cast<Th14LaserCurve*>(child);std::vector<u32> values(213);std::memcpy(values.data(),c.path.request,0x354);values.push_back(u32(c.path.transform_index));values.push_back(u32(c.curve_timer.previous));values.push_back(u32(c.curve_timer.current));values.push_back(float_bits(c.curve_timer.fractional));fragment_results.push_back(values);}
 return selected;
}
__attribute__((export_name("reward_count"))) u32 reward_count(){return world.rewards.size()/4;}
__attribute__((export_name("reward_word"))) u32 reward_word(u32 n){return world.rewards.at(n);}
__attribute__((export_name("fragment_count"))) u32 fragment_count(){return fragment_results.size();}
__attribute__((export_name("fragment_word"))) u32 fragment_word(u32 n,u32 k){return fragment_results.at(n).at(k);}
__attribute__((export_name("collision"))) i32 collision(const u32* p,u32 count,u32 width,float rate,u32 age,i32 hit,u32 probe){
 Th14LaserCurve curve{};world.rate=rate;world.player={17.25f,120.5f,3.25f};world.hit=hit;world.collisions.clear();curve.base.width=float_bits(width);curve.base.graze_timer.set(i32(age),&world.rate);curve.path.samples.resize(count);
 static_assert(sizeof(Th14CurveSample)==32);std::memcpy(curve.path.samples.data(),p,count*32);const i32 value=laser_th14_curve_collision(curve,world,probe);const auto& t=curve.base.graze_timer;result={u32(t.previous),u32(t.current),float_bits(t.fractional)};return value;
}
__attribute__((export_name("collision_count"))) u32 collision_count(){return world.collisions.size();}
__attribute__((export_name("collision_size"))) u32 collision_size(u32 n){return world.collisions.at(n).size();}
__attribute__((export_name("collision_word"))) u32 collision_word(u32 n,u32 k){return world.collisions.at(n).at(k);}

}
