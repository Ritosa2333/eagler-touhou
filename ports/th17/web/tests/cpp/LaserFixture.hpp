#pragma once
#include "../../cpp/game/LaserDynamics.hpp"
API(laser_motion_create) MovingLaserMotion* laser_motion_create(){return new MovingLaserMotion;}
API(laser_motion_delete) void laser_motion_delete(MovingLaserMotion* p){delete p;}
API(laser_motion_field) void* laser_motion_field(MovingLaserMotion* p,i32 field){switch(field){case 0:return &p->position;case 1:return &p->velocity;case 2:return &p->angle;case 3:return &p->length;case 4:return &p->width;case 5:return &p->speed;case 6:return &p->traveled;case 7:return &p->maximum_length;case 8:return &p->end_distance;case 9:return &p->outside_delay;case 10:return &p->invulnerability;default:return &p->animation_age;}}
API(laser_motion_velocity) void laser_motion_velocity(MovingLaserMotion* p){p->set_velocity();}
API(laser_motion_update) u32 laser_motion_update(MovingLaserMotion* p,float rate){const bool ended=p->advance(rate);if(!ended)p->finish_frame(rate);return ended;}
struct LaserDynamicsFixture:BulletSoundHost {MovingLaserMotion motion;LaserAcceleration acceleration;LaserTurn turn;u32 flags=0;i32 sound=-1;std::vector<i32> sounds;void play(i32 id)override{sounds.push_back(id);}};
API(laser_dynamics_create) LaserDynamicsFixture* laser_dynamics_create(){return new LaserDynamicsFixture;}
API(laser_dynamics_delete) void laser_dynamics_delete(LaserDynamicsFixture* p){delete p;}
API(laser_dynamics_motion) MovingLaserMotion* laser_dynamics_motion(LaserDynamicsFixture* p){return &p->motion;}
API(laser_dynamics_field) void* laser_dynamics_field(LaserDynamicsFixture* p,u32 field){switch(field){case 0:return &p->flags;case 1:return &p->acceleration.timer;case 2:return &p->acceleration.speed;case 3:return &p->acceleration.angle;case 4:return &p->acceleration.vector;case 5:return &p->acceleration.duration;case 6:return &p->turn.timer;case 7:return &p->turn.speed;case 8:return &p->turn.angle;case 9:return &p->turn.duration;case 10:return &p->turn.limit;case 11:return &p->turn.count;case 12:return &p->turn.mode;default:return &p->sound;}}
API(laser_dynamics_update) u32 laser_dynamics_update(LaserDynamicsFixture* p,u32 turn,float rate){p->sounds.clear();return turn?p->turn.advance(p->motion,p->flags,rate,p->sound,p):p->acceleration.advance(p->motion,p->flags,rate);}
API(laser_dynamics_sound_count) u32 laser_dynamics_sound_count(LaserDynamicsFixture* p){return p->sounds.size();}
API(laser_dynamics_sounds) const i32* laser_dynamics_sounds(LaserDynamicsFixture* p){return p->sounds.data();}

