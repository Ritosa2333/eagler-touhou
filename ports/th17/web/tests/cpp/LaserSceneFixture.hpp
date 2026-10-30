#pragma once
#include "../../cpp/game/LaserScene.hpp"
struct LaserSceneFixture:LaserSceneHost {
    AnmManagerFixture& animations;PlayerCollisionFixture& player;EffectManager effects;BulletCancellationRewards rewards;LaserScene scene;MovingLaserRequest request;StationaryLaserRequest stationary_request;CurveLaserRequest curve_request;SegmentedLaserRequest segmented_request;std::vector<u32> events;
    LaserSceneFixture(AnmManagerFixture& a,PlayerCollisionFixture& p,i32 id):animations(a),player(p),effects(a.manager,8),scene(*this,a.manager,effects,a.environment.game_rng,a.random,rewards,id){}
    static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}
    const PlayerCollision& player_collision()const noexcept override{return player.state;}
    void hit()override{player.hit();}
    void sound(i32 id)override{events.insert(events.end(),{9,u32(id)});}
    void laser_sound(i32 id,bool queued)override{events.insert(events.end(),{queued?10u:9u,u32(id)});}
    bool emit_laser_bullets(const BulletShooter& shooter)override{
        std::array<u8,896> bytes{};auto put=[&](u32 at,const void* p,u32 n){std::memcpy(bytes.data()+at,p,n);};
        put(0,&shooter.sprite,4);put(4,&shooter.color,4);put(8,&shooter.position,12);put(20,&shooter.angle,4);put(24,&shooter.angle_step,4);put(28,&shooter.speed,4);put(32,&shooter.speed_step,4);put(36,&shooter.radius,4);
        for(u32 i=0;i<18;i++){const auto& transform=shooter.transforms[i];put(40+i*44,transform.floats.data(),16);put(56+i*44,transform.integers.data(),16);put(72+i*44,&transform.type,4);put(76+i*44,&transform.active,4);}
        put(832,shooter.extra.data(),36);put(868,&shooter.count,2);put(870,&shooter.rows,2);put(872,&shooter.pattern,2);put(874,&shooter.reserved,2);put(876,&shooter.flags,4);put(880,&shooter.shoot_sound,4);put(884,&shooter.transform_sound,4);put(888,shooter.tail.data(),8);
        events.push_back(11);for(u32 i=0;i<896;i+=4){u32 value;std::memcpy(&value,bytes.data()+i,4);events.push_back(value);}return true;
    }
    void cancellation_effect(i32 id,const Vec3& p,const Vec3& v)override{events.insert(events.end(),{8,u32(id),bits(p.x),bits(p.y),bits(p.z),bits(v.x),bits(v.y),bits(v.z)});}
    void graze_spark(const Vec3& p)override{events.insert(events.end(),{2,bits(p.x),bits(p.y),bits(p.z)});}
    void graze()override{events.push_back(3);}
    void graze_resonance(float f)override{events.insert(events.end(),{4,bits(f)});}
    void item(i32 type,const Vec3& p,float angle,float speed)override{events.insert(events.end(),{5,u32(type),bits(p.x),bits(p.y),bits(p.z),bits(angle),bits(speed)});}
};
API(laser_scene_create) LaserSceneFixture* laser_scene_create(AnmManagerFixture* a,PlayerCollisionFixture* p,i32 id){return new LaserSceneFixture(*a,*p,id);}
API(laser_scene_delete) void laser_scene_delete(LaserSceneFixture* p){delete p;}
API(laser_scene_request) void* laser_scene_request(LaserSceneFixture* p,u32 field){auto& r=p->request;switch(field){case 0:return &r.position;case 1:return &r.angle;case 2:return &r.maximum_length;case 3:return &r.initial_length;case 4:return &r.end_distance;case 5:return &r.width;case 6:return &r.speed;case 7:return &r.type;case 8:return &r.color;case 9:return &r.start_offset;case 10:return &r.sound;case 11:return &r.transform_parameter;case 13:return &r.reflection_sound;default:return &r.transform_index;}}
API(laser_scene_emit) u32 laser_scene_emit(LaserSceneFixture* p){return p->scene.create_moving(p->request);}
API(laser_scene_update) u32 laser_scene_update(LaserSceneFixture* p,float rate,u32 flags){p->events.clear();return p->scene.update(rate,flags);}
API(laser_scene_motion) MovingLaserMotion* laser_scene_motion(LaserSceneFixture* p,u32 id){return p->scene.moving_motion(id);}
API(laser_scene_vm) AnmVm* laser_scene_vm(LaserSceneFixture* p,u32 id,u32 which){auto* v=p->scene.moving_visual(id);if(!v)v=p->scene.stationary_visual(id);if(!v)v=p->scene.curve_visual(id);return v?(which==0?&v->body:which==1?&v->head:&v->tip):nullptr;}
API(laser_scene_contact) Timer* laser_scene_contact(LaserSceneFixture* p,u32 id){auto* v=p->scene.moving_contact(id);if(!v)v=p->scene.stationary_contact(id);if(!v)v=p->scene.curve_contact(id);return v?&v->graze_age:nullptr;}
API(laser_scene_count) u32 laser_scene_count(LaserSceneFixture* p){return p->scene.manager.count();}
API(laser_scene_order) u32 laser_scene_order(LaserSceneFixture* p,u32 index){u32 id=0;p->scene.manager.each([&](LaserObject& v){if(!index--)id=v.id;});return id;}
API(laser_scene_object) void* laser_scene_object(LaserSceneFixture* p,u32 id,u32 field){auto* o=p->scene.manager.find(id);if(!o)return nullptr;switch(field){case 0:return &o->flags;case 1:return &o->state;case 2:return &o->age;case 3:return &o->outside_delay;default:return &o->outside_count;}}
API(laser_scene_events) const u32* laser_scene_events(LaserSceneFixture* p){return p->events.data();}
API(laser_scene_event_count) u32 laser_scene_event_count(LaserSceneFixture* p){return p->events.size();}
API(laser_scene_error) const char* laser_scene_error(LaserSceneFixture* p){return p->scene.error.c_str();}
API(laser_scene_cancel) i32 laser_scene_cancel(LaserSceneFixture* p,u32 kind,const Vec3* position,float radius,const Vec2* size,float angle,i32 reward,u32 honor){p->events.clear();return kind==0?p->scene.cancel_circle(*position,radius,reward,honor!=0):kind==1?p->scene.cancel_rectangle(*position,*size,angle,reward):p->scene.cancel_all(reward,honor!=0);}
API(laser_scene_rewards) BulletCancellationRewards* laser_scene_rewards(LaserSceneFixture* p){return &p->rewards;}
API(laser_scene_tracking) void* laser_scene_tracking(LaserSceneFixture* p,u32 cursor){return cursor?static_cast<void*>(&p->effects.cursor):static_cast<void*>(p->effects.handles.data());}

