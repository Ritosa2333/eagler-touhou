#include "World.hpp"
#include "GameplayData.hpp"
#include "../game/LaserBehavior.hpp"
#include "../game/HighRefresh.hpp"
#include <cstdlib>
namespace th10::browser {
namespace {
constexpr u32 straight_kind=1,timed_kind=2;
struct Lasers;
struct Behavior final:LaserBehaviorEnvironment {
    World& w;explicit Behavior(World& world):w(world){rate=&w.engine.speed;sprite_scripts=gameplay_data::sprite_scripts;animations=&w.engine;animation_file=w.actors.lasers->animation_file;bullet_file=w.actors.bullets->animation_file;started_animations=&w.engine.manager.started_scripts;player_position=&w.actors.player->position;boss_position=w.actors.enemies&&w.actors.enemies->bosses[0]?&w.actors.enemies->bosses[0]->state.current.position:nullptr;}
    void run_commands(StraightLaser& laser) override{laser.process_commands(*this);}
    void update_feature(StraightLaser& laser,LaserFeature feature) override{if(feature==LaserFeature::VectorAcceleration)laser.accelerate(*this);else if(feature==LaserFeature::Turn)laser.turn(*this);else if(feature==LaserFeature::Reflect)laser.reflect(*this);}
    i32 collide_player(const Vec3& p,float angle,float width,float length) override{return w.collide_player_laser(p,angle,width,length);}
    void cancel_rectangle(EnemyLaser& laser,const Vec3& p,const Vec3& size,i32 convert) override{if(laser.original_virtual_table==straight_kind)reinterpret_cast<StraightLaser&>(laser).cancel_rectangle(p,size,convert,*this);else if(laser.original_virtual_table==timed_kind)reinterpret_cast<TimedLaser&>(laser).cancel_rectangle(p,size,convert,*this);else __builtin_trap();}
    void graze_effect(const Vec3& p) override{w.effect(*bullet_file,0x1b2,p);}
    void play_sound(i32 id,float x) override{w.sound(id,x);}
    void play_turn_sound(i32 id) override{w.sound(id);}
    void spawn_straight(const StraightLaserParameters& p) override{w.create_laser(0,&p);}
    void cancel_effect(i32 script,const Vec3& p) override{w.effect(*animation_file,script,p);}
    void spawn_faith(const Vec3& p) override{w.spawn_item(p,8,0xffffffff,-1.5707964f,.6f);}
    void submit(AnmVm& vm) override{w.engine.draw(vm);}
    bool presentation(const EnemyLaser& laser,Vec3& position,float& angle,float& length,float& width) override{
        if(!high_refresh::render_only||!high_refresh::active)return false;const auto found=w.laser_presentation.find(&laser);if(found==w.laser_presentation.end())return false;const auto& before=found->second;
        const float dx=laser.position.x-before.position.x,dy=laser.position.y-before.position.y;if(before.id!=laser.id||before.state!=laser.state||before.kind!=laser.original_virtual_table||dx*dx+dy*dy>=16384.0f)return false;
        position={high_refresh::lerp_world(before.position.x,laser.position.x),high_refresh::lerp_world(before.position.y,laser.position.y),high_refresh::lerp_world(before.position.z,laser.position.z)};constexpr float pi=3.1415927410125732f,tau=6.2831854820251465f;float delta=laser.angle-before.angle;if(delta>pi)delta-=tau;else if(delta<-pi)delta+=tau;angle=normalize_angle(before.angle+delta*high_refresh::world_alpha).to_float();length=high_refresh::lerp_world(before.length,laser.length);width=high_refresh::lerp_world(before.width,laser.width);return true;
    }
};
struct Lasers final:LaserEnvironment {
    World& w;explicit Lasers(World& world):w(world){default_rate=&w.engine.speed;straight_methods=straight_kind;timed_methods=timed_kind;}
    void* allocate(u32 size) override{return std::malloc(size);}
    void release(void* p) override{std::free(p);}
    void initialize_laser(EnemyLaser& laser,const void* p) override{Behavior env(w);if(laser.original_virtual_table==straight_kind)reinterpret_cast<StraightLaser&>(laser).start(*static_cast<const StraightLaserParameters*>(p),env);else if(laser.original_virtual_table==timed_kind)reinterpret_cast<TimedLaser&>(laser).start(*static_cast<const TimedLaserParameters*>(p),env);else __builtin_trap();}
    i32 update_laser(EnemyLaser& laser) override{Behavior env(w);if(laser.original_virtual_table==straight_kind)return reinterpret_cast<StraightLaser&>(laser).update(env);if(laser.original_virtual_table==timed_kind)return reinterpret_cast<TimedLaser&>(laser).update(env);__builtin_trap();}
    void draw_laser(EnemyLaser& laser) override{Behavior env(w);if(laser.original_virtual_table==straight_kind)reinterpret_cast<StraightLaser&>(laser).draw(env);else if(laser.original_virtual_table==timed_kind)reinterpret_cast<TimedLaser&>(laser).draw(env);else __builtin_trap();}
    void destroy_laser(EnemyLaser& laser) override{if(laser.original_virtual_table!=straight_kind&&laser.original_virtual_table!=timed_kind)__builtin_trap();}
    void cancel_laser(EnemyLaser& laser,i32 convert) override{Behavior env(w);if(laser.original_virtual_table==straight_kind)reinterpret_cast<StraightLaser&>(laser).cancel_all(convert,env);else if(laser.original_virtual_table==timed_kind)reinterpret_cast<TimedLaser&>(laser).cancel_all(convert,env);else __builtin_trap();}
    i32 cancel_rectangle(EnemyLaser& laser,const Vec3& p,const Vec3& size,i32 convert) override{Behavior env(w);if(laser.original_virtual_table==straight_kind)return reinterpret_cast<StraightLaser&>(laser).cancel_rectangle(p,size,convert,env);if(laser.original_virtual_table==timed_kind)return reinterpret_cast<TimedLaser&>(laser).cancel_rectangle(p,size,convert,env);__builtin_trap();}
    i32 cancel_circle(EnemyLaser& laser,const Vec3& p,float radius,i32 convert) override{Behavior env(w);if(laser.original_virtual_table==straight_kind)return reinterpret_cast<StraightLaser&>(laser).cancel_circle(p,radius,convert,env);if(laser.original_virtual_table==timed_kind)return reinterpret_cast<TimedLaser&>(laser).cancel_circle(p,radius,convert,env);__builtin_trap();}
};
}
void World::destroy_laser(EnemyLaser& laser){Lasers env(*this);env.destroy_laser(laser);}
void World::clear_lasers(){Lasers env(*this);actors.lasers->clear(env);}
i32 World::update_lasers(){laser_presentation.clear();for(auto* p=actors.lasers->sentinel.next;p;p=p->next)laser_presentation[p]={p->position,p->angle,p->length,p->width,p->id,p->state,p->original_virtual_table};Lasers env(*this);return actors.lasers->tick(actors.session->session_flags,engine.speed,env);}
i32 World::draw_lasers(){Lasers env(*this);return actors.lasers->render(actors.session->session_flags,env);}
i32 World::create_laser(i32 type,const void* p){Lasers env(*this);return actors.lasers->create(type,p,env);}
i32 World::cancel_lasers(i32 convert){Lasers env(*this);return actors.lasers->cancel_all(convert,env);}
i32 World::cancel_laser_circle(const Vec3& p,float radius,i32 convert){Lasers env(*this);return actors.lasers->cancel_circle(p,radius,convert,env);}
i32 World::cancel_laser_rectangle(const Vec3& p,const Vec3& size,i32 convert){Lasers env(*this);return actors.lasers->cancel_rectangle(p,size,convert,env);}
}
