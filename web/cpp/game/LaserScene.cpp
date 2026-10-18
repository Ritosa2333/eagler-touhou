#include "LaserScene.hpp"
#include <cmath>
namespace th15 {
struct LaserScene::MovingLaser final:LaserObject,LaserCancellationHost,LaserContactHost,LaserProgramHost {
    LaserScene& scene;MovingLaserRequest request;MovingLaserMotion motion;LaserVisual visual;LaserContact contact;LaserCancellation cancellation;LaserProgram program;
    MovingLaser(LaserScene& s,const MovingLaserRequest& r):scene(s),request(r),visual(s.animations,s.visual_random,s.resource),cancellation(s.game_random,s.rewards,*this){}
    bool initialize(){
        state=2;kind=LaserKind::moving;program.index=request.transform_index;if(!visual.initialize_moving(request.type,request.color)){error=visual.error;return false;}
        outside_delay.set(30);motion.outside_delay=outside_delay;motion.invulnerability.set(3);contact.graze_age.set(0);motion.animation_age.set(0);if(request.sound>=0)scene.host.laser_sound(request.sound,false);
        motion.position=request.position;if(request.start_offset!=0){motion.position.x=float(motion.position.x+float(std::cos(double(request.angle))*double(request.start_offset)));motion.position.y=float(motion.position.y+float(std::sin(double(request.angle))*double(request.start_offset)));}
        motion.angle=request.angle;motion.maximum_length=request.maximum_length;motion.length=request.initial_length;motion.end_distance=request.end_distance;motion.width=request.width;motion.speed=request.speed;motion.traveled=motion.length>motion.maximum_length?.01f:0;motion.set_velocity();return true;
    }
    bool update(float rate,bool& finished)override{
        const auto& player=scene.host.player_collision();LaserProgramContext context{motion,visual,id,state,outside_count,{player.position.x,player.position.y,0},*this,request.reflection_sound};if(!program.update(request.transforms,context,rate)){error=program.error;return false;}motion.outside_delay=outside_delay;finished=motion.advance(rate);request.maximum_length=motion.maximum_length;outside_delay=motion.outside_delay;if(finished)return true;
        if(!update_visual(rate,false))return false;if(!visual.update(motion.width,motion.length,motion.traveled,true)){error=visual.error;return false;}motion.finish_frame(rate);return true;
    }
    bool update_visual(float rate,bool graze_only)override{return contact.moving(motion.position,motion.angle,motion.length,motion.width,u32(request.transform_parameter),graze_only,rate,scene.host.player_collision(),scene.host,*this);}
    bool retire()override{return true;}
    i32 cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward,bool honor)override{const i32 n=cancellation.rectangle(motion,flags,request.type,request.color,outside_count,honor,p,size,angle,reward);request.maximum_length=motion.maximum_length;if(n<0)error=cancellation.error;return n;}
    i32 cancel_circle(const Vec3& p,float radius,i32 reward,bool honor)override{const i32 n=cancellation.circle(motion,flags,request.type,request.color,outside_count,honor,p,radius,reward);request.maximum_length=motion.maximum_length;if(n<0)error=cancellation.error;return n;}
    bool cancel_all(i32 reward,bool honor)override{const i32 n=cancellation.all(motion.position,motion.angle,motion.length,request.type,request.color,outside_count,honor,reward,state);if(n<0)error=cancellation.error;return n>=0;}
    i32 query_circle(const Vec3& p,float radius)override{return LaserCancellation::query(motion.position,motion.angle,motion.length,motion.width,p,radius);}
    i32 query_rectangle(const Vec3&,const Vec2&,float,i32,i32,i32)override{error="Moving laser enemy rectangle query is not connected";return -1;}
    bool effect(i32 script,const Vec3& p,bool tracked)override{const u32 id=scene.animations.create(scene.resource,script,-1,0,p);if(!id){error=scene.animations.error;return false;}scene.animations.registry.find(id)->visual.inherited_color=0;if(tracked)scene.effects.track(id);return true;}
    bool segment(const Vec3& p,float length)override{auto child=request;child.position=p;child.initial_length=child.maximum_length=length;const u32 id=scene.create_moving(child);if(!id&&!scene.error.empty()){error=scene.error;return false;}return true;}
    void laser_sound(i32 id,bool queued)override{scene.host.laser_sound(id,queued);}
    Vec3 reflection_origin()const override{return request.position;}
    bool emit_laser_bullets(const BulletShooter& shooter)override{if(!scene.bullets)return scene.host.emit_laser_bullets(shooter);const bool ok=scene.bullets->emit(shooter,scene.bullets->manager.minimum_distance_squared);if(!ok)error=scene.bullets->error;return ok;}
    bool erase_emitting_laser()override{return cancel_all(0,false);}
    bool reflected_laser(const Vec3& p,float angle,float speed)override{request.position=p;request.angle=angle;request.speed=speed;request.start_offset=0;const u32 id=scene.create_moving(request);if(!id&&!scene.error.empty()){error=scene.error;return false;}return true;}
    void item(i32 type,const Vec3& p,float angle,float speed)override{scene.host.item(type,p,angle,speed);}
    bool cancel_hit(const Vec3& p)override{return cancel_rectangle(p,{32,32},0,0,true)>=0;}
    void graze_spark(const Vec3& p)override{scene.host.graze_spark(p);}
    void graze_flash()override{scene.host.graze();}
    void graze_resonance(float value)override{scene.host.graze_resonance(value);}
};
struct LaserScene::StationaryLaser final:LaserObject,LaserCancellationHost,LaserContactHost {
    LaserScene& scene;StationaryLaserRequest request;StationaryLaserMotion motion;LaserVisual visual;LaserContact contact;LaserCancellation cancellation;StationaryLaserProgram program;
    StationaryLaser(LaserScene& s,const StationaryLaserRequest& r):scene(s),request(r),visual(s.animations,s.visual_random,s.resource),cancellation(s.game_random,s.rewards,*this){}
    bool initialize(){
        state=3;kind=LaserKind::stationary;program.index=request.transform_index;if(!visual.initialize_stationary(request.type,request.color)){error=visual.error;return false;}
        if(request.sound>=0)scene.host.laser_sound(request.sound,false);motion.position=request.position;if(request.start_offset!=0){motion.position.x=float(motion.position.x+float(std::cos(double(request.angle))*double(request.start_offset)));motion.position.y=float(motion.position.y+float(std::sin(double(request.angle))*double(request.start_offset)));}
        motion.drift=request.drift;motion.angle=request.angle;motion.angular_velocity=request.angular_velocity;motion.maximum_length=request.maximum_length;motion.length=request.initial_length;motion.target_width=request.width;motion.speed=request.speed;motion.delay=request.delay;motion.warmup=request.warmup;motion.active=request.active;motion.fade=request.fade;contact.graze_age.set(0);return true;
    }
    bool update(float rate,bool& finished)override{
        if(!program.update(request.transforms,visual,state,outside_count,rate)){error=program.error;return false;}motion.age=age;motion.state=state;finished=motion.advance(rate,(request.flags&1)?scene.host.stationary_laser_anchor():nullptr);state=motion.state;age=motion.age;if(finished)return true;
        if(!update_visual(rate,false))return false;if(!visual.update(motion.width,motion.length,motion.traveled,false)){error=visual.error;return false;}return true;
    }
    bool update_visual(float rate,bool graze_only)override{return contact.stationary(motion.position,motion.angle,motion.length,motion.width,state,graze_only,rate,scene.host.player_collision(),scene.host,*this);}
    bool retire()override{return true;}
    i32 cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward,bool honor)override{MovingLaserMotion partial;partial.position=motion.position;partial.angle=motion.angle;partial.length=motion.length;const i32 n=cancellation.rectangle(partial,flags,request.type,request.color,outside_count,honor,p,size,angle,reward,true);motion.length=partial.length;if(n<0)error=cancellation.error;return n;}
    i32 cancel_circle(const Vec3& p,float radius,i32 reward,bool honor)override{MovingLaserMotion partial;partial.position=motion.position;partial.angle=motion.angle;partial.length=motion.length;const i32 n=cancellation.circle(partial,flags,request.type,request.color,outside_count,honor,p,radius,reward,true);motion.length=partial.length;if(n<0)error=cancellation.error;return n;}
    bool cancel_all(i32 reward,bool honor)override{const i32 n=cancellation.all(motion.position,motion.angle,motion.length,request.type,request.color,outside_count,honor,reward,state,true);motion.state=state;if(n<0)error=cancellation.error;return n>=0;}
    i32 query_circle(const Vec3& p,float radius)override{return LaserCancellation::query(motion.position,motion.angle,motion.length,motion.width,p,radius);}
    i32 query_rectangle(const Vec3&,const Vec2&,float,i32,i32,i32)override{error="Stationary laser enemy rectangle query is not connected";return -1;}
    bool effect(i32 script,const Vec3& p,bool tracked)override{const u32 id=scene.animations.create(scene.resource,script,-1,0,p);if(!id){error=scene.animations.error;return false;}scene.animations.registry.find(id)->visual.inherited_color=0;if(tracked)scene.effects.track(id);return true;}
    bool segment(const Vec3&,float)override{error="Stationary laser child requires its source distance";return false;}
    bool stationary_segment(const Vec3& position,float length,float distance,bool rectangle)override{MovingLaserRequest child;child.position=position;child.angle=motion.angle;child.initial_length=child.maximum_length=length;child.end_distance=float(motion.maximum_length-distance);child.width=motion.width;child.speed=8;child.type=request.type;child.color=request.color;child.sound=rectangle?-1:0;child.reflection_sound=0;child.transform_parameter=rectangle?i32((request.flags>>1)&1):0;const u32 id=scene.create_moving(child);if(!id&&!scene.error.empty()){error=scene.error;return false;}return true;}
    void item(i32 type,const Vec3& p,float angle,float speed)override{scene.host.item(type,p,angle,speed);}
    bool cancel_hit(const Vec3& p)override{return cancel_rectangle(p,{32,32},0,0,true)>=0;}
    void graze_spark(const Vec3& p)override{scene.host.graze_spark(p);}
    void graze_flash()override{scene.host.graze();}
    void graze_resonance(float value)override{scene.host.graze_resonance(value);}
};
struct LaserScene::CurveLaser final:LaserObject,CurveCancellationHost,LaserContactHost,CurveProgramHost {
    LaserScene& scene;CurveLaserRequest request;CurveLaserMotion motion;LaserVisual visual;LaserContact contact;CurveCancellation cancellation;CurveProgram program;
    CurveLaser(LaserScene& s,const CurveLaserRequest& r):scene(s),request(r),visual(s.animations,s.visual_random,s.resource),cancellation(s.game_random,s.rewards,*this){}
    bool initialize(){
        state=2;kind=LaserKind::curved;if(!visual.initialize_curved(request.type,request.color)){error=visual.error;return false;}
        if(request.start_offset!=0){request.position.x=float(request.position.x+float(std::cos(double(request.angle))*double(request.start_offset)));request.position.y=float(request.position.y+float(std::sin(double(request.angle))*double(request.start_offset)));request.start_offset=0;}
        if(!motion.initialize(request.position,request.angle,request.width,request.speed,request.count,request.initial_time,request.inherited_path)){error="Curve point count invalid";return false;}
        program.index=request.inherited_path?99:request.transform_index;request.inherited_path=nullptr;outside_delay=motion.outside_delay;contact.graze_age.set(0);if(request.sound>=0)scene.host.laser_sound(request.sound,false);return true;
    }
    bool update(float rate,bool& finished)override{
        CurveProgramContext context{motion,id,state,outside_count,*this,request.transform_sound};if(!program.update(request.transforms,context,rate)){error=program.error;return false;}
        motion.outside_delay=outside_delay;finished=motion.advance(rate,program.flags);outside_delay=motion.outside_delay;if(finished)return true;
        if(!update_visual(rate,false))return false;if(!visual.update_curved()){error=visual.error;return false;}motion.finish_frame(rate);return true;
    }
    bool update_visual(float rate,bool graze_only)override{return contact.curved(motion,graze_only,rate,scene.host.player_collision(),scene.host,*this);}
    bool retire()override{visual.retain_children_on_retire=true;return true;}
    i32 cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward,bool honor)override{if(cancellation.rectangle(motion,flags,request.color,outside_count,honor,p,size,angle,reward,scene.rate))return 0;error=cancellation.error;return -1;}
    i32 cancel_circle(const Vec3& p,float radius,i32 reward,bool honor)override{const i32 n=cancellation.circle(motion,flags,request.color,outside_count,honor,p,radius,reward);if(n<0)error=cancellation.error;return n;}
    bool cancel_all(i32,bool honor)override{const bool ok=cancellation.all(motion,request.color,outside_count,honor,state);if(!ok)error=cancellation.error;return ok;}
    i32 query_circle(const Vec3& p,float radius)override{return LaserCancellation::query(motion.position,motion.angle,motion.emission_distance,motion.width,p,radius);}
    i32 query_rectangle(const Vec3&,const Vec2&,float,i32,i32,i32)override{error="Curve laser enemy rectangle query is not connected";return -1;}
    bool curve_effect(i32 script,const Vec3& p,bool tracked)override{const u32 handle=scene.animations.create(scene.resource,script,-1,0,p);if(!handle){error=scene.animations.error;return false;}scene.animations.registry.find(handle)->visual.inherited_color=0;if(tracked)scene.effects.track(handle);return true;}
    bool split_curve(u32 count,float time,const CurvePath& path)override{auto child=request;child.count=count;child.initial_time=time;child.inherited_path=&path;child.sound=-1;const u32 handle=scene.create_curved(child);if(!handle&&!scene.error.empty()){error=scene.error;return false;}return true;}
    bool cancel_hit(const Vec3& p)override{return cancel_rectangle(p,{32,32},0,0,true)>=0;}
    void graze_spark(const Vec3& p)override{scene.host.graze_spark(p);}
    void graze_flash()override{scene.host.graze();}
    void graze_resonance(float value)override{scene.host.graze_resonance(value);}
    void laser_sound(i32 id,bool queued)override{scene.host.laser_sound(id,queued);}
    Vec3 reflection_origin()const override{return motion.position;}
    bool reflected_laser(const Vec3&,float,float)override{return true;}
    bool rebind_curve_body(i32 type,i32 color)override{const bool ok=visual.rebind_body(type,color);if(!ok)error=visual.error;return ok;}
    void curve_blend(bool additive)override{auto& flags=visual.body.visual.flags;flags=additive?(flags&~0x1c0u)|0x20:(flags&~0x1e0u);}
    bool emit_laser_bullets(const BulletShooter& shooter)override{if(!scene.bullets)return scene.host.emit_laser_bullets(shooter);const bool ok=scene.bullets->emit(shooter,scene.bullets->manager.minimum_distance_squared);if(!ok)error=scene.bullets->error;return ok;}
    bool erase_emitting_laser()override{return cancel_all(0,false);}
    void item(i32 type,const Vec3& p,float angle,float speed)override{scene.host.item(type,p,angle,speed);}
};
// The fourth native factory kind is an inactive segmented object: all update,
// contact, drawing and cancellation callbacks return zero in the target build.
struct LaserScene::SegmentedLaser final:LaserObject {
    SegmentedLaserRequest request;MovingLaserMotion motion;Vec3 script_vector{};float script_width=0;std::array<float,512> history{};
    explicit SegmentedLaser(const SegmentedLaserRequest& r):request(r){state=3;kind=LaserKind::segmented;motion.position=r.position;motion.angle=r.angle;motion.length=r.length;motion.width=1;history.fill(r.length);}
    bool update(float,bool& finished)override{finished=false;return true;}
    bool update_visual(float,bool)override{return true;}bool retire()override{return true;}
    i32 cancel_rectangle(const Vec3&,const Vec2&,float,i32,bool)override{return 0;}
    i32 query_rectangle(const Vec3&,const Vec2&,float,i32,i32,i32)override{return 0;}
    i32 cancel_circle(const Vec3&,float,i32,bool)override{return 0;}
    bool cancel_all(i32,bool)override{return true;}i32 query_circle(const Vec3&,float)override{return 0;}
};
LaserScene::LaserScene(LaserSceneHost& h,AnmManager& a,EffectManager& e,Rng& game,Rng& visual,BulletCancellationRewards& r,i32 id):host(h),animations(a),effects(e),game_random(game),visual_random(visual),rewards(r),resource(id){}
LaserScene::~LaserScene()=default;
u32 LaserScene::create_moving(const MovingLaserRequest& request){error.clear();if(manager.count()>=LaserManager::capacity)return 0;auto object=std::make_unique<MovingLaser>(*this,request);if(!object->initialize()){error=object->error;return 0;}return manager.insert(std::move(object));}
u32 LaserScene::create_stationary(const StationaryLaserRequest& request){error.clear();if(manager.count()>=LaserManager::capacity)return 0;auto object=std::make_unique<StationaryLaser>(*this,request);if(!object->initialize()){error=object->error;return 0;}auto* laser=object.get();const u32 generation=manager.insert(std::move(object));laser->id=request.id;return generation;}
bool LaserScene::update(float value,u32 game_flags){error.clear();animations.set_paused((game_flags&2)!=0);rate=(game_flags&2)?0:value;const float prior=animations.rate;animations.rate=rate;const bool ok=manager.update(value,game_flags);animations.rate=prior;if(!ok)error=manager.error;return ok;}
i32 LaserScene::cancel_circle(const Vec3& p,float radius,i32 reward,bool honor){error.clear();const i32 n=manager.cancel_circle(p,radius,reward,honor);if(n<0)error=manager.error;return n;}
i32 LaserScene::cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward){error.clear();const i32 n=manager.cancel_rectangle(p,size,angle,reward);if(n<0)error=manager.error;return n;}
bool LaserScene::cancel_all(i32 reward,bool honor){error.clear();const bool ok=manager.cancel_all(reward,honor);if(!ok)error=manager.error;return ok;}
MovingLaserMotion* LaserScene::moving_motion(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::moving?&static_cast<MovingLaser*>(object)->motion:nullptr;}
LaserVisual* LaserScene::moving_visual(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::moving?&static_cast<MovingLaser*>(object)->visual:nullptr;}
LaserContact* LaserScene::moving_contact(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::moving?&static_cast<MovingLaser*>(object)->contact:nullptr;}
LaserProgram* LaserScene::moving_program(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::moving?&static_cast<MovingLaser*>(object)->program:nullptr;}
StationaryLaserMotion* LaserScene::stationary_motion(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::stationary?&static_cast<StationaryLaser*>(object)->motion:nullptr;}
LaserVisual* LaserScene::stationary_visual(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::stationary?&static_cast<StationaryLaser*>(object)->visual:nullptr;}
LaserContact* LaserScene::stationary_contact(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::stationary?&static_cast<StationaryLaser*>(object)->contact:nullptr;}
StationaryLaserProgram* LaserScene::stationary_program(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::stationary?&static_cast<StationaryLaser*>(object)->program:nullptr;}
bool LaserScene::set_position(u32 id,const Vec3& value){auto* p=manager.find(id);if(!p)return true;if(auto* m=moving_motion(id))m->position=value;else if(auto* m=stationary_motion(id))m->position=value;else if(auto* m=curve_motion(id))m->position=value;else if(auto* m=segmented_motion(id))m->position=value;else{error="Laser position kind is not connected";return false;}return true;}
void LaserScene::offset_origins(float without_identifier,float with_identifier){manager.each([&](LaserObject& p){Vec3* position=nullptr;switch(p.kind){case LaserKind::moving:position=&static_cast<MovingLaser&>(p).motion.position;break;case LaserKind::stationary:position=&static_cast<StationaryLaser&>(p).motion.position;break;case LaserKind::curved:position=&static_cast<CurveLaser&>(p).motion.position;break;case LaserKind::segmented:position=&static_cast<SegmentedLaser&>(p).motion.position;break;}position->y=float((p.id?with_identifier:without_identifier)+position->y);});}
Vec3* LaserScene::origin_at(u32 index)noexcept{Vec3* position=nullptr;manager.each([&](LaserObject& p){if(index--)return;switch(p.kind){case LaserKind::moving:position=&static_cast<MovingLaser&>(p).motion.position;break;case LaserKind::stationary:position=&static_cast<StationaryLaser&>(p).motion.position;break;case LaserKind::curved:position=&static_cast<CurveLaser&>(p).motion.position;break;case LaserKind::segmented:position=&static_cast<SegmentedLaser&>(p).motion.position;break;}});return position;}
bool LaserScene::set_script_vector(u32 id,const Vec3& value){auto* p=manager.find(id);if(!p)return true;if(p->kind==LaserKind::moving){auto& laser=static_cast<MovingLaser&>(*p);laser.request.angle=value.x;laser.request.maximum_length=laser.motion.maximum_length=value.y;laser.request.initial_length=value.z;}else if(p->kind==LaserKind::stationary){auto& laser=static_cast<StationaryLaser&>(*p);laser.request.drift=laser.motion.drift=value;}else if(p->kind==LaserKind::curved){auto& laser=static_cast<CurveLaser&>(*p);laser.request.angle=laser.motion.source_angle=value.x;laser.request.width=laser.motion.render_width=value.y;laser.request.speed=laser.motion.source_speed=value.z;}else if(p->kind==LaserKind::segmented)static_cast<SegmentedLaser&>(*p).script_vector=value;else{error="Laser vector kind is not connected";return false;}return true;}
bool LaserScene::set_speed(u32 id,float value){auto* p=manager.find(id);if(!p)return true;if(auto* m=moving_motion(id))m->speed=value;else if(auto* m=stationary_motion(id))m->speed=value;else if(auto* m=curve_motion(id))m->speed=value;else if(auto* m=segmented_motion(id))m->speed=value;else{error="Laser speed kind is not connected";return false;}return true;}
bool LaserScene::set_width(u32 id,float value){auto* p=manager.find(id);if(!p)return true;if(auto* m=moving_motion(id))m->width=value;else if(auto* m=stationary_motion(id))m->width=value;else if(auto* m=curve_motion(id))m->width=value;else if(auto* m=segmented_motion(id))m->width=value;else{error="Laser width kind is not connected";return false;}return true;}
bool LaserScene::set_angle(u32 id,float value){auto* p=manager.find(id);if(!p)return true;if(auto* m=moving_motion(id))m->angle=value;else if(auto* m=stationary_motion(id))m->angle=value;else if(auto* m=curve_motion(id))m->angle=value;else if(auto* m=segmented_motion(id))m->angle=value;else{error="Laser angle kind is not connected";return false;}return true;}
bool LaserScene::set_script_angular_velocity(u32 id,float value){auto* p=manager.find(id);if(!p)return true;if(p->kind==LaserKind::moving)static_cast<MovingLaser&>(*p).request.width=value;else if(p->kind==LaserKind::stationary){auto& laser=static_cast<StationaryLaser&>(*p);laser.request.angular_velocity=laser.motion.angular_velocity=value;}else if(p->kind==LaserKind::curved)static_cast<CurveLaser&>(*p).request.color=signed_bits(float_to_bits(value));else if(p->kind==LaserKind::segmented)static_cast<SegmentedLaser&>(*p).script_width=value;else{error="Laser angular velocity kind is not connected";return false;}return true;}
bool LaserScene::cancel_identifier(u32 id){while(auto* p=manager.find(id)){if(!p->cancel_all(0,false)){error=p->error;return false;}p->id=0;}return true;}
bool LaserScene::draw(AnmRenderer& renderer){error.clear();bool ok=true;manager.each([&](LaserObject& object){if(!ok)return;if(object.kind==LaserKind::moving){auto& laser=static_cast<MovingLaser&>(object);if(!laser.visual.draw_moving(laser.motion,renderer)){error=laser.visual.error;ok=false;}}else if(object.kind==LaserKind::stationary){auto& laser=static_cast<StationaryLaser&>(object);if(!laser.visual.draw_stationary(laser.motion,renderer)){error=laser.visual.error;ok=false;}}else if(object.kind==LaserKind::curved){auto& laser=static_cast<CurveLaser&>(object);if(!laser.visual.draw_curved(laser.motion,renderer)){error=laser.visual.error;ok=false;}}else if(object.kind==LaserKind::segmented)return;else{error="Laser draw kind is not connected";ok=false;}});return ok;}
}