API(laser_scene_rng) Rng* laser_scene_rng(LaserSceneFixture* p,u32 visual){return visual?&p->animations.random:&p->animations.environment.game_rng;}

API(laser_scene_transform) void* laser_scene_transform(LaserSceneFixture* p,u32 index,u32 field){auto& t=p->request.transforms[index];switch(field){case 0:return t.floats.data();case 1:return t.integers.data();case 2:return &t.type;default:return &t.active;}}
API(laser_scene_program) void* laser_scene_program(LaserSceneFixture* p,u32 id,u32 field){auto* s=p->scene.moving_program(id);if(!s)return nullptr;switch(field){case 0:return &s->flags;case 1:return &s->index;case 2:return &s->boost_timer;case 3:return &s->boost_stage;case 4:return &s->angular_timer;case 5:return &s->angular_speed;case 6:return &s->angular_angle;case 7:return &s->angular_duration;case 8:return &s->wait;case 9:return &s->freeze;case 10:return &s->acceleration.timer;case 11:return &s->acceleration.speed;case 12:return &s->acceleration.angle;case 13:return &s->acceleration.vector;case 14:return &s->acceleration.duration;case 15:return &s->turn.timer;case 16:return &s->turn.speed;case 17:return &s->turn.angle;case 18:return &s->turn.duration;case 19:return &s->turn.limit;case 20:return &s->turn.count;case 21:return &s->turn.mode;case 22:return &s->reflection.speed;case 23:return &s->reflection.count;case 24:return &s->reflection.limit;default:return &s->reflection.sides;}}