#include "../../cpp/game/LaserVisual.hpp"
#include "../../cpp/game/LaserContact.hpp"
#include "../../cpp/game/CurveProgram.hpp"
#include "../../cpp/game/SegmentIntersection.hpp"
API(laser_stationary_create) StationaryLaserMotion* laser_stationary_create(){return new StationaryLaserMotion;}
API(laser_stationary_delete) void laser_stationary_delete(StationaryLaserMotion* p){delete p;}
API(laser_stationary_field) void* laser_stationary_field(StationaryLaserMotion* p,i32 field){switch(field){case 0:return &p->position;case 1:return &p->drift;case 2:return &p->angle;case 3:return &p->length;case 4:return &p->width;case 5:return &p->speed;case 6:return &p->angular_velocity;case 7:return &p->maximum_length;case 8:return &p->target_width;case 9:return &p->state;case 10:return &p->age;case 11:return &p->delay;case 12:return &p->warmup;case 13:return &p->active;default:return &p->fade;}}
API(laser_stationary_update) u32 laser_stationary_update(StationaryLaserMotion* p,float rate,const Vec3* anchor){const bool ended=p->advance(rate,anchor);if(!ended)p->finish_frame(rate);return ended;}
API(segment_intersection) u32 segment_intersection_test(const Vec2* a,const Vec2* b,const Vec2* c,const Vec2* d,Vec2* out){return segment_intersection(*a,*b,*c,*d,*out);}
struct LaserVisualFixture {AnmManager& manager;LaserVisual visual;LaserVisualFixture(AnmManagerFixture& a,i32 resource):manager(a.manager),visual(a.manager,a.random,resource){}};
API(laser_visual_create) LaserVisualFixture* laser_visual_create(AnmManagerFixture* a,i32 resource){return new LaserVisualFixture(*a,resource);}
API(laser_visual_delete) void laser_visual_delete(LaserVisualFixture* p){delete p;}
API(laser_visual_initialize) u32 laser_visual_initialize(LaserVisualFixture* p,u32 moving,i32 type,i32 color){p->manager.rate=1;return moving?p->visual.initialize_moving(type,color):p->visual.initialize_stationary(type,color);}
API(laser_visual_vm) AnmVm* laser_visual_vm(LaserVisualFixture* p,i32 kind){return kind==0?&p->visual.body:kind==1?&p->visual.head:&p->visual.tip;}
API(laser_visual_update) u32 laser_visual_update(LaserVisualFixture* p,float width,float length,float traveled,u32 moving,float rate){p->manager.rate=rate;return p->visual.update(width,length,traveled,moving!=0);}
API(laser_visual_error) const char* laser_visual_error(LaserVisualFixture* p){return p->visual.error.c_str();}
API(curve_path_segment_create) CurvePathSegment* curve_path_segment_create(){return new CurvePathSegment;}
API(curve_path_segment_delete) void curve_path_segment_delete(CurvePathSegment* p){delete p;}
API(curve_path_segment_field) void* curve_path_segment_field(CurvePathSegment* p,u32 i){switch(i){case 0:return &p->begin;case 1:return &p->end;case 2:return &p->mode;case 3:return &p->direction;case 4:return &p->origin;case 5:return &p->angle;case 6:return &p->speed;case 7:return &p->acceleration;default:return &p->angular_acceleration;}}
API(curve_path_sample_create) CurvePathSample* curve_path_sample_create(){return new CurvePathSample;}
API(curve_path_sample_delete) void curve_path_sample_delete(CurvePathSample* p){delete p;}
API(curve_path_evaluate) u32 curve_path_evaluate(const CurvePathSegment* p,float time,CurvePathSample* sample){return CurvePath::evaluate(*p,time,*sample);}
API(curve_path_preceding) u32 curve_path_preceding(const CurvePathSegment* p,float time,const CurvePathSample* newer,CurvePathSample* sample){return CurvePath::preceding(*p,time,*newer,*sample);}

struct CurveGeometryFixture {std::vector<AnmGeometryVertex> vertices;};
API(curve_geometry_create) CurveGeometryFixture* curve_geometry_create(){return new CurveGeometryFixture;}
API(curve_geometry_delete) void curve_geometry_delete(CurveGeometryFixture* p){delete p;}
API(curve_geometry_build) u32 curve_geometry_build(CurveGeometryFixture* p,const CurvePathSample* points,u32 count,float width,const Vec2* offset,const Vec2* uv){return curve_strip(points,count,width,*offset,*uv,p->vertices);}
API(curve_geometry_vertices) const AnmGeometryVertex* curve_geometry_vertices(CurveGeometryFixture* p){return p->vertices.data();}
API(curve_geometry_count) u32 curve_geometry_count(CurveGeometryFixture* p){return p->vertices.size();}