namespace th15 {
u32 LaserScene::create_curved(const CurveLaserRequest& request){error.clear();if(manager.count()>=LaserManager::capacity)return 0;auto object=std::make_unique<CurveLaser>(*this,request);if(!object->initialize()){error=object->error;return 0;}return manager.insert(std::move(object));}
CurveLaserMotion* LaserScene::curve_motion(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::curved?&static_cast<CurveLaser*>(object)->motion:nullptr;}
LaserVisual* LaserScene::curve_visual(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::curved?&static_cast<CurveLaser*>(object)->visual:nullptr;}
LaserContact* LaserScene::curve_contact(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::curved?&static_cast<CurveLaser*>(object)->contact:nullptr;}
CurveProgram* LaserScene::curve_program(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::curved?&static_cast<CurveLaser*>(object)->program:nullptr;}
}

namespace th15 {
u32 LaserScene::create_segmented(const SegmentedLaserRequest& request){error.clear();if(manager.count()>=LaserManager::capacity)return 0;auto object=std::make_unique<SegmentedLaser>(request);auto* laser=object.get();const u32 generation=manager.insert(std::move(object));laser->id=request.id;return generation;}
MovingLaserMotion* LaserScene::segmented_motion(u32 id)noexcept{auto* object=manager.find(id);return object&&object->kind==LaserKind::segmented?&static_cast<SegmentedLaser*>(object)->motion:nullptr;}
}