API(laser_scene_stationary_request) void* laser_scene_stationary_request(LaserSceneFixture* p,u32 field){auto& r=p->stationary_request;switch(field){case 0:return &r.position;case 1:return &r.drift;case 2:return &r.angle;case 3:return &r.angular_velocity;case 4:return &r.maximum_length;case 5:return &r.initial_length;case 6:return &r.width;case 7:return &r.speed;case 8:return &r.delay;case 9:return &r.warmup;case 10:return &r.active;case 11:return &r.fade;case 12:return &r.sound;case 13:return &r.transform_sound;case 14:return &r.id;case 15:return &r.start_offset;case 16:return &r.transform_index;case 17:return &r.type;case 18:return &r.color;default:return &r.flags;}}
API(laser_scene_stationary_transform) void* laser_scene_stationary_transform(LaserSceneFixture* p,u32 index,u32 field){auto& t=p->stationary_request.transforms[index];return field==0?static_cast<void*>(t.floats.data()):field==1?static_cast<void*>(t.integers.data()):field==2?static_cast<void*>(&t.type):static_cast<void*>(&t.active);}
API(laser_scene_stationary_emit) u32 laser_scene_stationary_emit(LaserSceneFixture* p){return p->scene.create_stationary(p->stationary_request);}
API(laser_scene_stationary_motion) StationaryLaserMotion* laser_scene_stationary_motion(LaserSceneFixture* p,u32 id){return p->scene.stationary_motion(id);}
API(laser_scene_stationary_program) void* laser_scene_stationary_program(LaserSceneFixture* p,u32 id,u32 field){auto* v=p->scene.stationary_program(id);return !v?nullptr:field==0?static_cast<void*>(&v->flags):field==1?static_cast<void*>(&v->index):static_cast<void*>(&v->freeze);}


struct LaserCancellationFixture:LaserCancellationHost {
    AnmManagerFixture& animations;i32 resource;Vec3 position{};float angle=0,length=0,width=0;i32 type=0,color=0,protection=0,state=0;BulletCancellationRewards rewards;LaserCancellation cancellation;EffectManager tracking;std::vector<u32> events;MovingLaserMotion motion;u32 flags=0;std::vector<float> segments;
    LaserCancellationFixture(AnmManagerFixture& a,i32 id):animations(a),resource(id),cancellation(a.environment.game_rng,rewards,*this),tracking(a.manager,id){}
    bool effect(i32 script,const Vec3& p,bool tracked)override{auto& a=animations.manager;const u32 handle=a.create(resource,script,-1,0,p);if(!handle)return false;a.registry.find(handle)->visual.inherited_color=0;if(tracked)tracking.track(handle);return true;}
    bool segment(const Vec3& p,float length)override{segments.insert(segments.end(),{p.x,p.y,p.z,length});return true;}
    void item(i32 type,const Vec3& p,float angle,float speed)override{auto bits=[](float f){u32 value;std::memcpy(&value,&f,4);return value;};events.insert(events.end(),{u32(type),bits(p.x),bits(p.y),bits(p.z),bits(angle),bits(speed)});}
};
API(laser_cancel_tracking) void* laser_cancel_tracking(LaserCancellationFixture* p,u32 cursor){return cursor?static_cast<void*>(&p->tracking.cursor):static_cast<void*>(p->tracking.handles.data());}
API(laser_cancel_create) LaserCancellationFixture* laser_cancel_create(AnmManagerFixture* a,i32 id){return new LaserCancellationFixture(*a,id);}
API(laser_cancel_delete) void laser_cancel_delete(LaserCancellationFixture* p){delete p;}
API(laser_cancel_field) void* laser_cancel_field(LaserCancellationFixture* p,i32 field){switch(field){case 0:return &p->position;case 1:return &p->angle;case 2:return &p->length;case 3:return &p->width;case 4:return &p->type;case 5:return &p->color;case 6:return &p->protection;case 7:return &p->state;case 8:return &p->rewards.count;case 9:return &p->animations.environment.game_rng;case 10:return &p->animations.random;default:return &p->rewards.spell_active;}}
API(laser_cancel_all) i32 laser_cancel_all(LaserCancellationFixture* p,i32 kind,u32 honor){p->events.clear();return p->cancellation.all(p->position,p->angle,p->length,p->type,p->color,p->protection,honor!=0,kind,p->state);}
API(laser_cancel_stationary_all) i32 laser_cancel_stationary_all(LaserCancellationFixture* p,i32 kind,u32 honor){p->events.clear();return p->cancellation.all(p->position,p->angle,p->length,p->type,p->color,p->protection,honor!=0,kind,p->state,true);}
API(laser_cancel_query) i32 laser_cancel_query(LaserCancellationFixture* p,float x,float y,float radius){return LaserCancellation::query(p->position,p->angle,p->length,p->width,{x,y,0},radius);}
API(laser_cancel_events) const u32* laser_cancel_events(LaserCancellationFixture* p){return p->events.data();}
API(laser_cancel_event_count) u32 laser_cancel_event_count(LaserCancellationFixture* p){return p->events.size();}
API(laser_cancel_motion) void* laser_cancel_motion(LaserCancellationFixture* p,i32 field){switch(field){case 0:return &p->motion.position;case 1:return &p->motion.length;case 2:return &p->motion.maximum_length;case 3:return &p->motion.traveled;case 4:return &p->motion.angle;default:return &p->flags;}}
API(laser_cancel_circle) i32 laser_cancel_circle(LaserCancellationFixture* p,float x,float y,float z,float radius,i32 kind,u32 honor){p->events.clear();p->segments.clear();return p->cancellation.circle(p->motion,p->flags,p->type,p->color,p->protection,honor!=0,{x,y,z},radius,kind);}
API(laser_cancel_rectangle) i32 laser_cancel_rectangle(LaserCancellationFixture* p,float x,float y,float width,float height,float angle,i32 kind,u32 honor){p->events.clear();p->segments.clear();return p->cancellation.rectangle(p->motion,p->flags,p->type,p->color,p->protection,honor!=0,{x,y,0},{width,height},angle,kind);}
API(laser_cancel_stationary_circle) i32 laser_cancel_stationary_circle(LaserCancellationFixture* p,float x,float y,float z,float radius,i32 kind,u32 honor){p->events.clear();p->segments.clear();return p->cancellation.circle(p->motion,p->flags,p->type,p->color,p->protection,honor!=0,{x,y,z},radius,kind,true);}
API(laser_cancel_stationary_rectangle) i32 laser_cancel_stationary_rectangle(LaserCancellationFixture* p,float x,float y,float width,float height,float angle,i32 kind,u32 honor){p->events.clear();p->segments.clear();return p->cancellation.rectangle(p->motion,p->flags,p->type,p->color,p->protection,honor!=0,{x,y,0},{width,height},angle,kind,true);}
API(laser_cancel_segments) const float* laser_cancel_segments(LaserCancellationFixture* p){return p->segments.data();}
API(laser_cancel_segment_count) u32 laser_cancel_segment_count(LaserCancellationFixture* p){return p->segments.size()/4;}