API(curve_motion_create) CurveLaserMotion* curve_motion_create(){return new CurveLaserMotion;}
API(curve_motion_delete) void curve_motion_delete(CurveLaserMotion* p){delete p;}
API(curve_motion_initialize) u32 curve_motion_initialize(CurveLaserMotion* p,const Vec3* origin,float angle,float width,float speed,u32 count,float initial_time){return p->initialize(*origin,angle,width,speed,count,initial_time);}
API(curve_motion_path) CurvePathSegment* curve_motion_path(CurveLaserMotion* p,u32 index){if(index>=p->path.segments.size())p->path.segments.resize(index+1);return &p->path.segments[index];}
API(curve_motion_sample) const CurvePathSample* curve_motion_sample(CurveLaserMotion* p,u32 index){return index<p->points.size()?&p->points[index]:nullptr;}
API(curve_motion_count) u32 curve_motion_count(CurveLaserMotion* p){return p->points.size();}
API(curve_motion_timer) Timer* curve_motion_timer(CurveLaserMotion* p,u32 outside){return outside?&p->outside_delay:&p->path_age;}
API(curve_motion_step) u32 curve_motion_step(CurveLaserMotion* p,float rate,u32 flags){const bool finished=p->advance(rate,flags);if(!finished)p->finish_frame(rate);return finished;}

struct CurveProgramFixture:CurveProgramHost {
    CurveLaserMotion motion;CurveProgram program;std::array<BulletTransform,18> entries{};u32 id=65536,blend_flags=6,object_flags=0;i32 state=2,protection=0;std::vector<u32> events;
    void laser_sound(i32 id,bool queued)override{events.insert(events.end(),{queued?10u:9u,u32(id)});}
    Vec3 reflection_origin()const override{return motion.position;}
    bool reflected_laser(const Vec3&,float,float)override{return false;}
    bool rebind_curve_body(i32 type,i32 color)override{events.insert(events.end(),{11u,u32(type),u32(color)});return true;}
    void curve_blend(bool additive)override{blend_flags=additive?(blend_flags&~0x1c0u)|0x20:(blend_flags&~0x1e0u);}
};
API(curve_program_create) CurveProgramFixture* curve_program_create(){return new CurveProgramFixture;}
API(curve_program_delete) void curve_program_delete(CurveProgramFixture* p){delete p;}
API(curve_program_motion) CurveLaserMotion* curve_program_motion(CurveProgramFixture* p){return &p->motion;}
API(curve_program_transform) void* curve_program_transform(CurveProgramFixture* p,u32 i,u32 field){auto& t=p->entries[i];switch(field){case 0:return t.floats.data();case 1:return t.integers.data();case 2:return &t.type;default:return &t.active;}}
API(curve_program_field) void* curve_program_field(CurveProgramFixture* p,u32 i){auto& s=p->program;switch(i){case 0:return &s.flags;case 1:return &s.index;case 2:return &p->id;case 3:return &p->state;case 4:return &p->protection;case 5:return &s.freeze;case 6:return &s.protection_timer;case 7:return &s.turn.timer;case 8:return &s.turn.angle;case 9:return &s.turn.speed;case 10:return &s.turn.duration;case 11:return &s.turn.limit;case 12:return &s.turn.count;case 13:return &s.turn.mode;case 14:return &s.wait;case 15:return &s.boost_timer;case 16:return &s.boost_stage;case 17:return &p->blend_flags;case 19:return &p->object_flags;default:return &s.protection_parameter;}}
API(curve_program_kinematics) void* curve_program_kinematics(CurveProgramFixture* p,u32 i){switch(i){case 0:return &p->motion.velocity;case 1:return &p->motion.angle;default:return &p->motion.speed;}}
API(curve_program_step) i32 curve_program_step(CurveProgramFixture* p,float rate,i32 sound){p->events.clear();CurveProgramContext context{p->motion,p->id,p->state,p->object_flags,p->protection,*p,sound};if(!p->program.update(p->entries,context,rate))return -1;const bool finished=p->motion.advance(rate,p->program.flags,p->object_flags);if(!finished)p->motion.finish_frame(rate);return finished;}
API(curve_program_error) const char* curve_program_error(CurveProgramFixture* p){return p->program.error.c_str();}
API(curve_program_events) const u32* curve_program_events(CurveProgramFixture* p){return p->events.data();}
API(curve_program_event_count) u32 curve_program_event_count(CurveProgramFixture* p){return p->events.size();}
API(curve_motion_path_count) u32 curve_motion_path_count(CurveLaserMotion* p){return p->path.segments.size();}

