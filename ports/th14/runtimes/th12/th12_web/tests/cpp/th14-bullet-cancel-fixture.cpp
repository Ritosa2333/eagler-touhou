#include "../../cpp/game/BulletManager.hpp"
#include "../../cpp/game/AnmOpcodes.hpp"
#include "../../cpp/game/EclContext.hpp"
#include "../../cpp/game/EnemyManager.hpp"
#include "../../cpp/game/LaserManager.hpp"
#include "../../cpp/game/BulletTh14.hpp"
#include <cstdlib>
using namespace th11;
#define API(name) extern "C" __attribute__((export_name(#name)))
static u32 events[32],event_count;
struct World:BulletWorld{
    bool effect(const Vec3& p,i32 script)override{for(u32 v:{0u,u32(script),float_bits(p.x),float_bits(p.y),float_bits(p.z),0u,0xffffffffu})events[event_count++]=v;return true;}
    bool sound(i32 id,float x,bool)override{events[event_count++]=1;events[event_count++]=u32(id);events[event_count++]=float_bits(x);return true;}
    bool th14_cancel_item(const Vec3& p)override{for(u32 v:{2u,1u,float_bits(p.x),float_bits(p.y),float_bits(p.z)})events[event_count++]=v;return true;}
};
static World world;static AnmEnvironment animations;static AnmResource resource;
static std::unique_ptr<BulletManager> manager;static BulletState* bullet;
static i32 run_case(i32 state,u32 flags,float rate,float x,float y,bool all,u32 slot,bool reward=false,u32 opcode=0,i32 delay=0,bool skip_delayed=false){
    animations.rate=rate;resource={};resource.scripts.resize(294);
    const AnmInstruction code[]={{i16(anm_canonical_opcode(2)),8,0,0},{-1,8,0,0}};
    for(auto& s:resource.scripts){s.bytes.resize(sizeof code);std::memcpy(s.bytes.data(),code,sizeof code);}
    manager=std::make_unique<BulletManager>(resource,animations,world,7);
    Th14BulletEmitter e{};e.sprite=0;e.color=0;e.count=e.layers=1;e.aim=1;
    if(manager->spawn_th14(e,0,0,0))return -2;
    manager->at(BulletManager::capacity-1).state=0;
    bullet=&manager->at(slot);bullet->state=i16(state);bullet->flags=flags;bullet->position={x,y,2};bullet->velocity={1.25,-.75,.5};bullet->hitbox={8,8};bullet->cancel_effect=6;
    bullet->collision_delay=delay;bullet->lifetime.set(20,&manager->rate);bullet->lifetime.previous=19;bullet->lifetime.fractional=20.5f;bullet->vm.pending_interrupt=0;event_count=0;
    if(opcode){
        struct Frame:EnemyFrameWorld{void clear_shot_targets(Enemy*)override{}} frame;
        LaserWorld lw;LaserManager lasers(resource,animations,lw,7);EnemyEnvironment env;EnemyCommandEnvironment commands;commands.bullets=manager.get();commands.lasers=&lasers;
        EclProgram program;EnemyManager enemies(program,env,commands,frame);EnemyState owner;
        alignas(4) u8 bytes[20]{};auto* instruction=reinterpret_cast<EclInstruction*>(bytes);instruction->opcode=u16(opcode);instruction->length=20;const i32 ignored=60;std::memcpy(bytes+16,&ignored,4);
        EclContext context;context.instruction=instruction;EnemyScriptServices services(owner,env,commands);return services.command(context);
    }
    return (all?manager->cancel_all_th14():manager->cancel_circle_th14({},10,reward,skip_delayed))?0:-2;
}
API(run) i32 run(i32 state,u32 flags,float rate,float x,float y){return run_case(state,flags,rate,x,y,false,BulletManager::capacity-1);}
API(run_delayed) i32 run_delayed(i32 state,u32 flags,float rate,u32 x,u32 y,i32 delay){return run_case(state,flags,rate,float_bits(x),float_bits(y),false,BulletManager::capacity-1,false,0,delay,true);}
API(run_all) i32 run_all(i32 state,u32 flags,float rate,float x,float y,u32 slot){return run_case(state,flags,rate,x,y,true,slot);}
API(run_all_changed_rate) i32 run_all_changed_rate(i32 state,u32 flags,float cached_rate,float current_rate,float x,float y,u32 slot){
    if(run_case(0,flags,cached_rate,x,y,true,slot))return -2;
    bullet->state=i16(state);animations.rate=current_rate;
    return manager->cancel_all_th14()?0:-2;
}
API(run_all_ecl) i32 run_all_ecl(i32 state,u32 flags,float rate,float x,float y,u32 slot,u32 opcode){return run_case(state,flags,rate,x,y,true,slot,false,opcode);}
static EnemyState timer_owner;static float old_rate;
API(run_timer) i32 run_timer(i32 value,u32 flags,u32 pointer){
    timer_owner={};auto& t=timer_owner.damage_immunity;t.previous=-16;t.current=19;t.fractional=21.25;t.flags=flags;t.rate=pointer?&old_rate:nullptr;
    struct Frame:EnemyFrameWorld{void clear_shot_targets(Enemy*)override{}} frame;EnemyEnvironment env;EnemyCommandEnvironment commands;EclProgram program;EnemyManager enemies(program,env,commands,frame);
    alignas(4) u8 bytes[20]{};auto* instruction=reinterpret_cast<EclInstruction*>(bytes);instruction->opcode=515;instruction->length=20;std::memcpy(bytes+16,&value,4);EclContext context;context.instruction=instruction;EnemyScriptServices services(timer_owner,env,commands);return services.command(context);
}
API(timer_value) u32 timer_value(u32 n){const auto& t=timer_owner.damage_immunity;const u32 a[]={u32(t.previous),u32(t.current),float_bits(t.fractional),!t.rate?0u:t.rate==&old_rate?1u:2u,t.flags};return a[n];}
API(run_reward) i32 run_reward(i32 state,u32 flags,float rate,float x,float y){return run_case(state,flags,rate,x,y,false,BulletManager::capacity-1,true);}
API(run_etex) i32 run_etex(i32 state,u32 flags,i32 parameter,i32 index,u32 active,u32 concurrent,float rate){
 if(run_case(0,flags,rate,12.25f,-9.5f,false,BulletManager::capacity-1))return -2;
 bullet->state=i16(state);bullet->active_transforms=active;bullet->transform_index=index;auto& t=bullet->th14_transforms[index];t={};t.opcode=0x400;t.p=parameter;t.concurrent=concurrent;
 return bullet_th14_start_transforms(*bullet,*manager);
}
API(run_rectangle) i32 run_rectangle(i32 state,i32 delay,u32 slot,float scale,float angle,float x,float y){
    if(run_case(0,0,1,x,y,false,slot))return -2;bullet->state=i16(state);bullet->collision_delay=delay;bullet->th14_size=scale;event_count=0;
    if(!manager->cancel_rectangle_th14({12,200,0},{64,256},angle,0))return -2;return bullet->state==4&&state!=4;
}
API(value) u32 value(u32 n){auto& b=*bullet;const u32 values[]={b.flags,float_bits(b.position.x),float_bits(b.position.y),float_bits(b.position.z),u32(b.lifetime.previous),u32(b.lifetime.current),float_bits(b.lifetime.fractional),u32(b.vm.pending_interrupt),u32(b.state),u32(b.cancel_effect),u32(b.transform_index),b.active_transforms};return values[n];}
API(event_size) u32 event_size(){return event_count;}
API(event_value) u32 event_value(u32 n){return events[n];}