API(enemy_laser_emitted_count) u32 enemy_laser_emitted_count(EnemyVariableFixture* p){return p->laser_emitted.count;}
API(enemy_laser_emitted_kind) u32 enemy_laser_emitted_kind(EnemyVariableFixture* p){return p->laser_emitted.kind;}
API(enemy_laser_emitted_size) u32 enemy_laser_emitted_size(EnemyVariableFixture* p){return p->laser_emitted.bytes.size();}
API(enemy_laser_emitted_bytes) const u8* enemy_laser_emitted_bytes(EnemyVariableFixture* p){return p->laser_emitted.bytes.data();}

API(enemy_laser_scene) void enemy_laser_scene(EnemyVariableFixture* p,LaserSceneFixture* scene){p->commands.lasers.scene=&scene->scene;}

API(laser_scene_curve_request) void* laser_scene_curve_request(LaserSceneFixture* p,u32 field){auto& r=p->curve_request;switch(field){case 0:return &r.position;case 1:return &r.angle;case 2:return &r.width;case 3:return &r.speed;case 4:return &r.type;case 5:return &r.color;case 6:return &r.count;case 7:return &r.start_offset;case 8:return &r.flags;case 9:return &r.sound;case 10:return &r.transform_sound;case 11:return &r.transform_index;default:return &r.initial_time;}}
API(laser_scene_curve_transform) void* laser_scene_curve_transform(LaserSceneFixture* p,u32 index,u32 field){auto& t=p->curve_request.transforms[index];return field==0?static_cast<void*>(t.floats.data()):field==1?static_cast<void*>(t.integers.data()):field==2?static_cast<void*>(&t.type):static_cast<void*>(&t.active);}
API(laser_scene_curve_emit) u32 laser_scene_curve_emit(LaserSceneFixture* p){return p->scene.create_curved(p->curve_request);}
API(laser_scene_curve_motion) CurveLaserMotion* laser_scene_curve_motion(LaserSceneFixture* p,u32 id){return p->scene.curve_motion(id);}
API(laser_scene_curve_program) CurveProgram* laser_scene_curve_program(LaserSceneFixture* p,u32 id){return p->scene.curve_program(id);}
API(curve_program_index) i32* curve_program_index(CurveProgram* p){return &p->index;}
API(curve_motion_field) void* curve_motion_field(CurveLaserMotion* p,u32 i){switch(i){case 0:return &p->position;case 1:return &p->velocity;case 2:return &p->angle;case 3:return &p->width;case 4:return &p->speed;case 5:return &p->emission_distance;case 6:return &p->source;case 7:return &p->source_angle;default:return &p->source_speed;}}