API(laser_visual_initialize_curve) u32 laser_visual_initialize_curve(LaserVisualFixture* p,i32 type,i32 color){return p->visual.initialize_curved(type,color);}
API(laser_visual_update_curve) u32 laser_visual_update_curve(LaserVisualFixture* p,float rate){const float prior=p->manager.rate;p->manager.rate=rate;const bool ok=p->visual.update_curved();p->manager.rate=prior;return ok;}



struct LaserContactFixture:LaserContactHost {
    PlayerCollisionFixture& player;LaserContact contact;Vec3 position{};float angle=0,length=128,width=16,rate=1;u32 flags=0;i32 state=2;u32 grazes=0;std::vector<u32> events;
    explicit LaserContactFixture(PlayerCollisionFixture& p):player(p){}
    static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}
    bool cancel_hit(const Vec3& p)override{events.insert(events.end(),{1,bits(p.x),bits(p.y),bits(p.z)});return true;}
    void graze_spark(const Vec3& p)override{events.insert(events.end(),{2,bits(p.x),bits(p.y),bits(p.z)});}
    void graze()override{grazes++;}
};
API(laser_contact_create) LaserContactFixture* laser_contact_create(PlayerCollisionFixture* p){return new LaserContactFixture(*p);}
API(laser_contact_delete) void laser_contact_delete(LaserContactFixture* p){delete p;}
API(laser_contact_field) void* laser_contact_field(LaserContactFixture* p,u32 field){switch(field){case 0:return &p->position;case 1:return &p->angle;case 2:return &p->length;case 3:return &p->width;case 4:return &p->flags;case 5:return &p->state;case 6:return &p->contact.graze_age;case 7:return &p->rate;default:return &p->grazes;}}
API(laser_contact_update) u32 laser_contact_update(LaserContactFixture* p,u32 moving,u32 graze){p->events.clear();return moving?p->contact.moving(p->position,p->angle,p->length,p->width,p->flags,graze!=0,p->rate,p->player.state,p->player,*p):p->contact.stationary(p->position,p->angle,p->length,p->width,p->state,graze!=0,p->rate,p->player.state,p->player,*p);}
API(laser_contact_events) const u32* laser_contact_events(LaserContactFixture* p){return p->events.data();}
API(laser_contact_event_count) u32 laser_contact_event_count(LaserContactFixture* p){return p->events.size();}

struct LaserReflectionFixture:LaserReflectionHost {
    struct Spawn {Vec3 position;float angle,speed;};MovingLaserMotion motion;LaserReflection reflection;Vec3 origin{};u32 flags=0;std::vector<Spawn> spawned;std::vector<i32> sounds;
    Vec3 reflection_origin()const override{return origin;}
    bool reflected_laser(const Vec3& p,float angle,float speed)override{origin=p;spawned.push_back({p,angle,speed});return true;}
    void reflection_sound(i32 sound)override{sounds.push_back(sound);}
};
API(laser_reflection_create) LaserReflectionFixture* laser_reflection_create(){return new LaserReflectionFixture;}
API(laser_reflection_delete) void laser_reflection_delete(LaserReflectionFixture* p){delete p;}
API(laser_reflection_motion) MovingLaserMotion* laser_reflection_motion(LaserReflectionFixture* p){return &p->motion;}
API(laser_reflection_field) void* laser_reflection_field(LaserReflectionFixture* p,u32 field){switch(field){case 0:return &p->flags;case 1:return &p->reflection.speed;case 2:return &p->reflection.sides;case 3:return &p->origin;case 4:return &p->reflection.count;default:return &p->reflection.limit;}}
API(laser_reflection_update) i32 laser_reflection_update(LaserReflectionFixture* p,i32 sound){p->spawned.clear();p->sounds.clear();return p->reflection.advance(p->motion,p->flags,sound,*p);}
API(laser_reflection_spawns) const LaserReflectionFixture::Spawn* laser_reflection_spawns(LaserReflectionFixture* p){return p->spawned.data();}
API(laser_reflection_spawn_count) u32 laser_reflection_spawn_count(LaserReflectionFixture* p){return p->spawned.size();}
API(laser_reflection_sounds) const i32* laser_reflection_sounds(LaserReflectionFixture* p){return p->sounds.data();}
API(laser_reflection_sound_count) u32 laser_reflection_sound_count(LaserReflectionFixture* p){return p->sounds.size();}
