#include "../../cpp/game/ScreenMotionFrame.hpp"
#include "../../cpp/game/AnmOverlay.hpp"
#include "../../cpp/game/SessionRuntime.hpp"
#include "../../cpp/game/MenuCursor.hpp"
#include "../../cpp/game/TitleMainMenu.hpp"
#include "../../cpp/game/TitleSelectionMenu.hpp"
#include "../../cpp/game/TitleCharacterMenu.hpp"
#include "../../cpp/game/TitlePracticeMenu.hpp"
#include "../../cpp/game/ChapterCheckpoint.hpp"
#include "../../cpp/game/AnmCheckpoint.hpp"
#include "../../cpp/game/SessionInitialization.hpp"
#include "../../cpp/game/ChapterReward.hpp"
#include "../../cpp/game/PopupManager.hpp"
#include "../../cpp/game/TextRaster.hpp"
#include "../../cpp/game/Dialogue.hpp"
#include "../../cpp/game/MessageController.hpp"
#include "../../cpp/game/TextureImage.hpp"
#include "../../cpp/game/EnemyCallbacks.hpp"
#include "../../cpp/game/BossHud.hpp"
#include "../../cpp/game/StageAssets.hpp"
#include "../../cpp/game/StageDefinition.hpp"
#include "../../cpp/game/SpellCard.hpp"
#include "../../cpp/game/StageScene.hpp"
#include "../../cpp/game/StageVisibility.hpp"
#include "../../cpp/game/StageCamera.hpp"
#include "../../cpp/game/StageScript.hpp"
#include "../../cpp/game/StageResource.hpp"
#include "../../cpp/game/FrameScheduler.hpp"
#include "../../cpp/game/EnemyCollision.hpp"
#include "../../cpp/game/EnemyCombat.hpp"
#include "../../cpp/game/GameBattle.hpp"
#include "../../cpp/game/BattleFramePolicy.hpp"
#include "../../cpp/game/LaserDynamics.hpp"
#include "../../cpp/game/CurvePath.hpp"
#include "../../cpp/game/CurveGeometry.hpp"
#include "../../cpp/game/CurveMotion.hpp"
#include "../../cpp/game/CurveProgram.hpp"
#include "../../cpp/game/CurveCancellation.hpp"
#include "../../cpp/game/SegmentIntersection.hpp"
#include "../../cpp/game/LaserReflection.hpp"
#include "../../cpp/game/LaserScene.hpp"
#include "../../cpp/game/LaserContact.hpp"
#include "../../cpp/game/LineIntersection.hpp"
#include "../../cpp/game/Archive.hpp"
#include "../../cpp/game/AnmResource.hpp"
#include "../../cpp/game/ResourceCrypt.hpp"
#include "../../cpp/game/Rng.hpp"
#include "../../cpp/game/EclResource.hpp"
#include "../../cpp/game/Replay.hpp"
#include "../../cpp/game/Timer.hpp"
#include "../../cpp/game/GameConfig.hpp"
#include "../../cpp/game/AnmVariables.hpp"
#include "../../cpp/game/AnmVm.hpp"
#include "../../cpp/game/AnmRegistry.hpp"
#include "../../cpp/game/AnmManager.hpp"
#include "../../cpp/game/AnmCoordinates.hpp"
#include "../../cpp/game/AnmRenderer.hpp"
#include "../../cpp/game/AnmTransforms.hpp"
#include "../../cpp/game/AnmCamera.hpp"
#include "../../cpp/game/BulletSpriteSource.hpp"
#include "../../cpp/game/BulletVisual.hpp"
#include "../../cpp/game/ShtResource.hpp"
#include "../../cpp/game/Interpolation.hpp"
#include "../../cpp/game/EclContext.hpp"
#include "../../cpp/game/EclThreads.hpp"
#include "../../cpp/game/EnemyVariables.hpp"
#include "../../cpp/game/EnemyCommands.hpp"
#include "../../cpp/game/EnemyAnmHost.hpp"
#include "../../cpp/game/EnemyRuntime.hpp"
#include "../../cpp/game/EnemyManager.hpp"
#include "../../cpp/game/EnemyDamage.hpp"
#include "../../cpp/game/EnemyDeath.hpp"
#include "../../cpp/game/PositionInterpolation.hpp"
#include "../../cpp/game/BulletFormation.hpp"
#include "../../cpp/game/BulletState.hpp"
#include "../../cpp/game/BulletManager.hpp"
#include "../../cpp/game/BulletScene.hpp"
#include "../../cpp/game/PlayerCollision.hpp"
#include "../../cpp/game/PlayerAnmHost.hpp"
#include "../../cpp/game/DamageSources.hpp"
#include "../../cpp/game/PlayerShots.hpp"
#include "../../cpp/game/PlayerShooting.hpp"
#include "../../cpp/game/PlayerLife.hpp"
#include "../../cpp/game/PlayerOptions.hpp"
#include "../../cpp/game/PlayerBounds.hpp"
#include "../../cpp/game/ItemManager.hpp"
#include "../../cpp/game/ItemCollection.hpp"
#include "../../cpp/game/BombReisen.hpp"
#include "../../cpp/game/BombSanae.hpp"
#include "../../cpp/game/BombMarisa.hpp"
#include "../../cpp/game/BombReimu.hpp"
#include "../../cpp/game/BombCheckpoint.hpp"
#include "../../cpp/game/BattleCheckpoint.hpp"
#include "../../cpp/game/LaserManager.hpp"
#include "../../cpp/game/LaserMotion.hpp"
#include "../../cpp/game/LaserVisual.hpp"
#include "../../cpp/game/LaserCancellation.hpp"
#include "../../cpp/game/VectorMath.hpp"
#include <cstdlib>
using namespace th15;
#if defined(__EMSCRIPTEN__)
// Keep fixture export names while avoiding Emscripten's POSIX timer prototypes.
#define timer_create th15_test_timer_create
#define timer_delete th15_test_timer_delete
#endif
#define API(name) extern "C" __attribute__((export_name(#name)))
API(laser_motion_create) MovingLaserMotion* laser_motion_create(){return new MovingLaserMotion;}
API(laser_motion_delete) void laser_motion_delete(MovingLaserMotion* p){delete p;}
API(laser_motion_field) void* laser_motion_field(MovingLaserMotion* p,i32 field){switch(field){case 0:return &p->position;case 1:return &p->velocity;case 2:return &p->angle;case 3:return &p->length;case 4:return &p->width;case 5:return &p->speed;case 6:return &p->traveled;case 7:return &p->maximum_length;case 8:return &p->end_distance;case 9:return &p->outside_delay;case 10:return &p->invulnerability;default:return &p->animation_age;}}
API(laser_motion_velocity) void laser_motion_velocity(MovingLaserMotion* p){p->set_velocity();}
API(laser_motion_update) u32 laser_motion_update(MovingLaserMotion* p,float rate){const bool ended=p->advance(rate);if(!ended)p->finish_frame(rate);return ended;}
API(laser_stationary_create) StationaryLaserMotion* laser_stationary_create(){return new StationaryLaserMotion;}
API(laser_stationary_delete) void laser_stationary_delete(StationaryLaserMotion* p){delete p;}
API(laser_stationary_field) void* laser_stationary_field(StationaryLaserMotion* p,i32 field){switch(field){case 0:return &p->position;case 1:return &p->drift;case 2:return &p->angle;case 3:return &p->length;case 4:return &p->width;case 5:return &p->speed;case 6:return &p->angular_velocity;case 7:return &p->maximum_length;case 8:return &p->target_width;case 9:return &p->state;case 10:return &p->age;case 11:return &p->delay;case 12:return &p->warmup;case 13:return &p->active;default:return &p->fade;}}
API(laser_stationary_update) u32 laser_stationary_update(StationaryLaserMotion* p,float rate,const Vec3* anchor){const bool ended=p->advance(rate,anchor);if(!ended)p->finish_frame(rate);return ended;}
struct LaserManagerFixture {
    struct Laser:LaserObject {
        LaserManagerFixture& owner;i32 result=0;
        explicit Laser(LaserManagerFixture& f):owner(f){}
        bool update(float rate,bool& finished)override{owner.push({1,id,bits(rate)});finished=result!=0;owner.spawn(id);return true;}
        bool update_visual(float rate,bool paused)override{owner.push({2,id,bits(rate),u32(paused)});return true;}
        bool retire()override{owner.push({3,id});return true;}
        i32 cancel_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward,bool flag)override{owner.push({4,id,bits(p.x),bits(p.y),bits(p.z),bits(size.x),bits(size.y),bits(angle),u32(reward),u32(flag)});owner.spawn(id);return result;}
        i32 query_rectangle(const Vec3& p,const Vec2& size,float angle,i32 a,i32 b,i32 c)override{owner.push({5,id,bits(p.x),bits(p.y),bits(p.z),bits(size.x),bits(size.y),bits(angle),u32(a),u32(b),u32(c)});owner.spawn(id);return result;}
        i32 cancel_circle(const Vec3& p,float radius,i32 reward,bool flag)override{owner.push({6,id,bits(p.x),bits(p.y),bits(p.z),bits(radius),u32(reward),u32(flag)});owner.spawn(id);return result;}
        bool cancel_all(i32 reward,bool flag)override{owner.push({7,id,u32(reward),u32(flag)});owner.spawn(id);return true;}
        i32 query_circle(const Vec3& p,float radius)override{owner.push({8,id,bits(p.x),bits(p.y),bits(p.z),bits(radius)});owner.spawn(id);return result;}
    };
    LaserManager manager;std::vector<u32> events;u32 spawn_id=0,spawn_remaining=0;void spawn(u32 id){if(id!=spawn_id||!spawn_remaining)return;spawn_remaining--;auto p=std::make_unique<Laser>(*this);p->flags=1;manager.insert(std::move(p));}static u32 bits(float f){u32 x;std::memcpy(&x,&f,4);return x;}void push(std::initializer_list<u32> e){events.insert(events.end(),e);}
};
API(laser_manager_create) LaserManagerFixture* laser_manager_create(){return new LaserManagerFixture;}
API(laser_manager_delete) void laser_manager_delete(LaserManagerFixture* p){delete p;}
API(laser_manager_add) u32 laser_manager_add(LaserManagerFixture* p){return p->manager.insert(std::make_unique<LaserManagerFixture::Laser>(*p));}
API(laser_manager_field) void* laser_manager_field(LaserManagerFixture* p,u32 id,i32 field){auto* o=static_cast<LaserManagerFixture::Laser*>(p->manager.find(id));if(!o)return nullptr;switch(field){case 0:return &o->flags;case 1:return &o->state;case 2:return &o->age;case 3:return &o->outside_delay;case 4:return &o->outside_count;default:return &o->result;}}
API(laser_manager_generation) u32* laser_manager_generation(LaserManagerFixture* p){return &p->manager.generation;}
API(laser_manager_count) u32 laser_manager_count(LaserManagerFixture* p){return p->manager.count();}
API(laser_manager_update) u32 laser_manager_update(LaserManagerFixture* p,float rate,u32 flags){p->events.clear();return p->manager.update(rate,flags);}
API(laser_manager_query) i32 laser_manager_query(LaserManagerFixture* p,i32 kind,const Vec3* position,const Vec2* size,float angle,float radius,i32 a,i32 b,i32 c){p->events.clear();switch(kind){case 0:return p->manager.cancel_rectangle(*position,*size,angle,a);case 1:return p->manager.query_rectangle(*position,*size,angle,a,b,c);case 2:return p->manager.cancel_circle(*position,radius,a,b!=0);case 3:return p->manager.cancel_all(a,b!=0);case 4:return p->manager.clear_with_rewards()?0:-1;default:return p->manager.query_circle(*position,radius);}}
API(laser_manager_events) u32* laser_manager_events(LaserManagerFixture* p){return p->events.data();}
API(laser_manager_event_count) u32 laser_manager_event_count(LaserManagerFixture* p){return p->events.size();}
API(laser_manager_spawn_on) void laser_manager_spawn_on(LaserManagerFixture* p,u32 id,u32 count){p->spawn_id=id;p->spawn_remaining=count;}
struct ItemCollectionFixture:ItemCollectionHost {
    PlayerLifeSession player;ItemScoreState score;PlayerMotion motion;ItemState item;std::vector<u32> events;ItemCollection collection;
    ItemCollectionFixture(i32 character):collection(player,score,motion,*this,character){}
    void event(u32 kind,i32 a=0,i32 b=0,const Vec3& position={}){u32 xyz[3];std::memcpy(xyz,&position,12);events.insert(events.end(),{kind,u32(a),u32(b),xyz[0],xyz[1],xyz[2]});}
    bool options_changed()override{event(0);return true;}bool notice(i32 id)override{event(1,id);return true;}
    bool sound(i32 id,bool queued)override{event(queued?3:2,id);return true;}
    bool popup(const Vec3& p,i32 value,u32 color)override{event(4,value,i32(color),p);return true;}
    bool life_hud(i32 lives,i32 pieces)override{event(5,lives,pieces);return true;}bool bomb_hud(i32 bombs,i32 pieces)override{event(6,bombs,pieces);return true;}
};
API(item_collection_create) ItemCollectionFixture* item_collection_create(i32 character){return new ItemCollectionFixture(character);}
API(item_collection_delete) void item_collection_delete(ItemCollectionFixture* p){delete p;}
API(item_collection_field) void* item_collection_field(ItemCollectionFixture* p,i32 field){switch(field){case 0:return &p->player;case 1:return &p->score;case 2:return &p->motion.position;default:return &p->item;}}
API(item_collection_award) u32 item_collection_award(ItemCollectionFixture* p,i32 kind){p->events.clear();p->collection.error.clear();p->item.kind=kind;return p->collection.award(p->item);}
API(item_collection_events) u32* item_collection_events(ItemCollectionFixture* p){return p->events.data();}
API(item_collection_event_count) u32 item_collection_event_count(ItemCollectionFixture* p){return p->events.size();}
API(item_collection_error) const char* item_collection_error(ItemCollectionFixture* p){return p->collection.error.c_str();}
API(allocate) void* allocate(u32 n){return std::malloc(n?n:1);}
API(release) void release(void* p){std::free(p);}
struct PlayerShootingFixture {PlayerShooting state;std::vector<i32> events;PlayerShootingFixture(){state.fire=[this](i32 shot,i32 continuous){events.push_back(shot);events.push_back(continuous);return true;};}};
API(player_shooting_create) PlayerShootingFixture* player_shooting_create(){return new PlayerShootingFixture;}
API(player_shooting_delete) void player_shooting_delete(PlayerShootingFixture* p){delete p;}
API(player_shooting_timer) Timer* player_shooting_timer(PlayerShootingFixture* p,i32 i){return i?&p->state.continuous:&p->state.shot;}
API(player_shooting_aux) i32 player_shooting_aux(PlayerShootingFixture* p,i32 value,i32 active){p->state.auxiliary=value;p->state.auxiliary_active=active!=0;return 0;}
API(player_shooting_aux_value) i32 player_shooting_aux_value(PlayerShootingFixture* p,i32 active){return active?p->state.auxiliary_active:p->state.auxiliary;}
API(player_shooting_step) i32 player_shooting_step(PlayerShootingFixture* p,i32 state,i32 held,float rate){return p->state.update(state,held!=0,rate);}
API(player_shooting_count) i32 player_shooting_count(PlayerShootingFixture* p){return i32(p->events.size()/2);}
API(player_shooting_events) i32* player_shooting_events(PlayerShootingFixture* p){return p->events.data();}
API(damage_sources_create) DamageSources* damage_sources_create(){return new DamageSources;}
struct AnmManagerFixture {Rng random;AnmEnvironment environment;AnmManager manager{random,environment};};
struct LaserVisualFixture {AnmManager& manager;LaserVisual visual;LaserVisualFixture(AnmManagerFixture& a,i32 resource):manager(a.manager),visual(a.manager,a.random,resource){}};
API(laser_visual_create) LaserVisualFixture* laser_visual_create(AnmManagerFixture* a,i32 resource){return new LaserVisualFixture(*a,resource);}
API(laser_visual_delete) void laser_visual_delete(LaserVisualFixture* p){delete p;}
API(laser_visual_initialize) u32 laser_visual_initialize(LaserVisualFixture* p,u32 moving,i32 type,i32 color){p->manager.rate=1;return moving?p->visual.initialize_moving(type,color):p->visual.initialize_stationary(type,color);}
API(laser_visual_vm) AnmVm* laser_visual_vm(LaserVisualFixture* p,i32 kind){return kind==0?&p->visual.body:kind==1?&p->visual.head:&p->visual.tip;}
API(laser_visual_update) u32 laser_visual_update(LaserVisualFixture* p,float width,float length,float traveled,u32 moving,float rate){p->manager.rate=rate;return p->visual.update(width,length,traveled,moving!=0);}
API(laser_visual_error) const char* laser_visual_error(LaserVisualFixture* p){return p->visual.error.c_str();}
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
struct ItemManagerFixture {AnmManager& animations;EffectManager effects;ItemManager manager;Rng& game;Rng& visual;std::vector<i32> sounds;ItemCollectionFixture collection;PlayerBounds bounds;ItemFrameContext context;ItemManagerFixture(AnmManager& a,Rng& g,Rng& v,i32 id,i32 character=0):animations(a),effects(a,8),manager(a,effects,id),game(g),visual(v),collection(character),context{collection.motion,bounds,collection.score,collection.collection,v}{context.character=character;manager.sound=[this](i32 id){sounds.push_back(id);collection.event(3,id);return true;};manager.immediate_sound=[this](i32 id){collection.event(2,id);return true;};}};
API(item_manager_create) ItemManagerFixture* item_manager_create(AnmManagerFixture* a,i32 id){return new ItemManagerFixture(a->manager,a->environment.game_rng,a->random,id);}
API(item_manager_delete) void item_manager_delete(ItemManagerFixture* p){delete p;}
API(item_manager_spawn) u32 item_manager_spawn(ItemManagerFixture* p,i32 kind,float x,float y,float z,float angle,float speed){return p->manager.spawn(kind,{x,y,z},angle,speed);}
API(item_manager_state) ItemState* item_manager_state(ItemManagerFixture* p,u32 id){auto* s=p->manager.find(id);return s?&s->state:nullptr;}
API(item_manager_vm) AnmVm* item_manager_vm(ItemManagerFixture* p,u32 id,i32 arrow){auto* s=p->manager.find(id);return s?(arrow?s->arrow.get():s->body.get()):nullptr;}
API(item_manager_field) void* item_manager_field(ItemManagerFixture* p,i32 field){switch(field){case 0:return &p->manager.requests;case 1:return &p->manager.cancel_density;case 2:return &p->manager.cancel_stamp;case 3:return &p->manager.alternating_pieces;case 4:return &p->manager.active_count;case 5:return &p->manager.motion_scale;case 6:return &p->game;case 7:return &p->visual;case 8:return &p->collection.player;case 9:return &p->collection.score;case 10:return &p->collection.motion.position;case 11:return &p->bounds.item_near;case 12:return &p->bounds.graze;case 13:return &p->bounds.item_full;case 14:return &p->context.player_state;case 15:return &p->context.bomb_state;case 16:return &p->context.bomb_frame;case 17:return &p->context.held;case 18:return &p->context.attraction_speed;default:return nullptr;}}
API(item_frame_create) ItemManagerFixture* item_frame_create(AnmManagerFixture* a,i32 resource,i32 character){return new ItemManagerFixture(a->manager,a->environment.game_rng,a->random,resource,character);}
API(item_frame_update) u32 item_frame_update(ItemManagerFixture* p,float rate,u32 hud_collect){p->context.rate=rate;p->context.hud_collect=hud_collect!=0;p->collection.events.clear();return p->manager.update(p->context);}
API(item_frame_events) const u32* item_frame_events(ItemManagerFixture* p){return p->collection.events.data();}
API(item_frame_event_count) u32 item_frame_event_count(ItemManagerFixture* p){return p->collection.events.size();}
API(item_manager_error) const char* item_manager_error(ItemManagerFixture* p){return p->manager.error.c_str();}
API(item_manager_free) u32 item_manager_free(ItemManagerFixture* p,i32 cancel){return p->manager.free_head(cancel!=0);}
API(item_manager_release) i32 item_manager_release(ItemManagerFixture* p,u32 id){return p->manager.release(id);}
API(item_manager_reset) i32 item_manager_reset(ItemManagerFixture* p){return p->manager.reset();}
API(item_manager_falling) i32 item_manager_falling(ItemManagerFixture* p,u32 id){return p->manager.falling(id);}
API(item_manager_sounds) i32* item_manager_sounds(ItemManagerFixture* p){return p->sounds.data();}
API(item_manager_sound_count) u32 item_manager_sound_count(ItemManagerFixture* p){return u32(p->sounds.size());}
struct PlayerLifecycleFixtureHost:PlayerLifecycleHost {std::vector<u32> events;bool can_bomb=false;static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}bool push(std::initializer_list<u32> data){events.insert(events.end(),data);return true;}
bool allow_bomb()override{push({1});return can_bomb;}bool begin_bomb()override{return push({2});}bool move()override{return push({3});}
bool cancel_lasers_near(const Vec3& p,float r,i32 kind,bool flag)override{return push({4,bits(p.x),bits(p.y),bits(p.z),bits(r),u32(kind),u32(flag)});}
bool cancel_bullets_near(const Vec3& p,float r,i32 kind)override{return push({5,bits(p.x),bits(p.y),bits(p.z),bits(r),u32(kind)});}
bool cancel_lasers(i32 kind,bool flag)override{return push({6,u32(kind),u32(flag)});}
bool item(i32 type,const Vec3& p,float angle,float speed)override{return push({7,u32(type),bits(p.x),bits(p.y),bits(p.z),bits(angle),bits(speed)});}
bool options_changed()override{return push({8});}bool game_over()override{return push({9});}
bool respawn_damage(const Vec3& p,float r,float growth,i32 frames,i32 damage)override{return push({10,bits(p.x),bits(p.y),bits(p.z),bits(r),bits(growth),u32(frames),u32(damage)});}
bool bomb_hud(i32 bombs,i32 pieces)override{return push({11,u32(bombs),u32(pieces)});}};
struct PlayerLifeFixture {PlayerMotion motion;PlayerAnmHost visuals;EffectManager effects;PlayerLifeSession session;PlayerSpellStatus spell;PlayerLife life;PlayerLifecycleFixtureHost host;float rate=1;std::vector<i32> events;PlayerLifeFixture(AnmManager& a,i32 id):visuals(a,id,8),effects(a,8),life(motion,visuals,effects,session,spell){visuals.initialize();life.sound=[this](i32 id){events.push_back(1);events.push_back(id);events.push_back(0);return true;};life.life_hud=[this](i32 lives,i32 pieces){events.push_back(2);events.push_back(lives);events.push_back(pieces);return true;};}};
struct BombReisenFixture:BombHost {
    PlayerLifeFixture player;PlayerBounds bounds;ShtHeader header;BombEnemyState enemies;BombContext context;BombReisen bomb;std::vector<u32> events;
    BombReisenFixture(AnmManager& a,i32 id):player(a,id),context{a,player.motion,player.life,bounds,header,player.session,player.spell,enemies,*this,id},bomb(context){}
    static u32 bits(float f){u32 x;std::memcpy(&x,&f,4);return x;}
    bool sound(i32 id,bool queued)override{events.insert(events.end(),{1,u32(id),u32(queued)});return true;}
    bool bomb_hud(i32 bombs,i32 pieces)override{events.insert(events.end(),{2,u32(bombs),u32(pieces)});return true;}
    bool shake(const ScreenShakeSpec& s)override{events.insert(events.end(),{5,u32(s.amplitude),u32(s.ramp),u32(s.hold),u32(s.fade)});return true;}
    bool nudge(const ScreenNudgeSpec& s)override{events.insert(events.end(),{8,u32(s.duration),u32(s.first),u32(s.last)});return true;}
    bool cancel_bullets_rectangle(const Vec3& p,const Vec2& s,float angle,i32 reward)override{events.insert(events.end(),{6,bits(p.x),bits(p.y),bits(p.z),bits(s.x),bits(s.y),bits(angle),u32(reward)});return true;}
    bool cancel_lasers_rectangle(const Vec3& p,const Vec2& s,float angle,i32 reward,bool flag)override{events.insert(events.end(),{7,bits(p.x),bits(p.y),bits(p.z),bits(s.x),bits(s.y),bits(angle),u32(reward),u32(flag)});return true;}
    bool cancel_bullets(const Vec3& p,float radius,i32 reward)override{events.insert(events.end(),{3,bits(p.x),bits(p.y),bits(p.z),bits(radius),u32(reward)});return true;}
    bool cancel_lasers(const Vec3& p,float radius,i32 reward,bool flag)override{events.insert(events.end(),{4,bits(p.x),bits(p.y),bits(p.z),bits(radius),u32(reward),u32(flag)});return true;}
};
API(bomb_reisen_create) BombReisenFixture* bomb_reisen_create(AnmManagerFixture* a,i32 resource){return new BombReisenFixture(a->manager,resource);}
API(bomb_reisen_delete) void bomb_reisen_delete(BombReisenFixture* p){delete p;}
API(bomb_reisen_field) void* bomb_reisen_field(BombReisenFixture* p,i32 field){switch(field){case 0:return &p->bomb.position;case 1:return &p->bomb.angle;case 2:return &p->bomb.age;case 3:return &p->player.session;case 4:return &p->player.spell;case 5:return &p->player.life.state;case 6:return &p->player.life.age;case 7:return &p->player.life.invulnerability;case 8:return &p->player.motion.position;case 9:return &p->header.hitbox;case 10:return &p->bounds.hit_half_size;case 11:return &p->enemies.uses;case 12:return &p->enemies.chain;case 13:return &p->bomb.barrier;case 14:return &p->bomb.aura;case 15:return &p->bomb.charges;default:return nullptr;}}
API(bomb_reisen_setup) void bomb_reisen_setup(BombReisenFixture* p,u32 hud,u32 collect,u32 enemies){p->context.hud_available=hud!=0;p->context.hud_collect=collect!=0;p->enemies.available=enemies!=0;}
API(bomb_reisen_allowed) u32 bomb_reisen_allowed(BombReisenFixture* p){return p->bomb.allowed();}
API(bomb_reisen_begin) u32 bomb_reisen_begin(BombReisenFixture* p){p->events.clear();return p->bomb.begin();}
API(bomb_reisen_update) u32 bomb_reisen_update(BombReisenFixture* p,float rate){p->events.clear();return p->bomb.update(rate);}
API(bomb_reisen_error) const char* bomb_reisen_error(BombReisenFixture* p){return p->bomb.error.c_str();}
API(bomb_reisen_events) const u32* bomb_reisen_events(BombReisenFixture* p){return p->events.data();}
API(bomb_reisen_event_count) u32 bomb_reisen_event_count(BombReisenFixture* p){return p->events.size();}
struct BombSanaeFixture:BombReisenFixture {
    DamageSources damage;Rng& random;BombSanae sanae;
    BombSanaeFixture(AnmManager& a,Rng& r,Rng& v,i32 resource):BombReisenFixture(a,resource),random(r),sanae(context,player.effects,r,v,damage){}
};
API(bomb_sanae_create) BombSanaeFixture* bomb_sanae_create(AnmManagerFixture* a,i32 resource){return new BombSanaeFixture(a->manager,a->environment.game_rng,a->random,resource);}
API(bomb_sanae_delete) void bomb_sanae_delete(BombSanaeFixture* p){delete p;}
API(bomb_sanae_field) void* bomb_sanae_field(BombSanaeFixture* p,i32 field){switch(field){case 0:return &p->sanae.position;case 1:return &p->sanae.angle;case 2:return &p->sanae.age;case 13:return &p->sanae.field;case 14:return &p->sanae.aura;case 16:return p->damage.sources.data();case 17:return &p->damage.cursor;case 18:return p->player.effects.handles.data();case 19:return &p->player.effects.cursor;case 20:return &p->random;default:return bomb_reisen_field(p,field);}}
API(bomb_sanae_begin) u32 bomb_sanae_begin(BombSanaeFixture* p){p->events.clear();return p->sanae.begin();}
API(bomb_sanae_update) u32 bomb_sanae_update(BombSanaeFixture* p,float rate){p->events.clear();return p->sanae.update(rate);}
API(bomb_sanae_error) const char* bomb_sanae_error(BombSanaeFixture* p){return p->sanae.error.c_str();}
struct BombMarisaFixture:BombReisenFixture {
    DamageSources damage;BombMarisa marisa;
    BombMarisaFixture(AnmManager& a,i32 resource):BombReisenFixture(a,resource),marisa(context,damage){}
};
API(bomb_marisa_create) BombMarisaFixture* bomb_marisa_create(AnmManagerFixture* a,i32 resource){return new BombMarisaFixture(a->manager,resource);}
API(bomb_marisa_delete) void bomb_marisa_delete(BombMarisaFixture* p){delete p;}
API(bomb_marisa_field) void* bomb_marisa_field(BombMarisaFixture* p,i32 field){switch(field){case 0:return &p->marisa.position;case 1:return &p->marisa.angle;case 2:return &p->marisa.age;case 13:return &p->marisa.beam;case 14:return &p->marisa.aura;case 16:return p->damage.sources.data();case 17:return &p->damage.cursor;case 21:return &p->player.motion.behavior_flags;case 22:return &p->player.motion.movement_scale;case 23:return &p->player.motion.velocity;default:return bomb_reisen_field(p,field);}}
API(bomb_marisa_begin) u32 bomb_marisa_begin(BombMarisaFixture* p){p->events.clear();return p->marisa.begin();}
API(bomb_marisa_update) u32 bomb_marisa_update(BombMarisaFixture* p,float rate){p->events.clear();return p->marisa.update(rate);}
API(bomb_marisa_error) const char* bomb_marisa_error(BombMarisaFixture* p){return p->marisa.error.c_str();}
struct BombReimuFixture:BombReisenFixture {
    DamageSources damage;EnemyWorldState targets;std::array<EnemyState,4> target_storage;BombReimu reimu;
    BombReimuFixture(AnmManager& a,i32 resource):BombReisenFixture(a,resource),reimu(context,damage,&targets){}
};
API(bomb_reimu_create) BombReimuFixture* bomb_reimu_create(AnmManagerFixture* a,i32 resource){return new BombReimuFixture(a->manager,resource);}
API(bomb_reimu_delete) void bomb_reimu_delete(BombReimuFixture* p){delete p;}
API(bomb_reimu_field) void* bomb_reimu_field(BombReimuFixture* p,i32 field){switch(field){case 0:return &p->reimu.position;case 1:return &p->reimu.angle;case 2:return &p->reimu.age;case 14:return &p->reimu.aura;case 16:return p->damage.sources.data();case 17:return &p->damage.cursor;default:return bomb_reisen_field(p,field);}}
API(bomb_reimu_orb) void* bomb_reimu_orb(BombReimuFixture* p,i32 index,i32 field){if(index<0||index>=8)return nullptr;auto& o=p->reimu.orbs[index];switch(field){case 0:return &o.animation;case 1:return &o.motion;case 2:return &o.active;case 3:return &o.age;case 4:return &o.displacement;case 5:return &o.target;case 6:return &o.index;case 7:return &o.damage_source;default:return nullptr;}}
API(bomb_reimu_target) void bomb_reimu_target(BombReimuFixture* p,i32 index,u32 id,u32 flags,float x,float y){if(index<0||index>=4)return;auto& e=p->target_storage[index];if(!e.id)p->targets.enemies.push_back(&e);e.id=id;e.flags=flags;e.motion.position={x,y,0};}
API(bomb_reimu_begin) u32 bomb_reimu_begin(BombReimuFixture* p){p->events.clear();return p->reimu.begin();}
API(bomb_reimu_update) u32 bomb_reimu_update(BombReimuFixture* p,float rate){p->events.clear();BombController& bomb=p->reimu;return bomb.update(rate);}
API(bomb_reimu_error) const char* bomb_reimu_error(BombReimuFixture* p){return p->reimu.error.c_str();}
API(bomb_reimu_stop) u32 bomb_reimu_stop(BombReimuFixture* p){p->events.clear();return p->reimu.stop();}
struct ScreenShakeFixture {Rng random;ScreenShake shake{random,{}};};
API(screen_shake_create) ScreenShakeFixture* screen_shake_create(i32 amplitude,i32 ramp,i32 hold,i32 fade){auto* p=new ScreenShakeFixture;p->shake.spec={amplitude,ramp,hold,fade};return p;}
API(screen_shake_delete) void screen_shake_delete(ScreenShakeFixture* p){delete p;}
API(screen_shake_field) void* screen_shake_field(ScreenShakeFixture* p,i32 field){switch(field){case 0:return &p->random;case 1:return &p->shake.age;case 2:return &p->shake.world_offset;default:return &p->shake.screen_offset;}}
API(screen_shake_update) i32 screen_shake_update(ScreenShakeFixture* p,u32 shutdown,u32 game,u32 flags,float rate,float scale){return p->shake.update({shutdown!=0,game!=0,flags,rate,scale});}
struct ScreenNudgeFixture {Rng random;ScreenNudge nudge{random,{}};};
API(screen_nudge_create) ScreenNudgeFixture* screen_nudge_create(i32 duration,i32 first,i32 last){auto* p=new ScreenNudgeFixture;p->nudge.spec={duration,first,last};return p;}
API(screen_nudge_delete) void screen_nudge_delete(ScreenNudgeFixture* p){delete p;}
API(screen_nudge_field) void* screen_nudge_field(ScreenNudgeFixture* p,i32 field){switch(field){case 0:return &p->random;case 1:return &p->nudge.age;case 2:return &p->nudge.world_offset;default:return &p->nudge.screen_offset;}}
API(screen_nudge_update) i32 screen_nudge_update(ScreenNudgeFixture* p,u32 shutdown,u32 game,u32 flags,float rate,float scale){return p->nudge.update({shutdown!=0,game!=0,flags,rate,scale});}
API(player_life_create) PlayerLifeFixture* player_life_create(AnmManagerFixture* a,i32 id){return new PlayerLifeFixture(a->manager,id);}
API(player_life_delete) void player_life_delete(PlayerLifeFixture* p){delete p;}
API(player_life_field) void* player_life_field(PlayerLifeFixture* p,i32 field){switch(field){case 0:return &p->life.state;case 1:return &p->life.age;case 2:return &p->life.invulnerability;case 3:return &p->session;case 4:return &p->spell;case 5:return &p->motion.behavior_flags;case 6:return &p->motion.position;case 7:return p->effects.handles.data();case 8:return &p->effects.cursor;case 9:return &p->motion.option_count;default:return nullptr;}}
API(player_life_root) AnmVm* player_life_root(PlayerLifeFixture* p){return &p->visuals.root;}
API(player_life_option_handles) void* player_life_option_handles(PlayerLifeFixture* p){return p->visuals.option_handles.data();}
API(player_life_option_active) i32* player_life_option_active(PlayerLifeFixture* p,i32 i){return &p->motion.options[u32(i)].active;}
API(player_life_transition) i32 player_life_transition(PlayerLifeFixture* p,i32 kind){if(kind==0)return p->life.hit();if(kind==1)return p->life.commit_death();p->life.cancel_death();return 1;}
API(player_life_error) const char* player_life_error(PlayerLifeFixture* p){return p->life.error.c_str();}
API(player_life_events) i32* player_life_events(PlayerLifeFixture* p){return p->events.data();}
API(player_life_count) i32 player_life_count(PlayerLifeFixture* p){return i32(p->events.size()/3);}
API(effects_reserve) i32 effects_reserve(PlayerLifeFixture* p){return p->effects.reserve();}
struct PlayerBoundsFixture {PlayerMotion motion;Timer age;AnmVm vm;PlayerBounds bounds;};
API(player_bounds_create) PlayerBoundsFixture* player_bounds_create(){return new PlayerBoundsFixture;}
API(player_bounds_delete) void player_bounds_delete(PlayerBoundsFixture* p){delete p;}
API(player_bounds_field) void* player_bounds_field(PlayerBoundsFixture* p,i32 field){switch(field){case 0:return &p->motion.position;case 1:return &p->bounds.hit_half_size;case 2:return &p->bounds.item_half_size;case 3:return &p->bounds.graze_half_size;case 4:return &p->bounds.hit;case 5:return &p->bounds.item_near;case 6:return &p->bounds.graze;case 7:return &p->bounds.item_full;case 8:return &p->bounds.enlargement;case 9:return &p->motion.enlargement;case 10:return &p->motion.behavior_flags;case 11:return &p->age;case 12:return &p->vm.visual.flags;case 13:return &p->vm.visual.scale;default:return nullptr;}}
API(player_bounds_step) void player_bounds_step(PlayerBoundsFixture* p,float rate){p->bounds.update(p->motion,p->age,p->vm,rate);}
API(player_lifecycle_field) void* player_lifecycle_field(PlayerLifeFixture* p,i32 i){switch(i){case 0:return &p->life.last_death_position;case 1:return &p->motion.position_fixed;case 2:return &p->rate;default:return &p->session;}}
API(player_lifecycle_step) i32 player_lifecycle_step(PlayerLifeFixture* p,u32 pressed,float rate,i32 hud,i32 can){p->rate=rate;p->host.can_bomb=can!=0;return p->life.advance_state(pressed,p->rate,hud!=0,p->host);}
API(player_lifecycle_count) u32 player_lifecycle_count(PlayerLifeFixture* p){return u32(p->host.events.size());}
API(player_lifecycle_events) u32* player_lifecycle_events(PlayerLifeFixture* p){return p->host.events.data();}
#include "PlayerFixture.hpp"
API(player_create) PlayerFixture* player_create(ShtResource* s,AnmManagerFixture* a,i32 character){return new PlayerFixture(*s,a->manager,a->environment.game_rng,a->random,character);}
API(player_delete) void player_delete(PlayerFixture* p){delete p;}
API(player_initialize) i32 player_initialize(PlayerFixture* p){return p->player.initialize();}
API(player_reset_for_stage) i32 player_reset_for_stage(PlayerFixture* p){return p->player.reset_for_stage();}
API(player_error) const char* player_error(PlayerFixture* p){return p->player.error.c_str();}
API(player_update) i32 player_update(PlayerFixture* p,u32 held,u32 pressed,u32 flags,u32 game_flags,float rate,float dx,float dy,float dz){p->rate=rate;PlayerFrameContext c;c.held=held;c.pressed=pressed;c.focus_allowed=flags&1;c.enemy_present=flags&2;c.hud_available=flags&4;c.hud_bomb_active=flags&8;c.game_flags=game_flags;c.background_delta={dx,dy,dz};return p->player.update(c,p->rate);}
API(player_options_update) i32 player_options_update(PlayerFixture* p){return p->player.configure_options();}
API(player_hit) void player_hit(PlayerFixture* p){p->player.hit();}
API(player_contact) i32 player_contact(PlayerFixture* p,u32 kind,float x,float y,float a,float b,u32 graze){auto& v=p->player;auto& collision=v.frame.collision;return i32(kind==0?collision.rectangle({x,y},{a,b},graze,&v):kind==1?collision.circle({x,y},a,graze,&v):collision.laser({x,y},0,a,b,graze,&v));}
API(player_root) AnmVm* player_root(PlayerFixture* p){return &p->player.visuals.root;}
API(player_field) void* player_field(PlayerFixture* p,i32 f){auto& v=p->player;switch(f){case 0:return &p->session;case 1:return &v.life.state;case 2:return &v.life.age;case 3:return &v.life.invulnerability;case 4:return &v.frame.input_age;case 5:return &v.frame.ready_age;case 6:return &v.frame.shooting.shot;case 7:return &v.frame.shooting.continuous;case 8:return &v.motion.behavior_flags;case 9:return &v.motion.position;case 10:return &v.motion.position_fixed;case 11:return &v.motion.last_step;case 12:return &v.motion.step;case 13:return &v.motion.velocity;case 14:return &v.motion.last_direction;case 15:return &v.motion.normal_speed;case 16:return &v.resource.header;case 17:return &v.motion.focus;case 18:return v.damage.sources.data();case 19:return v.shots.shots.data();case 20:return &v.damage.cursor;case 21:return &v.motion.option_count;case 22:return v.shot_context.laser_power.data();case 23:return &p->game_random;case 24:return &p->visual_random;case 25:return &p->rate;case 26:return &v.life.last_death_position;case 27:return &v.motion.direction;case 28:return &v.motion.option_follow_percentage;case 29:return &v.motion.collapse_frame;case 30:return &v.frame.bounds.hit;case 31:return &v.frame.bounds.item_near;case 32:return &v.frame.bounds.graze;case 33:return &v.frame.bounds.item_full;case 34:return &v.frame.bounds.enlargement;case 35:return &v.motion.enlargement;case 36:return &v.motion.movement_scale;case 37:return &v.motion.external_velocity;case 38:return &v.motion.barrier_timer;case 39:return v.visuals.option_handles.data();case 40:return &v.visuals.focus_handle;case 41:return &v.visuals.barrier_handle;case 42:return &v.frame.bounds.hit_half_size;case 43:return &v.frame.bounds.item_half_size;case 44:return &v.frame.bounds.graze_half_size;case 45:return &v.options.power_level;case 46:return &v.options.auxiliary;case 47:return v.shots.reward_counters.data();case 48:return p->effects.handles.data();case 49:return &p->effects.cursor;default:return nullptr;}}
API(player_option_field) void* player_option_field(PlayerFixture* p,i32 i,i32 f){auto& o=p->player.motion.options[u32(i)];switch(f){case 0:return &o.active;case 1:return &o.target;case 2:return &o.position;case 3:return &o.normal_offset;case 4:return &o.focus_offset;case 5:return &o.snap;case 6:return &o.index;default:return nullptr;}}
API(player_service_count) u32 player_service_count(PlayerFixture* p){return u32(p->host.events.size());}
API(player_services) u32* player_services(PlayerFixture* p){return p->host.events.data();}
API(player_sound_count) u32 player_sound_count(PlayerFixture* p){return u32(p->sounds.size());}
API(player_sounds) i32* player_sounds(PlayerFixture* p){return p->sounds.data();}
API(player_life_event_count) u32 player_life_event_count(PlayerFixture* p){return u32(p->life_events.size());}
API(player_life_events_data) i32* player_life_events_data(PlayerFixture* p){return p->life_events.data();}
API(player_audio_ids) i32* player_audio_ids(PlayerFixture* p){return p->audio_ids.data();}
API(player_audio_values) i32* player_audio_values(PlayerFixture* p){return p->audio_values.data();}
struct PlayerShotItemEvent {i32 kind;Vec3 position;float angle,speed;};
struct PlayerShotsFixture {PlayerMotion motion;PlayerShotContext context{motion};DamageSources damage;Rng& random;Rng& visual_random;PlayerShots shots;std::vector<i32> sounds;std::vector<PlayerShotItemEvent> items;EnemyWorldState world;std::array<EnemyState,8> targets;PlayerShotsFixture(ShtResource& s,AnmManager& a,Rng& game,Rng& visual,i32 id):random(game),visual_random(visual),shots(s,context,damage,a,random,visual_random,id,8){shots.sound=[this](i32 id,float,PlayerShots::SoundAction action){sounds.push_back(id);sounds.push_back(i32(action));return true;};shots.item=[this](i32 kind,const Vec3& p,float angle,float speed){items.push_back({kind,p,angle,speed});return true;};}};
API(player_shots_create) PlayerShotsFixture* player_shots_create(ShtResource* s,AnmManagerFixture* a,i32 id){return new PlayerShotsFixture(*s,a->manager,a->environment.game_rng,a->random,id);}
API(player_shots_delete) void player_shots_delete(PlayerShotsFixture* p){delete p;}
API(player_shots_data) PlayerShot* player_shots_data(PlayerShotsFixture* p){return p->shots.shots.data();}
API(player_shots_damage) DamageSource* player_shots_damage(PlayerShotsFixture* p){return p->damage.sources.data();}
API(player_shots_random) Rng* player_shots_random(PlayerShotsFixture* p){return &p->random;}
API(player_shots_visual_random) Rng* player_shots_visual_random(PlayerShotsFixture* p){return &p->visual_random;}
API(player_shots_rewards) i32* player_shots_rewards(PlayerShotsFixture* p){return p->shots.reward_counters.data();}
API(player_shots_items) PlayerShotItemEvent* player_shots_items(PlayerShotsFixture* p){return p->items.data();}
API(player_shots_item_count) u32 player_shots_item_count(PlayerShotsFixture* p){return p->items.size();}
API(player_shots_cursor) i32* player_shots_cursor(PlayerShotsFixture* p){return &p->damage.cursor;}
API(player_shots_position) Vec3* player_shots_position(PlayerShotsFixture* p){return &p->motion.position;}
API(player_shots_option) PlayerFixedPosition* player_shots_option(PlayerShotsFixture* p,u32 i){return &p->motion.options[i].position;}
API(player_shots_angles) float* player_shots_angles(PlayerShotsFixture* p){return p->context.option_angles.data();}
API(player_shots_locks) i32* player_shots_locks(PlayerShotsFixture* p){return p->context.laser_power.data();}
API(player_shots_options) void player_shots_options(PlayerShotsFixture* p,i32 focus,i32 power,i32 step,float rate){p->motion.focus=focus;p->context.power=power;p->context.power_step=step;p->shots.set_rate(rate);}
API(player_shots_fire) u32 player_shots_fire(PlayerShotsFixture* p,i32 frame,i32 sound_frame){return p->shots.fire(frame,sound_frame);}
API(player_shots_update) u32 player_shots_update(PlayerShotsFixture* p){return p->shots.update();}
API(player_shots_hit) i32 player_shots_hit(PlayerShotsFixture* p,u32 source,DamageQuery* q,u32 rectangle){q->rectangle=rectangle!=0;return p->shots.hit(p->damage.sources[source],*q);}
API(player_shots_targets) void player_shots_targets(PlayerShotsFixture* p,u32 count){p->world.enemies.clear();for(u32 i=0;i<count&&i<8;i++)p->world.enemies.push_back(&p->targets[i]);p->context.enemies=count?&p->world:nullptr;}
API(player_shots_target) void* player_shots_target(PlayerShotsFixture* p,u32 i,u32 field){auto& target=p->targets[i];return field==0?static_cast<void*>(&target.motion.position):field==1?static_cast<void*>(&target.flags):static_cast<void*>(&target.id);}
API(player_shots_context) void player_shots_context(PlayerShotsFixture* p,i32 frame,u32 enemy,u32 bomb,float x,float y){p->context.shoot_frame=frame;p->context.enemy_scene=enemy!=0;p->context.bomb_active=bomb!=0;p->context.screen_origin={x,y};}
API(player_shots_spawn) i32 player_shots_spawn(PlayerShotsFixture* p,u32 id){return p->shots.spawn(id,p->motion.position);}
API(player_shots_error) const char* player_shots_error(PlayerShotsFixture* p){return p->shots.error.c_str();}
API(player_shots_sound_count) u32 player_shots_sound_count(PlayerShotsFixture* p){return p->sounds.size()/2;}
API(player_shots_sound) i32* player_shots_sound(PlayerShotsFixture* p,u32 i){return p->sounds.data()+i*2;}
API(damage_sources_delete) void damage_sources_delete(DamageSources* p){delete p;}
API(damage_sources_data) DamageSource* damage_sources_data(DamageSources* p){return p->sources.data();}
API(damage_sources_cursor) i32* damage_sources_cursor(DamageSources* p){return &p->cursor;}
API(damage_sources_score) i32* damage_sources_score(DamageSources* p){return &p->score;}
API(damage_sources_maximum) i32* damage_sources_maximum(DamageSources* p){return &p->maximum_damage;}
API(damage_sources_circle) i32 damage_sources_circle(DamageSources* p,const Vec3* position,float radius,float growth,i32 lifetime,i32 damage){return p->circle(*position,radius,growth,lifetime,damage);}
API(damage_sources_rectangle) i32 damage_sources_rectangle(DamageSources* p,const Vec3* position,float angle,const Vec2* size,i32 lifetime,i32 damage){return p->rectangle(*position,angle,*size,lifetime,damage);}
API(damage_sources_tick) void damage_sources_tick(DamageSources* p,float rate){p->tick(rate);}
API(damage_query_create) DamageQuery* damage_query_create(){return new DamageQuery;}
API(line_rectangle) u32 line_rectangle_export(const Vec2* origin,float angle,const Vec2* center,const Vec2* size,float rotation,Vec2* near,Vec2* far){return line_rectangle(*origin,angle,*center,*size,rotation,*near,*far);}
API(damage_query_delete) void damage_query_delete(DamageQuery* p){delete p;}
API(damage_query_field) void* damage_query_field(DamageQuery* p,u32 field){switch(field){case 0:return &p->position;case 1:return &p->size;case 2:return &p->radius;case 3:return &p->angle;case 4:return &p->target;default:return nullptr;}}
API(damage_sources_query) const DamageResult* damage_sources_query(DamageSources* p,DamageQuery* q,u32 rectangle,u32 preview,u32 changed,i32 bomb){static DamageResult value;q->rectangle=rectangle!=0;q->preview=preview!=0;value=p->query(*q,changed!=0,bomb);return &value;}
API(rng_create) Rng* rng_create(){return new Rng;}
API(rng_delete) void rng_delete(Rng* p){delete p;}
API(rng_next) u32 rng_next(Rng* p,u32 kind){if(kind==0)return p->next16();if(kind==1)return p->next32();float value=kind==2?p->unit():p->signed_unit();u32 bits;std::memcpy(&bits,&value,4);return bits;}
struct AnmRegistryFixture {AnmRegistry registry;Rng random;};
struct GraphicsCaptureState {std::array<Matrix4,4> matrices;std::vector<u32> states;u32 binding;std::array<u32,4> alpha;};
struct GraphicsCapture final:ZunGraphics {
    touhou::graphics::PipelineState state;std::vector<AnmGeometryVertex> vertices;std::vector<u8> bytes;std::vector<std::vector<u8>> history;std::array<Matrix4,4> matrices;std::vector<u32> states;std::vector<GraphicsCaptureState> submissions;u32 calls=0,binding=0,layout=0,topology=0,primitive_count=0,vertex_stride=0;
    bool targets_enabled=false;u32 target=0;std::vector<u32> target_events;
    void target_event(u32 kind,u32 color,const GraphicsViewport* rect){target_events.insert(target_events.end(),{kind,target,color,rect?1u:0u,rect?rect->x:0u,rect?rect->y:0u,rect?rect->width:0u,rect?rect->height:0u});}
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}void bind_texture(u32 handle)override{binding=handle;}
    void set_layout(touhou::graphics::VertexLayout value)override{layout=u32(value);}void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& value)override{matrices[u32(kind)]=value;}
    void capture(u32 n,const void* data,u32 stride){calls++;vertex_stride=stride;bytes.assign(static_cast<const u8*>(data),static_cast<const u8*>(data)+n*stride);history.push_back(bytes);vertices.resize(n);for(u32 i=0;i<n;i++)std::memcpy(&vertices[i],static_cast<const u8*>(data)+i*stride,std::min<u32>(sizeof(AnmGeometryVertex),stride));states={state.alphaTest,u32(state.sourceBlend),u32(state.destinationBlend),u32(state.blendEquation),u32(state.minFilter),u32(state.addressU),u32(state.addressV),state.depthWrite,state.textureTransform,state.textureFactor,state.fog,layout,topology,primitive_count,vertex_stride};submissions.push_back({matrices,states,binding,{state.separateAlphaBlend,u32(state.sourceAlphaBlend),u32(state.destinationAlphaBlend),u32(state.alphaBlendEquation)}});}
    void triangles(u32 count,const void* data,u32 stride)override{topology=u32(touhou::graphics::Topology::Triangles);primitive_count=count;capture(count*3,data,stride);}void primitives(touhou::graphics::Topology kind,u32 count,const void* data,u32 stride)override{topology=u32(kind);primitive_count=count;capture(touhou::graphics::vertex_count(kind,count),data,stride);}
    bool select_target(const AnmResource* file,u32 index)override{if(!targets_enabled)return false;target=file?index+1:0;target_event(0,0,nullptr);return true;}
    bool clear_target(u32 color,const GraphicsViewport* rect)override{if(!targets_enabled)return false;target_event(1,color,rect);return true;}
    bool clear_depth(const GraphicsViewport* rect)override{if(!targets_enabled)return false;target_event(2,0,rect);return true;}void set_viewport(const GraphicsViewport&)override{}
};
struct AnmRendererFixture {GraphicsCapture graphics;AnmRenderer renderer{graphics};AnmRendererFixture(){graphics.configure_game(1);}};
API(laser_visual_draw) u32 laser_visual_draw(LaserVisualFixture* p,AnmRendererFixture* renderer,u32 moving,const Vec3* position,float angle,float length,float traveled){bool ok;if(moving){MovingLaserMotion m;m.position=*position;m.angle=angle;m.length=length;m.traveled=traveled;ok=p->visual.draw_moving(m,renderer->renderer);}else{StationaryLaserMotion m;m.position=*position;m.angle=angle;m.length=length;m.traveled=traveled;ok=p->visual.draw_stationary(m,renderer->renderer);}renderer->renderer.flush();return ok;}
API(anm_renderer_create) AnmRendererFixture* anm_renderer_create(){return new AnmRendererFixture;}
API(anm_renderer_delete) void anm_renderer_delete(AnmRendererFixture* p){delete p;}
API(anm_renderer_draw) i32 anm_renderer_draw(AnmRendererFixture* p,AnmVm* vm,u32 flush){const int result=p->renderer.draw(*vm);if(flush)p->renderer.flush();return result;}
API(anm_renderer_offset) Vec2* anm_renderer_offset(AnmRendererFixture* p){return &p->renderer.offset;}
API(anm_renderer_tint) void anm_renderer_tint(AnmRendererFixture* p,u32 enabled,u32 color){p->renderer.tint_enabled=enabled!=0;p->renderer.tint=color;}
API(anm_renderer_reset) void anm_renderer_reset(AnmRendererFixture* p){p->renderer.invalidate();p->graphics.vertices.clear();p->graphics.bytes.clear();p->graphics.history.clear();p->graphics.submissions.clear();p->graphics.states.clear();p->graphics.calls=0;p->renderer.error.clear();}
API(anm_renderer_vertices) const AnmGeometryVertex* anm_renderer_vertices(AnmRendererFixture* p){return p->graphics.vertices.data();}
API(anm_renderer_count) u32 anm_renderer_count(AnmRendererFixture* p){return p->graphics.vertices.size();}
API(anm_renderer_calls) u32 anm_renderer_calls(AnmRendererFixture* p){return p->graphics.calls;}
API(anm_renderer_states) const u32* anm_renderer_states(AnmRendererFixture* p){return p->graphics.states.data();}
API(anm_renderer_bytes) const u8* anm_renderer_bytes(AnmRendererFixture* p){return p->graphics.bytes.data();}
API(anm_renderer_history_state) const u32* anm_renderer_history_state(AnmRendererFixture* p,u32 index){return p->graphics.submissions[index].states.data();}
API(anm_renderer_history) const u8* anm_renderer_history(AnmRendererFixture* p,u32 index){return p->graphics.history[index].data();}
API(anm_renderer_matrix) const Matrix4* anm_renderer_matrix(AnmRendererFixture* p,u32 kind){return &p->graphics.matrices[kind];}
API(anm_renderer_camera) AnmCamera* anm_renderer_camera(AnmRendererFixture* p){return &p->renderer.camera;}
API(anm_renderer_error) const char* anm_renderer_error(AnmRendererFixture* p){return p->renderer.error.c_str();}
API(anm_vm_inherited_color) u32* anm_vm_inherited_color(AnmVm* p){return &p->visual.inherited_color;}
API(anm_vm_quad) const Vec3* anm_vm_quad(AnmVm* p){return p->visual.quad.data();}
API(anm_vm_transform) u32 anm_vm_transform(AnmVm* p,Matrix4* out,u32 kind){return kind==0?anm_billboard_transform(*p,*out):kind==1?anm_world_quad_transform(*p,*out):anm_mesh_transform(*p,*out);}
API(anm_vm_texture_transform) void anm_vm_texture_transform(AnmVm* p,Matrix4* out){*out=anm_texture_transform(*p);}
API(anm_vm_allocate_geometry) u32 anm_vm_allocate_geometry(AnmVm* p,u32 count,u32 world){return p->geometry.allocate(count,world!=0);}
API(anm_vm_allocate_distortion) void anm_vm_allocate_distortion(AnmVm* p){p->geometry.clear();p->geometry.distortion=std::make_unique<AnmDistortion>();}
API(anm_camera_create) AnmCamera* anm_camera_create(){return new AnmCamera;}
API(anm_camera_delete) void anm_camera_delete(AnmCamera* p){delete p;}
API(anm_camera_eye) Vec3* anm_camera_eye(AnmCamera* p){return &p->eye;}
API(anm_camera_reference) Vec3* anm_camera_reference(AnmCamera* p){return &p->reference;}
API(anm_camera_matrix) Matrix4* anm_camera_matrix(AnmCamera* p,u32 kind){return kind?&p->projection:&p->view;}
API(anm_camera_fog_range) float* anm_camera_fog_range(AnmCamera* p){return &p->fog_near;}
API(anm_camera_fog_rgb) Vec3* anm_camera_fog_rgb(AnmCamera* p){return &p->fog_rgb;}
API(anm_camera_project) void anm_camera_project(AnmCamera* p,Vec3* position,Matrix4* world,GraphicsViewport* viewport,Vec3* out){*out=project_vertex(*position,*world,*p,*viewport);}
API(anm_camera_quad) i32 anm_camera_quad(AnmCamera* p,AnmVm* vm,GraphicsViewport* viewport,Vec3* out){Vec3 values[4];const int result=anm_projected_quad(*vm,*p,*viewport,values);if(!result)std::memcpy(out,values,sizeof(values));return result;}
API(anm_manager_create) AnmManagerFixture* anm_manager_create(){return new AnmManagerFixture;}
API(anm_manager_delete) void anm_manager_delete(AnmManagerFixture* p){delete p;}
API(anm_manager_load) u32 anm_manager_load(AnmManagerFixture* p,i32 id,const u8* bytes,u32 size){return p->manager.load(id,bytes,size);}
API(anm_manager_template) AnmVm* anm_manager_template(AnmManagerFixture* p,i32 id,u32 script){return p->manager.script_template(id,script);}
API(anm_manager_bind) u32 anm_manager_bind(AnmManagerFixture* p,AnmVm* vm,i32 id,i32 script){return p->manager.bind_template(*vm,id,script);}
API(anm_manager_spawn) u32 anm_manager_spawn(AnmManagerFixture* p,i32 id,i32 script,i32 layer,u32 ordering){return p->manager.create(id,script,layer,ordering);}
API(anm_manager_find) AnmVm* anm_manager_find(AnmManagerFixture* p,u32 handle){return p->manager.registry.find(handle);}
API(anm_manager_destroy_tree) u32 anm_manager_destroy_tree(AnmManagerFixture* p,AnmVm* vm){return p->manager.registry.destroy_tree(*vm);}
API(anm_manager_child) AnmVm* anm_manager_child(AnmManagerFixture* p,AnmVm* source,i32 script,u32 ordering,u32 detached){return detached?p->manager.spawn_detached(*source,script):p->manager.spawn_child(*source,script,ordering);}
API(anm_manager_rebind) u32 anm_manager_rebind(AnmManagerFixture* p,u32 handle,i32 script){return p->manager.rebind(handle,script)?handle:0;}
API(anm_manager_registry) AnmRegistry* anm_manager_registry(AnmManagerFixture* p){return &p->manager.registry;}
API(anm_manager_random) Rng* anm_manager_random(AnmManagerFixture* p){return &p->random;}
API(anm_manager_environment) AnmEnvironment* anm_manager_environment(AnmManagerFixture* p){return &p->environment;}
API(anm_manager_fallback) u32 anm_manager_fallback(AnmManagerFixture* p,i32 id){p->environment.fallback_sprite_resource=p->manager.resource(id);return p->environment.fallback_sprite_resource!=nullptr;}
API(anm_manager_update) u32 anm_manager_update(AnmManagerFixture* p,u32 alternate,float rate){p->manager.rate=rate;return p->manager.update(alternate!=0);}
API(anm_manager_order) u32 anm_manager_order(AnmManagerFixture* p,u32 alternate,u32 index){return p->manager.registry.ordered_handle(alternate!=0,index);}
API(anm_manager_count) u32 anm_manager_count(AnmManagerFixture* p,u32 alternate){return alternate<2?p->manager.registry.ordered_count(alternate!=0):p->manager.registry.count();}
API(anm_manager_error) const char* anm_manager_error(AnmManagerFixture* p){return p->manager.error.c_str();}
struct PlayerMotionFixture {PlayerMotion motion;PlayerAnmHost visuals;AnmManager& manager;PlayerMotionFixture(AnmManager& manager,i32 player,i32 effects):visuals(manager,player,effects),manager(manager){visuals.initialize();}};
API(player_motion_create) PlayerMotionFixture* player_motion_create(AnmManagerFixture* p,i32 player,i32 effects){return new PlayerMotionFixture(p->manager,player,effects);}
API(player_motion_delete) void player_motion_delete(PlayerMotionFixture* p){delete p;}
API(player_options_create) PlayerOptions* player_options_create(ShtResource* s,PlayerMotionFixture* m,i32 resource,i32 character){return new PlayerOptions(*s,m->motion,m->visuals,m->manager,resource,character);}
API(player_options_delete) void player_options_delete(PlayerOptions* p){delete p;}
API(player_options_configure) i32 player_options_configure(PlayerOptions* p,i32 power,i32 step,i32 max){return p->configure(power,step,max);}
API(player_options_state) i32* player_options_state(PlayerOptions* p,i32 i){return i?&p->auxiliary:&p->power_level;}
API(player_options_error) const char* player_options_error(PlayerOptions* p){return p->error.c_str();}
API(player_motion_configure) void player_motion_configure(PlayerMotionFixture* p,ShtHeader* header){p->motion.configure(*header);}
API(player_motion_step) u32 player_motion_step(PlayerMotionFixture* p,u32 input,u32 allowed,float rate){p->manager.rate=rate;return p->motion.update(input,allowed!=0,rate,p->visuals);}
API(player_motion_error) const char* player_motion_error(PlayerMotionFixture* p){return p->motion.error.empty()?p->manager.error.c_str():p->motion.error.c_str();}
API(player_motion_root) AnmVm* player_motion_root(PlayerMotionFixture* p){return &p->visuals.root;}
API(player_motion_handles) u32* player_motion_handles(PlayerMotionFixture* p,u32 kind){return kind==0?&p->visuals.focus_handle:kind==1?&p->visuals.barrier_handle:p->visuals.option_handles.data()->data();}
API(player_motion_field) void* player_motion_field(PlayerMotionFixture* p,u32 field){auto& v=p->motion;switch(field){case 0:return &v.position_fixed;case 1:return &v.position;case 2:return &v.last_step;case 3:return &v.step;case 4:return &v.velocity;case 5:return &v.last_direction;case 6:return &v.normal_speed;case 7:return &v.direction;case 8:return &v.focus;case 9:return &v.input_frame;case 10:return &v.option_follow_percentage;case 11:return &v.collapse_frame;case 12:return &v.option_count;case 13:return &v.behavior_flags;case 14:return &v.enlargement;case 15:return &v.movement_scale;case 16:return &v.external_velocity;case 17:return &v.barrier_timer;default:return nullptr;}}
API(player_motion_option) void* player_motion_option(PlayerMotionFixture* p,u32 index,u32 field){auto& v=p->motion.options[index];switch(field){case 0:return &v.active;case 1:return &v.target;case 2:return &v.position;case 3:return &v.normal_offset;case 4:return &v.focus_offset;case 5:return &v.snap;case 6:return &v.index;default:return nullptr;}}
API(anm_registry_create) AnmRegistryFixture* anm_registry_create(){return new AnmRegistryFixture;}
API(anm_registry_delete) void anm_registry_delete(AnmRegistryFixture* p){delete p;}
API(anm_registry_allocate) AnmVm* anm_registry_allocate(AnmRegistryFixture* p){return p->registry.allocate();}
API(anm_registry_submit) u32 anm_registry_submit(AnmRegistryFixture* p,AnmVm* vm,u32 ordering){return p->registry.submit(*vm,ordering);}
API(anm_registry_find) AnmVm* anm_registry_find(AnmRegistryFixture* p,u32 handle){return p->registry.find(handle);}
API(anm_registry_generation) u32* anm_registry_generation(AnmRegistryFixture* p){return &p->registry.generation_counter();}
API(anm_registry_slot) u32 anm_registry_slot(AnmRegistryFixture* p,AnmVm* vm){return p->registry.slot(*vm);}
API(anm_registry_release) u32 anm_registry_release(AnmRegistryFixture* p,AnmVm* vm,u32 recursive){return recursive?p->registry.destroy_tree(*vm):p->registry.discard(*vm);}
API(anm_registry_update) u32 anm_registry_update(AnmRegistryFixture* p,u32 alternate,float rate){return p->registry.update(alternate!=0,p->random,rate);}
API(anm_registry_attach) u32 anm_registry_attach(AnmRegistryFixture* p,AnmVm* child,AnmVm* parent){return p->registry.attach(*child,*parent);}
API(anm_registry_pause) u32 anm_registry_pause(AnmRegistryFixture* p,AnmVm* vm,u32 paused){return p->registry.pause_tree(*vm,paused!=0);}
API(anm_registry_interrupt) u32 anm_registry_interrupt(AnmRegistryFixture* p,AnmVm* vm,i32 label,u32 immediate,float rate){return p->registry.interrupt(*vm,label,p->random,rate,immediate!=0);}
API(anm_registry_count) u32 anm_registry_count(AnmRegistryFixture* p,u32 alternate){return alternate<2?p->registry.ordered_count(alternate!=0):p->registry.count();}
API(anm_registry_order) u32 anm_registry_order(AnmRegistryFixture* p,u32 alternate,u32 index){return p->registry.ordered_handle(alternate!=0,index);}
API(anm_registry_layer_count) u32 anm_registry_layer_count(AnmRegistryFixture* p,u32 layer){return p->registry.layer(layer).size();}
API(anm_registry_layer_handle) u32 anm_registry_layer_handle(AnmRegistryFixture* p,u32 layer,u32 index){return p->registry.handle(*p->registry.layer(layer)[index]);}
API(anm_registry_error) const char* anm_registry_error(AnmRegistryFixture* p){return p->registry.error.c_str();}
API(ecl_create) EclResource* ecl_create(){return new EclResource;}
API(ecl_context_create) EclContext* ecl_context_create(){return new EclContext;}
API(ecl_context_delete) void ecl_context_delete(EclContext* p){delete p;}
API(ecl_context_stack) u8* ecl_context_stack(EclContext* p){return p->stack.data();}
API(ecl_context_bind) void ecl_context_bind(EclContext* p,const EclInstruction* instruction,i32 stack_pointer,i32 base){p->instruction=instruction;p->stack_pointer=stack_pointer;p->local_base=base;p->error.clear();}
API(ecl_context_field) i32 ecl_context_field(EclContext* p,u32 kind){return kind?p->local_base:p->stack_pointer;}
API(ecl_context_integer) i32 ecl_context_integer(EclContext* p,u32 argument,i32 literal,u32 pop){i32 value=0;p->integer(argument,literal,value,pop);return value;}
API(ecl_context_float) u32 ecl_context_float(EclContext* p,u32 argument,float literal,u32 pop){float value=0;p->floating(argument,literal,value,pop);u32 bits;std::memcpy(&bits,&value,4);return bits;}
API(ecl_context_destination) const void* ecl_context_destination(EclContext* p,u32 argument,u32 floating){return floating?static_cast<void*>(p->float_destination(argument)):static_cast<void*>(p->integer_destination(argument));}
API(ecl_context_error) const char* ecl_context_error(EclContext* p){return p->error.c_str();}
API(ecl_context_push) u32 ecl_context_push(EclContext* p,u32 value,u32 floating){if(floating){float f;std::memcpy(&f,&value,4);return p->push(f);}return p->push(signed_bits(value));}
API(ecl_context_code) u32 ecl_context_code(EclContext* p,const u8* bytes,u32 n,u32 mask,float time){const bool result=p->bind_code(bytes,n);p->difficulty=u8(mask);p->time=time;return result;}
API(ecl_context_step) i32 ecl_context_step(EclContext* p,float elapsed){return p->step(elapsed);}
API(ecl_context_state) const void* ecl_context_state(EclContext* p){return &p->time;}
API(ecl_context_interpolation) ScalarInterpolation* ecl_context_interpolation(EclContext* p,u32 slot){return &p->interpolators[slot];}
API(ecl_context_rng) void ecl_context_rng(EclContext* p,Rng* rng){p->visual_rng=rng;}
API(ecl_context_rate) void ecl_context_rate(EclContext* p,float rate){p->interpolation_rate=rate;}
API(ecl_context_program) u32 ecl_context_program(EclContext* p,EclProgram* program,u32 id){return p->bind(*program,id);}
API(ecl_threads_create) EclThreads* ecl_threads_create(EclProgram* program){return new EclThreads(*program);}
API(ecl_threads_delete) void ecl_threads_delete(EclThreads* p){delete p;}
API(ecl_threads_start) u32 ecl_threads_start(EclThreads* p,const char* name,u32 mask){return p->start(name,u8(mask));}
API(ecl_threads_step) i32 ecl_threads_step(EclThreads* p,float elapsed){return p->step(elapsed);}
API(ecl_threads_count) u32 ecl_threads_count(EclThreads* p){return p->count();}
API(ecl_threads_at) EclContext* ecl_threads_at(EclThreads* p,u32 index){return p->at(index);}
API(ecl_thread_field) i32 ecl_thread_field(EclContext* p,u32 field){return field==0?p->thread_id:field==1?p->thread_state:i32(p->thread_flags);}
struct CaptureBullets:BulletEmissionHost{BulletShooter last;float distance=0;u32 count=0;bool emit(const BulletShooter& shot,float value)override{last=shot;distance=value;count++;return true;}};
struct CaptureEnemyEvents:EnemyCommandHost{std::vector<u32> distortion_events;bool retire_distortion(EnemyState&)override{distortion_events.push_back(1);return true;}bool begin_distortion(EnemyState&)override{distortion_events.push_back(2);return true;}EffectRequest effect_request;u32 effect_requests=0;bool effect(const EffectRequest& e)override{effect_request=e;effect_requests++;return true;}BossHud hud;bool boss_segment(i32 boss,i32 index,float fraction,u32 color)override{return hud.segment(boss,index,fraction,color);}bool boss_segments(i32 count)override{hud.displayed_segments=count;return true;}std::vector<u32> scene_events;bool capture_scene=false,message_present=false,message_done=false;bool scene_message(i32 id)override{scene_events.insert(scene_events.end(),{1,u32(id)});return capture_scene;}bool message_complete(bool& result)override{result=!message_present||message_done;return capture_scene;}bool nudge(const ScreenNudgeSpec& s)override{scene_events.insert(scene_events.end(),{5,u32(s.duration),u32(s.first),u32(s.last)});return capture_scene;}SpellStartRequest spell_request;u32 spell_starts=0,spell_ends=0;bool begin_spell(const SpellStartRequest& request)override{spell_request=request;spell_starts++;return true;}bool finish_spell()override{spell_ends++;return true;}StageScript* background=nullptr;BulletScene* bullet_scene=nullptr;LaserManager* laser_manager=nullptr;bool cancel_all_bullets(i32 reward)override{if(capture_scene){scene_events.insert(scene_events.end(),{2,u32(reward)});return true;}return bullet_scene&&bullet_scene->cancel_all(reward);}bool cancel_all_lasers(i32 reward,bool honor)override{if(capture_scene){scene_events.insert(scene_events.end(),{3,u32(reward),u32(honor)});return true;}return laser_manager&&laser_manager->cancel_all(reward,honor);}bool clear_lasers_with_rewards()override{return laser_manager&&laser_manager->clear_with_rewards();}std::vector<u32> area_events;u32 pauses=0,sounds=0;u32 last_handle=0;i32 last_sound=-1;bool last_pause=false;Vec3 sound_position{};bool background_interrupt(i32 label)override{return background&&background->interrupt(label);}bool background_fog(i32 duration,i32 mode,const StageFog& target)override{if(!background)return false;background->interpolate_fog(duration,mode,target);return true;}static u32 bits(float v){return float_to_bits(v);}bool cancel_bullets_circle(const Vec3& p,float r,i32 reward,bool honor)override{area_events.insert(area_events.end(),{1,bits(p.x),bits(p.y),bits(p.z),bits(r),u32(reward),honor});return true;}bool cancel_lasers_circle(const Vec3& p,float r,i32 reward,bool honor)override{area_events.insert(area_events.end(),{2,bits(p.x),bits(p.y),bits(p.z),bits(r),u32(reward),honor});return true;}bool cancel_bullets_rectangle(const Vec3& p,const Vec2& size,float angle,i32 reward)override{area_events.insert(area_events.end(),{3,bits(p.x),bits(p.y),bits(p.z),bits(size.x),bits(size.y),bits(angle),u32(reward)});return true;}bool pause_animation(u32 handle,bool paused)override{pauses++;last_handle=handle;last_pause=paused;return true;}bool sound(i32 id,const Vec3& position)override{sounds++;last_sound=id;sound_position=position;return true;}};
struct CaptureEnemySpawn:EnemySpawnHost {std::vector<u32>* scene_events=nullptr;bool clear_field(bool suppress)override{if(!scene_events)return false;scene_events->insert(scene_events->end(),{4,u32(suppress)});return true;}EnemySpawnRequest last;u32 count=0;bool boss=false;bool spawn(const EnemySpawnRequest& request)override{last=request;count++;return true;}bool boss_exists(i32)const noexcept override{return boss;}};
struct CaptureEnemyVisual:EnemyVisualHost {
    std::array<AnmVm,16> instances;std::vector<i32> events;u32 next=0;
    AnmVm* find(u32 handle)override{return handle&&handle<=16?&instances[handle-1]:nullptr;}
    bool create(u32& handle,i32 resource,i32 script,i32 layer)override{handle=next++%16+1;events.insert(events.end(),{1,i32(handle),resource,script,layer});return true;}
    bool retire(u32& handle)override{auto* vm=find(handle);if(vm&&!(vm->visual.render_flags&0x4000000))vm->visual.render_flags=(vm->visual.render_flags&~64u)|32;events.insert(events.end(),{2,i32(handle),0,0,0});handle=0;return true;}
    bool pause(u32 handle)override{auto* vm=find(handle);if(vm)vm->visual.flags&=~2u;events.insert(events.end(),{3,i32(handle),0,0,0});return true;}
    bool interrupt(u32 handle,i32 label)override{auto* vm=find(handle);if(vm)vm->pending_interrupt=label;events.insert(events.end(),{4,i32(handle),label,0,0});return true;}
    bool rebind(u32& handle,i32 script)override{events.insert(events.end(),{5,i32(handle),script,0,0});return true;}
    bool size(u32 handle,Vec2& out)const noexcept override{if(!handle||handle>16)return false;const auto& visual=instances[handle-1].visual;out={float(visual.sprite_size.y*visual.scale.y),float(visual.sprite_size.x*visual.scale.x)};return true;}
    bool change_direction(u32& handle,i32 resource,i32 script,i32 layer)override{return retire(handle)&&create(handle,resource,script,layer);}
};
struct CaptureLasers:LaserEmissionHost {
    u32 count=0,kind=0;std::vector<u8> bytes;
    void put(u32 at,const void* source,u32 size){std::memcpy(bytes.data()+at,source,size);}
    void transforms(u32 offset,const std::array<BulletTransform,18>& entries){for(u32 i=0;i<18;i++){const auto& t=entries[i];const u32 at=offset+i*44;put(at,t.floats.data(),16);put(at+16,t.integers.data(),16);put(at+32,&t.type,4);put(at+36,&t.active,4);}}
    bool emit(const MovingLaserRequest& r)override{count++;kind=0;bytes.assign(0x358,0);put(0,&r.position,12);put(12,&r.angle,4);put(16,&r.maximum_length,4);put(20,&r.initial_length,4);put(24,&r.end_distance,4);put(28,&r.width,4);put(32,&r.speed,4);put(36,&r.type,4);put(40,&r.color,4);put(44,&r.start_offset,4);put(48,&r.transform_index,4);put(52,&r.transform_parameter,4);transforms(56,r.transforms);put(848,&r.sound,4);put(852,&r.reflection_sound,4);return true;}
    bool emit(const StationaryLaserRequest& r)override{count++;kind=1;bytes.assign(0x378,0);put(0,&r.position,12);put(12,&r.drift,12);put(24,&r.angle,4);put(28,&r.angular_velocity,4);put(32,&r.maximum_length,4);put(36,&r.initial_length,4);put(40,&r.width,4);put(44,&r.speed,4);put(48,&r.delay,4);put(52,&r.warmup,4);put(56,&r.active,4);put(60,&r.fade,4);put(64,&r.sound,4);put(68,&r.transform_sound,4);put(72,&r.id,4);put(76,&r.start_offset,4);put(80,&r.transform_index,4);put(84,&r.type,4);put(88,&r.color,4);put(92,&r.flags,4);transforms(96,r.transforms);return true;}
    bool emit(const CurveLaserRequest& r)override{count++;kind=2;bytes.assign(0x358,0);put(0,&r.position,12);put(12,&r.angle,4);put(16,&r.width,4);put(20,&r.speed,4);put(24,&r.type,4);put(28,&r.color,4);put(32,&r.count,4);put(36,&r.start_offset,4);put(40,&r.flags,4);transforms(44,r.transforms);put(836,&r.sound,4);put(840,&r.transform_sound,4);put(844,&r.transform_index,4);put(852,&r.initial_time,4);return true;}
    bool emit(const SegmentedLaserRequest& r)override{count++;kind=3;bytes.assign(0x354,0);put(0,&r.position,12);put(24,&r.angle,4);put(32,&r.length,4);put(36,&r.width,4);put(40,&r.id,4);put(44,&r.color,4);put(48,&r.start_offset,4);put(52,&r.flags,4);transforms(60,r.transforms);return true;}
};
struct EnemyVariableFixture{EnemyState enemy,boss,other;EnemyWorldState world;Rng random;EnemyVariables variables;EnemyCommands commands;CaptureBullets emitted;CaptureLasers laser_emitted;CaptureEnemyEvents events;CaptureEnemySpawn spawning;std::unique_ptr<CaptureEnemyVisual> visuals;std::unique_ptr<EnemyAnmHost> real_visuals;EnemyVariableFixture():variables(enemy,world,random),commands(enemy,world){commands.host=&events;commands.spawning=&spawning;spawning.scene_events=&events.scene_events;world.random=&random;world.boss=&boss;world.enemies={&enemy,&boss,&other};commands.bullets.host=&emitted;commands.lasers.host=&laser_emitted;}};
API(enemy_visual_enable) void enemy_visual_enable(EnemyVariableFixture* p){p->visuals=std::make_unique<CaptureEnemyVisual>();p->commands.visuals.host=p->visuals.get();}
API(enemy_visual_manager) void enemy_visual_manager(EnemyVariableFixture* p,AnmManagerFixture* manager){p->real_visuals=std::make_unique<EnemyAnmHost>(manager->manager);p->commands.visuals.host=p->real_visuals.get();}
API(enemy_visual_vm) AnmVm* enemy_visual_vm(EnemyVariableFixture* p,u32 index){return &p->visuals->instances[index];}
API(enemy_visual_reset) void enemy_visual_reset(EnemyVariableFixture* p,u32 next){p->visuals->events.clear();p->visuals->next=next;}
API(enemy_visual_events) const i32* enemy_visual_events(EnemyVariableFixture* p){return p->visuals->events.data();}
API(enemy_visual_count) u32 enemy_visual_count(EnemyVariableFixture* p){return p->visuals->events.size()/5;}
API(enemy_animation_field) i32* enemy_animation_field(EnemyVariableFixture* p,u32 index){auto& e=p->enemy;switch(index){case 0:return &e.selected_animation_resource;case 1:return &e.animation_resource;case 2:return &e.animation_script;case 3:return &e.animation_base;case 4:return &e.movement_direction;case 5:return &e.animation_rotation_mode;default:return &e.animation_layer;}}
API(enemy_animation_offsets) Vec3* enemy_animation_offsets(EnemyVariableFixture* p){return p->enemy.animation_offsets.data();}
API(enemy_animation_parents) i32* enemy_animation_parents(EnemyVariableFixture* p){return p->enemy.animation_parents.data();}
static EnemySpawnRequest spawn_request(const u8* bytes){EnemySpawnRequest r;std::memcpy(&r.position,bytes,12);std::memcpy(&r.score,bytes+12,4);std::memcpy(&r.item,bytes+16,4);std::memcpy(&r.life,bytes+20,4);r.mirrored=*reinterpret_cast<const i32*>(bytes+24)!=0;r.persistent=*reinterpret_cast<const i32*>(bytes+28)!=0;std::memcpy(r.integers.data(),bytes+32,16);std::memcpy(r.floats.data(),bytes+48,16);std::memcpy(r.temporary.data(),bytes+64,16);std::memcpy(&r.parent,bytes+80,4);return r;}
API(enemy_initialize) void enemy_initialize(EnemyVariableFixture* p,const u8* bytes,u32 identifier,i32 chapter){p->enemy.initialize(spawn_request(bytes),identifier,chapter);}
API(enemy_finish_spawn) void enemy_finish_spawn(EnemyVariableFixture* p){p->enemy.finish_spawn();}
API(enemy_metadata) i32* enemy_metadata(EnemyVariableFixture* p,u32 index){auto& e=p->enemy;switch(index){case 0:return &e.score;case 1:return &e.boss_slot;case 2:return &e.hit_sound;case 3:return &e.death_sound;case 4:return &e.death_animation;case 5:return &e.death_animation_resource;case 6:return &e.damage_limit;default:return reinterpret_cast<i32*>(&e.lifecycle_flags);}}
API(enemy_additional_timer) Timer* enemy_additional_timer(EnemyVariableFixture* p,u32 index){return index?&p->enemy.phase_timer:&p->enemy.lifetime_timer;}
struct EnemyRuntimeFixture:EnemyRuntimeHost,EnemyDamageHost {
    EnemyVariableFixture projection;Rng visual_random;EnemyRuntime runtime;std::vector<u32> events;i32 callback_result=0,damage_result=0;
    explicit EnemyRuntimeFixture(EclProgram& program):runtime(program,projection.world,projection.random,visual_random,*this){projection.visuals=std::make_unique<CaptureEnemyVisual>();runtime.commands.visuals.host=projection.visuals.get();runtime.commands.host=&projection.events;runtime.commands.bullets.host=&projection.emitted;runtime.commands.spawning=&projection.spawning;}
    EnemyVisualHost* animations()override{return projection.real_visuals?static_cast<EnemyVisualHost*>(projection.real_visuals.get()):projection.visuals.get();}
    void record(u32 event,float rate){u32 bits;std::memcpy(&bits,&rate,4);events.insert(events.end(),{event,bits});}
    int after_script(EnemyRuntime&,float rate)override{record(1,rate);return callback_result;}
    bool use_real_damage=false;
    int collide_and_damage(EnemyRuntime& entity,float rate)override{record(2,rate);if(use_real_damage){EnemyDamage damage(projection.world,*this,*animations());return damage.update(entity,rate);}return damage_result;}
    bool update_distortion(EnemyState&,float rate)override{record(3,rate);return true;}
    std::array<i32,5> damage_configuration{};
    bool shot_damage(EnemyState&,bool special,EnemyShotDamage& hit)override{events.insert(events.end(),{4,u32(special)});hit.amount=damage_configuration[0];hit.direct=damage_configuration[1]!=0;return true;}
    bool additional_damage(EnemyState&,i32 incoming,i32& amount)override{events.insert(events.end(),{5,u32(incoming)});amount=damage_configuration[2];return true;}
    int die(EnemyRuntime&,float)override{events.insert(events.end(),{6,0});return damage_configuration[3];}
    bool contact(EnemyState&,AnmVm*,i32& result)override{events.insert(events.end(),{7,0});result=damage_configuration[4];return true;}
    bool graze(const Vec3&)override{events.insert(events.end(),{8,0});return true;}
    bool sound(i32 value,const Vec3&)override{events.insert(events.end(),{9,u32(value)});return true;}
};
API(enemy_runtime_create) EnemyRuntimeFixture* enemy_runtime_create(EclProgram* program){return new EnemyRuntimeFixture(*program);}
API(enemy_runtime_animation_manager) void enemy_runtime_animation_manager(EnemyRuntimeFixture* p,AnmManagerFixture* manager){enemy_visual_manager(&p->projection,manager);p->runtime.commands.visuals.host=p->projection.real_visuals.get();}
API(enemy_runtime_delete) void enemy_runtime_delete(EnemyRuntimeFixture* p){delete p;}
API(enemy_runtime_projection) EnemyVariableFixture* enemy_runtime_projection(EnemyRuntimeFixture* p){return &p->projection;}
API(enemy_runtime_start) u32 enemy_runtime_start(EnemyRuntimeFixture* p,const char* name,const u8* bytes,u32 identifier){auto request=spawn_request(bytes);request.routine=name;const bool result=p->runtime.initialize(request,identifier);p->projection.enemy=p->runtime.state;return result;}
API(enemy_runtime_step) i32 enemy_runtime_step(EnemyRuntimeFixture* p,float rate,float x,float y,float z,u32 begin){p->runtime.state=p->projection.enemy;if(begin)p->runtime.clear_update_guard();p->events.clear();const i32 result=p->runtime.step(rate,{x,y,z});p->projection.enemy=p->runtime.state;return result;}
API(enemy_runtime_error) const char* enemy_runtime_error(EnemyRuntimeFixture* p){return p->runtime.error.c_str();}
API(enemy_runtime_context) EclContext* enemy_runtime_context(EnemyRuntimeFixture* p,u32 index){return p->runtime.scripts.at(index);}
API(enemy_runtime_threads) u32 enemy_runtime_threads(EnemyRuntimeFixture* p){return p->runtime.scripts.count();}
API(enemy_runtime_slowdown) float* enemy_runtime_slowdown(EnemyRuntimeFixture* p){return &p->projection.enemy.slowdown;}
API(enemy_runtime_results) void enemy_runtime_results(EnemyRuntimeFixture* p,i32 callback,i32 damage){p->callback_result=callback;p->damage_result=damage;}
API(enemy_runtime_event_count) u32 enemy_runtime_event_count(EnemyRuntimeFixture* p){return p->events.size()/2;}
API(enemy_runtime_events) const u32* enemy_runtime_events(EnemyRuntimeFixture* p){return p->events.data();}
API(enemy_damage_configuration) i32* enemy_damage_configuration(EnemyRuntimeFixture* p){return p->damage_configuration.data();}
API(enemy_runtime_use_damage) void enemy_runtime_use_damage(EnemyRuntimeFixture* p,u32 enabled){p->use_real_damage=enabled!=0;}
API(enemy_damage_step) i32 enemy_damage_step(EnemyRuntimeFixture* p,float rate){p->runtime.state=p->projection.enemy;p->events.clear();EnemyDamage damage(p->projection.world,*p,*p->projection.visuals);const int result=damage.update(p->runtime,rate);p->projection.enemy=p->runtime.state;return result;}
API(enemy_damage_field) void* enemy_damage_field(EnemyVariableFixture* p,u32 kind){auto& e=p->enemy;switch(kind){case 0:return &e.pending_damage;case 1:return &e.cumulative_damage;case 2:return &e.damage_flash_frames;case 3:return &e.inactive_animation;case 4:return &e.normal_animation;case 5:return &e.damage_timer;case 6:return &e.last_hit_position;default:return &e.life_threshold;}}
API(enemy_damage_world) i32* enemy_damage_world(EnemyVariableFixture* p,u32 kind){auto& w=p->world;switch(kind){case 0:return &w.player_damage_state;case 1:return &w.damage_disabled;case 2:return &w.shot_damage;default:return &w.other_damage;}}
API(enemy_apply_damage) void enemy_apply_damage(EnemyVariableFixture* p,i32 amount){p->enemy.apply_damage(amount);}
struct EnemyDeathFixture:EnemyRuntimeFixture,EnemyDeathHost {
    std::vector<ItemSpawnRequest> items;std::vector<EnemyDeathEffect> effects;
    explicit EnemyDeathFixture(EclProgram& program):EnemyRuntimeFixture(program){}
    bool spawn_item(const ItemSpawnRequest& request)override{events.insert(events.end(),{10,u32(request.type)});items.push_back(request);return true;}
    bool effect(const EnemyDeathEffect& request)override{events.insert(events.end(),{11,u32(request.script)});effects.push_back(request);return true;}
    bool sound(i32 id,const Vec3& position)override{return EnemyRuntimeFixture::sound(id,position);}
    bool callback(EnemyRuntime&)override{events.insert(events.end(),{12,0});return true;}
};
API(enemy_death_create) EnemyDeathFixture* enemy_death_create(EclProgram* program){return new EnemyDeathFixture(*program);}
API(enemy_death_delete) void enemy_death_delete(EnemyDeathFixture* p){delete p;}
API(enemy_death_step) i32 enemy_death_step(EnemyDeathFixture* p,float rate){p->runtime.state=p->projection.enemy;p->events.clear();p->items.clear();p->effects.clear();EnemyDeath death(p->projection.world,p->projection.random,*p);const int result=death.execute(p->runtime,rate);p->projection.enemy=p->runtime.state;return result;}
API(enemy_drop_items) u32 enemy_drop_items(EnemyDeathFixture* p){p->events.clear();p->items.clear();return drop_enemy_items(p->projection.enemy,p->projection.random,*p);}
API(enemy_drop_items_now) u32 enemy_drop_items_now(EnemyDeathFixture* p){p->events.clear();p->items.clear();return drop_enemy_items_now(p->projection.enemy,p->projection.random,*p);}
API(enemy_death_items) const ItemSpawnRequest* enemy_death_items(EnemyDeathFixture* p){return p->items.data();}
API(enemy_death_item_count) u32 enemy_death_item_count(EnemyDeathFixture* p){return p->items.size();}
API(enemy_death_effects) const EnemyDeathEffect* enemy_death_effects(EnemyDeathFixture* p){return p->effects.data();}
API(enemy_death_effect_count) u32 enemy_death_effect_count(EnemyDeathFixture* p){return p->effects.size();}
struct EnemyManagerFixture:EnemyManagerHost {
    EnemyWorldState world;Rng random,visual_random;CaptureEnemyVisual visuals;CaptureBullets emitted;EnemyVariableFixture projection;std::vector<u32> destroyed,scene_deaths;EnemyManager manager;LaserScene* laser_scene=nullptr;
    explicit EnemyManagerFixture(EclProgram& program):manager(program,world,random,visual_random,*this){world.random=&random;world.enemy_control=1000;}
    EnemyVisualHost* animations()override{return &visuals;}
    LaserScene* lasers()override{return laser_scene;}
    int after_script(EnemyRuntime&,float)override{return 0;}int collide_and_damage(EnemyRuntime&,float)override{return 0;}bool update_distortion(EnemyState&,float)override{return true;}
    bool pause_animation(u32,bool)override{return true;}bool sound(i32,const Vec3&)override{return true;}
    BulletEmissionHost* bullets()override{return &emitted;}
    bool destroy(EnemyState& state)override{destroyed.push_back(state.id);return true;}
    int scene_death(EnemyRuntime& e,float)override{scene_deaths.push_back(e.state.id);return 1;}
};
API(enemy_manager_create) EnemyManagerFixture* enemy_manager_create(EclProgram* program){return new EnemyManagerFixture(*program);}
API(enemy_manager_delete) void enemy_manager_delete(EnemyManagerFixture* p){delete p;}
API(enemy_manager_spawn) u32 enemy_manager_spawn(EnemyManagerFixture* p,const char* name,const u8* bytes){auto request=spawn_request(bytes);request.routine=name;return p->manager.spawn(request);}
API(enemy_manager_step) u32 enemy_manager_step(EnemyManagerFixture* p,float rate){p->manager.rate=rate;return p->manager.update();}
API(enemy_manager_clear) u32 enemy_manager_clear(EnemyManagerFixture* p){return p->manager.clear();}
API(enemy_manager_rate) float* enemy_manager_rate(EnemyManagerFixture* p){return &p->manager.rate;}
API(enemy_manager_next_id) u32* enemy_manager_next_id(EnemyManagerFixture* p){return &p->manager.next_identifier;}
API(enemy_manager_count) u32 enemy_manager_count(EnemyManagerFixture* p){return p->manager.count();}
API(enemy_manager_timer) Timer* enemy_manager_timer(EnemyManagerFixture* p){return &p->manager.timer;}
API(enemy_manager_world_counts) i32* enemy_manager_world_counts(EnemyManagerFixture* p,u32 index){return index==0?&p->world.enemy_count:reinterpret_cast<i32*>(&p->world.total);}
API(enemy_manager_context) EclContext* enemy_manager_context(EnemyManagerFixture* p,u32 index,u32 thread){auto* enemy=p->manager.at(index);return enemy?enemy->scripts.at(thread):nullptr;}
API(enemy_manager_projection) EnemyVariableFixture* enemy_manager_projection(EnemyManagerFixture* p,u32 index){auto* enemy=p->manager.at(index);if(!enemy)return nullptr;p->projection.enemy=enemy->state;p->projection.world=p->world;return &p->projection;}
API(enemy_manager_slowdown) float* enemy_manager_slowdown(EnemyManagerFixture* p,u32 index){auto* enemy=p->manager.at(index);return enemy?&enemy->state.slowdown:nullptr;}
API(enemy_manager_error) const char* enemy_manager_error(EnemyManagerFixture* p){return p->manager.error.c_str();}
API(enemy_manager_destroyed) const u32* enemy_manager_destroyed(EnemyManagerFixture* p){return p->destroyed.data();}
API(enemy_manager_destroyed_count) u32 enemy_manager_destroyed_count(EnemyManagerFixture* p){return p->destroyed.size();}
API(enemy_spawn_count) u32 enemy_spawn_count(EnemyVariableFixture* p){return p->spawning.count;}
API(enemy_spawn_boss) void enemy_spawn_boss(EnemyVariableFixture* p,u32 present){p->spawning.boss=present!=0;}
API(enemy_spawn_name) const char* enemy_spawn_name(EnemyVariableFixture* p){return p->spawning.last.routine.c_str();}
API(enemy_spawn_bytes) const u8* enemy_spawn_bytes(EnemyVariableFixture* p){static std::array<u8,84> bytes;bytes={};const auto& r=p->spawning.last;std::memcpy(bytes.data(),&r.position,12);const i32 fields[]={r.score,r.item,r.life,i32(r.mirrored),i32(r.persistent)};std::memcpy(bytes.data()+12,fields,20);std::memcpy(bytes.data()+32,r.integers.data(),16);std::memcpy(bytes.data()+48,r.floats.data(),16);std::memcpy(bytes.data()+64,r.temporary.data(),16);std::memcpy(bytes.data()+80,&r.parent,4);return bytes.data();}
static void shooter_read(BulletShooter& s,const u8* bytes){std::memcpy(&s.sprite,bytes,4);std::memcpy(&s.color,bytes+4,4);std::memcpy(&s.position,bytes+8,12);std::memcpy(&s.angle,bytes+20,4);std::memcpy(&s.angle_step,bytes+24,4);std::memcpy(&s.speed,bytes+28,4);std::memcpy(&s.speed_step,bytes+32,4);std::memcpy(&s.radius,bytes+36,4);for(u32 i=0;i<18;i++){auto& t=s.transforms[i];const u8* p=bytes+40+i*44;std::memcpy(t.floats.data(),p,16);std::memcpy(t.integers.data(),p+16,16);std::memcpy(&t.type,p+32,4);std::memcpy(&t.active,p+36,4);t.payload.clear();}std::memcpy(s.extra.data(),bytes+832,36);std::memcpy(&s.count,bytes+868,2);std::memcpy(&s.rows,bytes+870,2);std::memcpy(&s.pattern,bytes+872,2);std::memcpy(&s.reserved,bytes+874,2);std::memcpy(&s.flags,bytes+876,4);std::memcpy(&s.shoot_sound,bytes+880,4);std::memcpy(&s.transform_sound,bytes+884,4);std::memcpy(s.tail.data(),bytes+888,8);}
static const u8* shooter_bytes(const BulletShooter& s){static std::array<u8,896> bytes;bytes={};std::memcpy(bytes.data(),&s.sprite,4);std::memcpy(bytes.data()+4,&s.color,4);std::memcpy(bytes.data()+8,&s.position,12);std::memcpy(bytes.data()+20,&s.angle,4);std::memcpy(bytes.data()+24,&s.angle_step,4);std::memcpy(bytes.data()+28,&s.speed,4);std::memcpy(bytes.data()+32,&s.speed_step,4);std::memcpy(bytes.data()+36,&s.radius,4);for(u32 i=0;i<18;i++){const auto& t=s.transforms[i];u8* p=bytes.data()+40+i*44;std::memcpy(p,t.floats.data(),16);std::memcpy(p+16,t.integers.data(),16);std::memcpy(p+32,&t.type,4);std::memcpy(p+36,&t.active,4);}std::memcpy(bytes.data()+832,s.extra.data(),36);std::memcpy(bytes.data()+868,&s.count,2);std::memcpy(bytes.data()+870,&s.rows,2);std::memcpy(bytes.data()+872,&s.pattern,2);std::memcpy(bytes.data()+874,&s.reserved,2);std::memcpy(bytes.data()+876,&s.flags,4);std::memcpy(bytes.data()+880,&s.shoot_sound,4);std::memcpy(bytes.data()+884,&s.transform_sound,4);std::memcpy(bytes.data()+888,s.tail.data(),8);return bytes.data();}
API(enemy_shooter_read) void enemy_shooter_read(EnemyVariableFixture* p,u32 slot,const u8* bytes){shooter_read(p->enemy.shooters.shooters[slot],bytes);}
API(enemy_shooter_bytes) const u8* enemy_shooter_bytes(EnemyVariableFixture* p,u32 slot){return shooter_bytes(p->enemy.shooters.shooters[slot]);}
API(enemy_shooter_field) void* enemy_shooter_field(EnemyVariableFixture* p,u32 slot,u32 field){auto& s=p->enemy.shooters;return field==0?static_cast<void*>(&s.offset[slot]):field==1?static_cast<void*>(&s.origin[slot]):field==2?static_cast<void*>(&s.next_transform[slot]):static_cast<void*>(&p->enemy.minimum_bullet_distance_squared);}
API(enemy_emitted_bytes) const u8* enemy_emitted_bytes(EnemyVariableFixture* p){return shooter_bytes(p->emitted.last);}
API(enemy_emitted_count) u32 enemy_emitted_count(EnemyVariableFixture* p){return p->emitted.count;}
API(enemy_emitted_distance) float enemy_emitted_distance(EnemyVariableFixture* p){return p->emitted.distance;}
API(enemy_shooter_payload) const u8* enemy_shooter_payload(EnemyVariableFixture* p,u32 slot,u32 index){return p->enemy.shooters.shooters[slot].transforms[index].payload.data();}
API(bullet_initial_create) BulletInitialMotion* bullet_initial_create(){return new BulletInitialMotion;}
API(bullet_initial_delete) void bullet_initial_delete(BulletInitialMotion* p){delete p;}
API(bullet_initial_form) u32 bullet_initial_form(EnemyVariableFixture* p,BulletInitialMotion* out,u32 slot,i32 column,i32 row,float aim,float minimum){return form_bullet(p->enemy.shooters.shooters[slot],column,row,aim,p->random,p->world.player_position,minimum,*out);}
API(bullet_aim) float bullet_aim_api(float x,float y,float px,float py){return bullet_aim({x,y,0},{px,py,0});}
struct BulletStateFixture:BulletFrameHost,BulletContactHost{BulletState state;BulletState* external_state=nullptr;BulletManager* owner=nullptr;u32 slot=0;Vec2 bounds{},size{};Vec3 player{};u32 sounds=0,interrupts=0;i32 last_sound=-1,last_interrupt=-1;Rng random;std::string error;void play(i32 sound)override{sounds++;last_sound=sound;}u32 contacts=0,main_updates=0,overlay_updates=0,recycles=0,effects=0; i32 contact_result=0; bool main_finished=false; i32 contact(BulletState&,bool)override{contacts++;return contact_result;} bool animation_finished(bool overlay)override{if(overlay){overlay_updates++;return false;}main_updates++;return main_finished;} void cancellation_effect(i32,const Vec3&,const Vec3&)override{effects++;} void recycle()override{recycles++;if(owner)owner->recycle(slot);}PlayerCollision collision;Rng visual_random;u32 deaths=0,hit_callbacks=0,overlay_interrupts=0,sparks=0,grazes=0,items=0;i32 item_type=-1;float resonance=0,item_angle=0,item_speed=0;Vec3 item_position{};void hit()override{deaths++;}bool hit_animation()override{hit_callbacks++;return true;}bool overlay_interrupt(i32)override{overlay_interrupts++;return true;}void graze_spark(const Vec3&)override{sparks++;}void graze()override{grazes++;}void graze_resonance(float v)override{resonance=v;}void item(i32 t,const Vec3& p,float a,float v)override{items++;item_type=t;item_position=p;item_angle=a;item_speed=v;}u32 bindings=0,spawn_callbacks=0;i32 bound_script=-1,bound_overlay=-1;bool bind_appearance(i32 script,i32 overlay)override{bindings++;bound_script=script;bound_overlay=overlay;return true;}bool spawn_animation()override{spawn_callbacks++;return true;}BulletShooter children;u32 emissions=0;BulletCancellationRewards rewards;float cancel_rate=1;bool emit_children(const BulletShooter& shot)override{children=shot;emissions++;return owner?owner->emit(shot,owner->minimum_distance_squared):true;}bool cancel_bullet(i32 kind)override{auto& s=external_state?*external_state:state;return s.cancel(kind,owner?owner->context.rate:cancel_rate,owner?*owner->random:random,owner?owner->cancellation_rewards:rewards,*this,*this,error);}bool interrupt(i32 id)override{interrupts++;last_interrupt=id;return true;}CaptureLasers object_lasers;EnemySpawnRequest spawned;u32 spawned_count=0;bool spawn_enemy(const EnemySpawnRequest& request)override{spawned=request;spawned_count++;return true;}bool create_laser(const MovingLaserRequest& request)override{return object_lasers.emit(request);}bool create_laser(const StationaryLaserRequest& request)override{return object_lasers.emit(request);}};
API(bullet_state_create) BulletStateFixture* bullet_state_create(){return new BulletStateFixture;}
API(bullet_state_delete) void bullet_state_delete(BulletStateFixture* p){delete p;}
API(bullet_state_field) void* bullet_state_field(BulletStateFixture* fixture,u32 kind,u32 field){auto& s=fixture->external_state?*fixture->external_state:fixture->state;switch(kind){case 0:return &s.motion;case 1:return &s.transform_flags;case 2:return &s.boost;case 3:case 4:{auto& a=kind==3?s.acceleration:s.approach;switch(field){case 0:return &a.timer;case 1:return &a.speed;case 2:return &a.angle;case 3:return &a.vector;default:return &a.duration;}}case 5:{auto& a=s.angular;switch(field){case 0:return &a.timer;case 1:return &a.speed;case 2:return &a.angle;default:return &a.duration;}}case 6:{auto& r=s.reflection;switch(field){case 0:return &r.speed;case 1:return &r.bounds;case 2:return &r.count;case 3:return &r.limit;default:return &r.sides;}}case 7:{auto& w=s.wrapping;return field==0?static_cast<void*>(&w.count):field==1?static_cast<void*>(&w.limit):static_cast<void*>(&w.sides);}case 8:{auto& d=s.drift;return field==0?static_cast<void*>(&d.timer):field==1?static_cast<void*>(&d.velocity):static_cast<void*>(&d.duration);}case 9:{auto& c=s.position_curve;switch(field){case 0:return &c.timer;case 1:return &c.speed;case 2:return &c.target;case 3:return &c.duration;case 4:return &c.mode;default:return &c.interpolation;}}case 10:return &s.transform_sound;case 11:return &fixture->bounds;case 12:return &fixture->size;case 13:{auto& t=s.turn;switch(field){case 0:return &t.timer;case 1:return &t.speed;case 2:return &t.angle;case 3:return &t.duration;case 4:return &t.limit;case 5:return &t.count;case 6:return &t.mode;default:return &t.extra;}}case 14:return field==0?static_cast<void*>(&s.saved.position):field==1?static_cast<void*>(&s.saved.angle):static_cast<void*>(&s.saved.speed);case 15:return &fixture->player;case 16:return &s.transform_index;case 17:return field?static_cast<void*>(&s.wait_outside):static_cast<void*>(&s.wait);case 18:return &s.freeze;case 19:return &s.invulnerability;case 20:return &s.boost_stage;case 21:return &s.step_limit;case 22:return &s.collision_delay;case 23:return &s.cancel_item;case 24:return &s.phase;case 25:return &s.flags;case 26:return &s.visual_flags;case 27:return &s.scale_curve;case 28:return &s.scale;case 30:return &s.lifetime;case 31:return &s.spawn;case 32:return &s.offscreen_grace;case 33:return &s.cancellation_script;case 34:return &s.spawn_animation_finished;case 35:return &s.overlay_active;case 36:return &s.hitbox;case 37:return &s.graze_flash;case 38:return &s.graze_duration;case 39:return &s.graze_interval;case 40:return &s.hit_interrupt;case 41:return &s.visual_jitter;case 42:return &s.graze_color;case 43:return &fixture->visual_random;case 44:return &fixture->resonance;case 45:return &fixture->item_position;case 46:return &fixture->item_angle;case 47:return &fixture->item_speed;case 48:return &s.sprite_type;case 49:return &s.initial_transform_flags;default:return field?static_cast<void*>(&s.drift.angle):static_cast<void*>(&s.drift.speed);}}
API(bullet_state_step) u32 bullet_state_step(BulletStateFixture* p,u32 kind,float rate){auto& s=p->state;switch(kind){case 0:return s.update_boost(rate);case 1:return s.update_acceleration(rate);case 2:return s.update_acceleration(rate,true);case 3:return s.update_angular(rate);case 4:return s.update_drift(rate);case 5:return s.reflect_edge(4,p->bounds);case 6:return s.reflect_edge(8,p->bounds);case 7:return s.reflect_edge(1,p->bounds);case 8:return s.reflect_edge(2,p->bounds);case 9:return s.update_reflection(p->bounds,p);case 10:return s.update_wrap(p->size,p);case 11:return s.update_position_curve(rate);case 13:return s.update_wait(rate,p->size);default:return s.update_turn(rate,p->player,p);}}
API(bullet_sound_count) u32 bullet_sound_count(BulletStateFixture* p){return p->sounds;}
API(bullet_sound_last) i32 bullet_sound_last(BulletStateFixture* p){return p->last_sound;}
API(bullet_transform_read) void bullet_transform_read(BulletStateFixture* p,u32 slot,const u8* bytes){auto& t=(p->external_state?*p->external_state:p->state).transforms[slot];std::memcpy(t.floats.data(),bytes,16);std::memcpy(t.integers.data(),bytes+16,16);std::memcpy(&t.type,bytes+32,4);std::memcpy(&t.active,bytes+36,4);t.payload.clear();}
API(bullet_activate) u32 bullet_activate(BulletStateFixture* p){p->error.clear();return p->state.activate(p->player,p->random,p,p,p->error);}
API(bullet_activation_error) const char* bullet_activation_error(BulletStateFixture* p){return p->error.c_str();}
API(bullet_activation_rng) Rng* bullet_activation_rng(BulletStateFixture* p){return &p->random;}
API(bullet_interrupt_count) u32 bullet_interrupt_count(BulletStateFixture* p){return p->interrupts;}
API(bullet_interrupt_last) i32 bullet_interrupt_last(BulletStateFixture* p){return p->last_interrupt;}
API(bullet_frame_step) i32 bullet_frame_step(BulletStateFixture* p,float rate){p->error.clear();return i32(p->state.update({rate,p->size,p->bounds,p->player,true},p->random,*p,p->error));}
API(bullet_frame_controls) void bullet_frame_controls(BulletStateFixture* p,i32 contact,u32 finished){p->contact_result=contact;p->main_finished=finished!=0;}
API(bullet_frame_event) u32 bullet_frame_event(BulletStateFixture* p,u32 kind){switch(kind){case 0:return p->contacts;case 1:return p->main_updates;case 2:return p->overlay_updates;case 3:return p->recycles;default:return p->effects;}}
API(bullet_collision_step) i32 bullet_collision_step(BulletStateFixture* p,u32 graze_only,float rate){p->error.clear();return i32(p->state.collide(p->collision,graze_only!=0,rate,p->random,p->visual_random,*p,p->error));}
API(bullet_collision_player) void bullet_collision_player(BulletStateFixture* p,float x,float y,float radius,i32 state,i32 immunity,u32 bomb){auto& c=p->collision;c.position={x,y};c.hitbox_min={x-3.f,y-4.f};c.hitbox_max={x+3.f,y+4.f};c.radius=radius;c.state=state;c.invulnerability=immunity;c.bomb_active=bomb!=0;}
API(bullet_collision_event) u32 bullet_collision_event(BulletStateFixture* p,u32 kind){switch(kind){case 0:return p->deaths;case 1:return p->hit_callbacks;case 2:return p->overlay_interrupts;case 3:return p->sparks;case 4:return p->grazes;default:return p->items;}}
API(bullet_appearance_parameter) i32 bullet_appearance_parameter(i32 type,i32 color,i32 parameter){return bullet_animation_parameter(type,color,parameter);}
API(bullet_spawn_event) i32 bullet_spawn_event(BulletStateFixture* p,u32 kind){switch(kind){case 0:return p->bindings;case 1:return p->spawn_callbacks;case 2:return p->bound_script;default:return p->bound_overlay;}}
API(bullet_spawn_initialize) u32 bullet_spawn_initialize(BulletStateFixture* p,EnemyVariableFixture* e,u32 slot,i32 column,i32 row,float aim,float minimum){p->error.clear();return p->state.initialize(e->enemy.shooters.shooters[slot],column,row,aim,p->player,minimum,p->random,*p,p->error);}
struct BulletManagerFixture:BulletManagerHost{std::array<BulletStateFixture,BulletManager::capacity> hosts;BulletManager manager;Rng random;u32 sounds=0;i32 last_sound=-1;BulletManagerFixture():manager(*this){manager.random=&random;for(u32 i=0;i<BulletManager::capacity;i++){hosts[i].external_state=manager.state(i);hosts[i].owner=&manager;hosts[i].slot=i;}}BulletFrameHost& bullet(u32 slot)override{return hosts[slot];}void prepare(u32,BulletFrameContext& context)override{context.sprite_size={16,16};}void play(i32 sound)override{sounds++;last_sound=sound;}};
API(bullet_manager_create) BulletManagerFixture* bullet_manager_create(){return new BulletManagerFixture;}
API(bullet_manager_delete) void bullet_manager_delete(BulletManagerFixture* p){delete p;}
API(bullet_manager_reset) void bullet_manager_reset(BulletManagerFixture* p){p->manager.reset();}
API(bullet_manager_bullet) BulletStateFixture* bullet_manager_bullet(BulletManagerFixture* p,u32 slot){return slot<BulletManager::capacity?&p->hosts[slot]:nullptr;}
API(bullet_manager_list) u32 bullet_manager_list(BulletManagerFixture* p,u32 kind,u32 slot){return kind==0?p->manager.first_free():kind==1?p->manager.first_active():p->manager.next_active(slot);}
API(bullet_manager_emit) u32 bullet_manager_emit(BulletManagerFixture* p,EnemyVariableFixture* e,float minimum){p->manager.error.clear();return p->manager.emit(e->enemy.shooters.shooters[0],minimum);}
API(bullet_manager_update) u32 bullet_manager_update(BulletManagerFixture* p,float rate,u32 paused){p->manager.context.rate=rate;p->manager.world_paused=paused!=0;return p->manager.update();}
API(bullet_manager_recycle) void bullet_manager_recycle(BulletManagerFixture* p,u32 slot){p->manager.recycle(slot);}
API(bullet_manager_rng) Rng* bullet_manager_rng(BulletManagerFixture* p){return &p->random;}
API(bullet_manager_player) Vec3* bullet_manager_player(BulletManagerFixture* p){return &p->manager.context.player;}
API(bullet_manager_error) const char* bullet_manager_error(BulletManagerFixture* p){return p->manager.error.c_str();}
API(bullet_manager_counter) i32 bullet_manager_counter(BulletManagerFixture* p,u32 kind){return kind==0?p->manager.visible_count:kind==1?p->sounds:p->last_sound;}
API(bullet_manager_group_size) u32 bullet_manager_group_size(BulletManagerFixture* p,u32 group){return group<6?p->manager.draw_groups[group].size():0;}
API(bullet_manager_group_slot) u32 bullet_manager_group_slot(BulletManagerFixture* p,u32 group,u32 index){return p->manager.draw_groups[group][index];}
struct BulletSceneFixture:BulletSceneHost {
    Rng game_random,visual_random;AnmEnvironment environment;PlayerCollision collision;BulletScene scene{*this,game_random,visual_random};std::vector<BulletStateFixture> views;
    u32 sounds=0,deaths=0,sparks=0,grazes=0,items=0,effects=0;i32 last_sound=-1;float resonance=0;
    BulletSceneFixture():views(BulletManager::capacity){environment.resolution_scale=2;scene.environment=&environment;for(u32 i=0;i<BulletManager::capacity;i++)views[i].external_state=scene.manager.state(i);}
    const PlayerCollision& player_collision()const noexcept override{return collision;}
    void hit()override{deaths++;}
    void sound(i32 id)override{sounds++;last_sound=id;}
    void cancellation_effect(i32,const Vec3&,const Vec3&)override{effects++;}
    void graze_spark(const Vec3&)override{sparks++;}
    void graze()override{grazes++;}
    void graze_resonance(float value)override{resonance+=value;}
    void item(i32,const Vec3&,float,float)override{items++;}
};
API(bullet_scene_create) BulletSceneFixture* bullet_scene_create(){return new BulletSceneFixture;}
API(bullet_scene_delete) void bullet_scene_delete(BulletSceneFixture* p){delete p;}
API(bullet_scene_resource) void bullet_scene_resource(BulletSceneFixture* p,AnmResource* resource){p->scene.resource=resource;}
API(bullet_scene_reset) void bullet_scene_reset(BulletSceneFixture* p){p->scene.reset();}
API(bullet_scene_bullet) BulletStateFixture* bullet_scene_bullet(BulletSceneFixture* p,u32 slot){return slot<BulletManager::capacity?&p->views[slot]:nullptr;}
API(bullet_scene_vm) AnmVm* bullet_scene_vm(BulletSceneFixture* p,u32 slot){auto* visual=p->scene.visual(slot);return visual?&visual->body:nullptr;}
API(bullet_scene_list) u32 bullet_scene_list(BulletSceneFixture* p,u32 kind,u32 slot){auto& m=p->scene.manager;return kind==0?m.first_free():kind==1?m.first_active():m.next_active(slot);}
API(bullet_scene_emit) u32 bullet_scene_emit(BulletSceneFixture* p,EnemyVariableFixture* e,float minimum){return p->scene.emit(e->enemy.shooters.shooters[0],minimum);}
API(bullet_scene_update) u32 bullet_scene_update(BulletSceneFixture* p,float rate,u32 paused){p->scene.manager.context.rate=rate;p->scene.manager.world_paused=paused!=0;return p->scene.update();}
API(bullet_scene_rate) void bullet_scene_rate(BulletSceneFixture* p,float rate){p->scene.manager.context.rate=rate;}
API(bullet_scene_rng) Rng* bullet_scene_rng(BulletSceneFixture* p,u32 visual){return visual?&p->visual_random:&p->game_random;}
API(bullet_scene_player) PlayerCollision* bullet_scene_player(BulletSceneFixture* p){return &p->collision;}
API(bullet_scene_target) Vec3* bullet_scene_target(BulletSceneFixture* p){return &p->scene.manager.context.player;}
API(bullet_scene_error) const char* bullet_scene_error(BulletSceneFixture* p){return p->scene.error.c_str();}
API(bullet_scene_counter) i32 bullet_scene_counter(BulletSceneFixture* p,u32 kind){switch(kind){case 0:return p->scene.manager.visible_count;case 1:return p->sounds;case 2:return p->last_sound;case 3:return p->deaths;case 4:return p->sparks;case 5:return p->grazes;case 6:return p->items;default:return p->effects;}}
API(bullet_cancel) u32 bullet_cancel(BulletStateFixture* p,i32 kind,float rate){p->error.clear();p->cancel_rate=rate;return p->state.cancel(kind,rate,p->random,p->rewards,*p,*p,p->error);}
API(bullet_cancel_counter) i32* bullet_cancel_counter(BulletStateFixture* p){return &p->rewards.count;}
API(bullet_cancel_spell) void bullet_cancel_spell(BulletStateFixture* p,u32 active){p->rewards.spell_active=active!=0;}
API(bullet_children_count) u32 bullet_children_count(BulletStateFixture* p){return p->emissions;}
API(bullet_children_bytes) const u8* bullet_children_bytes(BulletStateFixture* p){return shooter_bytes(p->children);}
struct PlayerCollisionFixture:PlayerDamageHost{PlayerCollision state;u32 hits=0;void hit()override{hits++;}};
API(player_collision_create) PlayerCollisionFixture* player_collision_create(){return new PlayerCollisionFixture;}
API(player_collision_delete) void player_collision_delete(PlayerCollisionFixture* p){delete p;}
API(player_collision_field) void* player_collision_field(PlayerCollisionFixture* p,u32 field){auto& s=p->state;switch(field){case 0:return &s.position;case 1:return &s.hitbox_min;case 2:return &s.hitbox_max;case 3:return &s.radius;case 4:return &s.size_multiplier;case 5:return &s.state;case 7:return &s.laser_half_size;default:return &s.invulnerability;}}
API(player_collision_options) void player_collision_options(PlayerCollisionFixture* p,u32 enlarged,u32 bomb){p->state.enlarged=enlarged;p->state.bomb_active=bomb;}
API(player_collision_check) i32 player_collision_check(PlayerCollisionFixture* p,u32 circle,float x,float y,float a,float b,u32 graze){return i32(circle?p->state.circle({x,y},a,graze,p):p->state.rectangle({x,y},{a,b},graze,p));}
API(player_collision_hits) u32 player_collision_hits(PlayerCollisionFixture* p){return p->hits;}
API(player_collision_laser) i32 player_collision_laser(PlayerCollisionFixture* p,float x,float y,float angle,float length,float width,u32 graze){return i32(p->state.laser({x,y},angle,length,width,graze!=0,p));}
API(normalize_vector) const Vec2* normalize_vector_api(float x,float y){static Vec2 out;out=normalize_vector({x,y});return &out;}
API(enemy_variables_create) EnemyVariableFixture* enemy_variables_create(){return new EnemyVariableFixture;}
API(enemy_variables_delete) void enemy_variables_delete(EnemyVariableFixture* p){delete p;}
API(enemy_variables_rng) Rng* enemy_variables_rng(EnemyVariableFixture* p){return &p->random;}
API(enemy_boss_field) u32* enemy_boss_field(EnemyVariableFixture* p,u32 kind){return kind?&p->world.manager_flags:p->world.boss_ids.data();}
API(enemy_attribute_field) void* enemy_attribute_field(EnemyVariableFixture* p,u32 kind){auto& e=p->enemy;switch(kind){case 0:return &e.hitbox;case 1:return &e.hurtbox;case 2:return &e.rotation_angle;case 3:return &e.damage_multiplier;case 4:return e.animation_handles.data();case 5:return &e.life;case 6:return &e.primary_drop;case 7:return e.item_drops.data();case 8:return &e.drop_radius;case 9:return &e.age_timer;case 10:return &e.collision_timer;case 11:return &e.invulnerability_timer;case 12:return &e.chapter;default:return &e.life_flags;}}
API(enemy_chapter_field) void* enemy_chapter_field(EnemyVariableFixture* p,u32 kind){auto& w=p->world;switch(kind){case 0:return &w.current_chapter;case 1:return &w.chapter_total;case 2:return &w.chapter_defeated;case 3:return &w.requested_chapter;case 4:return &w.scene_flags;case 5:return &w.spell_flags();default:return &w.enemy_control;}}
API(enemy_event_value) u32 enemy_event_value(EnemyVariableFixture* p,u32 kind){auto& e=p->events;switch(kind){case 0:return e.pauses;case 1:return e.last_handle;case 2:return e.last_pause;case 3:return e.sounds;default:return u32(e.last_sound);}}
API(enemy_event_position) Vec3* enemy_event_position(EnemyVariableFixture* p){return &p->events.sound_position;}
API(enemy_interrupt_fields) i32* enemy_interrupt_fields(EnemyVariableFixture* p,u32 slot){return &p->enemy.interrupts[slot].life;}
API(enemy_interrupt_name) const char* enemy_interrupt_name(EnemyVariableFixture* p,u32 slot,u32 timeout){return (timeout?p->enemy.interrupts[slot].timeout_script:p->enemy.interrupts[slot].script).c_str();}
API(enemy_interrupt_names) void enemy_interrupt_names(EnemyVariableFixture* p,u32 slot,const char* name,const char* timeout){p->enemy.interrupts[slot].script=name;p->enemy.interrupts[slot].timeout_script=timeout;}
API(enemy_death_script) const char* enemy_death_script(EnemyVariableFixture* p){return p->enemy.death_script.c_str();}
API(enemy_set_death_script) void enemy_set_death_script(EnemyVariableFixture* p,const char* name){p->enemy.death_script=name;}
API(enemy_life_threshold) i32* enemy_life_threshold(EnemyVariableFixture* p){return &p->enemy.life_threshold;}
API(enemy_phase_world) void* enemy_phase_world(EnemyVariableFixture* p,u32 kind){auto& w=p->world;switch(kind){case 0:return &w.mode_flags;case 1:return &w.boss_seconds;case 2:return &w.boss_hundredths;case 3:return &w.spell_elapsed();case 4:return &w.spell_bonus();case 5:return &w.game_state;default:return &w.boss_damage_gate;}}
API(enemy_interrupt_check) const char* enemy_interrupt_check(EnemyVariableFixture* p){const auto* name=p->enemy.check_interrupt(p->world);return name?name->c_str():nullptr;}
API(motion_create) MotionState* motion_create(){return new MotionState;}
API(motion_delete) void motion_delete(MotionState* p){delete p;}
API(motion_step) void motion_step(MotionState* p,float rate){p->advance(rate);}
API(motion_integrate) void motion_integrate(MotionState* p,float rate){p->integrate(rate);}
API(position_interpolation_create) PositionInterpolation* position_interpolation_create(){return new PositionInterpolation;}
API(position_interpolation_delete) void position_interpolation_delete(PositionInterpolation* p){delete p;}
API(position_interpolation_step) const void* position_interpolation_step(PositionInterpolation* p,float rate){static std::array<float,3> result;result=p->step(rate);return result.data();}
API(enemy_variables_boss) void enemy_variables_boss(EnemyVariableFixture* p,u32 present){p->world.boss=present?&p->boss:nullptr;}
API(enemy_context) void enemy_context(EnemyVariableFixture* p,EclContext* context){context->variables=&p->variables;context->commands=&p->commands;context->visual_rng=&p->random;}
API(enemy_motion) MotionState* enemy_motion(EnemyVariableFixture* p,u32 field){return field==0?&p->enemy.motion:field==1?&p->enemy.absolute:field==2?&p->enemy.relative:&p->enemy.previous;}
API(enemy_bounds) Vec2* enemy_bounds(EnemyVariableFixture* p,u32 field){return field?&p->enemy.bound_size:&p->enemy.bound_center;}
API(enemy_recompose) void enemy_recompose(EnemyVariableFixture* p){p->enemy.recompose();}
API(enemy_movement_step) i32 enemy_movement_step(EnemyVariableFixture* p,float rate,float dx,float dy,float dz){const auto result=p->enemy.update_movement(rate,{dx,dy,dz});return result==EnemyMovementResult::active?0:result==EnemyMovementResult::outside?-1:-2;}
API(enemy_sprite_size) Vec2* enemy_sprite_size(EnemyVariableFixture* p){return &p->enemy.sprite_size;}
API(enemy_position_interpolation) PositionInterpolation* enemy_position_interpolation(EnemyVariableFixture* p,u32 relative){return relative?&p->enemy.relative_position:&p->enemy.absolute_position;}
API(enemy_scalar_interpolation) ScalarInterpolation* enemy_scalar_interpolation(EnemyVariableFixture* p,u32 field){return field==0?&p->enemy.absolute_angle:field==1?&p->enemy.absolute_speed:field==2?&p->enemy.relative_angle:&p->enemy.relative_speed;}
API(enemy_vec2_interpolation) Vec2Interpolation* enemy_vec2_interpolation(EnemyVariableFixture* p,u32 field){return field==0?&p->enemy.absolute_shape:field==1?&p->enemy.relative_shape:field==2?&p->enemy.absolute_wave:&p->enemy.relative_wave;}
API(enemy_fixture_field) void* enemy_fixture_field(EnemyVariableFixture* p,u32 which,u32 field,u32 index){auto& e=which==0?p->enemy:which==1?p->boss:p->other;switch(field){case 0:return &e.motion.position;case 1:return &e.motion.velocity;case 2:return &e.absolute.position;case 3:return &e.relative.position;case 4:return index==0?&e.absolute.angle:index==1?&e.absolute.speed:&e.absolute.radius;case 5:return index==0?&e.relative.angle:index==1?&e.relative.speed:&e.relative.radius;case 6:return &e.integer_registers[index];case 7:return &e.float_registers[index];case 8:return &e.temporary_registers[index];case 9:return index==0?static_cast<void*>(&e.age_timer.current):index==1?static_cast<void*>(&e.age_timer.fractional):index==2?static_cast<void*>(&e.flags):index==3?static_cast<void*>(&e.life):index==4?static_cast<void*>(&e.collision_timer.current):index==5?static_cast<void*>(&e.animation_frame):index==6?static_cast<void*>(&e.id):static_cast<void*>(&e.parent_id);default:return nullptr;}}
API(enemy_world_field) void* enemy_world_field(EnemyVariableFixture* p,u32 field,u32 index){auto& w=p->world;switch(field){case 0:return &w.player_position;case 1:switch(index){case 0:return &w.rank;case 1:return &w.difficulty;case 2:return &w.mode;case 3:return &w.submode;case 4:return &w.enemy_count;case 5:return &w.total;case 6:return &w.state;case 7:return &w.spell_state;case 8:return &w.player_state;case 9:return &w.counter1;case 10:return &w.current_chapter;default:return &w.counter3;}case 2:return &w.counters[index];case 3:return &w.integer_registers[index];case 4:return &w.float_registers[index];default:return nullptr;}}
API(enemy_variable_read) u32 enemy_variable_read(EnemyVariableFixture* p,i32 id,u32 floating){if(floating){float value=0;p->variables.floating(id,value);u32 bits;std::memcpy(&bits,&value,4);return bits;}i32 value=0;p->variables.integer(id,value);return u32(value);}
API(enemy_variable_destination) void* enemy_variable_destination(EnemyVariableFixture* p,i32 id,u32 floating){return floating?static_cast<void*>(p->variables.float_destination(id)):static_cast<void*>(p->variables.integer_destination(id));}
API(ecl_delete) void ecl_delete(EclResource* p){delete p;}
API(ecl_open) u32 ecl_open(EclResource* p,const u8* data,u32 n){return p->open(data,n);}
API(ecl_count) u32 ecl_count(EclResource* p,u32 kind){return kind==0?p->subroutines.size():kind==1?p->animations.size():p->includes.size();}
API(ecl_name) const char* ecl_name(EclResource* p,u32 kind,u32 n){return kind==0?p->subroutines[n].name.c_str():kind==1?p->animations[n].c_str():p->includes[n].c_str();}
API(ecl_offset) u32 ecl_offset(EclResource* p,u32 n){return p->subroutines[n].offset;}
API(ecl_size) u32 ecl_size(EclResource* p,u32 n){return p->subroutines[n].size;}
API(program_create) EclProgram* program_create(){return new EclProgram;}
API(program_delete) void program_delete(EclProgram* p){delete p;}
API(program_attach) i32 program_attach(EclProgram* p,const u8* b,u32 n){return p->attach(b,n);}
API(program_count) u32 program_count(EclProgram* p){return p->definitions.size();}
API(program_name) const char* program_name(EclProgram* p,u32 n){return p->subroutine(n).name.c_str();}
API(program_instruction) const u8* program_instruction(EclProgram* p,u32 n){auto& def=p->definitions[n];return def.file->bytes.data()+p->subroutine(n).offset+16;}
API(program_find) const void* program_find(EclProgram* p,const char* name){return p->find(name);}
API(replay_create) Replay* replay_create(){return new Replay;}
API(replay_delete) void replay_delete(Replay* p){delete p;}
API(replay_open) u32 replay_open(Replay* p,const u8* data,u32 n){return p->open(data,n);}
API(replay_error) const char* replay_error(Replay* p){return p->error().c_str();}
API(replay_data) const u8* replay_data(Replay* p){return p->decoded().data();}
API(replay_size) u32 replay_size(Replay* p){return p->decoded().size();}
API(replay_stage) const ReplayStage* replay_stage(Replay* p,u32 n){return p->stage(n);}
API(replay_select) u32 replay_select(Replay* p,u32 n){return p->select(n);}
API(replay_tick) const ReplayInput* replay_tick(Replay* p,u32 active){static ReplayInput input;input=p->tick(active);return &input;}
API(replay_frame) u32 replay_frame(Replay* p){return p->frame();}
API(replay_field) u32 replay_field(Replay* p,u32 kind){return kind==0?p->stage_count():kind==1?p->character():kind==2?p->difficulty():p->mode_flags();}
API(timer_create) Timer* timer_create(){return new Timer;}
API(timer_delete) void timer_delete(Timer* p){delete p;}
API(timer_action) void timer_action(Timer* p,u32 action,i32 frame,float speed,u32 use_rate){if(action==0)p->set(frame);else if(action==1)p->tick(use_rate?&speed:nullptr);else p->decrement(use_rate?&speed:nullptr);}
API(config_create) GameConfig* config_create(){return new GameConfig;}
API(config_delete) void config_delete(GameConfig* p){delete p;}
API(config_reset) void config_reset(GameConfig* p,const u8* bindings){p->reset(bindings);}
API(config_open) u32 config_open(GameConfig* p,const u8* data,u32 n){return p->open(data,n);}
API(variables_create) AnmVariables* variables_create(){return new AnmVariables;}
API(variables_delete) void variables_delete(AnmVariables* p){delete p;}
API(variables_integer) i32 variables_integer(AnmVariables* p,Rng* rng,i32 value){return p->integer(value,*rng);}
API(variables_float) u32 variables_float(AnmVariables* p,Rng* rng,float value){const float result=p->floating(value,*rng);u32 bits;std::memcpy(&bits,&result,4);return bits;}
API(variables_destination) const void* variables_destination(AnmVariables* p,void* argument,u32 floating){return floating?static_cast<void*>(p->float_destination(static_cast<float*>(argument))):static_cast<void*>(p->integer_destination(static_cast<i32*>(argument)));}
API(anm_vm_create) AnmVm* anm_vm_create(){return new AnmVm;}
struct BulletSpriteFixture {BulletState state;BulletSpriteSource source{state};};
API(bullet_sprite_source_create) BulletSpriteFixture* bullet_sprite_source_create(){return new BulletSpriteFixture;}
API(bullet_sprite_source_delete) void bullet_sprite_source_delete(BulletSpriteFixture* p){delete p;}
API(bullet_sprite_source_set) void bullet_sprite_source_set(BulletSpriteFixture* p,i32 type,i32 color){p->state.sprite_type=i16(type);p->state.color=i16(color);}
API(anm_vm_sprite_source) void anm_vm_sprite_source(AnmVm* p,BulletSpriteFixture* source){p->sprite_source=source?&source->source:nullptr;}
struct AnmObjectFixture:AnmObjectHost {
    AnmVm child;std::vector<i32> events;
    bool release_tree(AnmVm&)override{return true;}
    AnmVm* spawn_child(AnmVm&,i32 script,u32 ordering)override{events.insert(events.end(),{0,script,i32(ordering),0});return &child;}
    AnmVm* spawn_detached(AnmVm&,i32 script)override{events.insert(events.end(),{1,script,0,0});return &child;}
    bool spawn_effect(AnmVm&,i32 script)override{events.insert(events.end(),{2,script,0,0});return true;}
};
API(anm_object_fixture_create) AnmObjectFixture* anm_object_fixture_create(){return new AnmObjectFixture;}
API(anm_object_fixture_delete) void anm_object_fixture_delete(AnmObjectFixture* p){delete p;}
API(anm_object_fixture_reset) void anm_object_fixture_reset(AnmObjectFixture* p){p->events.clear();p->child=AnmVm{};}
API(anm_object_fixture_child) AnmVm* anm_object_fixture_child(AnmObjectFixture* p){return &p->child;}
API(anm_object_fixture_events) const i32* anm_object_fixture_events(AnmObjectFixture* p){return p->events.data();}
API(anm_object_fixture_count) u32 anm_object_fixture_count(AnmObjectFixture* p){return p->events.size()/4;}
API(anm_vm_object_host) void anm_vm_object_host(AnmVm* p,AnmObjectFixture* host){p->object_host=host;}
API(anm_vm_creation_parent) void anm_vm_creation_parent(AnmVm* p,AnmVm* parent){p->creation_parent=parent;}
struct BulletVisualFixture {BulletState state;Rng random;BulletVisual visual{state,random};};
API(bullet_visual_create) BulletVisualFixture* bullet_visual_create(){return new BulletVisualFixture;}
API(bullet_visual_delete) void bullet_visual_delete(BulletVisualFixture* p){delete p;}
API(bullet_visual_bind) u32 bullet_visual_bind(BulletVisualFixture* p,AnmResource* resource,i32 type,i32 color,float rate){const auto* spec=bullet_appearance(type);if(!spec)return 0;p->state.sprite_type=i16(type);p->state.color=i16(color);p->visual.resource=resource;p->visual.rate=rate;return p->visual.bind(spec->script,spec->overlay);}
API(bullet_visual_vm) AnmVm* bullet_visual_vm(BulletVisualFixture* p,u32 overlay){return overlay?&p->visual.overlay:&p->visual.body;}
API(bullet_visual_rng) Rng* bullet_visual_rng(BulletVisualFixture* p){return &p->random;}
API(bullet_visual_interrupt) u32 bullet_visual_interrupt(BulletVisualFixture* p,i32 id){return p->visual.interrupt(id);}
API(bullet_visual_tick) i32 bullet_visual_tick(BulletVisualFixture* p,u32 overlay){return p->visual.tick(overlay);}
API(bullet_visual_state_field) void* bullet_visual_state_field(BulletVisualFixture* p,u32 field){switch(field){case 0:return &p->state.visual_flags;case 1:return &p->state.visual_jitter;case 2:return &p->state.graze_color;case 3:return &p->state.hit_interrupt;default:return &p->state.spawn_animation_finished;}}
API(bullet_visual_error) const char* bullet_visual_error(BulletVisualFixture* p){return p->visual.error.c_str();}
API(anm_vm_delete) void anm_vm_delete(AnmVm* p){delete p;}
API(anm_vm_bind) u32 anm_vm_bind(AnmVm* p,AnmResource* resource,u32 script){return p->bind(*resource,script);}
API(anm_vm_select_sprite) u32 anm_vm_select_sprite(AnmVm* p,i32 index){return p->select_sprite(index);}
API(anm_vm_resource) void anm_vm_resource(AnmVm* p,AnmResource* resource){p->resource=resource;}
API(anm_vm_sprite_values) const float* anm_vm_sprite_values(AnmVm* p){return reinterpret_cast<const float*>(p->visual.uv);}
API(anm_vm_tick) i32 anm_vm_tick(AnmVm* p,Rng* rng,float rate){return p->tick(*rng,rate);}
API(anm_vm_variables) AnmVariables* anm_vm_variables(AnmVm* p){return &p->variables;}
API(anm_vm_visual) AnmVisualState* anm_vm_visual(AnmVm* p){return &p->visual;}
API(anm_vm_translation) Vec3* anm_vm_translation(AnmVm* p){return &p->visual.translation;}
API(anm_vm_interpolation) const void* anm_vm_interpolation(AnmVm* p,u32 kind){auto& i=p->interpolators;switch(kind){case 0:return &i.position;case 1:return &i.color;case 2:return &i.alpha;case 3:return &i.rotation;case 4:return &i.angle;case 5:return &i.scale;case 6:return &i.secondary_scale;case 7:return &i.uv_scale;case 8:return &i.secondary_color;case 9:return &i.secondary_alpha;case 10:return &i.uv_x;default:return &i.uv_y;}}
API(anm_vm_timer) Timer* anm_vm_timer(AnmVm* p){return &p->timer;}
API(anm_vm_offset) i32 anm_vm_offset(AnmVm* p){return p->instruction_offset;}
API(anm_vm_interrupt) void anm_vm_interrupt(AnmVm* p,i32 id){p->pending_interrupt=id;}
API(anm_vm_pending_interrupt) i32 anm_vm_pending_interrupt(AnmVm* p){return p->pending_interrupt;}
API(anm_quad_positions) u32 anm_quad_positions_export(AnmVm* p,Vec3* output){Vec3 positions[4];if(!anm_quad_positions(*p,positions))return 0;std::memcpy(output,positions,sizeof positions);return 1;}
API(anm_adjust_position) u32 anm_adjust_position_export(AnmVm* p,Vec3* position){return anm_adjust_position(*p,*position);}
API(anm_position) u32 anm_position_export(AnmVm* p,Vec3* position){return anm_position(*p,*position);}
API(anm_environment_scale) float* anm_environment_scale(AnmEnvironment* p){return &p->resolution_scale;}
API(anm_environment_offsets) i32* anm_environment_offsets(AnmEnvironment* p){return p->screen_offsets.data();}
API(anm_vm_geometry_bytes) u32 anm_vm_geometry_bytes(AnmVm* p){return p->geometry.allocation_bytes;}
API(anm_vm_trail) AnmTrail* anm_vm_trail(AnmVm* p){return p->geometry.trail.get();}
API(anm_vm_trail_initialize) void anm_vm_trail_initialize(AnmVm* p,Rng* random){p->geometry.clear();p->geometry.trail=std::make_unique<AnmTrail>();p->geometry.trail->initialize(*random);p->geometry.allocation_bytes=0x318;}
API(anm_vm_screen_vertices) AnmGeometryVertex* anm_vm_screen_vertices(AnmVm* p){return p->geometry.screen_vertices.data();}
API(anm_vm_world_vertices) AnmWorldVertex* anm_vm_world_vertices(AnmVm* p){return p->geometry.world_vertices.data();}
API(anm_environment_delta) Vec3* anm_environment_delta(AnmEnvironment* p){return &p->background_delta;}
API(anm_environment_pause) void anm_environment_pause(AnmEnvironment* p,u32 paused){p->paused=paused!=0;}
API(anm_vm_distortion_vertices) AnmGeometryVertex* anm_vm_distortion_vertices(AnmVm* p){return p->geometry.distortion->vertices.data();}
API(anm_vm_distortion_radius) float* anm_vm_distortion_radius(AnmVm* p){return p->geometry.distortion->radius.data();}
API(anm_vm_distortion_speed) float* anm_vm_distortion_speed(AnmVm* p){return p->geometry.distortion->radial_speed.data();}
API(anm_vm_distortion_uv_speed) Vec2* anm_vm_distortion_uv_speed(AnmVm* p){return &p->geometry.distortion->uv_speed;}
API(anm_vm_slowdown) void anm_vm_slowdown(AnmVm* p,float amount){p->slowdown=amount;}
API(anm_environment_create) AnmEnvironment* anm_environment_create(){return new AnmEnvironment;}
API(anm_environment_delete) void anm_environment_delete(AnmEnvironment* p){delete p;}
API(anm_vm_environment) void anm_vm_environment(AnmVm* p,AnmEnvironment* environment){p->environment=environment;}
API(anm_vm_parent) void anm_vm_parent(AnmVm* p,AnmVm* parent){p->rotation_parent=parent;}
API(anm_vm_error) const char* anm_vm_error(AnmVm* p){return p->error.c_str();}
API(interpolation_weight) u32 interpolation_weight_api(i32 mode,float elapsed,float duration){const float value=interpolation_weight(mode,elapsed,duration);u32 bits;std::memcpy(&bits,&value,4);return bits;}
API(interpolation_create) void* interpolation_create(u32 dimension){return dimension==1?static_cast<void*>(new ScalarInterpolation):dimension==2?static_cast<void*>(new Vec2Interpolation):dimension==4?static_cast<void*>(new AngleInterpolation):static_cast<void*>(new Vec3Interpolation);}
API(interpolation_delete) void interpolation_delete(void* p,u32 dimension){if(dimension==1)delete static_cast<ScalarInterpolation*>(p);else if(dimension==2)delete static_cast<Vec2Interpolation*>(p);else if(dimension==4)delete static_cast<AngleInterpolation*>(p);else delete static_cast<Vec3Interpolation*>(p);}
API(interpolation_step) const void* interpolation_step(void* p,u32 dimension,float rate){static std::array<float,3> value;value={};if(dimension==1){auto result=static_cast<ScalarInterpolation*>(p)->step(rate);value[0]=result[0];}else if(dimension==2){auto result=static_cast<Vec2Interpolation*>(p)->step(rate);value[0]=result[0];value[1]=result[1];}else if(dimension==4){auto result=static_cast<AngleInterpolation*>(p)->step(rate);value[0]=result[0];}else value=static_cast<Vec3Interpolation*>(p)->step(rate);return value.data();}
API(integer_interpolation_create) void* integer_interpolation_create(u32 dimension){return dimension==1?static_cast<void*>(new AlphaInterpolation):static_cast<void*>(new ColorInterpolation);}
API(integer_interpolation_delete) void integer_interpolation_delete(void* p,u32 dimension){if(dimension==1)delete static_cast<AlphaInterpolation*>(p);else delete static_cast<ColorInterpolation*>(p);}
API(integer_interpolation_step) const void* integer_interpolation_step(void* p,u32 dimension,float rate){static std::array<i32,3> value;value={};if(dimension==1){auto result=static_cast<AlphaInterpolation*>(p)->step(rate);value[0]=result[0];}else value=static_cast<ColorInterpolation*>(p)->step(rate);return value.data();}
API(anm_fixture_create) AnmResource* anm_fixture_create(const u8* bytes,u32 n){auto* p=new AnmResource;AnmScript script;script.bytes.assign(bytes,bytes+n);p->scripts.push_back(std::move(script));return p;}
API(sht_create) ShtResource* sht_create(){return new ShtResource;}
API(sht_delete) void sht_delete(ShtResource* p){delete p;}
API(sht_open) u32 sht_open(ShtResource* p,const u8* data,u32 n){return p->open(data,n);}
API(sht_header) const ShtHeader* sht_header(ShtResource* p){return &p->header;}
API(sht_count) u32 sht_count(ShtResource* p,u32 group){return p->groups[group].size();}
API(sht_shot) const ShotSpec* sht_shot(ShtResource* p,u32 group,u32 n){return &p->groups[group][n];}
API(anm_sprite) const AnmSprite* anm_sprite(AnmResource* p,u32 n){return n<p->sprites.size()?&p->sprites[n]:nullptr;}
API(crypt) u32 crypt(const u8* input,u8* output,u32 size,u32 key,u32 step,u32 block,u32 limit,u32 encode){return resource_crypt(input,output,size,{u8(key),u8(step),block,limit},encode);}
API(codec_create) Lzss* codec_create(){return new Lzss;}
API(codec_delete) void codec_delete(Lzss* p){delete p;}
API(codec_decode) i32 codec_decode(Lzss* p,const u8* input,u32 n,u8* output,u32 capacity){u32 written=0;return p->decode(input,n,output,capacity,written)?i32(written):-1;}
API(codec_encode) u32 codec_encode(Lzss* p,const u8* input,u32 n,u8* output,u32 capacity){auto bytes=p->encode(input,n);if(bytes.size()>capacity)return 0;std::memcpy(output,bytes.data(),bytes.size());return bytes.size();}
API(stream_create) LzssStream* stream_create(){return new LzssStream;}
API(stream_delete) void stream_delete(LzssStream* p){delete p;}
API(stream_step) u32 stream_step(LzssStream* p,const u8* input,u32 size,u8* output,u32 capacity,u32 budget){return p->step(input,size,output,capacity,budget);}
API(stream_size) u32 stream_size(LzssStream* p){return p->size();}
API(stream_done) u32 stream_done(LzssStream* p){return p->done();}
API(archive_create) Archive* archive_create(){return new Archive;}
API(archive_delete) void archive_delete(Archive* p){delete p;}
API(archive_open) u32 archive_open(Archive* p,const u8* data,u32 n){return p->open(data,n);}
API(archive_count) u32 archive_count(Archive* p){return p->entries.size();}
API(archive_name) const char* archive_name(Archive* p,u32 n){return n<p->entries.size()?p->entries[n].name.c_str():nullptr;}
API(archive_field) u32 archive_field(Archive* p,u32 n,u32 field){if(n>=p->entries.size())return 0;auto& e=p->entries[n];return field==0?e.offset:field==1?e.size:e.compressed;}
API(archive_read) i32 archive_read(Archive* p,u32 n,u8* out,u32 capacity){std::vector<u8> data;if(!p->read(n,data)||data.size()>capacity)return -1;std::memcpy(out,data.data(),data.size());return data.size();}
API(anm_create) AnmResource* anm_create(){return new AnmResource;}
API(anm_delete) void anm_delete(AnmResource* p){delete p;}
API(anm_open) u32 anm_open(AnmResource* p,const u8* data,u32 n){return p->open(data,n);}
API(anm_count) u32 anm_count(AnmResource* p,u32 kind){return kind==0?p->textures.size():kind==1?p->sprites.size():p->scripts.size();}
API(anm_name) const char* anm_name(AnmResource* p,u32 n){return n<p->textures.size()?p->textures[n].name.c_str():nullptr;}
API(anm_texture) u32 anm_texture(AnmResource* p,u32 n,u32 field){auto& t=p->textures[n];switch(field){case 0:return t.width;case 1:return t.height;case 2:return t.pixel_format;case 3:return t.low_resolution_scale;case 4:return t.pixel_width;case 5:return t.pixel_height;default:return t.pixels.size();}}
API(anm_script) const u8* anm_script(AnmResource* p,u32 n){return n<p->scripts.size()?p->scripts[n].bytes.data():nullptr;}
API(anm_script_size) u32 anm_script_size(AnmResource* p,u32 n){return n<p->scripts.size()?p->scripts[n].bytes.size():0;}
API(anm_script_id) i32 anm_script_id(AnmResource* p,u32 n){return p->scripts[n].source_id;}
API(anm_rgba) i32 anm_rgba(AnmResource* p,u32 n,u8* out,u32 capacity){std::vector<u8> data;if(n>=p->textures.size()||!p->textures[n].rgba(data)||data.size()>capacity)return -1;std::memcpy(out,data.data(),data.size());return data.size();}

API(line_intersection) u32 line_intersection_export(const Vec2* first,float a,const Vec2* second,float b,Vec2* result){return line_intersection(*first,a,*second,b,*result);}

struct LaserContactFixture:LaserContactHost {
    PlayerCollisionFixture& player;LaserContact contact;Vec3 position{};float angle=0,length=128,width=16,rate=1;u32 flags=0;i32 state=2;Timer flash;float resonance=0;std::vector<u32> events;
    explicit LaserContactFixture(PlayerCollisionFixture& p):player(p){}
    static u32 bits(float f){u32 b;std::memcpy(&b,&f,4);return b;}
    bool cancel_hit(const Vec3& p)override{events.insert(events.end(),{1,bits(p.x),bits(p.y),bits(p.z)});return true;}
    void graze_spark(const Vec3& p)override{events.insert(events.end(),{2,bits(p.x),bits(p.y),bits(p.z)});}
    void graze_flash()override{flash.set(10);}
    void graze_resonance(float v)override{resonance=v;}
};
API(laser_contact_create) LaserContactFixture* laser_contact_create(PlayerCollisionFixture* p){return new LaserContactFixture(*p);}
API(laser_contact_delete) void laser_contact_delete(LaserContactFixture* p){delete p;}
API(laser_contact_field) void* laser_contact_field(LaserContactFixture* p,u32 field){switch(field){case 0:return &p->position;case 1:return &p->angle;case 2:return &p->length;case 3:return &p->width;case 4:return &p->flags;case 5:return &p->state;case 6:return &p->contact.graze_age;case 7:return &p->rate;case 8:return &p->flash;default:return &p->resonance;}}
API(laser_contact_update) u32 laser_contact_update(LaserContactFixture* p,u32 moving,u32 graze){p->events.clear();return moving?p->contact.moving(p->position,p->angle,p->length,p->width,p->flags,graze!=0,p->rate,p->player.state,p->player,*p):p->contact.stationary(p->position,p->angle,p->length,p->width,p->state,graze!=0,p->rate,p->player.state,p->player,*p);}
API(laser_contact_events) const u32* laser_contact_events(LaserContactFixture* p){return p->events.data();}
API(laser_contact_event_count) u32 laser_contact_event_count(LaserContactFixture* p){return p->events.size();}

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

struct LaserDynamicsFixture:BulletSoundHost {MovingLaserMotion motion;LaserAcceleration acceleration;LaserTurn turn;u32 flags=0;i32 sound=-1;std::vector<i32> sounds;void play(i32 id)override{sounds.push_back(id);}};
API(laser_dynamics_create) LaserDynamicsFixture* laser_dynamics_create(){return new LaserDynamicsFixture;}
API(laser_dynamics_delete) void laser_dynamics_delete(LaserDynamicsFixture* p){delete p;}
API(laser_dynamics_motion) MovingLaserMotion* laser_dynamics_motion(LaserDynamicsFixture* p){return &p->motion;}
API(laser_dynamics_field) void* laser_dynamics_field(LaserDynamicsFixture* p,u32 field){switch(field){case 0:return &p->flags;case 1:return &p->acceleration.timer;case 2:return &p->acceleration.speed;case 3:return &p->acceleration.angle;case 4:return &p->acceleration.vector;case 5:return &p->acceleration.duration;case 6:return &p->turn.timer;case 7:return &p->turn.speed;case 8:return &p->turn.angle;case 9:return &p->turn.duration;case 10:return &p->turn.limit;case 11:return &p->turn.count;case 12:return &p->turn.mode;default:return &p->sound;}}
API(laser_dynamics_update) u32 laser_dynamics_update(LaserDynamicsFixture* p,u32 turn,float rate){p->sounds.clear();return turn?p->turn.advance(p->motion,p->flags,rate,p->sound,p):p->acceleration.advance(p->motion,p->flags,rate);}
API(laser_dynamics_sound_count) u32 laser_dynamics_sound_count(LaserDynamicsFixture* p){return p->sounds.size();}
API(laser_dynamics_sounds) const i32* laser_dynamics_sounds(LaserDynamicsFixture* p){return p->sounds.data();}
API(segment_intersection) u32 segment_intersection_test(const Vec2* a,const Vec2* b,const Vec2* c,const Vec2* d,Vec2* out){return segment_intersection(*a,*b,*c,*d,*out);}
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

API(laser_scene_transform) void* laser_scene_transform(LaserSceneFixture* p,u32 index,u32 field){auto& t=p->request.transforms[index];switch(field){case 0:return t.floats.data();case 1:return t.integers.data();case 2:return &t.type;default:return &t.active;}}
API(laser_scene_program) void* laser_scene_program(LaserSceneFixture* p,u32 id,u32 field){auto* s=p->scene.moving_program(id);if(!s)return nullptr;switch(field){case 0:return &s->flags;case 1:return &s->index;case 2:return &s->boost_timer;case 3:return &s->boost_stage;case 4:return &s->angular_timer;case 5:return &s->angular_speed;case 6:return &s->angular_angle;case 7:return &s->angular_duration;case 8:return &s->wait;case 9:return &s->freeze;case 10:return &s->acceleration.timer;case 11:return &s->acceleration.speed;case 12:return &s->acceleration.angle;case 13:return &s->acceleration.vector;case 14:return &s->acceleration.duration;case 15:return &s->turn.timer;case 16:return &s->turn.speed;case 17:return &s->turn.angle;case 18:return &s->turn.duration;case 19:return &s->turn.limit;case 20:return &s->turn.count;case 21:return &s->turn.mode;case 22:return &s->reflection.speed;case 23:return &s->reflection.count;case 24:return &s->reflection.limit;default:return &s->reflection.sides;}}

API(laser_scene_stationary_request) void* laser_scene_stationary_request(LaserSceneFixture* p,u32 field){auto& r=p->stationary_request;switch(field){case 0:return &r.position;case 1:return &r.drift;case 2:return &r.angle;case 3:return &r.angular_velocity;case 4:return &r.maximum_length;case 5:return &r.initial_length;case 6:return &r.width;case 7:return &r.speed;case 8:return &r.delay;case 9:return &r.warmup;case 10:return &r.active;case 11:return &r.fade;case 12:return &r.sound;case 13:return &r.transform_sound;case 14:return &r.id;case 15:return &r.start_offset;case 16:return &r.transform_index;case 17:return &r.type;case 18:return &r.color;default:return &r.flags;}}
API(laser_scene_stationary_transform) void* laser_scene_stationary_transform(LaserSceneFixture* p,u32 index,u32 field){auto& t=p->stationary_request.transforms[index];return field==0?static_cast<void*>(t.floats.data()):field==1?static_cast<void*>(t.integers.data()):field==2?static_cast<void*>(&t.type):static_cast<void*>(&t.active);}
API(laser_scene_stationary_emit) u32 laser_scene_stationary_emit(LaserSceneFixture* p){return p->scene.create_stationary(p->stationary_request);}
API(laser_scene_stationary_motion) StationaryLaserMotion* laser_scene_stationary_motion(LaserSceneFixture* p,u32 id){return p->scene.stationary_motion(id);}
API(laser_scene_stationary_program) void* laser_scene_stationary_program(LaserSceneFixture* p,u32 id,u32 field){auto* v=p->scene.stationary_program(id);return !v?nullptr:field==0?static_cast<void*>(&v->flags):field==1?static_cast<void*>(&v->index):static_cast<void*>(&v->freeze);}

API(enemy_laser_emitted_count) u32 enemy_laser_emitted_count(EnemyVariableFixture* p){return p->laser_emitted.count;}
API(enemy_laser_emitted_kind) u32 enemy_laser_emitted_kind(EnemyVariableFixture* p){return p->laser_emitted.kind;}
API(enemy_laser_emitted_size) u32 enemy_laser_emitted_size(EnemyVariableFixture* p){return p->laser_emitted.bytes.size();}
API(enemy_laser_emitted_bytes) const u8* enemy_laser_emitted_bytes(EnemyVariableFixture* p){return p->laser_emitted.bytes.data();}

API(enemy_laser_scene) void enemy_laser_scene(EnemyVariableFixture* p,LaserSceneFixture* scene){p->commands.lasers.scene=&scene->scene;}
API(enemy_manager_lasers) void enemy_manager_lasers(EnemyManagerFixture* p,LaserSceneFixture* scene){p->laser_scene=scene?&scene->scene:nullptr;}

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
API(curve_motion_timer) Timer* curve_motion_timer(CurveLaserMotion* p,u32 outside){return outside?&p->outside_delay:&p->path_age;}
API(curve_motion_step) u32 curve_motion_step(CurveLaserMotion* p,float rate,u32 flags){const bool finished=p->advance(rate,flags);if(!finished)p->finish_frame(rate);return finished;}

struct CurveProgramFixture:CurveProgramHost {
    CurveLaserMotion motion;CurveProgram program;std::array<BulletTransform,18> entries{};u32 id=65536,blend_flags=6;i32 state=2,protection=0;std::vector<u32> events;
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
API(curve_program_field) void* curve_program_field(CurveProgramFixture* p,u32 i){auto& s=p->program;switch(i){case 0:return &s.flags;case 1:return &s.index;case 2:return &p->id;case 3:return &p->state;case 4:return &p->protection;case 5:return &s.freeze;case 6:return &s.protection_timer;case 7:return &s.turn.timer;case 8:return &s.turn.angle;case 9:return &s.turn.speed;case 10:return &s.turn.duration;case 11:return &s.turn.limit;case 12:return &s.turn.count;case 13:return &s.turn.mode;case 14:return &s.wait;case 15:return &s.boost_timer;case 16:return &s.boost_stage;case 17:return &p->blend_flags;default:return &s.protection_parameter;}}
API(curve_program_kinematics) void* curve_program_kinematics(CurveProgramFixture* p,u32 i){switch(i){case 0:return &p->motion.velocity;case 1:return &p->motion.angle;default:return &p->motion.speed;}}
API(curve_program_step) i32 curve_program_step(CurveProgramFixture* p,float rate,i32 sound){p->events.clear();CurveProgramContext context{p->motion,p->id,p->state,p->protection,*p,sound};if(!p->program.update(p->entries,context,rate))return -1;const bool finished=p->motion.advance(rate,p->program.flags);if(!finished)p->motion.finish_frame(rate);return finished;}
API(curve_program_error) const char* curve_program_error(CurveProgramFixture* p){return p->program.error.c_str();}
API(curve_program_events) const u32* curve_program_events(CurveProgramFixture* p){return p->events.data();}
API(curve_program_event_count) u32 curve_program_event_count(CurveProgramFixture* p){return p->events.size();}
API(curve_motion_path_count) u32 curve_motion_path_count(CurveLaserMotion* p){return p->path.segments.size();}

API(laser_visual_initialize_curve) u32 laser_visual_initialize_curve(LaserVisualFixture* p,i32 type,i32 color){return p->visual.initialize_curved(type,color);}
API(laser_visual_update_curve) u32 laser_visual_update_curve(LaserVisualFixture* p,float rate){const float prior=p->manager.rate;p->manager.rate=rate;const bool ok=p->visual.update_curved();p->manager.rate=prior;return ok;}

struct CurveCancellationFixture:CurveCancellationHost {
    AnmManagerFixture& animations;i32 resource;CurveLaserMotion motion;u32 flags=1;i32 color=0,protection=0,state=2;BulletCancellationRewards rewards;CurveCancellation cancellation;EffectManager tracking;std::vector<u32> events;std::vector<float> splits;
    CurveCancellationFixture(AnmManagerFixture& a,i32 id):animations(a),resource(id),cancellation(a.environment.game_rng,rewards,*this),tracking(a.manager,id){}
    bool curve_effect(i32 script,const Vec3& p,bool tracked)override{auto& a=animations.manager;const u32 handle=a.create(resource,script,-1,0,p);if(!handle)return false;a.registry.find(handle)->visual.inherited_color=0;if(tracked)tracking.track(handle);return true;}
    bool split_curve(u32 count,float time,const CurvePath&)override{splits.insert(splits.end(),{float(count),time});return true;}
    void item(i32 type,const Vec3& p,float angle,float speed)override{events.insert(events.end(),{u32(type),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(angle),float_to_bits(speed)});}
};
API(curve_cancel_create) CurveCancellationFixture* curve_cancel_create(AnmManagerFixture* a,i32 id){return new CurveCancellationFixture(*a,id);}
API(curve_cancel_delete) void curve_cancel_delete(CurveCancellationFixture* p){delete p;}
API(curve_cancel_motion) CurveLaserMotion* curve_cancel_motion(CurveCancellationFixture* p){return &p->motion;}
API(curve_cancel_field) void* curve_cancel_field(CurveCancellationFixture* p,u32 i){switch(i){case 0:return &p->flags;case 1:return &p->color;case 2:return &p->protection;case 3:return &p->state;case 4:return &p->rewards.count;case 5:return &p->animations.environment.game_rng;case 6:return &p->animations.random;case 7:return &p->rewards.spell_active;case 8:return p->tracking.handles.data();default:return &p->tracking.cursor;}}
API(curve_cancel_apply) i32 curve_cancel_apply(CurveCancellationFixture* p,u32 kind,const Vec3* center,float radius,const Vec2* size,float angle,i32 reward,u32 honor,float rate){p->animations.manager.rate=rate;p->events.clear();p->splits.clear();if(kind==0)return p->cancellation.circle(p->motion,p->flags,p->color,p->protection,honor!=0,*center,radius,reward);if(kind==1)return p->cancellation.rectangle(p->motion,p->flags,p->color,p->protection,honor!=0,*center,*size,angle,reward,rate)?0:-1;return p->cancellation.all(p->motion,p->color,p->protection,honor!=0,p->state)?0:-1;}
API(curve_cancel_events) const u32* curve_cancel_events(CurveCancellationFixture* p){return p->events.data();}
API(curve_cancel_event_count) u32 curve_cancel_event_count(CurveCancellationFixture* p){return p->events.size();}
API(curve_cancel_splits) const float* curve_cancel_splits(CurveCancellationFixture* p){return p->splits.data();}
API(curve_cancel_split_count) u32 curve_cancel_split_count(CurveCancellationFixture* p){return p->splits.size()/2;}
API(curve_motion_count) u32 curve_motion_count(CurveLaserMotion* p){return p->points.size();}
API(curve_cancel_error) const char* curve_cancel_error(CurveCancellationFixture* p){return p->cancellation.error.c_str();}

API(curve_contact_update) u32 curve_contact_update(LaserContactFixture* p,CurveLaserMotion* motion,u32 graze){p->events.clear();return p->contact.curved(*motion,graze!=0,p->rate,p->player.state,p->player,*p);}

API(laser_scene_curve_request) void* laser_scene_curve_request(LaserSceneFixture* p,u32 field){auto& r=p->curve_request;switch(field){case 0:return &r.position;case 1:return &r.angle;case 2:return &r.width;case 3:return &r.speed;case 4:return &r.type;case 5:return &r.color;case 6:return &r.count;case 7:return &r.start_offset;case 8:return &r.flags;case 9:return &r.sound;case 10:return &r.transform_sound;case 11:return &r.transform_index;default:return &r.initial_time;}}
API(laser_scene_curve_transform) void* laser_scene_curve_transform(LaserSceneFixture* p,u32 index,u32 field){auto& t=p->curve_request.transforms[index];return field==0?static_cast<void*>(t.floats.data()):field==1?static_cast<void*>(t.integers.data()):field==2?static_cast<void*>(&t.type):static_cast<void*>(&t.active);}
API(laser_scene_curve_emit) u32 laser_scene_curve_emit(LaserSceneFixture* p){return p->scene.create_curved(p->curve_request);}
API(laser_scene_curve_motion) CurveLaserMotion* laser_scene_curve_motion(LaserSceneFixture* p,u32 id){return p->scene.curve_motion(id);}
API(curve_motion_field) void* curve_motion_field(CurveLaserMotion* p,u32 i){switch(i){case 0:return &p->position;case 1:return &p->velocity;case 2:return &p->angle;case 3:return &p->width;case 4:return &p->speed;case 5:return &p->emission_distance;case 6:return &p->source;case 7:return &p->source_angle;default:return &p->source_speed;}}
API(laser_scene_curve_program) CurveProgram* laser_scene_curve_program(LaserSceneFixture* p,u32 id){return p->scene.curve_program(id);}
API(curve_program_index) i32* curve_program_index(CurveProgram* p){return &p->index;}

API(laser_visual_draw_curve) u32 laser_visual_draw_curve(LaserVisualFixture* p,AnmRendererFixture* renderer,const CurveLaserMotion* motion){const bool ok=p->visual.draw_curved(*motion,renderer->renderer);renderer->renderer.flush();return ok;}
API(anm_renderer_history_size) u32 anm_renderer_history_size(AnmRendererFixture* p,u32 i){return i<p->graphics.history.size()?p->graphics.history[i].size():0;}

#include "../../cpp/game/BulletArea.hpp"
API(bullet_area_select) u32 bullet_area_select(BulletStateFixture* p,u32 kind,const Vec3* center,float radius,const Vec2* size,float angle){const auto& s=p->external_state?*p->external_state:p->state;return kind==2?bullet_rectangle_area(s,*center,*size,angle):bullet_circle_area(s,*center,radius,kind!=0);}

API(enemy_area_clear) void enemy_area_clear(EnemyVariableFixture* p){p->events.area_events.clear();}
API(enemy_area_events) const u32* enemy_area_events(EnemyVariableFixture* p){return p->events.area_events.data();}
API(enemy_area_event_count) u32 enemy_area_event_count(EnemyVariableFixture* p){return p->events.area_events.size();}
API(bullet_scene_cancel_area) u32 bullet_scene_cancel_area(BulletSceneFixture* p,u32 kind,const Vec3* center,float radius,const Vec2* size,float angle,i32 reward){return kind==2?p->scene.cancel_rectangle(*center,*size,angle,reward):p->scene.cancel_circle(*center,radius,reward,kind!=0);}
API(bullet_scene_cancel_rewards) BulletCancellationRewards* bullet_scene_cancel_rewards(BulletSceneFixture* p){return &p->scene.manager.cancellation_rewards;}

API(laser_scene_segmented_request) void* laser_scene_segmented_request(LaserSceneFixture* p,u32 i){auto& r=p->segmented_request;switch(i){case 0:return &r.position;case 1:return &r.angle;case 2:return &r.length;case 3:return &r.width;case 4:return &r.id;case 5:return &r.color;case 6:return &r.start_offset;default:return &r.flags;}}
API(laser_scene_segmented_emit) u32 laser_scene_segmented_emit(LaserSceneFixture* p){return p->scene.create_segmented(p->segmented_request);}
API(laser_scene_segmented_motion) MovingLaserMotion* laser_scene_segmented_motion(LaserSceneFixture* p,u32 id){return p->scene.segmented_motion(id);}

struct FrameSchedulerFixture {
    struct Entry {FrameCallback callback;FrameSchedulerFixture* fixture=nullptr;u32 id=0;i32 first=1,later=1,operation=0,target=0,priority=0,pass=0,auxiliary=0,calls=0;};
    std::array<Entry,16> entries;FrameScheduler scheduler;std::vector<u32> events;
    FrameSchedulerFixture(){for(u32 i=0;i<entries.size();++i){auto& e=entries[i];e.fixture=this;e.id=i;e.callback.owner=&e;}}
    void record(u32 kind,u32 id){events.push_back(kind);events.push_back(id);}
    void mutate(Entry& e){auto& target=entries[u32(e.target)&15];switch(e.operation){
        case 1:scheduler.add(target.callback,e.pass?FramePass::Draw:FramePass::Update,e.priority);break;
        case 2:scheduler.remove(target.callback);break;
        case 3:target.callback.enabled=e.auxiliary!=0;break;
        case 4:scheduler.closing=e.auxiliary!=0;break;
        case 5:scheduler.clear(e.pass?FramePass::Draw:FramePass::Update);break;
        case 6:target.callback.run=nullptr;break;
    }}
    static i32 run(void* p){auto& e=*static_cast<Entry*>(p);auto& f=*e.fixture;f.record(1,e.id);++e.calls;if(e.calls==1)f.mutate(e);return e.calls==1?e.first:e.later;}
    static i32 initialize(void* p){auto& e=*static_cast<Entry*>(p);e.fixture->record(2,e.id);return e.auxiliary;}
    static i32 cleanup(void* p){auto& e=*static_cast<Entry*>(p);e.fixture->record(3,e.id);return 0;}
};
API(frame_scheduler_create) FrameSchedulerFixture* frame_scheduler_create(){return new FrameSchedulerFixture;}
API(frame_scheduler_delete) void frame_scheduler_delete(FrameSchedulerFixture* p){delete p;}
API(frame_scheduler_configure) void frame_scheduler_configure(FrameSchedulerFixture* p,u32 id,i32 first,i32 later,i32 operation,i32 target,i32 priority,i32 pass,i32 auxiliary,u32 flags){auto& e=p->entries[id];e.first=first;e.later=later;e.operation=operation;e.target=target;e.priority=priority;e.pass=pass;e.auxiliary=auxiliary;e.callback.run=flags&1?&FrameSchedulerFixture::run:nullptr;e.callback.initialize=flags&2?&FrameSchedulerFixture::initialize:nullptr;e.callback.cleanup=flags&4?&FrameSchedulerFixture::cleanup:nullptr;e.callback.enabled=flags&8;}
API(frame_scheduler_add) i32 frame_scheduler_add(FrameSchedulerFixture* p,u32 id,u32 pass,i32 priority){return p->scheduler.add(p->entries[id].callback,pass?FramePass::Draw:FramePass::Update,priority);}
API(frame_scheduler_remove) void frame_scheduler_remove(FrameSchedulerFixture* p,u32 id){p->scheduler.remove(p->entries[id].callback);}
API(frame_scheduler_clear) void frame_scheduler_clear(FrameSchedulerFixture* p,u32 pass){p->scheduler.clear(pass?FramePass::Draw:FramePass::Update);}
API(frame_scheduler_closing) void frame_scheduler_closing(FrameSchedulerFixture* p,u32 closing){p->scheduler.closing=closing;}
API(frame_scheduler_execute) i32 frame_scheduler_execute(FrameSchedulerFixture* p,u32 pass){return pass?p->scheduler.draw():p->scheduler.update();}
API(frame_scheduler_events) u32* frame_scheduler_events(FrameSchedulerFixture* p){return p->events.data();}
API(frame_scheduler_event_count) u32 frame_scheduler_event_count(FrameSchedulerFixture* p){return p->events.size()/2;}
API(frame_scheduler_event_reset) void frame_scheduler_event_reset(FrameSchedulerFixture* p){p->events.clear();}
API(frame_scheduler_state) u32 frame_scheduler_state(FrameSchedulerFixture* p,u32 id){const auto& e=p->entries[id];return u32(p->scheduler.contains(e.callback))|(u32(e.callback.run!=nullptr)<<1)|(u32(e.callback.initialize!=nullptr)<<2)|(u32(e.callback.enabled)<<3);}

API(enemy_collision_query) void enemy_collision_query(EnemyVariableFixture* p,u32 preview,u32* out){const auto q=enemy_shot_query(p->enemy,preview);out[0]=float_to_bits(q.position.x);out[1]=float_to_bits(q.position.y);out[2]=float_to_bits(q.position.z);out[3]=q.rectangle;out[4]=float_to_bits(q.size.x);out[5]=float_to_bits(q.size.y);out[6]=float_to_bits(q.angle);out[7]=float_to_bits(q.radius);out[8]=q.preview;out[9]=u32(q.target);}
API(enemy_collision_contact) void enemy_collision_contact(EnemyVariableFixture* p,u32 present,u32* out){const auto shape=enemy_contact_shape(p->enemy,present?p->visuals->find(p->enemy.animation_handles[0]):nullptr);out[0]=float_to_bits(shape.position.x);out[1]=float_to_bits(shape.position.y);out[2]=float_to_bits(shape.position.z);out[3]=shape.rectangle;out[4]=float_to_bits(shape.angle);out[5]=float_to_bits(shape.length);out[6]=float_to_bits(shape.width);out[7]=float_to_bits(shape.radius);}

struct EnemyCombatFixture final:EnemyCombatServices,PlayerDamageHost {
    DamageSources damage;PlayerCollision collision;Timer player_timer;EnemyCombat combat;
    i32 bomb=0,bomb_calls=0,hits=0;bool available=true;
    EnemyCombatFixture():combat(damage,collision,*this,player_timer,*this){}
    void hit()override{hits++;}
    bool bomb_damage(const DamageQuery&,i32& out)override{bomb_calls++;out=bomb;return available;}
    bool additional_damage(EnemyState&,i32,i32& out)override{out=0;return false;}
    int enemy_death(EnemyRuntime&,float)override{return -2;}
    bool contact_override(EnemyState&,AnmVm*,i32&,bool& handled)override{handled=false;return true;}
    bool graze(const Vec3&)override{return false;}bool sound(i32,const Vec3&)override{return false;}
};
API(enemy_combat_create) EnemyCombatFixture* enemy_combat_create(){return new EnemyCombatFixture;}
API(enemy_combat_delete) void enemy_combat_delete(EnemyCombatFixture* p){delete p;}
API(enemy_combat_damage) DamageSources* enemy_combat_damage(EnemyCombatFixture* p){return &p->damage;}
API(enemy_combat_timer) Timer* enemy_combat_timer(EnemyCombatFixture* p){return &p->player_timer;}
API(enemy_combat_configure) void enemy_combat_configure(EnemyCombatFixture* p,i32 bomb,u32 available){p->bomb=bomb;p->available=available;p->bomb_calls=0;}
API(enemy_combat_bomb_calls) i32 enemy_combat_bomb_calls(EnemyCombatFixture* p){return p->bomb_calls;}
API(enemy_combat_shot) u32 enemy_combat_shot(EnemyCombatFixture* p,EnemyVariableFixture* e,u32 special,u32* out){EnemyShotDamage hit;hit.position=e->enemy.last_hit_position;if(!p->combat.shot_damage(e->enemy,special,hit))return 0;out[0]=u32(hit.amount);out[1]=hit.direct;out[2]=float_to_bits(hit.position.x);out[3]=float_to_bits(hit.position.y);out[4]=float_to_bits(hit.position.z);return 1;}

struct GameBattleFixture final:BattleWorldServices {
    std::vector<u32> events;bool audio_available=true,dialogue=false;AnmManager& animations;GameBattle game;
    GameBattleFixture(AnmManagerFixture& a,EclProgram& ecl,const ShtResource& sht,i32 character):animations(a.manager),game(a.manager,a.environment,a.environment.game_rng,a.random,*this,ecl,sht,character,{9+character,7,8}){game.enemy_visuals.map_resource(2,1);}
    void record(u32 id,std::initializer_list<u32> values={}){events.push_back(id);events.push_back(values.size());events.insert(events.end(),values.begin(),values.end());}
    bool audio(i32 id,float pan,PlayerShots::SoundAction action,bool queued)override{record(1,{u32(id),float_to_bits(pan),u32(action),u32(queued)});return audio_available;}
    bool life_hud(i32 v,i32 p)override{record(2,{u32(v),u32(p)});return true;}
    bool bomb_hud(i32 v,i32 p)override{record(3,{u32(v),u32(p)});return true;}
    bool game_over()override{record(4);return true;}bool notice(i32 id)override{record(5,{u32(id)});return true;}
    bool popup(const Vec3&,i32 v,u32 color)override{record(6,{u32(v),color});return true;}
    bool cancellation_effect(i32 script,const Vec3&,const Vec3&)override{record(7,{u32(script)});return true;}
    bool graze_spark(const Vec3&)override{record(8);return true;}bool graze_flash()override{record(9);return true;}bool graze_resonance(float v)override{record(10,{float_to_bits(v)});return true;}
    bool shake(const ScreenShakeSpec&)override{record(11);return true;}bool nudge(const ScreenNudgeSpec&)override{record(12);return true;}
    bool dialogue_present()const noexcept override{return dialogue;}
    bool bomb_damage(const DamageQuery&,i32& out)override{out=0;return true;}
    int enemy_callback(EnemyRuntime&,float)override{return 0;}
    bool enemy_additional_damage(EnemyState&,i32,i32& out)override{out=0;return true;}
    bool enemy_contact(EnemyState&,AnmVm*,i32&,bool& handled)override{handled=false;return true;}
    bool enemy_distortion(EnemyState&,float)override{return true;}
    bool enemy_death_callback(EnemyRuntime&)override{record(13);return true;}
    bool spell_background_visible(bool v)override{record(14,{u32(v)});return true;}
    bool spell_prepare_hud(bool v)override{record(15,{u32(v)});return true;}
    bool spell_title(AnmVm&,const std::string&)override{record(16);return true;}
    bool spell_history_begin(i32 id,const std::string&)override{record(17,{u32(id)});return true;}
    bool spell_history_capture(i32 id)override{record(18,{u32(id)});return true;}
    bool spell_result(i32 bonus,bool failed)override{record(19,{u32(bonus),u32(failed)});return true;}
    bool spell_sound(i32 id)override{return audio(id,0,PlayerShots::SoundAction::play,true);}
};
API(game_battle_create) GameBattleFixture* game_battle_create(AnmManagerFixture* a,EclProgram* p,ShtResource* s,i32 character){return new GameBattleFixture(*a,*p,*s,character);}
API(game_battle_delete) void game_battle_delete(GameBattleFixture* p){delete p;}
API(game_battle_initialize) u32 game_battle_initialize(GameBattleFixture* p){return p->game.initialize();}
API(game_battle_error) const char* game_battle_error(GameBattleFixture* p){return p->game.error.c_str();}
API(game_battle_step) u32 game_battle_step(GameBattleFixture* p,u32 held,u32 pressed,u32 game_flags,float rate,u32 frame_flags){BattleFrame f;f.held=held;f.pressed=pressed;f.game_flags=game_flags;f.rate=rate;f.focus_allowed=frame_flags&1;f.hud_collect=frame_flags&2;f.animation_paused=frame_flags&4;f.player_enabled=!(frame_flags&8);f.bomb_enabled=!(frame_flags&16);f.bullets_enabled=!(frame_flags&32);f.items_enabled=!(frame_flags&64);return p->game.step(f);}
API(game_battle_spawn) u32 game_battle_spawn(GameBattleFixture* p,const char* routine,const u8* bytes){auto r=spawn_request(bytes);r.routine=routine;return p->game.spawn(r);}
API(game_battle_enemy_count) u32 game_battle_enemy_count(GameBattleFixture* p){return p->game.enemies->count();}
API(game_battle_enemy) EnemyState* game_battle_enemy(GameBattleFixture* p,u32 id){auto* e=p->game.enemies->find(id);return e?&e->state:nullptr;}
API(game_battle_enemy_life) i32 game_battle_enemy_life(GameBattleFixture* p,u32 id){auto* e=p->game.enemies->find(id);return e?e->state.life:INT32_MIN;}
API(game_battle_bullet_count) u32 game_battle_bullet_count(GameBattleFixture* p){return p->game.bullet_scene->manager.visible_count;}
API(game_battle_laser_count) u32 game_battle_laser_count(GameBattleFixture* p){return p->game.laser_scene->manager.count();}
API(game_battle_item_count) i32 game_battle_item_count(GameBattleFixture* p){return p->game.items->active_count;}
API(game_battle_field) void* game_battle_field(GameBattleFixture* p,u32 field){auto& g=p->game;auto& v=*g.player;switch(field){case 0:return &v.life.state;case 1:return &v.motion.position;case 2:return &v.motion.focus;case 3:return &g.session;case 4:return &g.spell;case 5:return &g.score;case 6:return &v.frame.input_age;case 7:return &v.damage;case 8:return &g.enemy_world;case 9:return &g.bomb->age;case 10:return &v.life.age;case 11:return &v.damage.sources;case 12:return &v.shots.shots;case 13:return &v.damage.cursor;case 14:return &v.life.invulnerability;default:return nullptr;}}
API(game_battle_damage) DamageSources* game_battle_damage(GameBattleFixture* p){return &p->game.player->damage;}
API(game_battle_root) AnmVm* game_battle_root(GameBattleFixture* p){return &p->game.player->visuals.root;}
API(game_battle_event_count) u32 game_battle_event_count(GameBattleFixture* p){return p->events.size();}
API(game_battle_events) u32* game_battle_events(GameBattleFixture* p){return p->events.data();}
API(game_battle_event_reset) void game_battle_event_reset(GameBattleFixture* p){p->events.clear();}
API(game_battle_audio_available) void game_battle_audio_available(GameBattleFixture* p,u32 value){p->audio_available=value;}
API(game_battle_dialogue) void game_battle_dialogue(GameBattleFixture* p,u32 value){p->dialogue=value!=0;}
API(game_battle_bomb_allowed) u32 game_battle_bomb_allowed(GameBattleFixture* p){return p->game.bomb->allowed();}
API(game_battle_collision_contact) i32 game_battle_collision_contact(GameBattleFixture* p,u32 kind,float x,float y,float a,float b,u32 graze){auto& g=p->game;const auto& c=g.player_collision();return i32(kind==0?c.rectangle({x,y},{a,b},graze,g.player.get()):kind==1?c.circle({x,y},a,graze,g.player.get()):c.laser({x,y},0,a,b,graze,g.player.get()));}
API(game_battle_collision_state) void game_battle_collision_state(GameBattleFixture* p,u32* out){const auto& c=p->game.player_collision();out[0]=u32(c.state);out[1]=u32(c.invulnerability);out[2]=c.bomb_active;out[3]=float_to_bits(c.radius);out[4]=float_to_bits(c.laser_half_size.x);out[5]=float_to_bits(c.laser_half_size.y);}

API(game_battle_bind_progress) void game_battle_bind_progress(GameBattleFixture* p,SessionState* state,i32* request){p->game.bind_progress(*state,request);}
API(game_battle_clock) const float* game_battle_clock(GameBattleFixture* p){return &p->game.active_frame().rate;}
API(game_battle_variable) i32 game_battle_variable(GameBattleFixture* p,u32 identifier,i32 variable){auto* e=p->game.enemies->find(identifier);if(!e)return INT32_MIN;EnemyVariables variables(e->state,p->game.enemy_world,*p->game.enemy_world.random);i32 value=0;return variables.integer(variable,value)?value:INT32_MIN;}

API(battle_frame_policy) u32 battle_frame_policy(u32 flags,u32 kind){return kind==0?u32(battle_enemy_frame(flags)):kind==1?u32(battle_bullet_frame(flags)):u32(battle_animation_frame(flags));}

API(stage_resource_create) StageResource* stage_resource_create(){return new StageResource;}
API(stage_resource_delete) void stage_resource_delete(StageResource* p){delete p;}
API(stage_resource_open) u32 stage_resource_open(StageResource* p,const u8* data,u32 size){return p->open(data,size);}
API(stage_resource_error) const char* stage_resource_error(StageResource* p){return p->error.c_str();}
API(stage_resource_name) const char* stage_resource_name(StageResource* p){return p->animation_name.c_str();}
API(stage_resource_count) u32 stage_resource_count(StageResource* p,u32 field){switch(field){case 0:return p->objects.size();case 1:return p->animation_count;case 2:return p->instances.size();case 3:return p->instructions.size();default:return 0;}}
API(stage_resource_object) void stage_resource_object(StageResource* p,u32 index,u8* out){const auto& o=p->objects[index];std::memcpy(out,&o.id,2);out[2]=o.layer;out[3]=o.flags;std::memcpy(out+4,&o.position,12);std::memcpy(out+16,&o.size,12);}
API(stage_resource_primitive_count) u32 stage_resource_primitive_count(StageResource* p,u32 index){return p->objects[index].primitives.size();}
API(stage_resource_primitive) void stage_resource_primitive(StageResource* p,u32 index,u32 q,u8* out){const auto& v=p->objects[index].primitives[q];i16 length=28,animation=i16(v.animation);std::memcpy(out,&v.type,2);std::memcpy(out+2,&length,2);std::memcpy(out+4,&v.script,2);std::memcpy(out+6,&animation,2);std::memcpy(out+8,&v.position,12);std::memcpy(out+20,&v.size,8);}
API(stage_resource_instance) void stage_resource_instance(StageResource* p,u32 index,u8* out){const auto& v=p->instances[index];std::memcpy(out,&v.object,2);std::memcpy(out+2,&v.flags,2);std::memcpy(out+4,&v.position,12);}
API(stage_resource_instruction) u32 stage_resource_instruction(StageResource* p,u32 index,u8* out){const auto& v=p->instructions[index];std::memcpy(out,&v.time,4);std::memcpy(out+4,&v.opcode,2);std::memcpy(out+6,&v.length,2);if(v.arguments.size())std::memcpy(out+8,v.arguments.data(),v.arguments.size()*4);return v.offset;}

API(stage_fog_create) StageFogInterpolation* stage_fog_create(){return new StageFogInterpolation;}
API(stage_fog_delete) void stage_fog_delete(StageFogInterpolation* p){delete p;}
API(stage_fog_step) void stage_fog_step(StageFogInterpolation* p,float rate,StageFog* out){*out=p->step(rate);}
struct StageScriptFixture final:StageScriptWorld {
    StageScript script;std::vector<u32> events;std::array<u32,8> animation_flags{};u32 clear=0;bool available=true;
    explicit StageScriptFixture(StageResource& file):script(file,*this){}
    bool clear_color(u32 color)override{clear=color;events.insert(events.end(),{0,color});return available;}
    bool animation(u32 index,i32 value,i32 layer)override{if(value<0)animation_flags[index]&=~1u;else{animation_flags[index]=1;events.insert(events.end(),{1,index,u32(value)});}return available;}
    bool deformation(i32 mode,i32 id)override{events.insert(events.end(),{2,u32(id)});return available;}
    bool object_interrupt(i32 id)override{events.insert(events.end(),{3,u32(id)});return available;}
};
API(stage_script_create) StageScriptFixture* stage_script_create(StageResource* p){return new StageScriptFixture(*p);}
API(stage_script_delete) void stage_script_delete(StageScriptFixture* p){delete p;}
API(stage_script_step) u32 stage_script_step(StageScriptFixture* p,float rate){return p->script.step(rate);}
API(stage_script_interrupt) u32 stage_script_interrupt(StageScriptFixture* p,i32 id){return p->script.interrupt(id);}
API(stage_script_error) const char* stage_script_error(StageScriptFixture* p){return p->script.error.c_str();}
API(stage_script_available) void stage_script_available(StageScriptFixture* p,u32 value){p->available=value;}
API(stage_script_event_reset) void stage_script_event_reset(StageScriptFixture* p){p->events.clear();}
API(stage_script_event_count) u32 stage_script_event_count(StageScriptFixture* p){return p->events.size();}
API(stage_script_events) u32* stage_script_events(StageScriptFixture* p){return p->events.data();}
API(stage_script_clear) u32 stage_script_clear(StageScriptFixture* p){return p->clear;}
API(stage_script_snapshot) void stage_script_snapshot(StageScriptFixture* p,u8* out){
 const auto& s=p->script.state;std::memset(out,0,0x3390);std::memcpy(out,&s.timer,20);std::memcpy(out+0x14,&s.instruction_offset,4);out[0x18]=s.camera_effect;std::memcpy(out+0x1c,&s.effect_timer,20);
 std::memcpy(out+0x30,&s.direction,88);std::memcpy(out+0x88,&s.position,88);std::memcpy(out+0xe0,&s.up,88);std::memcpy(out+0x138,&s.fog,168);
 std::memcpy(out+0x1e0,&s.camera.position,12);std::memcpy(out+0x1ec,&s.camera.direction,12);std::memcpy(out+0x1f8,&s.camera.up,12);std::memcpy(out+0x21c,&s.camera.eye_offset,12);std::memcpy(out+0x228,&s.camera.target_offset,12);std::memcpy(out+0x234,&s.camera.fov,4);std::memcpy(out+0x2e4,&s.camera.animation_delta,12);std::memcpy(out+0x2f0,&s.camera.fog,28);
 for(u32 i=0;i<8;++i)std::memcpy(out+0x310+i*0x608+0x18,&p->animation_flags[i],4);std::memcpy(out+0x3350,s.animation_layers.data(),32);std::memcpy(out+0x3370,&s.culling_distance_squared,4);std::memcpy(out+0x3378,&s.deformation_target,4);std::memcpy(out+0x337c,&s.deformation_radius,4);std::memcpy(out+0x3380,&s.deformation_color,4);std::memcpy(out+0x3384,&s.phase_x,4);std::memcpy(out+0x3388,&s.phase_y,4);std::memcpy(out+0x338c,&s.deformation_mode,4);
}

#include "../../cpp/game/BulletCheckpoint.hpp"
API(bullet_checkpoint_create) BulletCheckpoint* bullet_checkpoint_create(BulletSceneFixture* p,AnmManagerFixture* a,AnmCheckpoint* pool,i32 bank){return new BulletCheckpoint(p->scene,a->manager,*pool,bank);}
API(bullet_checkpoint_delete) void bullet_checkpoint_delete(BulletCheckpoint* p){delete p;}
API(bullet_checkpoint_capture) i32 bullet_checkpoint_capture(BulletCheckpoint* p){return p->capture();}
API(bullet_checkpoint_restore) i32 bullet_checkpoint_restore(BulletCheckpoint* p){return p->restore();}
API(bullet_checkpoint_error) const char* bullet_checkpoint_error(BulletCheckpoint* p){return p->error.c_str();}
API(bullet_scene_saved_effects) u32* bullet_scene_saved_effects(BulletSceneFixture* p){return p->scene.cancellation_animations.data();}
API(bullet_scene_reward_counter) i32* bullet_scene_reward_counter(BulletSceneFixture* p){return &p->scene.manager.cancellation_rewards.count;}

API(ecl_snapshot_create) EclThreadsSnapshot* ecl_snapshot_create(){return new EclThreadsSnapshot;}
API(ecl_snapshot_delete) void ecl_snapshot_delete(EclThreadsSnapshot* p){delete p;}
API(ecl_snapshot_capture) void ecl_snapshot_capture(EclThreadsSnapshot* out,EclThreads* p){*out=p->snapshot();}
API(ecl_snapshot_restore) i32 ecl_snapshot_restore(EclThreads* p,EclThreadsSnapshot* saved){return p->restore(*saved);}
#include "../../cpp/game/EnemyCheckpoint.hpp"
API(enemy_checkpoint_create) EnemyCheckpoint* enemy_checkpoint_create(EnemyManagerFixture* p,AnmManagerFixture* a,AnmCheckpoint* pool){return new EnemyCheckpoint(p->manager,a->manager,*pool,{{-1,-1,-1,-1,-1,-1}});}
API(enemy_checkpoint_delete) void enemy_checkpoint_delete(EnemyCheckpoint* p){delete p;}
API(enemy_checkpoint_capture) i32 enemy_checkpoint_capture(EnemyCheckpoint* p){return p->capture();}
API(enemy_checkpoint_restore) i32 enemy_checkpoint_restore(EnemyCheckpoint* p){return p->restore();}
API(enemy_checkpoint_count) u32 enemy_checkpoint_count(EnemyCheckpoint* p){return p->count();}
API(enemy_checkpoint_error) const char* enemy_checkpoint_error(EnemyCheckpoint* p){return p->error.c_str();}

#include "../../cpp/game/ItemCheckpoint.hpp"
API(item_checkpoint_create) ItemCheckpoint* item_checkpoint_create(ItemManagerFixture* p){return new ItemCheckpoint(p->manager);}
API(item_checkpoint_delete) void item_checkpoint_delete(ItemCheckpoint* p){delete p;}
API(item_checkpoint_capture) i32 item_checkpoint_capture(ItemCheckpoint* p){return p->capture();}
API(item_checkpoint_restore) i32 item_checkpoint_restore(ItemCheckpoint* p){return p->restore();}
API(item_checkpoint_state) const ItemState* item_checkpoint_state(ItemCheckpoint* p,u32 i){return p->state(i);}
API(item_checkpoint_vm) const AnmVm* item_checkpoint_vm(ItemCheckpoint* p,u32 i,u32 arrow){return p->animation(i,arrow!=0);}
API(item_checkpoint_error) const char* item_checkpoint_error(ItemCheckpoint* p){return p->error.c_str();}
API(item_manager_bind_alternating) void item_manager_bind_alternating(ItemManagerFixture* p,i32* counter){p->manager.alternating_counter=counter;}

API(scene_camera_normalize) void scene_camera_normalize(const Vec3* p,Vec3* out){*out=normalize_scene_vector(*p);}
API(scene_camera_look_at) void scene_camera_look_at(const Vec3* eye,const Vec3* target,const Vec3* up,Matrix4* out){*out=scene_look_at(*eye,*target,*up);}
API(scene_camera_perspective) void scene_camera_perspective(float fov,float aspect,float near_plane,float far_plane,Matrix4* out){*out=scene_perspective(fov,aspect,near_plane,far_plane);}
API(stage_camera_snapshot) void stage_camera_snapshot(const u8* data,const GraphicsViewport* vp,u8* out){StageCamera s;std::memcpy(&s.position,data,12);std::memcpy(&s.direction,data+0x24,12);std::memcpy(&s.up,data+0x18,12);std::memcpy(&s.eye_offset,data+0x3c,12);std::memcpy(&s.fov,data+0x54,4);const auto c=stage_camera(s,*vp);std::memcpy(out,&c.view,64);std::memcpy(out+64,&c.projection,64);std::memcpy(out+128,&c.reference,12);}

API(stage_visibility) u32 stage_visibility(const u8* object,const Vec3* instance,AnmCamera* camera,const GraphicsViewport* viewport,float x,float y,float radius,Vec3* out){StageObject o;std::memcpy(&o.position,object+4,12);std::memcpy(&o.size,object+16,12);std::array<Vec3,16> p{};const bool visible=stage_visible(o,*instance,*camera,*viewport,{x,y},radius,&p);std::memcpy(out,p.data(),192);return visible;}

struct StageSceneFixture final:StageSceneServices {
    StageScene scene;bool available=false;StageSceneFixture(StageResource& f,AnmManagerFixture& a,i32 bank):scene(f,a.manager,a.environment,*this,bank){}
    bool begin_deformation(i32,i32)override{return available;}bool update_deformation(StageScriptState&,float)override{return available;}
};
API(stage_scene_create) StageSceneFixture* stage_scene_create(StageResource* f,AnmManagerFixture* a,i32 bank){return new StageSceneFixture(*f,*a,bank);}
API(stage_scene_delete) void stage_scene_delete(StageSceneFixture* p){delete p;}
API(stage_scene_initialize) u32 stage_scene_initialize(StageSceneFixture* p,float fov){StageCamera camera;camera.fov=fov;camera.direction={0,0,1};return p->scene.initialize(camera);}
API(stage_scene_error) const char* stage_scene_error(StageSceneFixture* p){return p->scene.error.c_str();}
API(stage_scene_update) u32 stage_scene_update(StageSceneFixture* p,float rate,u32 flags,i32 age){return p->scene.update({rate,flags,age});}
API(stage_scene_interrupt) u32 stage_scene_interrupt(StageSceneFixture* p,i32 label,u32 objects){return objects?p->scene.interrupt_objects(label):p->scene.interrupt(label);}
API(stage_scene_vm) AnmVm* stage_scene_vm(StageSceneFixture* p,u32 index,u32 slots){return slots?p->scene.slot(index):p->scene.primitive(index);}
API(stage_scene_object_flags) u32 stage_scene_object_flags(StageSceneFixture* p,u32 index){return p->scene.object_state(index);}
API(stage_scene_instance_flags) i32 stage_scene_instance_flags(StageSceneFixture* p,u32 index){return p->scene.instance_state(index);}
API(stage_scene_draw) u32 stage_scene_draw(StageSceneFixture* p,AnmRendererFixture* r,i32 layer,const GraphicsViewport* viewport,float x,float y){return p->scene.draw(r->renderer,r->graphics,layer,*viewport,{x,y});}
API(stage_scene_direction) const Vec3* stage_scene_direction(StageSceneFixture* p){return &p->scene.camera_direction();}
API(stage_scene_color) u32 stage_scene_color(StageSceneFixture* p){return p->scene.color;}
API(stage_scene_frames) u32 stage_scene_frames(StageSceneFixture* p){return p->scene.frames;}

API(anm_renderer_submission_matrix) const Matrix4* anm_renderer_submission_matrix(AnmRendererFixture* p,u32 draw,u32 kind){return &p->graphics.submissions[draw].matrices[kind];}
API(anm_renderer_submission_states) const u32* anm_renderer_submission_states(AnmRendererFixture* p,u32 draw){return p->graphics.submissions[draw].states.data();}
API(stage_scene_draw_stats) void stage_scene_draw_stats(StageSceneFixture* p,u32* out,u32 reset){out[0]=p->scene.drawn_instances;out[1]=p->scene.culled_instances;out[2]=p->scene.drawn_primitives;if(reset)p->scene.drawn_instances=p->scene.culled_instances=p->scene.drawn_primitives=0;}

API(enemy_manager_clear_field) u32 enemy_manager_clear_field(EnemyManagerFixture* p,u32 suppress){p->scene_deaths.clear();return p->manager.clear_field(suppress!=0);}
API(enemy_manager_scene_deaths) const u32* enemy_manager_scene_deaths(EnemyManagerFixture* p){return p->scene_deaths.data();}
API(enemy_manager_scene_death_count) u32 enemy_manager_scene_death_count(EnemyManagerFixture* p){return p->scene_deaths.size();}
API(enemy_manager_clear_seed) void enemy_manager_clear_seed(EnemyManagerFixture* p,u32 index,const u8* bytes,const char* routine){auto& e=p->manager.at(index)->state;std::memcpy(&e.flags,bytes,4);std::memcpy(&e.primary_drop,bytes+4,4);std::memcpy(e.item_drops.data(),bytes+8,64);std::memcpy(&e.drop_radius,bytes+72,8);std::memcpy(&e.last_hit_position,bytes+80,12);std::memcpy(&e.chapter_contribution,bytes+92,4);e.death_script=routine;}
API(enemy_manager_clear_bytes) const u8* enemy_manager_clear_bytes(EnemyManagerFixture* p,u32 index){static std::array<u8,100> bytes;const auto& e=p->manager.at(index)->state;std::memcpy(bytes.data(),&e.flags,4);std::memcpy(bytes.data()+4,&e.primary_drop,4);std::memcpy(bytes.data()+8,e.item_drops.data(),64);std::memcpy(bytes.data()+72,&e.drop_radius,8);std::memcpy(bytes.data()+80,&e.last_hit_position,12);std::memcpy(bytes.data()+92,&e.chapter_contribution,4);const u32 present=!e.death_script.empty();std::memcpy(bytes.data()+96,&present,4);return bytes.data();}

API(enemy_background_stage) void enemy_background_stage(EnemyVariableFixture* p,StageScriptFixture* background){p->events.background=background?&background->script:nullptr;}
API(stage_script_camera_fog) StageFog* stage_script_camera_fog(StageScriptFixture* p){return &p->script.state.camera.fog;}
API(stage_script_fog_curve) StageFogInterpolation* stage_script_fog_curve(StageScriptFixture* p){return &p->script.state.fog;}

API(enemy_projectile_scenes) void enemy_projectile_scenes(EnemyVariableFixture* p,BulletSceneFixture* bullets,LaserManagerFixture* lasers){p->events.bullet_scene=bullets?&bullets->scene:nullptr;p->events.laser_manager=lasers?&lasers->manager:nullptr;}
API(bullet_scene_cancel_all) u32 bullet_scene_cancel_all(BulletSceneFixture* p,i32 reward){return p->scene.cancel_all(reward);}

API(laser_manager_reset_events) void laser_manager_reset_events(LaserManagerFixture* p){p->events.clear();}

struct SpellCardFixture:SpellCardHost {
    PlayerSpellStatus state;i32 score=0;SpellCard spell{state,score,*this};u32 background_flags=0;Vec3 boss{},ring_position{};i32 ring_available=1,personal_captures=0,aggregate_captures=0,personal_attempts=0,aggregate_attempts=0;std::string history_name;std::vector<u32> events;u32 next_handle=1;
    bool background_visible(bool visible)override{if(visible)background_flags|=1;else background_flags&=~1u;return true;}
    bool prepare_hud(bool start)override{events.insert(events.end(),{2,u32(start)});return true;}
    u32 create_visual(i32 resource,i32 script)override{const u32 id=next_handle++;events.insert(events.end(),{8,u32(resource),u32(script),id});return id;}
    bool interrupt(u32 handle,i32 label)override{events.insert(events.end(),{1,handle,u32(label)});return true;}
    bool retire(u32& handle)override{events.insert(events.end(),{3,handle});handle=0;return true;}
    bool text(u32 handle,const std::string&)override{events.insert(events.end(),{9,handle});return true;}
    bool child_integer(u32 handle,i32 script,i32 slot,i32 value)override{events.insert(events.end(),{10,handle,u32(script),u32(slot),u32(value)});return true;}
    bool position(u32,const Vec3& p)override{if(ring_available)ring_position=p;return true;}
    bool boss_position(Vec3& p)override{p=boss;return true;}
    bool history_begin(i32,const std::string& text)override{history_name=text;if(personal_attempts<99999)personal_attempts++;if(aggregate_attempts<99999)aggregate_attempts++;return true;}
    bool history_capture(i32)override{if(personal_captures<99999)personal_captures++;if(aggregate_captures<99999)aggregate_captures++;return true;}
    bool result(i32 bonus,bool failed)override{events.insert(events.end(),{4,u32(bonus),u32(failed)});return true;}
    bool sound(i32 id)override{events.insert(events.end(),{5,u32(id)});return true;}
};
struct SpellClockFixture {SpellPresentationClock clock;u32 flags=0;i32 active_frames=0;};
API(spell_clock_create) SpellClockFixture* spell_clock_create(){return new SpellClockFixture;}
API(spell_clock_delete) void spell_clock_delete(SpellClockFixture* p){delete p;}
API(spell_clock_field) void* spell_clock_field(SpellClockFixture* p,u32 field){switch(field){case 0:return &p->flags;case 1:return &p->active_frames;case 2:return &p->clock.sequence;case 3:return &p->clock.completed_frames;case 4:return &p->clock.encoded;default:return &p->clock.origin;}}
API(spell_clock_sample) u32 spell_clock_sample(SpellClockFixture* p,double now){return u32(p->clock.sample(p->flags,p->active_frames,now));}
API(spell_clock_needs_sample) u32 spell_clock_needs_sample(SpellClockFixture* p){return p->clock.needs_sample(p->flags);}
API(spell_clock_valid) u32 spell_clock_valid(i32 value){return SpellPresentationClock::valid(value);}
API(spell_clock_encode) i32 spell_clock_encode(i32 seconds,i32 hundredths){return SpellPresentationClock::encode(seconds,hundredths);}
API(spell_card_create) SpellCardFixture* spell_card_create(){return new SpellCardFixture;}
API(spell_card_delete) void spell_card_delete(SpellCardFixture* p){delete p;}
API(spell_card_field) void* spell_card_field(SpellCardFixture* p,u32 kind){switch(kind){case 0:return &p->state.flags;case 1:return &p->state.bonus;case 2:return &p->spell.maximum_bonus;case 3:return &p->spell.duration;case 4:return &p->spell.active_frames;case 5:return &p->spell.age;case 6:return &p->spell.anchor;case 7:return p->spell.handles.data();case 8:return &p->score;case 9:return &p->background_flags;case 10:return &p->boss;case 11:return &p->ring_position;case 12:return &p->spell.identifier;case 13:return &p->personal_captures;case 14:return &p->aggregate_captures;case 15:return &p->ring_available;default:return &p->state.frame;}}
API(spell_card_replay) void spell_card_replay(SpellCardFixture* p,u32 replay){p->spell.replay=replay!=0;}
API(spell_card_update) u32 spell_card_update(SpellCardFixture* p,float rate,float y,i32 bomb){p->events.clear();return p->spell.update({rate,y,bomb});}
API(spell_card_end) u32 spell_card_end(SpellCardFixture* p,u32 abort){p->events.clear();return abort?p->spell.abort():p->spell.finish();}
API(spell_card_events) u32* spell_card_events(SpellCardFixture* p){return p->events.data();}
API(spell_card_event_count) u32 spell_card_event_count(SpellCardFixture* p){return p->events.size();}
API(spell_card_error) const char* spell_card_error(SpellCardFixture* p){return p->spell.error.c_str();}

API(spell_card_begin) u32 spell_card_begin(SpellCardFixture* p,i32 id,i32 duration,i32 requested_bonus,const char* name,u32 survival,i32 stage,i32 difficulty,i32 character,i32 bomb,u32 replay,u32 secondary,u32 independent,u32 overlay){SpellStartContext c;c.stage=stage;c.difficulty=difficulty;c.character=character;c.bomb_state=bomb;c.replay=replay;c.visuals.ascii=0;c.visuals.name=1;c.visuals.effect=2;c.visuals.secondary=secondary;c.visuals.backgrounds={SpellBackground{3,101,independent!=0,overlay?4:-1,201},SpellBackground{4,102,independent!=0,overlay?5:-1,202},SpellBackground{5,103,independent!=0,overlay?3:-1,203}};p->events.clear();return p->spell.begin({id,duration,requested_bonus,name,survival!=0},c);}
API(spell_card_attempts) i32* spell_card_attempts(SpellCardFixture* p,u32 aggregate){return aggregate?&p->aggregate_attempts:&p->personal_attempts;}
API(spell_card_name) const char* spell_card_name(SpellCardFixture* p,u32 history){return history?p->history_name.c_str():p->spell.name.c_str();}

API(enemy_spell_request) void enemy_spell_request(EnemyVariableFixture* p,i32* out){out[0]=p->events.spell_request.id;out[1]=p->events.spell_request.duration;out[2]=p->events.spell_request.requested_bonus;out[3]=p->events.spell_request.survival;out[4]=p->events.spell_starts;out[5]=p->events.spell_ends;}
API(enemy_spell_title) const char* enemy_spell_title(EnemyVariableFixture* p){return p->events.spell_request.name.c_str();}

API(game_battle_spell_setup) void game_battle_spell_setup(GameBattleFixture* p){p->game.stage=1;p->game.spell_visuals.ascii=2;p->game.spell_visuals.name=4;p->game.spell_visuals.effect=8;p->game.spell_visuals.backgrounds={SpellBackground{5,11,false,5,18},SpellBackground{5,11,false,5,18},SpellBackground{5,11,false,-1,-1}};}
API(game_battle_spell_child) AnmVm* game_battle_spell_child(GameBattleFixture* p,i32 script){auto* vm=p->game.enemy_visuals.find(p->game.spell_card.handles[4]);return vm?p->animations.registry.find_child_script(*vm,script,0):nullptr;}

API(game_battle_resource_map) u32 game_battle_resource_map(GameBattleFixture* p,i32 slot,i32 resource){return p->game.enemy_visuals.map_resource(slot,resource);}

API(enemy_scene_capture) void enemy_scene_capture(EnemyVariableFixture* p,u32 present,u32 complete){p->events.capture_scene=true;p->events.message_present=present;p->events.message_done=complete;p->events.scene_events.clear();}
API(enemy_scene_events) u32* enemy_scene_events(EnemyVariableFixture* p){return p->events.scene_events.data();}
API(enemy_scene_event_count) u32 enemy_scene_event_count(EnemyVariableFixture* p){return p->events.scene_events.size();}

API(stage_definition_text) const char* stage_definition_text(u32 stage,u32 kind,u32 index){const auto* d=stage_definition(stage);if(!d)return nullptr;switch(kind){case 0:return d->background;case 1:return d->script;case 2:return index<2?d->music[index]:nullptr;case 3:return index<4?d->dialogue[index]:nullptr;default:return d->logo;}}
API(stage_definition_values) u32 stage_definition_values(u32 stage,i32 age,const i32* banks,i32* out){const auto* d=stage_definition(stage);if(!d)return 0;std::array<i32,6> ids;std::memcpy(ids.data(),banks,24);const auto config=d->spell_visuals(age,51,52,53,ids);out[0]=d->id;out[1]=d->music_unlock[0];out[2]=d->music_unlock[1];out[3]=config.secondary;for(u32 i=0;i<3;i++){const auto& s=config.backgrounds[i];out[4+i*5]=s.resource;out[5+i*5]=s.script;out[6+i*5]=s.independent;out[7+i*5]=s.overlay_resource;out[8+i*5]=s.overlay_script;}return 1;}

struct DialogueCapture:DialogueHost {
    std::vector<u32> events;u32 next=0;
    bool create(i32 r,i32 s,u32& h)override{h=++next;events.insert(events.end(),{1,u32(r),u32(s),h});return true;}
    bool interrupt(u32 h,i32 l,bool now)override{events.insert(events.end(),{2,h,u32(l),u32(now)});return true;}
    bool retire(u32& h)override{if(h)events.insert(events.end(),{3,h});h=0;return true;}
    bool text_initialize(u32)override{return true;}bool text(const DialogueText& t)override{u32 hash=2166136261u;for(unsigned char b:t.bytes){hash^=b;hash*=16777619u;}events.insert(events.end(),{10,t.handle,u32(t.font),u32(t.style),t.color,t.secondary_color,u32(t.offset),hash});return true;}
    bool position(u32,const Vec3&)override{return true;}bool depth(u32,float)override{return true;}
    bool balloon_width(u32,i32,float)override{return true;}bool follow_balloon(DialogueState&)override{return true;}
    bool music(bool boss)override{events.insert(events.end(),{4,u32(boss)});return true;}
    bool fade_music(float time)override{u32 bits;std::memcpy(&bits,&time,4);events.insert(events.end(),{5,bits});return true;}
    bool stage_complete()override{events.push_back(6);return true;}bool name_banner()override{events.push_back(7);return true;}
    bool sound(i32 id)override{events.insert(events.end(),{8,u32(id)});return true;}
    bool clear_field()override{events.push_back(9);return true;}
};
struct DialogueFixture {MessageProgram program;DialogueCapture capture;u32 script_index=0;std::unique_ptr<Dialogue> dialogue;};
API(dialogue_create) DialogueFixture* dialogue_create(const u8* data,u32 size,u32 script,i32 character){auto* p=new DialogueFixture;if(!p->program.open(data,size)||script>=p->program.scripts.size()){delete p;return nullptr;}DialogueResources resources;resources.text=11;resources.player=12;resources.logo=13;resources.balloons=14;resources.character=character;p->script_index=script;for(u32 i=0;i<4;i++){resources.enemies[i]={i32(20+i),i32(100+i)};resources.names[i]={i32(30+i),i32(200+i)};}p->dialogue=std::make_unique<Dialogue>(p->program.scripts[script],p->capture,resources);return p;}
API(dialogue_delete) void dialogue_delete(DialogueFixture* p){delete p;}
API(dialogue_initialize) u32 dialogue_initialize(DialogueFixture* p){p->capture.events.clear();return p->dialogue->initialize();}
API(dialogue_update) i32 dialogue_update(DialogueFixture* p,float rate,u32 pressed,u32 held,u32 shoot,u32 skip,i32 stage){p->capture.events.clear();return p->dialogue->update({rate,pressed,held,shoot,skip,stage});}
API(dialogue_field) void* dialogue_field(DialogueFixture* p,u32 kind){auto& s=p->dialogue->state;switch(kind){case 0:return &s.age;case 1:return &s.clock;case 2:return &s.wait;case 3:return s.handles.data();case 4:return s.anchors.data();case 5:return &s.complete;case 6:return &s.flags;case 7:return &s.line;case 8:return &s.box_visible;case 9:return &s.speaker;case 10:return s.colors.data();case 11:return &s.position;case 12:return &s.width;case 13:return &s.portrait_state;default:return &s.balloon_style;}}
API(dialogue_offset) u32 dialogue_offset(DialogueFixture* p){const auto& commands=p->program.scripts[p->script_index].instructions;const auto cursor=p->dialogue->state.cursor;return cursor<commands.size()?commands[cursor].offset:0;}
API(dialogue_events) u32* dialogue_events(DialogueFixture* p){return p->capture.events.data();}
API(dialogue_event_count) u32 dialogue_event_count(DialogueFixture* p){return p->capture.events.size();}
API(dialogue_error) const char* dialogue_error(DialogueFixture* p){return p->dialogue->error.c_str();}

API(stage_definition_portraits) u32 stage_definition_portraits(u32 stage,const i32* banks,i32* out){const auto* d=stage_definition(stage);if(!d)return 0;std::array<i32,6> ids;std::memcpy(ids.data(),banks,24);const auto r=d->dialogue_resources(0,1,2,3,4,ids);for(u32 i=0;i<3;i++){out[4*i]=r.enemies[i].resource;out[4*i+1]=r.enemies[i].script;out[4*i+2]=r.names[i].resource;out[4*i+3]=r.names[i].script;}return 1;}

API(enemy_health_field) void* enemy_health_field(EnemyVariableFixture* p,u32 kind){switch(kind){case 0:return p->events.hud.health.data();case 1:return &p->events.hud.displayed_segments;case 2:return &p->enemy.initial_life;case 3:return &p->enemy.boss_slot;default:return p->enemy.script_control.data();}}

struct StageAssetSource:AssetSource {struct File {const u8* bytes;u32 size;};std::unordered_map<std::string,File> files;std::vector<std::string> reads;bool read(const std::string& name,std::vector<u8>& data)override{reads.push_back(name);auto it=files.find(name);if(it==files.end())return false;data.assign(it->second.bytes,it->second.bytes+it->second.size);return true;}};
struct StageAssetFixture {StageAssetSource source;Rng random;AnmEnvironment environment;AnmManager animations{random,environment};StageAssets assets{source,animations};};
API(stage_assets_create) StageAssetFixture* stage_assets_create(){return new StageAssetFixture;}
API(stage_assets_viewport) void stage_assets_viewport(StageAssetFixture* p,float x,float y,i32 cx,i32 cy){p->environment.playfield_origin={x,y};p->environment.screen_offsets={cx,cy,cx,cy};}
API(stage_assets_delete) void stage_assets_delete(StageAssetFixture* p){delete p;}
API(stage_assets_file) void stage_assets_file(StageAssetFixture* p,const char* name,const u8* bytes,u32 size){p->source.files[std::string(name)]={bytes,size};}
API(stage_assets_load) u32 stage_assets_load(StageAssetFixture* p,u32 stage,i32 character){return p->assets.load(stage,character);}
API(stage_assets_error) const char* stage_assets_error(StageAssetFixture* p){return p->assets.error.c_str();}
API(stage_assets_program) EclProgram* stage_assets_program(StageAssetFixture* p){return &p->assets.scripts;}
API(stage_assets_values) void stage_assets_values(StageAssetFixture* p,i32* out){const auto& a=p->assets;const i32 ids[]={a.ascii,a.text,a.front,a.bullet,a.effect,a.player,a.logo,a.scenery};std::memcpy(out,ids,32);std::memcpy(out+8,a.enemy_banks.data(),24);out[14]=a.messages.scripts.size();out[15]=a.background.objects.size();out[16]=a.background.instances.size();out[17]=a.shots.groups.size();out[18]=a.scripts.files.size();out[19]=a.scripts.definitions.size();}
API(stage_assets_read_count) u32 stage_assets_read_count(StageAssetFixture* p){return p->source.reads.size();}
API(stage_assets_read_name) const char* stage_assets_read_name(StageAssetFixture* p,u32 index){return index<p->source.reads.size()?p->source.reads[index].c_str():nullptr;}
API(stage_assets_animation_count) u32 stage_assets_animation_count(StageAssetFixture* p){return p->assets.animation_names.size();}
API(stage_assets_animation_name) const char* stage_assets_animation_name(StageAssetFixture* p,u32 index){return index<p->assets.animation_names.size()?p->assets.animation_names[index].c_str():nullptr;}

API(enemy_effect_request) void* enemy_effect_request(EnemyVariableFixture* p){return &p->events.effect_request;}

API(enemy_callback_field) void* enemy_callback_field(EnemyVariableFixture* p,u32 kind){return kind==0?static_cast<void*>(&p->enemy.update_rule):kind==1?static_cast<void*>(&p->enemy.damage_rule):static_cast<void*>(&p->enemy.callback_state);}
API(bullet_manager_inherit_motion) void bullet_manager_inherit_motion(BulletManagerFixture* p){inherit_nearby_bullet_motion(p->manager);}
API(bullet_manager_scale_near_player) void bullet_manager_scale_near_player(BulletManagerFixture* p,PlayerCollisionFixture* player,float factor,u32 dialogue){scale_bullets_near_player(p->manager,player->state,factor,dialogue!=0);}
API(enemy_callback_damage) u32 enemy_callback_damage(EnemyVariableFixture* p,i32 incoming,i32* result){return enemy_additional_damage(p->enemy,incoming,*result);}
API(laser_scene_offset_origins) void laser_scene_offset_origins(LaserSceneFixture* p,float without_identifier,float with_identifier){p->scene.offset_origins(without_identifier,with_identifier);}
API(laser_scene_origin) Vec3* laser_scene_origin(LaserSceneFixture* p,u32 index){return p->scene.origin_at(index);}

API(bullet_scene_cancel_unmarked) u32 bullet_scene_cancel_unmarked(BulletSceneFixture* p,const Vec3* position,float radius){return cancel_unmarked_bullets(p->scene,*position,radius);}
API(laser_scene_set_identifier) void laser_scene_set_identifier(LaserSceneFixture* p,u32 index,u32 id){p->scene.manager.each([&](LaserObject& o){if(!index--)o.id=id;});}

struct ScreenGridFixture {ScreenGrid grid;EnemyDistortionState state;ScreenGridViewport viewport;Vec3 position;std::vector<AnmGeometryVertex> strip;ScreenGridFixture(){grid.initialize(17,17);state.active=true;}};
API(screen_grid_create) ScreenGridFixture* screen_grid_create(){return new ScreenGridFixture;}
API(screen_grid_delete) void screen_grid_delete(ScreenGridFixture* p){delete p;}
API(screen_grid_field) void* screen_grid_field(ScreenGridFixture* p,u32 kind){switch(kind){case 0:return &p->state;case 1:return &p->viewport;case 2:return &p->position;case 3:return p->grid.vertices.data();case 4:return p->grid.sampling.data();default:return p->strip.data();}}
API(screen_grid_update) u32 screen_grid_update(ScreenGridFixture* p,float rate){return p->grid.radial(p->state,p->position,rate,p->viewport);}
API(screen_grid_strip) u32 screen_grid_strip(ScreenGridFixture* p,u32 column){return p->grid.strip(column,p->strip);}
API(enemy_distortion_field) EnemyDistortionState* enemy_distortion_field(EnemyVariableFixture* p){return &p->enemy.distortion;}
API(enemy_distortion_active) void enemy_distortion_active(EnemyVariableFixture* p,u32 value){p->enemy.distortion.active=value!=0;p->events.distortion_events.clear();}
API(enemy_distortion_events) u32* enemy_distortion_events(EnemyVariableFixture* p){return p->events.distortion_events.data();}
API(enemy_distortion_event_count) u32 enemy_distortion_event_count(EnemyVariableFixture* p){return p->events.distortion_events.size();}

API(texture_decode) u32 texture_decode(u32 format,u32 width,u32 height,const u8* pixels,u32 size,u8* out){AnmTexture t;t.pixel_format=format;t.pixel_width=width;t.pixel_height=height;t.pixels.assign(pixels,pixels+size);std::vector<u8> rgba;if(!t.rgba(rgba))return 0;std::memcpy(out,rgba.data(),rgba.size());return 1;}
API(texture_image) u32 texture_image(u32 input,u32 output,u32 width,u32 height,u32 texture_width,u32 texture_height,const u8* bytes,u32 size,u8* out){AnmTexture t;t.format=output;t.width=texture_width;t.height=texture_height;t.pixel_width=width;t.pixel_height=height;t.pixel_format=input;t.pixels.assign(bytes,bytes+size);TextureImage image;if(!image.load(t))return 0;std::memcpy(out,image.pixels.data(),image.pixels.size());return image.pixels.size();}

struct MessageBridgeFixture final:DialogueSceneServices,MessageSceneServices {
 AnmManagerFixture& owner;DialogueResources resources;DialogueState balloon;MessageProgram program;MessageSceneState state;DialogueAnmHost host;std::unique_ptr<MessageController> controller;std::vector<u32> events;
 MessageBridgeFixture(AnmManagerFixture& a,u32 stage):owner(a),host(a.manager,a.environment,*this){resources.text=0;resources.player=1;resources.logo=2;resources.balloons=3;program.scripts.resize(2);for(auto& script:program.scripts){MessageInstruction end;end.opcode=0;end.time=65535;script.instructions.push_back(end);}controller=std::make_unique<MessageController>(program,*stage_definition(stage),resources,a.manager,host,state,*this);}
 bool initialize_text(AnmVm&,i32 w,i32 h)override{events.insert(events.end(),{5,u32(w),u32(h)});return true;}
 bool paint_text(AnmVm&,const DialogueText&)override{return true;}
 bool dialogue_music(bool boss)override{return queue_music_control(2,boss);}
 bool fade_dialogue_music(float)override{return true;}
 bool dialogue_stage_complete()override{return complete_stage();}
 bool dialogue_name_banner()override{return true;}
 bool dialogue_sound(i32)override{return true;}
 bool clear_dialogue_field()override{events.push_back(6);return true;}
 bool queue_music_control(i32 kind,i32 value)override{events.insert(events.end(),{1,u32(kind),u32(value)});return true;}
 bool unlock_music(i32 value)override{events.insert(events.end(),{2,u32(value)});return true;}
 bool complete_stage()override{events.push_back(3);return true;}
 bool begin_game_over()override{events.push_back(4);return true;}
};
API(message_bridge_create) MessageBridgeFixture* message_bridge_create(AnmManagerFixture* p,u32 stage){return stage_definition(stage)?new MessageBridgeFixture(*p,stage):nullptr;}
API(message_bridge_delete) void message_bridge_delete(MessageBridgeFixture* p){delete p;}
API(message_bridge_state) MessageSceneState* message_bridge_state(MessageBridgeFixture* p){return &p->state;}
API(message_bridge_configuration) void message_bridge_configuration(MessageBridgeFixture* p,u32 flags,u32 spell,i32 transition,u32 restart,const char* wave){p->state.game_flags=flags;p->state.spell_flags=spell;p->state.transition_counter=transition;p->state.restart_music=restart;p->state.current_music_wave=wave;p->events.clear();}
API(message_bridge_request) u32 message_bridge_request(MessageBridgeFixture* p,i32 id){return p->controller->request(id);}
API(message_bridge_events) u32* message_bridge_events(MessageBridgeFixture* p){return p->events.data();}
API(message_bridge_event_count) u32 message_bridge_event_count(MessageBridgeFixture* p){return p->events.size();}
API(message_bridge_error) const char* message_bridge_error(MessageBridgeFixture* p){return p->controller->error.empty()?p->host.error.c_str():p->controller->error.c_str();}
API(message_bridge_dialogue) DialogueState* message_bridge_dialogue(MessageBridgeFixture* p){return p->controller->dialogue?&p->controller->dialogue->state:nullptr;}
API(message_bridge_update) u32 message_bridge_update(MessageBridgeFixture* p,float rate){return p->controller->update({rate});}
API(message_bridge_complete) u32 message_bridge_complete(MessageBridgeFixture* p){return p->controller->finished();}
API(dialogue_bridge_balloon) u32 dialogue_bridge_balloon(MessageBridgeFixture* p,i32 style,float x,float y,float width){p->host.retire(p->balloon.handles[11]);p->balloon.balloon_style=style;p->balloon.position={x,y,0};return p->host.create_balloon(3,style+221,p->balloon.position,width,style,p->balloon.handles[11]);}
API(dialogue_bridge_balloon_handle) u32 dialogue_bridge_balloon_handle(MessageBridgeFixture* p){return p->balloon.handles[11];}
API(dialogue_bridge_child) AnmVm* dialogue_bridge_child(MessageBridgeFixture* p,i32 script){auto* parent=p->owner.manager.registry.find(p->balloon.handles[11]);return parent?p->owner.manager.registry.find_child_script(*parent,script,0):nullptr;}
API(dialogue_bridge_width) u32 dialogue_bridge_width(MessageBridgeFixture* p,float width){return p->host.balloon_width(p->balloon.handles[11],p->balloon.balloon_style,width);}
API(dialogue_bridge_follow) u32 dialogue_bridge_follow(MessageBridgeFixture* p,i32 speaker,u32 a,u32 b,u32 c,u32 d){p->balloon.speaker=speaker;p->balloon.handles[6]=a;p->balloon.handles[7]=b;p->balloon.handles[8]=c;p->balloon.handles[9]=d;return p->host.follow_balloon(p->balloon);}

API(text_kernel) u32 text_kernel(u32 operation,u32 format,u32 width,u32 height,u32 rows,const u8* input,u8* output){TextureImage im;im.width=width;im.height=height;im.pitch=width*(format==21?4:2);im.format=format==21?touhou::graphics::PixelFormat::Bgra8:format==25?touhou::graphics::PixelFormat::Argb1555:touhou::graphics::PixelFormat::Argb4444;im.pixels.assign(input,input+im.pitch*height);const bool ok=operation==0?TextRaster::invert_alpha(im,rows):operation==1?TextRaster::expand_alpha(im,rows):TextRaster::bleed(im,rows);if(ok)std::memcpy(output,im.pixels.data(),im.pixels.size());return ok;}
API(text_create) TextRaster* text_create(){return new TextRaster;}
API(text_delete) void text_delete(TextRaster* p){delete p;}
API(text_load) u32 text_load(TextRaster* p,u32 slot,const u8* bytes,u32 size){return p->glyphs.load(slot,bytes,size);}
API(text_style) TextStyle* text_style(){return new TextStyle;}
API(text_style_delete) void text_style_delete(TextStyle* p){delete p;}
API(text_rasterize) u32 text_rasterize(TextRaster* p,const char* text,const TextStyle* style){return p->rasterize(text,*style);}
API(text_pixels) u8* text_pixels(TextRaster* p){return p->scratch.pixels.data();}
API(text_write) u32 text_write(TextRaster* p,u32 w,u32 h,const TextRect* rect,const char* value,const TextStyle* style,u8* output){TextureImage im;im.width=w;im.height=h;im.pitch=w*2;im.format=touhou::graphics::PixelFormat::Argb4444;im.pixels.resize(im.pitch*h);if(!p->write(im,*rect,value,*style))return 0;std::memcpy(output,im.pixels.data(),im.pixels.size());return 1;}

struct PopupTestGlyph{i32 sprite;Vec3 position;u32 color;};
struct PopupTestText{Vec3 position;u32 color;std::string value;};
struct PopupFixture:PopupDrawServices{PopupManager manager;std::vector<PopupTestGlyph> glyphs;std::vector<PopupTestText> texts;explicit PopupFixture(AnmManagerFixture& p):manager(p.manager){}bool draw_glyph(AnmVm& p)override{glyphs.push_back({p.visual.sprite,p.visual.translation,p.visual.color});return true;}bool draw_text(const Vec3& p,u32 color,const std::string& value)override{texts.push_back({p,color,value});return true;}};
API(popup_create) PopupFixture* popup_create(AnmManagerFixture* manager){return new PopupFixture(*manager);}
API(popup_delete) void popup_delete(PopupFixture* p){delete p;}
API(popup_init) u32 popup_init(PopupFixture* p,i32 resource){return p->manager.initialize(resource);}
API(popup_number) void popup_number(PopupFixture* p,const Vec3* position,i32 value,u32 color){p->manager.number(*position,value,color);}
API(popup_bonus) void popup_bonus(PopupFixture* p,const Vec3* position,i32 value,u32 color,float multiplier){p->manager.bonus(*position,value,color,multiplier);}
API(popup_update) void popup_update(PopupFixture* p,float rate){p->manager.update(rate);}
API(popup_entries) PopupEntry* popup_entries(PopupFixture* p){return p->manager.entries.data();}
API(popup_save) void popup_save(PopupFixture* p){p->manager.save();}
API(popup_restore) void popup_restore(PopupFixture* p){p->manager.restore();}
API(popup_draw) u32 popup_draw(PopupFixture* p,const Vec3* player){p->glyphs.clear();p->texts.clear();return p->manager.draw(*p,*player);}
API(popup_glyphs) PopupTestGlyph* popup_glyphs(PopupFixture* p){return p->glyphs.data();}
API(popup_glyph_count) u32 popup_glyph_count(PopupFixture* p){return p->glyphs.size();}
API(popup_text_count) u32 popup_text_count(PopupFixture* p){return p->texts.size();}
API(popup_text_position) Vec3* popup_text_position(PopupFixture* p,u32 i){return &p->texts[i].position;}
API(popup_text_color) u32 popup_text_color(PopupFixture* p,u32 i){return p->texts[i].color;}
API(popup_text_value) const char* popup_text_value(PopupFixture* p,u32 i){return p->texts[i].value.c_str();}

struct ChapterRewardFixture{ItemCollectionFixture items{0};EnemyWorldState world;ChapterReward reward;explicit ChapterRewardFixture(AnmManagerFixture& a,i32 front):reward(a.manager,items.score,items.player,world,items.collection,front){}};
API(chapter_reward_create) ChapterRewardFixture* chapter_reward_create(AnmManagerFixture* a,i32 bank){return new ChapterRewardFixture(*a,bank);}
API(chapter_reward_delete) void chapter_reward_delete(ChapterRewardFixture* p){delete p;}
API(chapter_reward_items) ItemCollectionFixture* chapter_reward_items(ChapterRewardFixture* p){return &p->items;}
API(chapter_reward_counts) i32* chapter_reward_counts(ChapterRewardFixture* p){return &p->world.chapter_total;}
API(chapter_reward_state) ChapterRewardState* chapter_reward_state(ChapterRewardFixture* p){return &p->reward.state;}
API(chapter_reward_complete) u32 chapter_reward_complete(ChapterRewardFixture* p,u32 boss,i32 deaths){p->items.events.clear();return p->reward.complete(boss,deaths);}
API(chapter_reward_error) const char* chapter_reward_error(ChapterRewardFixture* p){return p->reward.error.c_str();}

struct SessionInitializationFixture final:SessionRecords,SessionInitializationServices {
 SessionState state;PlayerLifeSession player;ItemScoreState score;EnemyWorldState world;
 SessionHighScore best;bool gui=true; i32 play_count=0;std::vector<u32> events;
 SessionInitialization initialization{state,player,score,world,*this,*this};
 SessionHighScore high_score(const SessionRecordQuery&)override{return best;}
 bool mark_stage(i32,i32,i32)override{return true;}
 bool count_play(i32,bool)override{if(play_count<9999999)++play_count;return true;}
 bool event(u32 id){events.push_back(id);return true;}
 bool bomb_hud(i32 bombs,i32 pieces)override{if(gui){events.insert(events.end(),{0,u32(bombs),u32(pieces)});}return true;}
 bool create_player()override{event(1);player.power_step=100;score.max_power=400;return true;}
 bool configure_player()override{return event(2);}
 bool register_session()override{return event(3);}
 bool create_replay()override{return event(4);}bool reset_replay()override{return event(5);}
 bool create_background()override{return event(6);}bool create_gui()override{return event(7);}bool reset_gui()override{return event(8);}
 bool create_bullets()override{return event(9);}bool create_items()override{return event(10);}bool create_lasers()override{return event(11);}
 bool create_pause_menu()override{return event(12);}bool create_popups()override{return event(13);}bool create_checkpoint_storage()override{return event(14);}
 bool create_enemies()override{return event(15);}bool restore_enemies()override{return event(16);}
 bool create_bomb()override{return event(17);}bool create_spell()override{return event(18);}
 bool load_stage_music()override{return event(19);}bool load_player_music(bool boss)override{return event(boss?21:20);}
 bool finish_scene()override{return event(22);}
};
API(session_initialization_create) SessionInitializationFixture* session_initialization_create(){return new SessionInitializationFixture;}
API(session_initialization_delete) void session_initialization_delete(SessionInitializationFixture* p){delete p;}
API(session_initialization_field) void* session_initialization_field(SessionInitializationFixture* p,u32 n){switch(n){case 0:return &p->state;case 1:return &p->player;case 2:return &p->score;case 3:return &p->state.continue_budget;case 4:return &p->best;case 6:return &p->world.rank;default:return &p->play_count;}}
API(session_initialization_configure) void session_initialization_configure(SessionInitializationFixture* p,u32 gui,u32 new_run,u32 replay){p->gui=gui;p->state.new_run=new_run;p->state.replay=replay;p->events.clear();}
API(session_initialization_run) u32 session_initialization_run(SessionInitializationFixture* p){return p->initialization.initialize();}
API(session_initialization_events) u32* session_initialization_events(SessionInitializationFixture* p){return p->events.data();}
API(session_initialization_event_count) u32 session_initialization_event_count(SessionInitializationFixture* p){return p->events.size();}
API(session_initialization_error) const char* session_initialization_error(SessionInitializationFixture* p){return p->initialization.error.c_str();}

API(anm_checkpoint_create) AnmCheckpoint* anm_checkpoint_create(){return new AnmCheckpoint;}
API(anm_checkpoint_delete) void anm_checkpoint_delete(AnmCheckpoint* p){delete p;}
API(anm_checkpoint_clear) void anm_checkpoint_clear(AnmCheckpoint* p){p->clear();}
API(anm_checkpoint_capture) u32 anm_checkpoint_capture(AnmCheckpoint* p,AnmManagerFixture* manager,u32 handle){return p->capture(handle,manager->manager.registry);}
API(anm_checkpoint_capture_vm) u32 anm_checkpoint_capture_vm(AnmCheckpoint* p,AnmManagerFixture* manager,AnmVm* vm){return p->capture(vm,manager->manager.registry);}
API(anm_checkpoint_find) const AnmVm* anm_checkpoint_find(AnmCheckpoint* p,u32 handle){return p->find(handle);}
API(anm_checkpoint_order) u32 anm_checkpoint_order(AnmCheckpoint* p,u32 index){return p->ordered_handle(index);}
API(anm_checkpoint_count) u32 anm_checkpoint_count(AnmCheckpoint* p){return p->count();}
API(anm_checkpoint_child) u32 anm_checkpoint_child(AnmCheckpoint* p,u32 handle,u32 index){return p->child_handle(handle,index);}
API(anm_checkpoint_restore) u32 anm_checkpoint_restore(AnmCheckpoint* p,AnmManagerFixture* manager,u32 handle){return p->restore(handle,manager->manager.registry);}
API(anm_checkpoint_generation) u32* anm_checkpoint_generation(AnmCheckpoint* p){return &p->generation_counter();}
API(anm_checkpoint_error) const char* anm_checkpoint_error(AnmCheckpoint* p){return p->error.c_str();}
API(anm_vm_ordering) void anm_vm_ordering(AnmVm* p,u32 ordering){p->ordering=ordering;}
API(anm_registry_children_count) u32 anm_registry_children_count(AnmRegistry* p,AnmVm* vm){return p->children(*vm).size();}
API(anm_registry_child_handle) u32 anm_registry_child_handle(AnmRegistry* p,AnmVm* vm,u32 index){const auto list=p->children(*vm);return index<list.size()?p->handle(*list[index]):0;}

struct ChapterCheckpointFixture final:ChapterCheckpointServices {
 SessionInitializationFixture model;AnmManagerFixture animations;PopupManager popups{animations.manager};std::string music;
 ChapterCheckpoint checkpoint{model.state,model.player,model.score,model.world,music,*this};
 bool event(u32 id){model.events.push_back(id);return true;}
 bool synchronize_resources()override{return event(0);}
 bool clear_saved_animations()override{return event(1);}
 bool save_player()override{return event(2);}
 bool save_enemies()override{return event(3);}
 bool save_background()override{return event(4);}
 bool save_bullets()override{return event(5);}
 bool save_items()override{return event(6);}
 bool save_effects()override{return event(7);}
 bool save_bomb()override{return event(9);}
 bool retire_reward()override{return event(11);}
 bool retire_message()override{return event(12);}
 bool retire_scene_effect()override{return event(13);}
 bool restore_player()override{return event(14);}
 bool restore_enemies()override{return event(15);}
 bool restore_background()override{return event(16);}
 bool restore_bullets()override{return event(17);}
 bool restore_items()override{return event(18);}
 bool reset_spell()override{return event(19);}
 bool clear_lasers()override{return event(20);}
 bool restore_effects()override{return event(21);}
 bool restore_bomb()override{return event(23);}
 bool reset_gui()override{return event(26);}
 bool stop_sounds()override{return event(27);}
 bool begin_restart_effect()override{return event(28);}
 bool begin_restart_overlay()override{return event(29);}
 bool save_popups()override{popups.save();return true;}bool restore_popups()override{popups.restore();return true;}
 bool life_hud(i32 value,i32 pieces)override{model.events.insert(model.events.end(),{24,u32(value),u32(pieces)});return true;}
 bool bomb_hud(i32 value,i32 pieces)override{model.events.insert(model.events.end(),{25,u32(value),u32(pieces)});return true;}
 bool checkpoint_file(bool restore)override{return event(restore?30:10);}
};
API(chapter_checkpoint_create) ChapterCheckpointFixture* chapter_checkpoint_create(){return new ChapterCheckpointFixture;}
API(chapter_checkpoint_delete) void chapter_checkpoint_delete(ChapterCheckpointFixture* p){delete p;}
API(chapter_checkpoint_model) SessionInitializationFixture* chapter_checkpoint_model(ChapterCheckpointFixture* p){return &p->model;}
API(chapter_checkpoint_state) const void* chapter_checkpoint_state(ChapterCheckpointFixture* p,u32 kind){switch(kind){case 0:return &p->checkpoint.state();case 1:return &p->checkpoint.player_state();default:return &p->checkpoint.score_state();}}
API(chapter_checkpoint_chapters) i32* chapter_checkpoint_chapters(ChapterCheckpointFixture* p){return &p->model.world.chapter_total;}
API(chapter_checkpoint_capture) u32 chapter_checkpoint_capture(ChapterCheckpointFixture* p,i32 chapter,const char* wave){p->music=wave;p->model.events.clear();return p->checkpoint.capture(chapter);}
API(chapter_checkpoint_restore) u32 chapter_checkpoint_restore(ChapterCheckpointFixture* p){p->model.events.clear();return p->checkpoint.restore();}
API(chapter_checkpoint_popups) PopupEntry* chapter_checkpoint_popups(ChapterCheckpointFixture* p){return p->popups.entries.data();}
API(chapter_checkpoint_error) const char* chapter_checkpoint_error(ChapterCheckpointFixture* p){return p->checkpoint.error.c_str();}
API(chapter_checkpoint_music) const char* chapter_checkpoint_music(ChapterCheckpointFixture* p){return p->music.c_str();}

API(game_battle_priorities) const i32* game_battle_priorities(){return GameBattle::update_priorities.data();}

struct SessionRuntimeFixture final:SessionRuntimeServices {
 SessionInitializationFixture model;SessionFrameState frame;SessionRuntime runtime{model.state,model.player,*this};bool waiting=false,restored=false;u32 input_flags=0;
 bool event(u32 id){model.events.push_back(id);return true;}
 bool ending_fade()override{return event(1);}
 bool finish_replay()override{return event(2);}
 bool activate_scene()override{return event(5);}
 bool release_background()override{return event(6);}
 bool prepare_stage_music()override{return event(10);}
 bool start_stage_music()override{return event(11);}
 bool start_boss_music()override{return event(12);}
 bool demo_fade()override{return event(14);}
 bool update_overlays()override{return event(18);}
 bool destination(SessionDestination d)override{model.events.insert(model.events.end(),{3,u32(d)});return true;}
 bool load_scene(bool& out)override{out=waiting;return event(4);}
 bool load_checkpoint_file(bool& out)override{out=restored;return event(7);}
 bool restart_overlay(i32 script)override{model.events.insert(model.events.end(),{8,u32(script)});return true;}
 bool restart_effect(i32 label)override{model.events.insert(model.events.end(),{9,u32(label)});return true;}
 bool seek_stage_music(double seconds)override{u32 words[2];std::memcpy(words,&seconds,8);model.events.insert(model.events.end(),{13,words[0],words[1]});return true;}
 bool update_score()override{model.state.scene_flags|=input_flags;return event(15);}
 bool chapter_reward(bool boss)override{model.events.insert(model.events.end(),{16,u32(boss)});return true;}
 bool chapter_checkpoint(i32 chapter)override{model.events.insert(model.events.end(),{17,u32(chapter)});return true;}
};
API(session_runtime_create) SessionRuntimeFixture* session_runtime_create(){return new SessionRuntimeFixture;}
API(session_runtime_delete) void session_runtime_delete(SessionRuntimeFixture* p){delete p;}
API(session_runtime_model) SessionInitializationFixture* session_runtime_model(SessionRuntimeFixture* p){return &p->model;}
API(session_runtime_frame) SessionFrameState* session_runtime_frame(SessionRuntimeFixture* p){return &p->frame;}
API(session_runtime_state) void* session_runtime_state(SessionRuntimeFixture* p,u32 kind){return kind?static_cast<void*>(&p->runtime.ending_frames):static_cast<void*>(&p->runtime.age);}
API(session_runtime_configure) void session_runtime_configure(SessionRuntimeFixture* p,u32 waiting,u32 restored,u32 input){p->waiting=waiting;p->restored=restored;p->input_flags=input;p->model.events.clear();}
API(session_runtime_update) i32 session_runtime_update(SessionRuntimeFixture* p){p->model.events.clear();return p->runtime.update(p->frame);}
API(session_runtime_error) const char* session_runtime_error(SessionRuntimeFixture* p){return p->runtime.error.c_str();}

#include "../../cpp/game/PlayerCheckpoint.hpp"
#include "../../cpp/game/EffectCheckpoint.hpp"
API(player_checkpoint_create) PlayerCheckpoint* player_checkpoint_create(PlayerFixture* p,AnmCheckpoint* a){return new PlayerCheckpoint(p->player,p->animations.registry,*a);}
API(player_checkpoint_delete) void player_checkpoint_delete(PlayerCheckpoint* p){delete p;}
API(player_checkpoint_capture) i32 player_checkpoint_capture(PlayerCheckpoint* p){return p->capture();}
API(player_checkpoint_restore) i32 player_checkpoint_restore(PlayerCheckpoint* p){return p->restore();}
API(player_checkpoint_error) const char* player_checkpoint_error(PlayerCheckpoint* p){return p->error.c_str();}
API(player_checkpoint_shots) const PlayerShot* player_checkpoint_shots(PlayerCheckpoint* p){return &p->shot(0);}
API(player_checkpoint_handle) u32 player_checkpoint_handle(PlayerCheckpoint* p,u32 i,u32 k){if(i<8)return p->option_handle(i,k);return i==8?p->focus_handle():p->barrier_handle();}
API(effect_checkpoint_create) EffectCheckpoint* effect_checkpoint_create(PlayerFixture* p,AnmCheckpoint* a){return new EffectCheckpoint(p->effects,p->animations.registry,*a);}
API(effect_checkpoint_delete) void effect_checkpoint_delete(EffectCheckpoint* p){delete p;}
API(effect_checkpoint_capture) i32 effect_checkpoint_capture(EffectCheckpoint* p){return p->capture();}
API(effect_checkpoint_restore) i32 effect_checkpoint_restore(EffectCheckpoint* p){return p->restore();}
API(effect_checkpoint_error) const char* effect_checkpoint_error(EffectCheckpoint* p){return p->error.c_str();}
API(effect_checkpoint_handles) const u32* effect_checkpoint_handles(EffectCheckpoint* p){return p->saved_handles().data();}
API(effect_checkpoint_cursor) i32 effect_checkpoint_cursor(EffectCheckpoint* p){return p->saved_cursor();}

#include "../../cpp/game/StageCheckpoint.hpp"
API(stage_checkpoint_create) StageCheckpoint* stage_checkpoint_create(StageSceneFixture* p){return new StageCheckpoint(p->scene);}
API(stage_checkpoint_delete) void stage_checkpoint_delete(StageCheckpoint* p){delete p;}
API(stage_checkpoint_capture) i32 stage_checkpoint_capture(StageCheckpoint* p){return p->capture();}
API(stage_checkpoint_restore) i32 stage_checkpoint_restore(StageCheckpoint* p){return p->restore();}
API(stage_checkpoint_vm) const AnmVm* stage_checkpoint_vm(StageCheckpoint* p,u32 i,u32 embedded){return embedded?p->slot(i):p->primitive(i);}
API(stage_checkpoint_error) const char* stage_checkpoint_error(StageCheckpoint* p){return p->error.c_str();}
API(stage_checkpoint_script) const StageScriptState* stage_checkpoint_script(StageCheckpoint* p){return &p->script_state();}
API(stage_scene_script) StageScriptState* stage_scene_script(StageSceneFixture* p){return &p->scene.script.state;}
API(stage_scene_snapshot) void stage_scene_snapshot(StageSceneFixture* p,u8* out){
 const auto& s=p->scene.script.state;std::memset(out,0,0x3390);std::memcpy(out,&s.timer,20);std::memcpy(out+0x14,&s.instruction_offset,4);out[0x18]=s.camera_effect;std::memcpy(out+0x1c,&s.effect_timer,20);
 std::memcpy(out+0x30,&s.direction,88);std::memcpy(out+0x88,&s.position,88);std::memcpy(out+0xe0,&s.up,88);std::memcpy(out+0x138,&s.fog,168);
 std::memcpy(out+0x1e0,&s.camera.position,12);std::memcpy(out+0x1ec,&s.camera.direction,12);std::memcpy(out+0x1f8,&s.camera.up,12);std::memcpy(out+0x21c,&s.camera.eye_offset,12);std::memcpy(out+0x228,&s.camera.target_offset,12);std::memcpy(out+0x234,&s.camera.fov,4);std::memcpy(out+0x2e4,&s.camera.animation_delta,12);std::memcpy(out+0x2f0,&s.camera.fog,28);
 for(u32 i=0;i<8;++i)std::memcpy(out+0x310+i*0x608+0x18,&p->scene.slot(i)->visual.flags,4);std::memcpy(out+0x3350,s.animation_layers.data(),32);std::memcpy(out+0x3370,&s.culling_distance_squared,4);std::memcpy(out+0x3378,&s.deformation_target,4);std::memcpy(out+0x337c,&s.deformation_radius,4);std::memcpy(out+0x3380,&s.deformation_color,4);std::memcpy(out+0x3384,&s.phase_x,4);std::memcpy(out+0x3388,&s.phase_y,4);std::memcpy(out+0x338c,&s.deformation_mode,4);
}

API(bomb_checkpoint_create) BombCheckpoint* bomb_checkpoint_create(BombReisenFixture* p,AnmCheckpoint* a,u32 kind){switch(kind){case 0:return new BombCheckpoint(static_cast<BombReimuFixture*>(p)->reimu,*a);case 1:return new BombCheckpoint(static_cast<BombMarisaFixture*>(p)->marisa,*a);case 2:return new BombCheckpoint(static_cast<BombSanaeFixture*>(p)->sanae,*a);default:return new BombCheckpoint(p->bomb,*a);}}
API(bomb_checkpoint_delete) void bomb_checkpoint_delete(BombCheckpoint* p){delete p;}
API(bomb_checkpoint_capture) i32 bomb_checkpoint_capture(BombCheckpoint* p){return p->capture();}
API(bomb_checkpoint_restore) i32 bomb_checkpoint_restore(BombCheckpoint* p){return p->restore();}
API(bomb_checkpoint_error) const char* bomb_checkpoint_error(BombCheckpoint* p){return p->error.c_str();}
API(bomb_checkpoint_handle) u32 bomb_checkpoint_handle(BombCheckpoint* p,u32 i){return p->saved_handle(i);}
API(bomb_checkpoint_age) const Timer* bomb_checkpoint_age(BombCheckpoint* p,u32 secondary){return &p->saved_age(secondary);}
API(bomb_checkpoint_orb) const ReimuOrb* bomb_checkpoint_orb(BombCheckpoint* p,u32 i){return &p->orb(i);}
API(bomb_controller_field) void* bomb_controller_field(BombReisenFixture* p,u32 kind,u32 field){BombController* b=&p->bomb;switch(kind){case 0:b=&static_cast<BombReimuFixture*>(p)->reimu;break;case 1:b=&static_cast<BombMarisaFixture*>(p)->marisa;break;case 2:b=&static_cast<BombSanaeFixture*>(p)->sanae;break;}return field?static_cast<void*>(&b->effective_against_spell):static_cast<void*>(&b->secondary_age);}
API(bomb_reimu_orb_available) void bomb_reimu_orb_available(BombReimuFixture* p,u32 value){p->reimu.orbs_available=value;}
API(bomb_reimu_orb_archived) u32 bomb_reimu_orb_archived(BombReimuFixture* p,u32 i){return p->reimu.orbs[i].archived;}

API(laser_manager_clear) i32 laser_manager_clear(LaserManagerFixture* p){return p->manager.clear();}
API(bullet_scene_own_cancellation) void bullet_scene_own_cancellation(BulletSceneFixture* p,AnmManagerFixture* a,i32 bank){p->scene.own_cancellation_animations(a->manager,bank);}

struct BattleCheckpointFixture final:CheckpointSceneServices {
    GameBattleFixture& battle;SessionState progress;PopupManager popups;std::string music="th15_02.wav";
    bool available=true;std::vector<u32> events;BattleCheckpoint checkpoint;
    BattleCheckpointFixture(GameBattleFixture& b,StageSceneFixture& s):battle(b),popups(b.animations),checkpoint(b.game,s.scene,b.animations,progress,popups,music,*this,{-1,-1,-1,-1,-1,-1},7){progress.stage=2;progress.character=b.game.selected_character();popups.initialize(2);}
    bool event(u32 id){events.push_back(id);return available;}
    bool synchronize_resources()override{return event(0);}
    bool retire_reward()override{return event(1);}bool retire_message()override{return event(2);}bool retire_scene_effect()override{return event(3);}
    bool life_hud(i32 a,i32 b)override{events.insert(events.end(),{4,u32(a),u32(b)});return available;}
    bool bomb_hud(i32 a,i32 b)override{events.insert(events.end(),{5,u32(a),u32(b)});return available;}
    bool reset_gui()override{return event(6);}bool stop_sounds()override{return event(7);}
    bool begin_restart_effect()override{return event(8);}bool begin_restart_overlay()override{return event(9);}
    bool checkpoint_file(bool restore)override{return event(restore?11:10);}
};
API(battle_checkpoint_create) BattleCheckpointFixture* battle_checkpoint_create(GameBattleFixture* b,StageSceneFixture* s){return new BattleCheckpointFixture(*b,*s);}
API(battle_checkpoint_delete) void battle_checkpoint_delete(BattleCheckpointFixture* p){delete p;}
API(battle_checkpoint_capture) i32 battle_checkpoint_capture(BattleCheckpointFixture* p,i32 chapter){p->events.clear();return p->checkpoint.capture(chapter);}
API(battle_checkpoint_restore) i32 battle_checkpoint_restore(BattleCheckpointFixture* p){p->events.clear();return p->checkpoint.restore();}
API(battle_checkpoint_error) const char* battle_checkpoint_error(BattleCheckpointFixture* p){return p->checkpoint.error.c_str();}
API(battle_checkpoint_available) void battle_checkpoint_available(BattleCheckpointFixture* p,u32 value){p->available=value;}
API(battle_checkpoint_field) void* battle_checkpoint_field(BattleCheckpointFixture* p,u32 field){return field?static_cast<void*>(p->popups.entries.data()):static_cast<void*>(&p->progress);}
API(battle_checkpoint_count) u32 battle_checkpoint_count(BattleCheckpointFixture* p){return p->checkpoint.animation_count();}
API(battle_checkpoint_popups) void battle_checkpoint_popups(BattleCheckpointFixture* p,i32 number){p->popups.number({32,144,0},number,0xff80ffff);}
API(game_battle_cancellation) u32* game_battle_cancellation(GameBattleFixture* p){return p->game.bullet_scene->cancellation_animations.data();}
API(game_battle_cancel_bullets) i32 game_battle_cancel_bullets(GameBattleFixture* p){return p->game.bullet_scene->cancel_all(0);}
API(game_battle_item) u32 game_battle_item(GameBattleFixture* p,i32 kind){return p->game.items->spawn(kind,{0,80,0},-1.57079637f,2.2f);}
API(game_battle_bullet) const BulletState* game_battle_bullet(GameBattleFixture* p,u32 slot){return p->game.bullet_scene->manager.state(slot);}
API(game_battle_bullet_size) u32 game_battle_bullet_size(){return sizeof(BulletState);}
API(game_battle_item_state) const ItemState* game_battle_item_state(GameBattleFixture* p,u32 id){auto* s=p->game.items->find(id);return s?&s->state:nullptr;}
API(game_battle_bomb_common) void* game_battle_bomb_common(GameBattleFixture* p,u32 field){auto& b=*p->game.bomb;switch(field){case 0:return &b.position;case 1:return &b.angle;case 2:return &b.age;default:return &b.secondary_age;}}

API(game_battle_options) i32 game_battle_options(GameBattleFixture* p){return p->game.player->configure_options();}
API(game_battle_enemy_field) void* game_battle_enemy_field(GameBattleFixture* p,u32 id,u32 field){auto* e=p->game.enemies->find(id);if(!e)return nullptr;switch(field){case 0:return &e->state.motion;case 1:return &e->state.age_timer;case 2:return &e->state.life;default:return &e->state.animation_handles;}}

API(stage_scene_state) StageScriptState* stage_scene_state(StageSceneFixture* p){return &p->scene.script.state;}

API(bullet_object_payload) void bullet_object_payload(BulletStateFixture* p,u32 index,const char* text){auto& t=(p->external_state?*p->external_state:p->state).transforms[index];t.payload.assign(text,text+std::strlen(text)+1);}
API(bullet_object_count) u32 bullet_object_count(BulletStateFixture* p,u32 enemy){return enemy?p->spawned_count:p->object_lasers.count;}
API(bullet_object_laser_kind) u32 bullet_object_laser_kind(BulletStateFixture* p){return p->object_lasers.kind;}
API(bullet_object_laser) const u8* bullet_object_laser(BulletStateFixture* p){return p->object_lasers.bytes.data();}
API(bullet_object_enemy) void bullet_object_enemy(BulletStateFixture* p,u8* out){const auto& r=p->spawned;std::memset(out,0,84);std::memcpy(out,&r.position,12);const i32 values[]={r.score,r.item,r.life,i32(r.mirrored),i32(r.persistent)};std::memcpy(out+12,values,20);std::memcpy(out+32,r.integers.data(),16);std::memcpy(out+48,r.floats.data(),16);std::memcpy(out+64,r.temporary.data(),16);std::memcpy(out+80,&r.parent,4);}
API(bullet_object_enemy_name) const char* bullet_object_enemy_name(BulletStateFixture* p){return p->spawned.routine.c_str();}

struct MenuCursorFixture {MenuCursor menu;std::array<u8,0xd8> bytes{};};
extern "C" {
API(menu_cursor_create) MenuCursorFixture* menu_cursor_create(){return new MenuCursorFixture;}
API(menu_cursor_delete) void menu_cursor_delete(MenuCursorFixture* f){delete f;}
API(menu_cursor_configure) void menu_cursor_configure(MenuCursorFixture* f,i32 cursor,i32 count,i32 wrap){f->menu.cursor=cursor;f->menu.count=count;f->menu.wrapping=wrap;}
API(menu_cursor_select) i32 menu_cursor_select(MenuCursorFixture* f,i32 entry){return f->menu.select(entry);}
API(menu_cursor_move) i32 menu_cursor_move(MenuCursorFixture* f,i32 direction){return f->menu.move(direction);}
API(menu_cursor_disable) i32 menu_cursor_disable(MenuCursorFixture* f,i32 entry){return f->menu.disable(entry);}
API(menu_cursor_history) void menu_cursor_history(MenuCursorFixture* f,i32 push){if(push)f->menu.push();else f->menu.pop();}
API(menu_cursor_bytes) const u8* menu_cursor_bytes(MenuCursorFixture* f){auto write_i32=[](u8* p,i32 value){const u32 v=u32(value);for(u32 i=0;i<4;i++)p[i]=u8(v>>(i*8));};auto& m=f->menu;auto* b=f->bytes.data();write_i32(b,m.cursor);write_i32(b+4,m.previous);write_i32(b+8,m.count);for(i32 i=0;i<16;i++){write_i32(b+12+i*4,m.history_cursor[i]);write_i32(b+76+i*4,m.history_count[i]);write_i32(b+144+i*4,m.disabled[i]);}write_i32(b+140,m.depth);write_i32(b+208,m.wrapping);write_i32(b+212,m.disabled_count);return b;}
API(menu_cursor_error) const char* menu_cursor_error(MenuCursorFixture* f){return f->menu.error.c_str();}
}

struct TitleMenuFixture {AnmManager& animations;TitleState state;SessionState progress;PlayerLifeSession player;TitleSelectionSettings settings;TitleMainMenu menu;TitleModeMenu mode_menu;TitleDifficultyMenu difficulty_menu;std::string selection_error;MenuCursorFixture projection;std::vector<i32> sounds;std::array<u32,254> bytes{};explicit TitleMenuFixture(AnmManager& a):animations(a),menu(state,progress,player,settings,a),mode_menu(state,progress,player,settings,a),difficulty_menu(state,progress,player,settings,a){menu.play_sound=[this](i32 id){sounds.push_back(id);return true;};mode_menu.play_sound=menu.play_sound;difficulty_menu.play_sound=menu.play_sound;}};
API(title_menu_create) TitleMenuFixture* title_menu_create(AnmManagerFixture* a){return new TitleMenuFixture(a->manager);}
API(title_menu_delete) void title_menu_delete(TitleMenuFixture* f){delete f;}
API(title_menu_configure) void title_menu_configure(TitleMenuFixture* f,i32 cursor,i32 previous,i32 flags,i32 mode){f->state.menu.cursor=cursor;f->state.previous_screen=TitleScreen(previous);f->state.flags=u32(flags);f->player.mode_flags=u32(mode);}
API(title_menu_field) void* title_menu_field(TitleMenuFixture* f,u32 which){switch(which){case 0:return &f->progress;case 1:return &f->player;case 2:return &f->state.age;default:return &f->settings;}}
API(title_menu_state) const u32* title_menu_state(TitleMenuFixture* f){const auto& s=f->state;auto& b=f->bytes;b[0]=u32(s.screen);b[1]=u32(s.previous_screen);b[2]=s.substate;b[3]=s.menu.wrapping;b[4]=s.flags;b[5]=s.portrait;std::copy(s.handles.begin(),s.handles.end(),b.begin()+6);return b.data();}
API(title_menu_cursor) const u8* title_menu_cursor(TitleMenuFixture* f){f->projection.menu=f->state.menu;return menu_cursor_bytes(&f->projection);}
API(title_menu_update) i32 title_menu_update(TitleMenuFixture* f,u32 pressed,u32 repeated,i32 extra){f->sounds.clear();return f->menu.update(pressed,repeated,extra);}
API(title_menu_tick) void title_menu_tick(TitleMenuFixture* f,float rate){f->state.age.rate_index=0;f->state.age.tick(&rate);}
API(title_menu_sounds) const i32* title_menu_sounds(TitleMenuFixture* f){return f->sounds.data();}
API(title_menu_sound_count) u32 title_menu_sound_count(TitleMenuFixture* f){return f->sounds.size();}
API(title_menu_error) const char* title_menu_error(TitleMenuFixture* f){return f->menu.error.c_str();}

API(title_selection_return_reason) void title_selection_return_reason(TitleMenuFixture* f,i32 reason){f->state.return_reason=reason;}
API(title_selection_update) i32 title_selection_update(TitleMenuFixture* f,u32 pressed,u32 repeated,i32 extra,u32 clear_mask){f->sounds.clear();bool ok=false;switch(f->state.screen){case TitleScreen::Main:ok=f->menu.update(pressed,repeated,extra);f->selection_error=f->menu.error;break;case TitleScreen::Mode:ok=f->mode_menu.update(pressed,repeated);f->selection_error=f->mode_menu.error;break;case TitleScreen::Difficulty:{std::array<bool,5> cleared{};for(u32 i=0;i<5;i++)cleared[i]=(clear_mask>>i)&1;ok=f->difficulty_menu.update(pressed,repeated,cleared);f->selection_error=f->difficulty_menu.error;break;}default:f->selection_error="Unsupported title test screen";break;}return ok;}
API(title_selection_error) const char* title_selection_error(TitleMenuFixture* f){return f->selection_error.c_str();}

struct TitleCharacterFixture final:TitleResumeServices {TitleMenuFixture& title;TitleRecords records;u32 resume_mask=0;i32 saved_stage=1,numbered_chapter=0;std::vector<i32> events;TitleCharacterMenu menu;TitlePracticeMenu practice_menu;TitleResumeMenu resume_menu;std::string extended_error;explicit TitleCharacterFixture(TitleMenuFixture& t):title(t),menu(t.state,t.progress,t.player,t.settings,t.animations,*this),practice_menu(t.state,t.progress,t.settings,t.animations,*this),resume_menu(t.state,t.progress,t.player,t.animations,*this){}bool checkpoint_available(i32 character,i32 difficulty,bool& available)override{events.insert(events.end(),{1,character,difficulty});available=(resume_mask>>character)&1;return true;}bool reset_resume_selection()override{events.push_back(2);return true;}bool sound(i32 id)override{title.sounds.push_back(id);return true;}bool prepare_game_music()override{events.push_back(3);return true;}bool begin_transition(u32& animation)override{events.push_back(4);animation=0;return true;}bool checkpoint_stage(i32 character,i32 difficulty,i32& stage)override{events.insert(events.end(),{6,character,difficulty});stage=saved_stage;return true;}bool start_game(i32 stage)override{events.insert(events.end(),{5,stage});return true;}};
API(title_character_create) TitleCharacterFixture* title_character_create(TitleMenuFixture* t){return new TitleCharacterFixture(*t);}
API(title_character_delete) void title_character_delete(TitleCharacterFixture* f){delete f;}
API(title_character_records) TitleRecords* title_character_records(TitleCharacterFixture* f){return &f->records;}
API(title_character_extra) i32 title_character_extra(TitleCharacterFixture* f,i32 character){return character<0?f->records.extra_available():f->records.extra_character(character);}
API(title_character_clear_mask) u32 title_character_clear_mask(TitleCharacterFixture* f,u32 flags){u32 mask=0;const auto values=f->records.all_characters(flags);for(u32 i=0;i<5;i++)if(values[i])mask|=1u<<i;return mask;}
API(title_character_configure) void title_character_configure(TitleCharacterFixture* f,u32 resume_mask){f->resume_mask=resume_mask;}
API(title_character_update) i32 title_character_update(TitleCharacterFixture* f,u32 pressed,u32 repeated){f->title.sounds.clear();f->events.clear();return f->menu.update(pressed,repeated,f->records);}
API(title_character_events) const i32* title_character_events(TitleCharacterFixture* f){return f->events.data();}
API(title_character_event_count) u32 title_character_event_count(TitleCharacterFixture* f){return f->events.size();}
API(title_character_error) const char* title_character_error(TitleCharacterFixture* f){return f->menu.error.c_str();}

API(title_extended_configure) void title_extended_configure(TitleCharacterFixture* f,i32 saved_stage,i32 chapter){f->saved_stage=saved_stage;f->numbered_chapter=chapter;}
API(title_extended_update) i32 title_extended_update(TitleCharacterFixture* f,u32 pressed,u32 repeated){f->title.sounds.clear();f->events.clear();bool ok=false;switch(f->title.state.screen){case TitleScreen::Character:ok=f->menu.update(pressed,repeated,f->records);f->extended_error=f->menu.error;break;case TitleScreen::Stage:ok=f->practice_menu.update(pressed,repeated,f->numbered_chapter,f->records);f->extended_error=f->practice_menu.error;break;case TitleScreen::ContinuePrompt:ok=f->resume_menu.update(pressed,repeated);f->extended_error=f->resume_menu.error;break;default:f->extended_error="Unsupported extended title test screen";break;}return ok;}
API(title_extended_error) const char* title_extended_error(TitleCharacterFixture* f){return f->extended_error.c_str();}
API(title_practice_enable) void title_practice_enable(TitleCharacterFixture* f,i32 character,i32 difficulty,i32 stage,i32 enabled){f->records.practice_stages[character][difficulty][stage]=enabled;}
API(title_practice_selection) i32 title_practice_selection(TitleCharacterFixture* f,u32 which){return which?f->title.state.return_reason:f->title.state.practice_chapter;}

#include "../../cpp/game/SessionActivation.hpp"
struct SessionActivationFixture final:SceneActivationServices {
 SessionState progress;PlayerLifeSession player;Timer age;SceneActivationFrame frame;std::vector<u32> events;u32 enabled=0;bool available=true;SessionActivation activation{progress,player,age,*this};
 bool event(u32 id){events.push_back(id);return available;}
 bool clear_stage_intro()override{return event(1);}bool discard_stage_assets()override{return event(2);}bool return_to_title(bool selection)override{events.insert(events.end(),{3,u32(selection?2:3)});return available;}
 bool initialize_background()override{return event(4);}bool capture_previous_background()override{return event(5);}bool prepare_background_transition()override{return event(6);}bool transition_banner()override{return event(7);}
 bool reset_bullets()override{return event(8);}bool reset_player()override{return event(9);}bool reset_items()override{return event(10);}bool reset_enemies()override{return event(11);}bool reset_lasers()override{return event(12);}
 bool prepare_replay_stage()override{return event(13);}bool start_main_script()override{return event(14);}bool initialize_hud()override{return event(15);}bool enable_game_callbacks()override{enabled|=2;return available;}bool configure_player_options()override{return event(16);}
 bool start_stage_music()override{return event(17);}bool interrupt_entrance()override{return event(18);}bool retire_restart_overlay()override{return event(19);}bool interrupt_resume_overlay()override{return event(20);}
 bool restore_pending_progress()override{return event(21);}bool queue_music(i32 kind)override{events.insert(events.end(),{22,u32(kind)});return available;}bool unlock_current_music()override{return available;}
};
API(session_activation_create) SessionActivationFixture* session_activation_create(){return new SessionActivationFixture;}
API(session_activation_delete) void session_activation_delete(SessionActivationFixture* p){delete p;}
API(session_activation_field) void* session_activation_field(SessionActivationFixture* p,u32 field){switch(field){case 0:return &p->progress;case 1:return &p->player;case 2:return &p->age;case 3:return &p->enabled;default:return &p->activation.pending_progress;}}
API(session_activation_configure) void session_activation_configure(SessionActivationFixture* p,u32 previous,u32 selection,u32 audio,u32 pending){p->frame={bool(previous),bool(selection),bool(audio)};p->activation.pending_progress=pending;p->activation.loaded=false;p->activation.error.clear();p->enabled=0;p->events.clear();}
API(session_activation_run) i32 session_activation_run(SessionActivationFixture* p,u32 mode){p->events.clear();if(mode)return p->activation.activate(p->frame)?0:-1;bool waiting=false;if(!p->activation.load(p->frame,waiting))return -1;return waiting;}
API(session_activation_events) const u32* session_activation_events(SessionActivationFixture* p){return p->events.data();}
API(session_activation_event_count) u32 session_activation_event_count(SessionActivationFixture* p){return p->events.size();}
API(session_activation_error) const char* session_activation_error(SessionActivationFixture* p){return p->activation.error.c_str();}

#include "../../cpp/game/GameHud.hpp"
struct GameHudFixture final:HudFrameServices,HudDrawServices {SessionState progress;PlayerLifeSession player;ItemScoreState score;EnemyWorldState world;std::array<EnemyState,2> enemies;Vec3 position;HudFrameContext context;std::vector<i32> sounds;std::vector<HudTextDraw> text;HudDrawContext draw_context;GameHud hud;GameHudFixture(AnmManagerFixture& a,i32 front,i32 ascii,i32 logo):hud(a.manager,progress,player,score,front,ascii,logo){}bool hud_text(const HudTextDraw& value)override{text.push_back(value);return true;}bool hud_sound(i32 id)override{sounds.push_back(id);return true;}};
API(game_hud_create) GameHudFixture* game_hud_create(AnmManagerFixture* a,i32 front,i32 ascii,i32 logo){return new GameHudFixture(*a,front,ascii,logo);}
API(game_hud_delete) void game_hud_delete(GameHudFixture* p){delete p;}
API(game_hud_field) void* game_hud_field(GameHudFixture* p,u32 index){switch(index){case 0:return &p->progress;case 1:return &p->player;case 2:return &p->score;case 3:return p->hud.life_icons.data();case 4:return p->hud.bomb_icons.data();case 5:return p->hud.boss_icons.data();case 6:return &p->hud.root;case 7:return &p->hud.pointdevice;case 8:return &p->hud.mode_notice;case 9:return &p->hud.difficulty_notice;case 10:return &p->hud.difficulty_label;case 11:return &p->hud.flags;case 12:return &p->hud.intro_age;default:return nullptr;}}
API(game_hud_initialize) i32 game_hud_initialize(GameHudFixture* p,i32 destination){return p->hud.initialize({destination});}
API(game_hud_stock) i32 game_hud_stock(GameHudFixture* p,u32 bomb,i32 stock,i32 pieces){return bomb?p->hud.bombs(stock,pieces):p->hud.life(stock,pieces);}
API(game_hud_error) const char* game_hud_error(GameHudFixture* p){return p->hud.error.c_str();}

#include "../../cpp/game/SessionCompletion.hpp"
struct SessionCompletionFixture final:CompletionRecords,CompletionServices {
 SessionState progress;PlayerLifeSession player;ItemScoreState score;u32 hud_flags=0; i32 clear_bonus=0,ending_frames=0,finish_count=0,clear_count=0,best_spell=0;u8 stage_cleared=0;std::vector<u32> events;
 SessionCompletion completion{progress,player,score,hud_flags,clear_bonus,ending_frames,*this,*this};
 bool stage_clear(i32,i32,i32)override{stage_cleared=1;return true;}
 bool finished_run(i32,bool,i32,bool clear)override{if(finish_count<99999)finish_count++;if(clear&&clear_count<99999)clear_count++;return true;}
 bool spell_score(i32,bool,i32,i32 value)override{if(best_spell<value)best_spell=value;return true;}
 bool remove_checkpoint(i32 character,i32 difficulty)override{events.insert(events.end(),{9,u32(character),u32(difficulty)});return true;}
 bool event(u32 n){events.push_back(n);return true;}
 bool clear_notice()override{return event(1);}bool finish_player_options()override{return event(2);}bool end_bomb()override{return event(3);}bool prepare_ending()override{return event(4);}bool finish_replay()override{return event(5);}bool finish_practice()override{return event(6);}bool queue_next_scene()override{return event(7);}bool select_stage_resources(i32 stage)override{events.insert(events.end(),{8,u32(stage)});return true;}
};
API(session_completion_create) SessionCompletionFixture* session_completion_create(){return new SessionCompletionFixture;}
API(session_completion_delete) void session_completion_delete(SessionCompletionFixture* p){delete p;}
API(session_completion_field) void* session_completion_field(SessionCompletionFixture* p,u32 field){switch(field){case 0:return &p->progress;case 1:return &p->player;case 2:return &p->score;case 3:return &p->hud_flags;case 4:return &p->clear_bonus;case 5:return &p->ending_frames;case 6:return &p->finish_count;case 7:return &p->clear_count;case 8:return &p->best_spell;default:return &p->stage_cleared;}}
API(session_completion_run) i32 session_completion_run(SessionCompletionFixture* p){p->events.clear();return p->completion.complete();}
API(session_completion_events) const u32* session_completion_events(SessionCompletionFixture* p){return p->events.data();}
API(session_completion_event_count) u32 session_completion_event_count(SessionCompletionFixture* p){return p->events.size();}
API(session_completion_error) const char* session_completion_error(SessionCompletionFixture* p){return p->completion.error.c_str();}

#include "../../cpp/game/StageGameplay.hpp"
struct StageGameplayFixture:StageGameplayServices {
 StageAssetFixture& assets;SessionState progress;std::vector<u32> events;bool available=true;u32 completions=0,music_requests=0,text_requests=0; i32 requested_chapter=-1;std::unique_ptr<StageGameplay> scene;
 explicit StageGameplayFixture(StageAssetFixture& a,bool construct_scene=true):assets(a){if(!construct_scene)return;progress.stage=a.assets.definition->id;progress.character=a.assets.character;progress.difficulty=progress.stage==7?4:1;scene=std::make_unique<StageGameplay>(a.assets,a.animations,a.environment,a.environment.game_rng,a.random,progress,*this,&requested_chapter);auto& g=scene->battle;g.session.power=400;g.session.power_step=100;g.score.difficulty=progress.difficulty;g.score.point_value=1000000;g.score.max_point_value=50000000;}
    void record(u32 id,std::initializer_list<u32> values={}){events.push_back(id);events.push_back(values.size());events.insert(events.end(),values.begin(),values.end());}
    bool audio(i32 id,float pan,PlayerShots::SoundAction action,bool queued)override{record(1,{u32(id),float_to_bits(pan),u32(action),u32(queued)});return available;}
    bool life_hud(i32,i32)override{return false;}
    bool bomb_hud(i32,i32)override{return false;}
    bool game_over()override{record(4);return true;}bool notice(i32 id)override{record(5,{u32(id)});return true;}
    bool popup(const Vec3&,i32,u32)override{return false;}
    bool cancellation_effect(i32 script,const Vec3&,const Vec3&)override{record(7,{u32(script)});return true;}
    bool graze_spark(const Vec3&)override{record(8);return true;}bool graze_flash()override{record(9);return true;}bool graze_resonance(float v)override{record(10,{float_to_bits(v)});return true;}
    bool shake(const ScreenShakeSpec&)override{record(11);return true;}bool nudge(const ScreenNudgeSpec&)override{record(12);return true;}
    bool bomb_damage(const DamageQuery&,i32& out)override{out=0;return true;}
    int enemy_callback(EnemyRuntime&,float)override{return 0;}
    bool enemy_additional_damage(EnemyState&,i32,i32& out)override{out=0;return true;}
    bool enemy_contact(EnemyState&,AnmVm*,i32&,bool& handled)override{handled=false;return true;}
    bool enemy_distortion(EnemyState& e,float)override{return !e.distortion.active;}
    bool enemy_death_callback(EnemyRuntime&)override{record(13);return true;}
    bool spell_background_visible(bool v)override{record(14,{u32(v)});return true;}
    bool spell_prepare_hud(bool v)override{record(15,{u32(v)});return true;}
    bool spell_title(AnmVm&,const std::string&)override{record(16);return true;}
    bool spell_history_begin(i32 id,const std::string&)override{record(17,{u32(id)});return true;}
    bool spell_history_capture(i32 id)override{record(18,{u32(id)});return true;}
    bool spell_result(i32 bonus,bool failed)override{record(19,{u32(bonus),u32(failed)});return true;}
    bool spell_sound(i32 id)override{return audio(id,0,PlayerShots::SoundAction::play,true);}
 bool queue_music_control(i32 kind,i32 value)override{record(20,{u32(kind),u32(value)});music_requests++;return available;}
 bool unlock_music(i32 index)override{record(21,{u32(index)});return available;}
 bool complete_stage()override{completions++;return available;}bool begin_game_over()override{completions++;return available;}
 bool initialize_text(AnmVm&,i32,i32)override{return available;}bool paint_text(AnmVm&,const DialogueText&)override{text_requests++;return available;}
 bool dialogue_music(bool)override{return false;}bool fade_dialogue_music(float)override{return available;}bool dialogue_stage_complete()override{return false;}bool dialogue_name_banner()override{record(22);return available;}bool dialogue_sound(i32)override{return false;}bool clear_dialogue_field()override{return false;}
 bool begin_deformation(i32,i32)override{return false;}bool update_deformation(StageScriptState&,float)override{return false;}
 bool synchronize_resources()override{return available;}bool retire_reward()override{return false;}bool retire_message()override{return false;}bool retire_scene_effect()override{return available;}bool reset_gui()override{return available;}bool stop_sounds()override{return available;}bool begin_restart_effect()override{return available;}bool begin_restart_overlay()override{return available;}bool checkpoint_file(bool)override{return available;}
};
API(stage_gameplay_create) StageGameplayFixture* stage_gameplay_create(StageAssetFixture* assets){if(!assets->assets.error.empty()||!assets->assets.definition)return nullptr;return new StageGameplayFixture(*assets);}
API(stage_gameplay_delete) void stage_gameplay_delete(StageGameplayFixture* p){delete p;}
API(stage_gameplay_prepare) i32 stage_gameplay_prepare(StageGameplayFixture* p){StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};return p->scene->prepare(camera);}
API(stage_gameplay_begin) i32 stage_gameplay_begin(StageGameplayFixture* p){return p->scene->begin(0);}
API(stage_gameplay_step) i32 stage_gameplay_step(StageGameplayFixture* p,u32 held,u32 pressed,u32 shoot_frames,u32 skip_frames){StageGameplayFrame frame;frame.battle.held=held;frame.battle.pressed=pressed;frame.battle.focus_allowed=true;frame.dialogue.held=held;frame.dialogue.pressed=pressed;frame.dialogue.shoot_frames=shoot_frames;frame.dialogue.skip_frames=skip_frames;frame.background.flags=1;auto& g=p->scene->battle;g.player->life.invulnerability.set(100000);return p->scene->step(frame);}
API(stage_gameplay_error) const char* stage_gameplay_error(StageGameplayFixture* p){return p->scene->error.empty()?p->scene->battle.error.c_str():p->scene->error.c_str();}
API(stage_gameplay_field) void* stage_gameplay_field(StageGameplayFixture* p,u32 field){auto& s=*p->scene;switch(field){case 0:return &p->progress;case 1:return &s.battle.session;case 2:return &s.battle.score;case 3:return &s.battle.player->motion.position;case 4:return &s.background.script.state;case 5:return &s.hud.boss;case 6:return s.popups.entries.data();case 7:return &s.reward.state;default:return &p->requested_chapter;}}
API(stage_gameplay_counts) void stage_gameplay_counts(StageGameplayFixture* p,u32* out){auto& s=*p->scene;out[0]=s.battle.enemies->count();out[1]=s.battle.bullet_scene->manager.visible_count;out[2]=s.battle.laser_scene->manager.count();out[3]=s.battle.items->active_count;out[4]=s.messages.active();out[5]=s.messages.finished();out[6]=p->completions;out[7]=p->music_requests;out[8]=p->text_requests;out[9]=s.background.frames;out[10]=p->requested_chapter;out[11]=s.checkpoint.ready();}
API(stage_gameplay_capture) i32 stage_gameplay_capture(StageGameplayFixture* p,i32 chapter){return p->scene->capture(chapter);}
API(stage_gameplay_restore) i32 stage_gameplay_restore(StageGameplayFixture* p){return p->scene->restore();}
API(stage_gameplay_reward) i32 stage_gameplay_reward(StageGameplayFixture* p,i32 boss){return p->scene->chapter_reward(boss!=0);}
API(stage_gameplay_music) const char* stage_gameplay_music(StageGameplayFixture* p){return p->scene->music.current_music_wave.c_str();}
API(stage_gameplay_available) void stage_gameplay_available(StageGameplayFixture* p,i32 value){p->available=value;}

API(stage_gameplay_enemy_trace) void stage_gameplay_enemy_trace(StageGameplayFixture* p,u32* out){auto& g=p->scene->battle;auto* e=g.enemies->at(0);std::memset(out,0,48);if(!e)return;out[0]=float_to_bits(e->scripts.main.time);out[1]=u32(e->scripts.main.routine);out[2]=u32(e->scripts.main.instruction_offset);out[3]=e->state.flags;out[4]=g.enemy_world.counter1;out[5]=g.enemy_world.player_state;out[6]=g.enemy_world.spell_state;out[7]=float_to_bits(g.active_frame().rate);out[8]=g.enemy_world.scene_flags;out[9]=e->state.life;out[10]=u32(e->state.age_timer.current);out[11]=g.enemy_world.current_chapter;}

struct ScreenGridObjectFixture {ScreenGridObject mesh;AnmManager& animations;EnemyDistortionState state;ScreenGridViewport view;Vec3 position;explicit ScreenGridObjectFixture(AnmManager& a):animations(a){}~ScreenGridObjectFixture(){mesh.retire(animations);}};
API(screen_grid_object_create) ScreenGridObjectFixture* screen_grid_object_create(AnmManagerFixture* a){return new ScreenGridObjectFixture(a->manager);}
API(screen_grid_object_delete) void screen_grid_object_delete(ScreenGridObjectFixture* p){delete p;}
API(screen_grid_object_initialize) i32 screen_grid_object_initialize(ScreenGridObjectFixture* p,i32 bank){return p->mesh.initialize(p->animations,bank);}
API(screen_grid_object_handle) u32 screen_grid_object_handle(ScreenGridObjectFixture* p,u32 index){return index==0?p->mesh.capture_handle:index<=16?p->mesh.strip_handles[index-1]:0;}
API(screen_grid_object_field) void* screen_grid_object_field(ScreenGridObjectFixture* p,u32 kind){return kind==0?static_cast<void*>(&p->state):kind==1?static_cast<void*>(&p->view):kind==2?static_cast<void*>(&p->position):kind==3?static_cast<void*>(p->mesh.grid.vertices.data()):static_cast<void*>(p->mesh.grid.sampling.data());}
API(screen_grid_object_update) i32 screen_grid_object_update(ScreenGridObjectFixture* p,float rate){return p->mesh.update(p->animations,p->state,p->position,rate,p->view);}
API(screen_grid_object_retire) i32 screen_grid_object_retire(ScreenGridObjectFixture* p){return p->mesh.retire(p->animations);}
API(stage_gameplay_enemy_limit) i32 stage_gameplay_enemy_limit(StageGameplayFixture* p){return p->scene->battle.enemy_world.enemy_control;}

#include "../../cpp/game/AnmSceneEffects.hpp"
API(anm_scene_effects_create) AnmSceneEffects* anm_scene_effects_create(AnmManagerFixture* p,i32 bank){return new AnmSceneEffects(p->manager,p->environment.game_rng,p->random,bank);}
API(anm_scene_effects_delete) void anm_scene_effects_delete(AnmSceneEffects* p){delete p;}
API(anm_scene_effects_configure) u32 anm_scene_effects_configure(AnmSceneEffects* p,AnmVm* vm,i32 kind){return p->configure(*vm,kind);}
API(anm_vm_gather) AnmGatherEffect* anm_vm_gather(AnmVm* p){return p->geometry.gather.get();}
API(anm_vm_age) Timer* anm_vm_age(AnmVm* p){return &p->age;}

API(anm_scene_effects_update) i32 anm_scene_effects_update(AnmSceneEffects* p,AnmVm* vm,float rate){return p->update(*vm,rate);}

API(anm_scene_effects_interrupt) u32 anm_scene_effects_interrupt(AnmSceneEffects* p,AnmVm* vm,i32 label,float rate){return p->interrupt(*vm,label,rate);}


API(game_hud_frame_field) void* game_hud_frame_field(GameHudFixture* p,u32 field){switch(field){case 0:return &p->hud.last_countdown;case 1:return p->hud.boss.health.data();case 2:return p->hud.boss_animations.data();case 3:return p->hud.boss_stars.data();case 4:return &p->hud.boss_banner;case 5:return &p->hud.frame_age;case 6:return &p->hud.result;case 7:return &p->hud.result_notice;case 8:return &p->hud.background_notice;case 9:return &p->hud.tutorial_state;case 10:return &p->hud.boss.displayed_segments;case 11:return &p->world.manager_flags;case 12:return &p->world.boss_seconds;case 13:return &p->position;case 14:return &p->context.spell_flags;case 15:return &p->context.dialogue;case 16:return p->context.boss_banners.data();default:return nullptr;}}
API(game_hud_frame_enemy) void game_hud_frame_enemy(GameHudFixture* p,u32 slot,u32 id,float x,float y,float z,i32 life,i32 initial,i32 phase,u32 flags,i32 collision){if(slot>=2)return;auto& e=p->enemies[slot];e.id=id;e.motion.position={x,y,z};e.life=life;e.initial_life=initial;e.phase_life=phase;e.flags=flags;e.collision_timer.set(collision);p->world.boss_ids[slot]=id;p->world.enemies.clear();for(auto& e:p->enemies)if(e.id)p->world.enemies.push_back(&e);}
API(game_hud_name_banner) u32 game_hud_name_banner(GameHudFixture* p){return p->hud.name_banner(p->context.boss_banners);}
API(game_hud_frame_update) u32 game_hud_frame_update(GameHudFixture* p,u32 manager,u32 player,float rate){p->progress.rate=rate;p->context.enemies=manager?&p->world:nullptr;p->context.player_position=player?&p->position:nullptr;p->sounds.clear();return p->hud.update_before_dialogue(p->context,*p)&&p->hud.update_after_dialogue(p->context);}
API(game_hud_frame_sounds) const i32* game_hud_frame_sounds(GameHudFixture* p){return p->sounds.data();}
API(game_hud_frame_sound_count) u32 game_hud_frame_sound_count(GameHudFixture* p){return p->sounds.size();}

API(game_hud_draw_field) void* game_hud_draw_field(GameHudFixture* p,u32 field){switch(field){case 0:return &p->hud.displayed_score;case 1:return &p->hud.score_increment;case 2:return &p->hud.clear_bonus;case 3:return &p->hud.result_grazes;case 4:return &p->hud.result_deaths;case 5:return &p->draw_context.spell_available;case 6:return &p->draw_context.spell_frames;case 7:return &p->draw_context.spell_best_time;case 8:return &p->hud.chapter_notice;case 9:return &p->world.boss_hundredths;default:return nullptr;}}
API(game_hud_score_update) void game_hud_score_update(GameHudFixture* p){p->hud.update_score();}
API(game_hud_draw) u32 game_hud_draw(GameHudFixture* p,u32 manager,u32 player){p->draw_context.frame=p->context;p->draw_context.frame.enemies=manager?&p->world:nullptr;p->draw_context.frame.player_position=player?&p->position:nullptr;p->text.clear();return p->hud.draw(p->draw_context,*p);}
API(game_hud_text_count) u32 game_hud_text_count(GameHudFixture* p){return p->text.size();}
API(game_hud_text_position) const Vec3* game_hud_text_position(GameHudFixture* p,u32 index){return index<p->text.size()?&p->text[index].position:nullptr;}
API(game_hud_text_style) const HudTextStyle* game_hud_text_style(GameHudFixture* p,u32 index){return index<p->text.size()?&p->text[index].style:nullptr;}
API(game_hud_text_value) const char* game_hud_text_value(GameHudFixture* p,u32 index){return index<p->text.size()?p->text[index].text.c_str():nullptr;}
API(number_text) const char* number_text(u32 value,u32 kind,i32 last){static std::string text;text=kind?score_number(value,last):grouped_number(value);return text.c_str();}

#include "../../cpp/game/AsciiText.hpp"
struct AsciiTextFixture final:AsciiGlyphServices {AsciiText text;HudTextDraw request;std::vector<AnmVm> glyphs;explicit AsciiTextFixture(AnmManagerFixture& a):text(a.manager,a.environment){}bool ascii_glyph(AnmVm& vm)override{glyphs.push_back(vm);return true;}};
API(ascii_text_create) AsciiTextFixture* ascii_text_create(AnmManagerFixture* a){return new AsciiTextFixture(*a);}
API(ascii_text_delete) void ascii_text_delete(AsciiTextFixture* p){delete p;}
API(ascii_text_initialize) u32 ascii_text_initialize(AsciiTextFixture* p,i32 bank){return p->text.initialize(bank);}
API(ascii_text_position) Vec3* ascii_text_position(AsciiTextFixture* p){return &p->request.position;}
API(ascii_text_style) HudTextStyle* ascii_text_style(AsciiTextFixture* p){return &p->request.style;}
API(ascii_text_enqueue) u32 ascii_text_enqueue(AsciiTextFixture* p,const char* value,i32 lifetime,u32 shadow){p->request.text=value;return p->text.enqueue(p->request,lifetime,shadow);}
API(ascii_text_update) void ascii_text_update(AsciiTextFixture* p){p->text.update();}
API(ascii_text_count) u32 ascii_text_count(AsciiTextFixture* p){return p->text.count();}
API(ascii_text_entry_position) const Vec3* ascii_text_entry_position(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?&e->text.position:nullptr;}
API(ascii_text_entry_style) const HudTextStyle* ascii_text_entry_style(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?&e->text.style:nullptr;}
API(ascii_text_entry_value) const char* ascii_text_entry_value(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?e->text.text.c_str():nullptr;}
API(ascii_text_entry_lifetime) i32 ascii_text_entry_lifetime(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?e->lifetime:-1;}
API(ascii_text_draw) u32 ascii_text_draw(AsciiTextFixture* p,i32 space){p->glyphs.clear();return p->text.draw(space,*p);}
API(ascii_text_glyph_count) u32 ascii_text_glyph_count(AsciiTextFixture* p){return p->glyphs.size();}
API(ascii_text_glyph) AnmVm* ascii_text_glyph(AsciiTextFixture* p,u32 i){return i<p->glyphs.size()?&p->glyphs[i]:nullptr;}
API(ascii_text_error) const char* ascii_text_error(AsciiTextFixture* p){return p->text.error.c_str();}

API(game_hud_stage_clear) u32 game_hud_stage_clear(GameHudFixture* p){return p->hud.stage_clear();}
API(game_hud_clear_intro) u32 game_hud_clear_intro(GameHudFixture* p){return p->hud.clear_intro();}
API(game_hud_prepare_spell) u32 game_hud_prepare_spell(GameHudFixture* p,u32 start){return p->hud.prepare_spell(start);}
API(game_hud_reset) u32 game_hud_reset(GameHudFixture* p){return p->hud.reset_for_retry();}
API(game_hud_lifecycle_field) void* game_hud_lifecycle_field(GameHudFixture* p,u32 field){switch(field){case 0:return p->hud.bonus_digits.data();case 1:return p->hud.result_banners.data();case 2:return &p->hud.retry_intro;default:return nullptr;}}

API(game_hud_notification) u32 game_hud_notification(GameHudFixture* p,i32 value,i32 kind){return p->hud.notification(value,kind);}

API(anm_vm_overlay_panel) AnmVm* anm_vm_overlay_panel(AnmVm* p,u32 i){return p->geometry.overlay&&i<5?p->geometry.overlay->panels[i].get():nullptr;}
API(anm_vm_overlay_field) i32* anm_vm_overlay_field(AnmVm* p,u32 i){return p->geometry.overlay?(i==0?&p->geometry.overlay->mode:&p->geometry.overlay->frames):nullptr;}
API(anm_environment_continue_prompt) void anm_environment_continue_prompt(AnmEnvironment* p,u32 value){p->render_target_has_alpha=value;}
API(anm_renderer_submission_alpha) const u32* anm_renderer_submission_alpha(AnmRendererFixture* p,u32 draw){return p->graphics.submissions[draw].alpha.data();}

API(enemy_combat_regions) u32 enemy_combat_regions(EnemyCombatFixture* p,EnemyVariableFixture* e,AnmVm* vm,i32 incoming,i32* out){return p->combat.animation_region_damage(e->enemy,vm,incoming,*out);}
#include "../../cpp/game/StageDeformation.hpp"
struct StageDeformationFixture {ScreenGridViewport view;StageScriptState state;AnmManager& animations;StageDeformation effect;StageDeformationFixture(AnmManagerFixture& a,i32 bank):animations(a.manager),effect(a.manager,a.random,view,bank){}}
;
API(stage_deformation_create) StageDeformationFixture* stage_deformation_create(AnmManagerFixture* a,i32 bank){return new StageDeformationFixture(*a,bank);}
API(stage_deformation_delete) void stage_deformation_delete(StageDeformationFixture* p){delete p;}
API(stage_deformation_begin) u32 stage_deformation_begin(StageDeformationFixture* p,i32 mode){p->state.deformation_mode=mode;return p->effect.begin(mode);}
API(stage_deformation_field) void* stage_deformation_field(StageDeformationFixture* p,u32 kind){return kind==0?static_cast<void*>(&p->state.deformation_target):kind==1?static_cast<void*>(&p->view):kind==2?static_cast<void*>(p->effect.mesh.grid.vertices.data()):static_cast<void*>(p->effect.mesh.grid.sampling.data());}
API(stage_deformation_handle) u32 stage_deformation_handle(StageDeformationFixture* p,u32 index){return index==0?p->effect.mesh.capture_handle:index<=16?p->effect.mesh.strip_handles[index-1]:0;}
API(stage_deformation_update) u32 stage_deformation_update(StageDeformationFixture* p,u32 spell,u32 draw){return p->effect.update(p->state,spell)&&(!draw||p->effect.prepare_draw());}
API(stage_deformation_error) const char* stage_deformation_error(StageDeformationFixture* p){return p->effect.error.c_str();}

#include "../../cpp/game/TitleOptionsMenu.hpp"
struct TitleOptionsFixture final:TitleOptionsServices {
 TitleMenuFixture& title;TitleAudioSettings settings;TitleOptionsMenu menu;std::array<i32,3> volume_state{};u32 volume_calls=0;
 explicit TitleOptionsFixture(TitleMenuFixture& t):title(t),menu(t.state,settings,t.animations,*this){}
 bool sound(i32 id)override{title.sounds.push_back(id);return true;}
 bool volume(i32 music,i32 sound,i32 attenuation)override{volume_state={music,sound,attenuation};volume_calls++;return true;}
};
API(title_options_create) TitleOptionsFixture* title_options_create(TitleMenuFixture* t){return new TitleOptionsFixture(*t);}
API(title_options_delete) void title_options_delete(TitleOptionsFixture* f){delete f;}
API(title_options_configure) void title_options_configure(TitleOptionsFixture* f,i32 bgm,i32 se){f->title.state.screen=TitleScreen::Options;f->settings.music_volume=bgm;f->settings.sound_volume=se;}
API(title_options_update) i32 title_options_update(TitleOptionsFixture* f,u32 pressed,u32 repeated){f->title.sounds.clear();f->volume_calls=0;return f->menu.update(pressed,repeated);}
API(title_options_settings) TitleAudioSettings* title_options_settings(TitleOptionsFixture* f){return &f->settings;}
API(title_options_volume) const i32* title_options_volume(TitleOptionsFixture* f){return f->volume_state.data();}
API(title_options_volume_calls) u32 title_options_volume_calls(TitleOptionsFixture* f){return f->volume_calls;}
API(title_options_error) const char* title_options_error(TitleOptionsFixture* f){return f->menu.error.c_str();}

API(title_menu_push) void title_menu_push(TitleMenuFixture* f){f->state.menu.push();}

#include "../../cpp/game/TitleMusicRoom.hpp"
struct TitleMusicFixture final:TitleMusicServices {
 struct Text {u32 handle,color;std::string bytes;};TitleMenuFixture& title;MusicComments comments;TitleMusicRoom menu;std::vector<Text> texts;std::vector<std::string> events;
 explicit TitleMusicFixture(TitleMenuFixture& t):title(t),menu(t.state,t.animations,comments,*this){}
 bool text(AnmVm& vm,const DialogueText& t)override{texts.push_back({title.animations.registry.handle(vm),t.color,t.bytes});return true;}
 bool sound(i32 id)override{events.push_back("sound:"+std::to_string(id));return true;}
 bool music(const std::string& file)override{events.push_back("music:"+file);return true;}
 bool music_command(i32 id)override{events.push_back("command:"+std::to_string(id));return true;}bool start_title_music()override{events.push_back("command:0");return true;}
};
API(title_music_create) TitleMusicFixture* title_music_create(TitleMenuFixture* t){return new TitleMusicFixture(*t);}
API(title_music_delete) void title_music_delete(TitleMusicFixture* f){delete f;}
API(title_music_configure) i32 title_music_configure(TitleMusicFixture* f,const u8* p,u32 n,u32 mask,i32 alternate){f->title.state.screen=TitleScreen::MusicRoom;f->title.state.age.set(1);for(u32 i=0;i<20;i++)f->menu.unlocked[i]=(mask>>i)&1;f->menu.alternate_audio=alternate;return f->comments.open(p,n);}
API(title_music_update) i32 title_music_update(TitleMusicFixture* f,u32 pressed,u32 repeated){f->texts.clear();f->events.clear();return f->menu.update(pressed,repeated);}
API(title_music_value) i32 title_music_value(TitleMusicFixture* f,u32 which){switch(which){case 0:return f->menu.scroll;case 1:return f->menu.comment_line;case 2:return f->menu.comment_track;case 3:return f->menu.warning;case 4:return f->comments.count();default:{u32 mask=0;for(u32 i=0;i<20;i++)if(f->menu.unlocked[i])mask|=1u<<i;return mask;}}}
API(title_music_event_count) u32 title_music_event_count(TitleMusicFixture* f){return f->events.size();}
API(title_music_event) const char* title_music_event(TitleMusicFixture* f,u32 i){return f->events[i].c_str();}
API(title_music_text_count) u32 title_music_text_count(TitleMusicFixture* f){return f->texts.size();}
API(title_music_text_value) u32 title_music_text_value(TitleMusicFixture* f,u32 i,u32 which){return which?f->texts[i].color:f->texts[i].handle;}
API(title_music_text_bytes) const char* title_music_text_bytes(TitleMusicFixture* f,u32 i){return f->texts[i].bytes.c_str();}
API(title_music_catalog) const char* title_music_catalog(TitleMusicFixture* f,u32 i,u32 which){const auto* e=f->comments.entry(i);return !e?nullptr:which==0?e->file.c_str():which==1?e->title.c_str():e->comments[which-2].c_str();}
API(title_music_error) const char* title_music_error(TitleMusicFixture* f){return f->comments.error.empty()?f->menu.error.c_str():f->comments.error.c_str();}

API(title_menu_count) void title_menu_count(TitleMenuFixture* f,i32 count){f->state.menu.count=count;}

#include "../../cpp/game/TitleControllerMenu.hpp"
struct TitleControllerFixture final:TitleControllerServices {
 TitleMenuFixture& title;TitleControllerSettings settings,saved;TitleControllerMenu menu;u32 save_calls=0;
 explicit TitleControllerFixture(TitleMenuFixture& t):title(t),menu(t.state,t.animations,settings,*this){}
 bool sound(i32 id)override{title.sounds.push_back(id);return true;}
 bool save(const TitleControllerSettings& s)override{saved=s;save_calls++;return true;}
};
API(title_controller_create) TitleControllerFixture* title_controller_create(TitleMenuFixture* t){t->state.screen=TitleScreen::Controller;return new TitleControllerFixture(*t);}
API(title_controller_delete) void title_controller_delete(TitleControllerFixture* f){delete f;}
API(title_controller_field) void* title_controller_field(TitleControllerFixture* f,u32 which){switch(which){case 0:return &f->settings;case 1:return f->menu.pending.data();default:return &f->saved;}}
API(title_controller_update) i32 title_controller_update(TitleControllerFixture* f,u32 pressed,u32 repeated,u32 gamepad){f->title.sounds.clear();return f->menu.update(pressed,repeated,gamepad);}
API(title_controller_error) const char* title_controller_error(TitleControllerFixture* f){return f->menu.error.c_str();}

#include "../../cpp/game/Manual.hpp"
struct ManualFixture final:ManualServices {
 Manual panel;std::vector<i32> events;explicit ManualFixture(AnmManager& a):panel(a,*this){}
 bool sound(i32 id)override{events.insert(events.end(),{1,id});return true;}bool clear_page()override{events.push_back(2);return true;}
 bool request_page(i32 index)override{events.insert(events.end(),{3,index});return true;}bool upload_page(i32 index)override{events.insert(events.end(),{4,index});return true;}
 std::array<u32,14> values{};MenuCursorFixture projection;
};
API(manual_create) ManualFixture* manual_create(AnmManagerFixture* a){return new ManualFixture(a->manager);}
API(manual_delete) void manual_delete(ManualFixture* p){delete p;}
API(manual_ready) i32 manual_ready(ManualFixture* p){return p->panel.page_loaded();}
API(manual_update) i32 manual_update(ManualFixture* p,u32 pressed,u32 repeated,float rate){p->events.clear();return p->panel.update(pressed,repeated,rate);}
API(manual_values) const u32* manual_values(ManualFixture* p){auto& v=p->values;v[0]=p->panel.mode;v[1]=p->panel.phase;v[2]=p->panel.finished;v[3]=p->panel.page;v[4]=p->panel.menu.wrapping;std::copy(p->panel.choices.begin(),p->panel.choices.end(),v.begin()+5);return v.data();}
API(manual_age) Timer* manual_age(ManualFixture* p){return &p->panel.age;}
API(manual_cursor) const u8* manual_cursor(ManualFixture* p){p->projection.menu=p->panel.menu;return menu_cursor_bytes(&p->projection);}
API(manual_event_count) u32 manual_event_count(ManualFixture* p){return p->events.size();}
API(manual_events) const i32* manual_events(ManualFixture* p){return p->events.data();}
API(manual_error) const char* manual_error(ManualFixture* p){return p->panel.error.c_str();}

#include "../../cpp/game/TitleManualMenu.hpp"
struct TitleManualFixture {TitleManualMenu menu;TitleManualFixture(TitleMenuFixture& t,ManualFixture& m):menu(t.state,t.animations,m.panel){t.state.screen=TitleScreen::Manual;}};
API(title_manual_create) TitleManualFixture* title_manual_create(TitleMenuFixture* t,ManualFixture* m){return new TitleManualFixture(*t,*m);}
API(title_manual_delete) void title_manual_delete(TitleManualFixture* p){delete p;}
API(title_manual_update) i32 title_manual_update(TitleManualFixture* p){return p->menu.update();}
API(manual_finished) void manual_finished(ManualFixture* p,u32 value){p->panel.finished=value;}
API(title_manual_error) const char* title_manual_error(TitleManualFixture* p){return p->menu.error.c_str();}

#include "../../cpp/game/ReplayRecording.hpp"
struct ReplayRecordingFixture {ReplayRecording recording;ReplayExportDetails details;};
API(recording_create) ReplayRecordingFixture* recording_create(){return new ReplayRecordingFixture;}
API(recording_delete) void recording_delete(ReplayRecordingFixture* p){delete p;}
API(recording_metadata) u8* recording_metadata(ReplayRecordingFixture* p){return p->recording.description().data();}
API(recording_details) ReplayExportDetails* recording_details(ReplayRecordingFixture* p){return &p->details;}
API(recording_begin) i32 recording_begin(ReplayRecordingFixture* p,const u8* h,u32 n){return p->recording.begin(h,n);}
API(recording_activate) void recording_activate(ReplayRecordingFixture* p){p->recording.activate();}
API(recording_tick) i32 recording_tick(ReplayRecordingFixture* p,u32 held,u32 pressed,u32 released,float fps,u32 paused){return p->recording.tick({u16(held),u16(pressed),u16(released)},fps,paused);}
API(recording_finish) void recording_finish(ReplayRecordingFixture* p,i64 timestamp,i32 stage,u32 clear){p->recording.finish(timestamp,stage,clear);}
API(recording_write) i32 recording_write(ReplayRecordingFixture* p,const char* name,u32 terminate){return p->recording.write(name,p->details,terminate);}
API(recording_data) const u8* recording_data(ReplayRecordingFixture* p){return p->recording.output().data();}
API(recording_size) u32 recording_size(ReplayRecordingFixture* p){return p->recording.output().size();}
API(recording_clock) i32 recording_clock(ReplayRecordingFixture* p){return p->recording.frame_clock();}
API(recording_error) const char* recording_error(ReplayRecordingFixture* p){return p->recording.error().c_str();}

#include "../../cpp/game/GameInput.hpp"
API(game_input_create) GameInput* game_input_create(){return new GameInput;}
API(game_input_delete) void game_input_delete(GameInput* p){delete p;}
API(game_input_calculate) void game_input_calculate(GameInput* p){p->calculate();}
API(game_input_update) void game_input_update(GameInput* p,u32 held){p->update(held);}
API(game_input_recording) void game_input_recording(GameInput* p,u32 held,u32 focus){p->recording_update(u16(held),focus);}

#include "../../cpp/game/ReplayPlayback.hpp"
API(replay_playback_create) ReplayPlayback* replay_playback_create(Replay* file){return new ReplayPlayback(*file);}
API(replay_playback_delete) void replay_playback_delete(ReplayPlayback* p){delete p;}
API(replay_playback_select) i32 replay_playback_select(ReplayPlayback* p,u32 stage){return p->select(stage);}
API(replay_playback_activate) void replay_playback_activate(ReplayPlayback* p){p->activate();}
API(replay_playback_tick) i32 replay_playback_tick(ReplayPlayback* p,u32 active){return p->tick(active);}
API(replay_playback_input) GameInput* replay_playback_input(ReplayPlayback* p){return &p->input;}
API(replay_playback_clock) i32 replay_playback_clock(ReplayPlayback* p){return p->frame_clock();}
API(replay_playback_fps) u32 replay_playback_fps(ReplayPlayback* p){return p->fps;}
API(replay_playback_finished) u32 replay_playback_finished(ReplayPlayback* p){return p->finished;}
API(replay_playback_error) const char* replay_playback_error(ReplayPlayback* p){return p->error().c_str();}

#include "../../cpp/game/SessionGameplay.hpp"
struct SessionGameplayFixture final:SessionGameplayServices {
 StageGameplayFixture& stage;std::unique_ptr<SessionGameplay> driver;std::vector<i32> events;u32 input_flags=0;bool available=true;
 SessionGameplayFixture(StageGameplayFixture& stage,ReplayRecording* recording,ReplayPlayback* playback):stage(stage){if(playback){stage.progress.replay=true;stage.progress.transition=1;stage.scene->battle.session.replay_state=1;}driver=std::make_unique<SessionGameplay>(*stage.scene,stage.progress,*this,recording,playback);}
 bool event(i32 id){events.push_back(id);return available;}
 bool ending_fade()override{return event(1);}bool finish_replay()override{return event(2);}bool destination(SessionDestination d)override{events.insert(events.end(),{3,i32(d)});return available;}
 bool load_scene(bool& waiting)override{waiting=false;return event(4);}bool activate_scene()override{return event(5);}bool release_background()override{return event(6);}
 bool load_checkpoint_file(bool& restored)override{restored=false;return event(7);}bool restart_overlay(i32 script)override{events.insert(events.end(),{8,script});return available;}bool restart_effect(i32 label)override{events.insert(events.end(),{9,label});return available;}
 bool prepare_stage_music()override{return event(10);}bool start_stage_music()override{return event(11);}bool start_boss_music()override{return event(12);}bool seek_stage_music(double)override{return event(13);}
 bool demo_fade()override{return event(14);}bool update_overlays()override{return event(18);}
};
API(session_gameplay_create) SessionGameplayFixture* session_gameplay_create(StageGameplayFixture* s,ReplayRecordingFixture* r,ReplayPlayback* p){return new SessionGameplayFixture(*s,r?&r->recording:nullptr,p);}
API(session_gameplay_delete) void session_gameplay_delete(SessionGameplayFixture* p){delete p;}
API(session_gameplay_step) i32 session_gameplay_step(SessionGameplayFixture* p,u32 held,u32 pressed,float fps){p->events.clear();p->stage.scene->battle.player->life.invulnerability.set(100000);SessionGameplayInput input;input.scene.battle.held=held;input.scene.battle.pressed=pressed;input.scene.battle.focus_allowed=true;input.scene.background.flags=1;input.physical_pressed=pressed;input.fps=fps;return p->driver->step(input);}
API(session_gameplay_field) void* session_gameplay_field(SessionGameplayFixture* p,u32 field){switch(field){case 0:return &p->driver->runtime.age;case 1:return &p->driver->runtime.requested_chapter;case 2:return const_cast<GameInput*>(&p->driver->controls());case 3:return &p->stage.scene->battle.enemy_world.rank;case 4:return &p->stage.assets.environment.game_rng;case 5:return &p->stage.assets.random;case 6:return &p->input_flags;default:return &p->driver->runtime;}}
API(session_gameplay_event_count) u32 session_gameplay_event_count(SessionGameplayFixture* p){return p->events.size();}
API(session_gameplay_events) const i32* session_gameplay_events(SessionGameplayFixture* p){return p->events.data();}
API(session_gameplay_error) const char* session_gameplay_error(SessionGameplayFixture* p){return p->driver->error.empty()?p->stage.scene->error.c_str():p->driver->error.c_str();}

#include "../../cpp/game/TitleReplayMenu.hpp"
struct TitleReplayFixture final:TitleReplayServices,HudDrawServices,ReplayCalendarServices {
 TitleMenuFixture& title;ReplayCatalog catalog,pending;std::vector<i32> events;i32 saved_selection=0;bool immediate_ready=false;ReplayStartRequest started;TitleReplayMenu menu;MenuCursorFixture projection;
 explicit TitleReplayFixture(TitleMenuFixture& t):title(t),menu(t.state,t.progress,t.player,t.animations,catalog,*this,saved_selection){t.state.screen=TitleScreen::Replay;}
 bool request_catalog()override{events.push_back(1);catalog=pending;if(immediate_ready)menu.catalog_finished();return true;}
 bool release_catalog()override{for(u32 i=0;i<100;i++)if(catalog.entry(i)){events.push_back(2);break;}return true;}
 bool sound(i32 id)override{title.sounds.push_back(id);return true;}
 bool fade_music(float value)override{events.insert(events.end(),{3,i32(float_to_bits(value))});return true;}
 bool begin_transition(u32& handle)override{events.push_back(4);handle=0;return true;}
 bool transition_size(float width,float height)override{events.insert(events.end(),{5,i32(float_to_bits(width)),i32(float_to_bits(height))});return true;}
 bool start_replay(const ReplayStartRequest& request)override{started=request;events.push_back(6);return true;}
 std::vector<HudTextDraw> texts;ReplayCalendar date;i64 calendar_timestamp=0;
 bool hud_text(const HudTextDraw& value)override{texts.push_back(value);return true;}
 bool calendar(i64 timestamp,ReplayCalendar& value)override{calendar_timestamp=timestamp;value=date;return true;}
};
API(title_replay_create) TitleReplayFixture* title_replay_create(TitleMenuFixture* t){return new TitleReplayFixture(*t);}
API(title_replay_delete) void title_replay_delete(TitleReplayFixture* p){delete p;}
API(title_replay_configure) void title_replay_configure(TitleReplayFixture* p,i32 saved,u32 ready){p->saved_selection=saved;p->immediate_ready=ready;}
API(title_replay_file) i32 title_replay_file(TitleReplayFixture* p,u32 index,const char* name,const u8* data,u32 size){return p->pending.set(index,name,data,size);}
API(title_replay_ready) void title_replay_ready(TitleReplayFixture* p){p->menu.catalog_finished();}
API(title_replay_update) i32 title_replay_update(TitleReplayFixture* p,u32 pressed,u32 repeated){p->events.clear();p->title.sounds.clear();return p->menu.update(pressed,repeated);}
API(title_replay_value) i32 title_replay_value(TitleReplayFixture* p,u32 which){switch(which){case 0:return p->menu.pages.cursor;case 1:return p->menu.selected_replay;case 2:return p->menu.selected_stage;case 3:return p->saved_selection;default:{i32 count=0;for(u32 i=0;i<100;i++)count+=p->catalog.entry(i)!=nullptr;return count;}}}
API(title_replay_pages) const u8* title_replay_pages(TitleReplayFixture* p){p->projection.menu=p->menu.pages;return menu_cursor_bytes(&p->projection);}
API(title_replay_events) const i32* title_replay_events(TitleReplayFixture* p){return p->events.data();}
API(title_replay_event_count) u32 title_replay_event_count(TitleReplayFixture* p){return p->events.size();}
API(title_replay_filename) const char* title_replay_filename(TitleReplayFixture* p){return p->started.filename.c_str();}
API(title_replay_error) const char* title_replay_error(TitleReplayFixture* p){return p->menu.error.c_str();}
API(title_replay_draw) i32 title_replay_draw(TitleReplayFixture* p){p->texts.clear();return p->menu.draw(*p,*p);}
API(title_replay_date) ReplayCalendar* title_replay_date(TitleReplayFixture* p){return &p->date;}
API(title_replay_timestamp) i64 title_replay_timestamp(TitleReplayFixture* p){return p->calendar_timestamp;}
API(title_replay_text_count) u32 title_replay_text_count(TitleReplayFixture* p){return p->texts.size();}
API(title_replay_text_position) const Vec3* title_replay_text_position(TitleReplayFixture* p,u32 i){return &p->texts[i].position;}
API(title_replay_text_style) const HudTextStyle* title_replay_text_style(TitleReplayFixture* p,u32 i){return &p->texts[i].style;}
API(title_replay_text) const char* title_replay_text(TitleReplayFixture* p,u32 i){return p->texts[i].text.c_str();}

#include "../../cpp/game/ReplayStageSnapshot.hpp"
struct ReplaySnapshotFixture {
 SessionInitializationFixture model;PlayerMotion motion;Rng game,visual;std::string music;ReplayStageSnapshot snapshot;
 ReplayRunState state(){return {model.state,model.player,model.score,model.world,motion,music};}
};
API(replay_snapshot_create) ReplaySnapshotFixture* replay_snapshot_create(){return new ReplaySnapshotFixture;}
API(replay_snapshot_delete) void replay_snapshot_delete(ReplaySnapshotFixture* p){delete p;}
API(replay_snapshot_field) void* replay_snapshot_field(ReplaySnapshotFixture* p,u32 kind){switch(kind){case 0:return &p->model.state;case 1:return &p->model.player;case 2:return &p->model.score;case 3:return &p->model.world.rank;case 4:return &p->model.world.chapter_total;case 5:return &p->game;case 6:return &p->visual;case 7:return &p->motion.position_fixed;case 8:return &p->motion.position;case 9:return &p->motion.focus;case 10:return &p->model.state.starting_stage;case 11:return &p->motion.options;default:return &p->model.state.checkpoint_power;}}
API(replay_snapshot_music) const char* replay_snapshot_music(ReplaySnapshotFixture* p){return p->music.c_str();}
API(replay_snapshot_configure) void replay_snapshot_configure(ReplaySnapshotFixture* p,const char* music,bool first){p->music=music;p->model.state.new_run=first;}
API(replay_snapshot_begin) i32 replay_snapshot_begin(ReplaySnapshotFixture* p){return p->snapshot.create(p->state(),p->game);}
API(replay_snapshot_bytes) const u8* replay_snapshot_bytes(ReplaySnapshotFixture* p){return p->snapshot.bytes().data();}
API(replay_snapshot_open) i32 replay_snapshot_open(ReplaySnapshotFixture* p,const u8* data,u32 size){return p->snapshot.read(data,size);}
API(replay_snapshot_refresh) i32 replay_snapshot_refresh(ReplaySnapshotFixture* p){return p->snapshot.refresh(p->state());}
API(replay_snapshot_restore) i32 replay_snapshot_restore(ReplaySnapshotFixture* p,bool visual){auto state=p->state();return p->snapshot.restore_progress(state)&&p->snapshot.restore_seed(p->game,visual?&p->visual:nullptr);}
API(replay_snapshot_player) i32 replay_snapshot_player(ReplaySnapshotFixture* p){return p->snapshot.restore_player(p->motion);}
API(replay_snapshot_error) const char* replay_snapshot_error(ReplaySnapshotFixture* p){return p->snapshot.error.c_str();}
API(replay_snapshot_option_snap) i32 replay_snapshot_option_snap(ReplaySnapshotFixture* p,u32 i){return p->motion.options[i].snap;}

#include "../../cpp/game/SessionReplay.hpp"
struct StageSessionReplayFixture {
 StageGameplayFixture& stage;GameConfig config;SessionReplay replay;std::unique_ptr<SessionGameplayFixture> session;std::string error;
 explicit StageSessionReplayFixture(StageGameplayFixture& s):stage(s),replay(config,s.assets.environment.game_rng,s.assets.random){}
 ReplayRunState state(){auto& s=*stage.scene;return {stage.progress,s.battle.session,s.battle.score,s.battle.enemy_world,s.battle.player->motion,s.music.current_music_wave};}
 bool enter(const u8* source,u32 size){auto state=this->state();if(!(source?replay.open(source,size,stage.progress.stage,state):replay.initialize_live(state)))return false;if(!stage.scene->reset_for_entry()||!replay.activate_stage(state))return false;if(source&&!stage.scene->battle.player->configure_options())return false;if(!stage.scene->start_for_entry(0))return false;session=std::make_unique<SessionGameplayFixture>(stage,replay.live(),replay.replay());session->driver->auto_focus=config.auto_focus();return true;}
};
API(stage_session_replay_create) StageSessionReplayFixture* stage_session_replay_create(StageGameplayFixture* p){return new StageSessionReplayFixture(*p);}
API(stage_session_replay_delete) void stage_session_replay_delete(StageSessionReplayFixture* p){delete p;}
API(stage_session_replay_enter) i32 stage_session_replay_enter(StageSessionReplayFixture* p,const u8* bytes,u32 size){return p->enter(bytes,size);}
API(stage_session_replay_session) SessionGameplayFixture* stage_session_replay_session(StageSessionReplayFixture* p){return p->session.get();}
API(stage_session_replay_recording) ReplayRecording* stage_session_replay_recording(StageSessionReplayFixture* p){return p->replay.live();}
API(stage_session_replay_clock) i32 stage_session_replay_clock(StageSessionReplayFixture* p){return p->replay.live()?p->replay.live()->frame_clock():p->replay.replay()->frame_clock();}
API(stage_session_replay_header) const u8* stage_session_replay_header(StageSessionReplayFixture* p,u32 n){const auto* saved=p->replay.snapshot(n);return saved?saved->bytes().data():nullptr;}
API(stage_session_replay_config) u8* stage_session_replay_config(StageSessionReplayFixture* p){return p->config.bytes.data();}
API(stage_session_replay_timing) i32 stage_session_replay_timing(StageSessionReplayFixture* p,i32 sequence,i32* encoded){return encoded&&p->replay.spell_timing(sequence,*encoded);}
API(stage_session_replay_export) i32 stage_session_replay_export(StageSessionReplayFixture* p,const char* name){auto* r=p->replay.live();if(!r)return false;ReplayExportDetails details;details.score=p->stage.scene->battle.score.score;r->finish(1700000000,p->stage.progress.stage,false);return r->write(name,details,true);}
API(stage_session_replay_data) const u8* stage_session_replay_data(StageSessionReplayFixture* p){return p->replay.live()->output().data();}
API(stage_session_replay_size) u32 stage_session_replay_size(StageSessionReplayFixture* p){return p->replay.live()->output().size();}
API(stage_session_replay_finished) bool stage_session_replay_finished(StageSessionReplayFixture* p){return p->replay.replay()&&p->replay.replay()->finished;}
API(stage_session_replay_error) const char* stage_session_replay_error(StageSessionReplayFixture* p){return p->replay.error.empty()?p->stage.scene->error.c_str():p->replay.error.c_str();}

API(anm_manager_unload) void anm_manager_unload(AnmManagerFixture* p,i32 id){p->manager.unload(id);}
API(anm_manager_collect_resources) void anm_manager_collect_resources(AnmManagerFixture* p){p->manager.collect_resources();}
API(anm_manager_resource_count) u32 anm_manager_resource_count(AnmManagerFixture* p){return p->manager.resource_count();}
API(anm_manager_retired_resources) u32 anm_manager_retired_resources(AnmManagerFixture* p){return p->manager.retired_resource_count();}
struct SharedStageAssetsFixture {
 StageAssetSource source;Rng random;AnmEnvironment environment;AnmManager animations{random,environment};std::array<std::unique_ptr<StageAssets>,2> scenes;u32 released=0;
 SharedStageAssetsFixture(){animations.resource_release=[this](const AnmResource&){released++;};}
};
API(shared_stage_assets_create) SharedStageAssetsFixture* shared_stage_assets_create(){return new SharedStageAssetsFixture;}
API(shared_stage_assets_delete) void shared_stage_assets_delete(SharedStageAssetsFixture* p){delete p;}
API(shared_stage_assets_file) void shared_stage_assets_file(SharedStageAssetsFixture* p,const char* name,const u8* bytes,u32 size){p->source.files[name]={bytes,size};}
API(shared_stage_assets_load) i32 shared_stage_assets_load(SharedStageAssetsFixture* p,u32 slot,u32 stage,i32 character){if(slot>=2||p->scenes[slot])return false;p->scenes[slot]=std::make_unique<StageAssets>(p->source,p->animations);return p->scenes[slot]->load(stage,character);}
API(shared_stage_assets_unload) void shared_stage_assets_unload(SharedStageAssetsFixture* p,u32 slot){p->scenes[slot].reset();p->animations.collect_resources();}
API(shared_stage_assets_resource) i32 shared_stage_assets_resource(SharedStageAssetsFixture* p,u32 slot,u32 which){const auto& a=*p->scenes[slot];const i32 banks[]={a.ascii,a.text,a.front,a.bullet,a.effect,a.player,a.logo,a.scenery};return which<8?banks[which]:a.enemy_banks[which-8];}
API(shared_stage_assets_value) u32 shared_stage_assets_value(SharedStageAssetsFixture* p,u32 which){return which==0?p->animations.resource_count():which==1?p->animations.retired_resource_count():which==2?p->released:p->animations.registry.count();}
API(shared_stage_assets_animation_count) u32 shared_stage_assets_animation_count(SharedStageAssetsFixture* p,u32 slot){return p->scenes[slot]->animation_names.size();}
API(shared_stage_assets_spawn) u32 shared_stage_assets_spawn(SharedStageAssetsFixture* p,u32 slot,i32 script){return p->animations.create(p->scenes[slot]->front,script);}
API(shared_stage_assets_find) AnmVm* shared_stage_assets_find(SharedStageAssetsFixture* p,u32 handle){return p->animations.registry.find(handle);}
API(shared_stage_assets_update) bool shared_stage_assets_update(SharedStageAssetsFixture* p){return p->animations.update(false)&&p->animations.update(true);}
API(shared_stage_assets_error) const char* shared_stage_assets_error(SharedStageAssetsFixture* p,u32 slot){return p->scenes[slot]&&!p->scenes[slot]->error.empty()?p->scenes[slot]->error.c_str():p->animations.error.c_str();}

#include "../../cpp/game/SessionTeardown.hpp"
struct SessionTeardownFixture final:SessionTeardownServices {
 SessionState progress;PlayerLifeSession player;SessionExitPresentation presentation;SessionTeardown exit{progress,player,presentation,*this};
 u32 owned=0;std::array<u32,3> callback_flags{};std::vector<i32> events;
 bool event(i32 id){events.push_back(id);return true;}
 bool owns(SessionObject kind)const noexcept override{return owned&(1u<<u32(kind));}
 bool stop_checkpoint_worker()override{return event(1);}
 bool save_records()override{return event(2);}
 bool clear_session_links()override{return true;}
 bool reset_transition()override{return event(3);}
 bool release(SessionObject kind)override{owned&=~(1u<<u32(kind));return event(100+i32(kind));}
 bool clear_checkpoint_data()override{return event(4);}
 bool reset_gui_for_stage()override{return event(5);}
 bool carry_background()override{if(owns(SessionObject::Background))owned|=1u<<u32(SessionObject::PreviousBackground);else owned&=~(1u<<u32(SessionObject::PreviousBackground));return true;}
 bool disable_callbacks(SessionObject kind)override{const u32 i=kind==SessionObject::PauseMenu?0:kind==SessionObject::Bullets?1:2;callback_flags[i]&=~2u;return true;}
 bool reset_items()override{return event(6);}
 bool clear_enemies()override{return event(7);}
 bool retire_stage_animations()override{return event(8)&&event(9);}
 bool detach_scene_callbacks()override{return event(10)&&event(11);}
 bool queue_music(i32 mode)override{return event(20+mode);}
 bool reset_audio_slots()override{return event(12);}
};
API(session_teardown_create) SessionTeardownFixture* session_teardown_create(){return new SessionTeardownFixture;}
API(session_teardown_delete) void session_teardown_delete(SessionTeardownFixture* p){delete p;}
API(session_teardown_configure) void session_teardown_configure(SessionTeardownFixture* p,i32 stage,i32 starting,i32 continues,u32 flags,u32 owners,u32 callbacks){p->progress.stage=stage;p->progress.starting_stage=starting;p->progress.continues=continues;p->progress.rate=0.25f;p->player.mode_flags=flags;p->owned=owners;p->callback_flags.fill(callbacks);p->events.clear();p->exit.error.clear();p->presentation={};}
API(session_teardown_run) i32 session_teardown_run(SessionTeardownFixture* p,i32 destination,bool restart_audio){return p->exit.finish(destination,restart_audio);}
API(session_teardown_state) const SessionState* session_teardown_state(SessionTeardownFixture* p){return &p->progress;}
API(session_teardown_flags) u32 session_teardown_flags(SessionTeardownFixture* p){return p->player.mode_flags;}
API(session_teardown_owned) u32 session_teardown_owned(SessionTeardownFixture* p){return p->owned;}
API(session_teardown_callback_flags) u32 session_teardown_callback_flags(SessionTeardownFixture* p,u32 i){return p->callback_flags[i];}
API(session_teardown_presentation) const SessionExitPresentation* session_teardown_presentation(SessionTeardownFixture* p){return &p->presentation;}
API(session_teardown_events) const i32* session_teardown_events(SessionTeardownFixture* p){return p->events.data();}
API(session_teardown_event_count) u32 session_teardown_event_count(SessionTeardownFixture* p){return p->events.size();}
API(session_teardown_error) const char* session_teardown_error(SessionTeardownFixture* p){return p->exit.error.c_str();}

struct ReplayLifecycleFixture {
 ReplaySnapshotFixture model;GameConfig config;SessionReplay replay{config,model.game,model.visual};
};
API(replay_lifecycle_create) ReplayLifecycleFixture* replay_lifecycle_create(){return new ReplayLifecycleFixture;}
API(replay_lifecycle_delete) void replay_lifecycle_delete(ReplayLifecycleFixture* p){delete p;}
API(replay_lifecycle_model) ReplaySnapshotFixture* replay_lifecycle_model(ReplayLifecycleFixture* p){return &p->model;}
API(replay_lifecycle_config) u8* replay_lifecycle_config(ReplayLifecycleFixture* p){return p->config.bytes.data();}
API(replay_lifecycle_begin) i32 replay_lifecycle_begin(ReplayLifecycleFixture* p){return p->replay.initialize_live(p->model.state());}
API(replay_lifecycle_prepare) i32 replay_lifecycle_prepare(ReplayLifecycleFixture* p){auto state=p->model.state();return p->replay.prepare_stage(state);}
API(replay_lifecycle_activate) i32 replay_lifecycle_activate(ReplayLifecycleFixture* p){auto state=p->model.state();return p->replay.activate_stage(state);}
API(replay_lifecycle_header) const u8* replay_lifecycle_header(ReplayLifecycleFixture* p,u32 stage){auto* s=p->replay.snapshot(stage);return s?s->bytes().data():nullptr;}
API(replay_lifecycle_description) const u8* replay_lifecycle_description(ReplayLifecycleFixture* p){auto* r=p->replay.live();return r?r->description().data():nullptr;}
API(replay_lifecycle_clock) i32 replay_lifecycle_clock(ReplayLifecycleFixture* p){auto* r=p->replay.live();return r?r->frame_clock():-2;}
API(replay_lifecycle_error) const char* replay_lifecycle_error(ReplayLifecycleFixture* p){return p->replay.error.c_str();}

API(replay_lifecycle_tick) i32 replay_lifecycle_tick(ReplayLifecycleFixture* p,u32 held,u32 pressed,u32 released,float fps,bool paused){auto* r=p->replay.live();return r&&r->tick({u16(held),u16(pressed),u16(released)},fps,paused);}
API(replay_lifecycle_input_count) u32 replay_lifecycle_input_count(ReplayLifecycleFixture* p,u32 stage){auto* r=p->replay.live();auto* s=r?r->stage(stage):nullptr;return s?s->inputs.size():0;}
API(replay_lifecycle_inputs) const u8* replay_lifecycle_inputs(ReplayLifecycleFixture* p,u32 stage){auto* r=p->replay.live();auto* s=r?r->stage(stage):nullptr;return s?reinterpret_cast<const u8*>(s->inputs.data()):nullptr;}

#include "../../cpp/game/RunGameplay.hpp"
struct RunGameplayFixture:StageGameplayFixture {
 RunGameplay run;
 explicit RunGameplayFixture(StageAssetFixture& a,FrameScheduler* scheduler=nullptr,const std::unordered_map<std::string,i32>* shared=nullptr):StageGameplayFixture(a,false),run(a.source,a.animations,a.environment,a.environment.game_rng,a.random,progress,*this,scheduler,shared){}
};
API(run_gameplay_create) RunGameplayFixture* run_gameplay_create(StageAssetFixture* assets){return new RunGameplayFixture(*assets);}
API(run_gameplay_delete) void run_gameplay_delete(RunGameplayFixture* p){delete p;}
API(run_gameplay_load) i32 run_gameplay_load(RunGameplayFixture* p,u32 stage,i32 character){p->progress.difficulty=stage==7?4:1;StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};return p->run.load(stage,character,camera,&p->requested_chapter);}
API(run_gameplay_next) i32 run_gameplay_next(RunGameplayFixture* p,u32 stage){StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};return p->run.next(stage,camera,&p->requested_chapter);}
API(run_gameplay_release_previous) void run_gameplay_release_previous(RunGameplayFixture* p){p->run.release_previous();}
API(run_gameplay_begin) i32 run_gameplay_begin(RunGameplayFixture* p){return p->run.scene()->begin(0);}
API(run_gameplay_step) i32 run_gameplay_step(RunGameplayFixture* p,u32 held,u32 pressed,u32 shoot_frames,u32 skip_frames){StageGameplayFrame frame;frame.battle.held=held;frame.battle.pressed=pressed;frame.battle.focus_allowed=true;frame.dialogue.held=held;frame.dialogue.pressed=pressed;frame.dialogue.shoot_frames=shoot_frames;frame.dialogue.skip_frames=skip_frames;frame.background.flags=1;p->run.scene()->battle.player->life.invulnerability.set(100000);return p->run.scene()->step(frame);}
API(run_gameplay_error) const char* run_gameplay_error(RunGameplayFixture* p){if(!p->run.error.empty())return p->run.error.c_str();auto* s=p->run.scene();return s?(s->error.empty()?s->battle.error.c_str():s->error.c_str()):"Run scene unavailable";}
API(run_gameplay_field) void* run_gameplay_field(RunGameplayFixture* p,u32 field){auto& s=*p->run.scene();switch(field){case 0:return &p->progress;case 1:return &s.battle.session;case 2:return &s.battle.score;case 3:return &s.battle.player->motion.position;case 4:return &s.background.script.state;case 5:return &s.hud.boss;case 6:return s.popups.entries.data();case 7:return &s.reward.state;default:return &p->requested_chapter;}}
API(run_gameplay_identity) void* run_gameplay_identity(RunGameplayFixture* p,u32 field){auto& s=*p->run.scene();switch(field){case 0:return &s.battle;case 1:return s.battle.player.get();case 2:return s.battle.bullet_scene.get();case 3:return s.battle.items.get();case 4:return s.battle.laser_scene.get();case 5:return &s.hud;case 6:return &s.popups;case 7:return &s.background;case 8:return s.battle.enemies.get();case 9:return s.battle.bomb.get();case 10:return &s.battle.spell_card;default:return p->run.previous_scene();}}
API(run_session_hud_score) const i32* run_session_hud_score(RunGameplayFixture* p){return &p->run.scene()->hud.displayed_score;}
API(run_gameplay_capture) i32 run_gameplay_capture(RunGameplayFixture* p,i32 chapter){return p->run.scene()->capture(chapter);}
API(run_gameplay_restore) i32 run_gameplay_restore(RunGameplayFixture* p){return p->run.scene()->restore();}
API(run_gameplay_counts) void run_gameplay_counts(RunGameplayFixture* p,u32* out){auto& s=*p->run.scene();out[0]=s.battle.enemies->count();out[1]=s.battle.bullet_scene->manager.visible_count;out[2]=s.battle.laser_scene->manager.count();out[3]=s.battle.items->active_count;out[4]=s.messages.active();out[5]=s.messages.finished();out[6]=p->completions;out[7]=p->music_requests;out[8]=p->text_requests;out[9]=s.background.frames;out[10]=p->requested_chapter;out[11]=s.checkpoint.ready();}
API(run_gameplay_close_spell) i32 run_gameplay_close_spell(RunGameplayFixture* p){auto& s=*p->run.scene();return s.battle.spell_card.abort()&&s.battle.bomb->stop();}
API(run_gameplay_resource_value) u32 run_gameplay_resource_value(RunGameplayFixture* p,u32 field){switch(field){case 0:return p->assets.animations.resource_count();case 1:return p->assets.animations.retired_resource_count();default:return p->assets.animations.registry.count();}}

#include "../../cpp/game/RunSession.hpp"
struct RunSessionFixture:RunGameplayFixture,SessionGameplayServices,SessionEntryServices {
 std::vector<std::unique_ptr<ScreenMotionFrame>> world_motion;bool whole_world=false;
 struct WorldObserver {FrameCallback frame;RunSessionFixture* owner=nullptr;u32 index=0;};
 static constexpr std::array<i32,17> observer_priorities{9,10,15,16,17,21,23,25,26,27,28,29,31,32,33,34,35};
 std::array<WorldObserver,17> observers;std::array<std::array<u32,3>,17> world_trace{};bool observing=false;
 bool trace_world(){
  if(observing)return true;if(!run.scene())return false;
  for(u32 i=0;i<observers.size();i++){auto& o=observers[i];o.owner=this;o.index=i;o.frame.owner=&o;o.frame.enabled=true;o.frame.run=[](void* p)->i32{auto& entry=*static_cast<WorldObserver*>(p);auto& fixture=*entry.owner;fixture.world_trace[entry.index]={u32(observer_priorities[entry.index]),u32(fixture.assets.random.seed),fixture.assets.random.calls};return 1;};if(run.scene()->frame_scheduler().add(o.frame,FramePass::Update,observer_priorities[i])<0)return false;}observing=true;return true;
 }
 ~RunSessionFixture(){if(run.scene())for(auto& o:observers)run.scene()->frame_scheduler().remove(o.frame);}

 bool shake(const ScreenShakeSpec& spec)override{if(!whole_world)return StageGameplayFixture::shake(spec);world_motion.push_back(std::make_unique<ScreenMotionFrame>(run.scene()->frame_scheduler(),assets.environment.game_rng,spec,[this](){return ScreenShakeContext{false,run.scene()!=nullptr,progress.scene_flags,progress.rate,assets.environment.resolution_scale};},[](Vec2,Vec2){}));return true;}
 bool nudge(const ScreenNudgeSpec& spec)override{if(!whole_world)return StageGameplayFixture::nudge(spec);world_motion.push_back(std::make_unique<ScreenMotionFrame>(run.scene()->frame_scheduler(),assets.random,spec,[this](){return ScreenShakeContext{false,run.scene()!=nullptr,progress.scene_flags,progress.rate,assets.environment.resolution_scale};},[](Vec2,Vec2){}));return true;}
 GameConfig config;SessionReplay replay;RunSession session;u32 captures=0,transition_banners=0,entrances=0;std::string session_error;
 explicit RunSessionFixture(StageAssetFixture& a):RunGameplayFixture(a),replay(config,a.environment.game_rng,a.random),session(run,progress,replay,*this,*this){}
 ReplayRunState state(){auto& s=*run.scene();return {progress,s.battle.session,s.battle.score,s.battle.enemy_world,s.battle.player->motion,s.music.current_music_wave};}
 bool initial(u32 stage,i32 character,const u8* bytes,u32 size){progress.difficulty=stage==7?4:1;if(bytes){Replay metadata;if(!metadata.open(bytes,size))return false;progress.difficulty=metadata.difficulty();}progress.replay=bytes!=nullptr;if(!run.construct(stage,character,&requested_chapter))return false;auto& s=*run.scene();s.battle.session.power=400;s.battle.score.point_value=1000000;s.battle.score.max_point_value=50000000;if(!s.initialize_player()||!s.initialize_popups())return false;auto snapshot=state();if(!(bytes?replay.open(bytes,size,stage,snapshot):replay.initialize_live(snapshot)))return false;StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};if(!session.attach(camera,0))return false;session.driver()->auto_focus=config.auto_focus();return true;}
 bool next(u32 stage,bool clear_wait){session.detach();auto& old=*run.scene();if(!old.battle.spell_card.abort()||!old.battle.bomb->stop())return false;if(clear_wait){old.hud.flags|=0x100;old.hud.intro_age.set(0);}progress.stage=stage;progress.chapter_deaths=0;progress.scene_flags=0;if(!run.construct_next(stage,&requested_chapter))return false;auto& s=*run.scene();if(!s.initialize_player()||!s.initialize_popups())return false;StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};if(!session.attach(camera,12,true))return false;session.driver()->auto_focus=config.auto_focus();return true;}
 bool ending_fade()override{return available;}bool finish_replay()override{if(whole_world)progress.scene_flags|=0x10;return available;}bool destination(SessionDestination)override{return available;}
 bool load_scene(bool&)override{return false;}bool activate_scene()override{return false;}bool release_background()override{return available;}
 bool load_checkpoint_file(bool& restored)override{restored=false;return available;}bool restart_overlay(i32)override{return available;}bool restart_effect(i32)override{return available;}
 bool prepare_stage_music()override{return available;}bool start_stage_music()override{return available;}bool start_boss_music()override{return available;}bool seek_stage_music(double)override{return available;}
 bool demo_fade()override{return available;}bool update_overlays()override{return available;}
 bool discard_stage_assets()override{return available;}bool return_to_title(bool)override{return available;}
 bool capture_previous_background(StageGameplay&)override{captures++;return available;}bool prepare_background_transition()override{return available;}bool transition_banner()override{transition_banners++;return available;}
 bool interrupt_entrance()override{entrances++;return available;}bool retire_restart_overlay()override{return available;}bool interrupt_resume_overlay()override{return available;}
 bool restore_pending_progress()override{return available;}bool queue_music(i32)override{return available;}bool unlock_current_music()override{return available;}
};
API(run_session_create) RunSessionFixture* run_session_create(StageAssetFixture* assets){return new RunSessionFixture(*assets);}
API(run_session_delete) void run_session_delete(RunSessionFixture* p){delete p;}
API(run_session_load) i32 run_session_load(RunSessionFixture* p,u32 stage,i32 character,const u8* bytes,u32 size){return p->initial(stage,character,bytes,size);}
API(run_session_next) i32 run_session_next(RunSessionFixture* p,u32 stage,bool clear_wait){return p->next(stage,clear_wait);}
API(run_session_step) i32 run_session_step(RunSessionFixture* p,u32 held,u32 pressed,float fps,bool background_finished){SessionGameplayInput input;input.scene.battle.held=held;input.scene.battle.pressed=pressed;input.scene.battle.focus_allowed=true;input.scene.background.flags=1;input.fps=fps;input.physical_pressed=pressed;input.background_finished=background_finished;p->run.scene()->battle.player->life.invulnerability.set(100000);return p->session.step(input);}
// Whole-world comparison uses natural player life and original replay input.
API(run_session_step_world) i32 run_session_step_world(RunSessionFixture* p){p->whole_world=true;SessionGameplayInput input;input.scene.background.flags=1;input.fps=60;return p->session.step(input);}
API(run_session_step_world_input) i32 run_session_step_world_input(RunSessionFixture* p,u32 held,bool protection){p->whole_world=true;SessionGameplayInput input;input.scene.battle.held=held;input.scene.background.flags=1;input.fps=60;if(protection)p->run.scene()->battle.player->life.invulnerability.set(100000);return p->session.step(input);}
API(run_session_world_item) const ItemState* run_session_world_item(RunSessionFixture* p,u32 id){auto* s=p->run.scene()->battle.items->find(id);return s?&s->state:nullptr;}
API(run_session_world_trace_enable) u32 run_session_world_trace_enable(RunSessionFixture* p){return p->trace_world();}
API(run_session_world_trace) const u32* run_session_world_trace(RunSessionFixture* p){return p->world_trace[0].data();}
API(run_session_world_state) void run_session_world_state(RunSessionFixture* p,u32* out){auto& s=*p->run.scene();const auto& random=p->assets.environment.game_rng;out[0]=p->progress.stage_frame;out[1]=p->session.driver()->controls().held;out[2]=float_to_bits(s.battle.player->motion.position.x);out[3]=float_to_bits(s.battle.player->motion.position.y);out[4]=s.battle.player->life.state;out[5]=s.battle.score.score;out[6]=s.battle.session.deaths;out[7]=s.battle.session.extra_lives;out[8]=s.battle.session.power;out[9]=s.battle.session.bombs;out[10]=random.seed;out[11]=random.calls;out[12]=s.battle.enemies->count();out[13]=s.battle.bullet_scene->manager.visible_count;out[14]=s.battle.enemy_world.rank;out[15]=p->progress.chapter;}
API(run_session_world_enemy_context) EclContext* run_session_world_enemy_context(RunSessionFixture* p,u32 id){auto* e=p->run.scene()->battle.enemies->find(id);return e?&e->scripts.main:nullptr;}
API(run_session_world_enemy_flags) u32 run_session_world_enemy_flags(RunSessionFixture* p,u32 id){auto* e=p->run.scene()->battle.enemy_world.find(id);return e?e->flags:0;}
API(run_session_world_enemy_motion) const MotionState* run_session_world_enemy_motion(RunSessionFixture* p,u32 id,u32 field){auto* e=p->run.scene()->battle.enemy_world.find(id);return !e?nullptr:field==1?&e->absolute:field==2?&e->relative:&e->motion;}
API(run_session_world_animation) AnmVm* run_session_world_animation(RunSessionFixture* p,u32 handle){return p->assets.animations.registry.find(handle);}
API(run_session_world_player_field) void* run_session_world_player_field(RunSessionFixture* p,u32 field){auto& v=*p->run.scene()->battle.player;switch(field){case 0:return v.damage.sources.data();case 1:return v.shots.shots.data();case 2:return &v.life.age;case 3:return &v.life.invulnerability;case 4:return &v.motion.external_velocity;case 5:return &v.frame.shooting.shot;case 6:return &v.frame.shooting.continuous;case 7:return v.shot_context.laser_power.data();case 8:return &v.damage.score;case 9:return &v.motion.position_fixed;case 10:return &p->assets.random;default:return nullptr;}}
API(run_session_error) const char* run_session_error(RunSessionFixture* p){if(!p->session.error.empty())return p->session.error.c_str();if(!p->replay.error.empty())return p->replay.error.c_str();return run_gameplay_error(p);}
API(run_session_replay_decoded) const u8* run_session_replay_decoded(RunSessionFixture* p){return p->replay.replay_file()?p->replay.replay_file()->decoded().data():nullptr;}
API(run_session_clock) i32 run_session_clock(RunSessionFixture* p){return p->replay.live()?p->replay.live()->frame_clock():p->replay.replay()->frame_clock();}

// Oracle stage driver uses the same completion controller as RunStageFlow.
// The older component fixture deliberately counts completion without binding
// it, so it cannot prove a terminal clear bonus or next-stage request.
#include "../../cpp/game/RunCompletion.hpp"
struct ReplayOracleCompletion final:CompletionRecords,RunCompletionServices {
 RunSessionFixture& owner;std::unique_ptr<RunCompletion> completion;
 bool next_requested=false;i32 next_stage=0;
 explicit ReplayOracleCompletion(RunSessionFixture& p):owner(p){
  owner.progress.transition=owner.progress.replay?1:0;
  completion=std::make_unique<RunCompletion>(*owner.run.scene(),owner.progress,owner.session.driver()->runtime,*this,*this);
  owner.run.scene()->stage_completion=[this](){return completion->complete();};
 }
 ~ReplayOracleCompletion(){if(owner.run.scene())owner.run.scene()->stage_completion={};}
 bool stage_clear(i32,i32,i32)override{return true;}
 bool finished_run(i32,bool,i32,bool)override{return true;}
 bool spell_score(i32,bool,i32,i32)override{return true;}
 bool remove_checkpoint(i32,i32)override{return true;}
 bool prepare_ending(StageGameplay&)override{return true;}
 bool finish_replay()override{return owner.finish_replay();}
 bool finish_practice()override{return false;}
 bool queue_next_stage()override{if(next_requested)return false;next_requested=true;return true;}
 bool prepare_next_stage(i32 stage)override{next_stage=stage;return stage_definition(stage)!=nullptr;}
};
API(run_session_oracle_completion_create) ReplayOracleCompletion* run_session_oracle_completion_create(RunSessionFixture* p){if(!p->run.scene()||!p->session.driver())return nullptr;return new ReplayOracleCompletion(*p);}
API(run_session_oracle_completion_delete) void run_session_oracle_completion_delete(ReplayOracleCompletion* p){delete p;}
API(run_session_oracle_completed) i32 run_session_oracle_completed(ReplayOracleCompletion* p){return p->next_requested||((p->owner.progress.scene_flags&0x4000)!=0);}
API(run_session_oracle_next_stage) i32 run_session_oracle_next_stage(ReplayOracleCompletion* p){return p->next_stage;}
API(run_session_oracle_completion_error) const char* run_session_oracle_completion_error(ReplayOracleCompletion* p){return p->completion->error.c_str();}
API(run_session_controls) const GameInput* run_session_controls(RunSessionFixture* p){return &p->session.driver()->controls();}
API(run_session_age) const Timer* run_session_age(RunSessionFixture* p){return &p->session.driver()->runtime.age;}
API(run_session_counters) u32 run_session_counters(RunSessionFixture* p,u32 i){return i==0?p->captures:i==1?p->transition_banners:p->entrances;}
API(run_session_header) const u8* run_session_header(RunSessionFixture* p,u32 stage){auto* s=p->replay.snapshot(stage);return s?s->bytes().data():nullptr;}
API(run_session_config) u8* run_session_config(RunSessionFixture* p){return p->config.bytes.data();}
API(run_session_export) i32 run_session_export(RunSessionFixture* p,const char* name){auto* r=p->replay.live();if(!r)return false;ReplayExportDetails d;d.score=p->run.scene()->battle.score.score;r->finish(1700000000,p->progress.stage,false);return r->write(name,d,true);}
API(run_session_data) const u8* run_session_data(RunSessionFixture* p){return p->replay.live()->output().data();}
API(run_session_size) u32 run_session_size(RunSessionFixture* p){return p->replay.live()->output().size();}

API(run_session_input_active) bool run_session_input_active(RunSessionFixture* p){return p->session.driver()->input_active();}

#include "../../cpp/game/ScoreFile.hpp"
#include "../../cpp/game/Lzss.hpp"
struct ScoreFileFixture {ScoreFile score;Rng random;std::vector<u8> output;};
API(score_file_create) ScoreFileFixture* score_file_create(){return new ScoreFileFixture;}
API(score_file_delete) void score_file_delete(ScoreFileFixture* p){delete p;}
API(score_file_reset) void score_file_reset(ScoreFileFixture* p,u32 seed,u32 calls){p->random.seed=u16(seed);p->random.reserved=u16(seed>>16);p->random.calls=calls;p->score.reset(p->random);}
API(score_file_field) void* score_file_field(ScoreFileFixture* p,i32 kind){if(kind==0)return p->score.characters.data();if(kind==1)return p->score.settings.data();if(kind==2)return &p->random;return const_cast<u8*>(p->score.file_header().data());}
API(score_file_open) bool score_file_open(ScoreFileFixture* p,const u8* bytes,u32 size){return p->score.open(bytes,size);}
API(score_file_save) bool score_file_save(ScoreFileFixture* p){return p->score.save(p->output);}
API(score_file_data) const u8* score_file_data(ScoreFileFixture* p){return p->output.data();}
API(score_file_size) u32 score_file_size(ScoreFileFixture* p){return p->output.size();}
API(score_file_error) const char* score_file_error(ScoreFileFixture* p){return p->score.error.c_str();}
// Test-only construction of original file payloads, including invalid records.
API(score_file_test_pack) bool score_file_test_pack(ScoreFileFixture* p,const u8* data,u32 size){Lzss codec;const auto packed=codec.encode(data,size);p->output.resize(24+packed.size());std::memcpy(p->output.data(),p->score.file_header().data(),24);u32 n=p->output.size(),m=packed.size();std::memcpy(p->output.data()+4,&n,4);std::memcpy(p->output.data()+16,&m,4);std::memcpy(p->output.data()+20,&size,4);return resource_crypt(packed.data(),p->output.data()+24,m,{0xac,0x35,0x10,m},true);}

#include "../../cpp/game/RecordStore.hpp"
struct RecordStoreFixture final:SessionInitializationServices,CompletionServices {
 RecordStore records;Rng random;std::vector<u8> output;SessionState progress;PlayerLifeSession player;ItemScoreState score;EnemyWorldState enemies;u32 flags=0;i32 bonus=0,ending_frames=0;
 SessionInitialization initialize{progress,player,score,enemies,records,*this};SessionCompletion completion{progress,player,score,flags,bonus,ending_frames,records,*this};
 bool bomb_hud(i32,i32)override{return true;}bool create_player()override{player.power_step=100;score.max_power=400;return true;}bool configure_player()override{return true;}
 bool register_session()override{return true;}bool create_replay()override{return true;}bool reset_replay()override{return true;}bool create_background()override{return true;}bool create_gui()override{return true;}bool reset_gui()override{return true;}
 bool create_bullets()override{return true;}bool create_items()override{return true;}bool create_lasers()override{return true;}bool create_pause_menu()override{return true;}bool create_popups()override{return true;}bool create_checkpoint_storage()override{return true;}bool create_enemies()override{return true;}bool restore_enemies()override{return true;}bool create_bomb()override{return true;}bool create_spell()override{return true;}
 bool load_stage_music()override{return true;}bool load_player_music(bool)override{return true;}bool finish_scene()override{return true;}
 bool clear_notice()override{return true;}bool finish_player_options()override{return true;}bool end_bomb()override{return true;}bool prepare_ending()override{return true;}bool finish_replay()override{return true;}bool finish_practice()override{return true;}bool queue_next_scene()override{return true;}bool select_stage_resources(i32)override{return true;}
 RecordStoreFixture(){records.checkpoint_remove=[](i32,i32){return true;};records.reset(random);}
};
API(records_create) RecordStoreFixture* records_create(){return new RecordStoreFixture;}
API(records_delete) void records_delete(RecordStoreFixture* p){delete p;}
API(records_reset) void records_reset(RecordStoreFixture* p,u32 seed){p->random.seed=u16(seed);p->random.reserved=u16(seed>>16);p->records.reset(p->random);}
API(records_import_blocks) bool records_import_blocks(RecordStoreFixture* p,const u8* data){return p->records.import_blocks(data,data+5*ScoreFile::character_size);}
API(records_blocks) const u8* records_blocks(RecordStoreFixture* p,u32 settings){const auto& file=p->records.export_blocks();return settings?file.settings.data():file.characters[0].data();}
API(records_field) void* records_field(RecordStoreFixture* p,u32 kind){switch(kind){case 0:return &p->progress;case 1:return &p->player;case 2:return &p->score;case 3:return &p->flags;case 4:return &p->bonus;case 5:return &p->ending_frames;case 6:return &p->enemies;case 7:return p->records.music.data();default:return p->records.name.data();}}
API(records_initialization) bool records_initialization(RecordStoreFixture* p,u32 fresh,u32 replay){p->progress.new_run=fresh;p->progress.replay=replay;p->initialize.error.clear();return p->initialize.initialize();}
API(records_completion) bool records_completion(RecordStoreFixture* p){p->completion.error.clear();return p->completion.complete();}
API(records_action) bool records_action(RecordStoreFixture* p,i32 action,i32 character,bool legacy,i32 stage,i32 difficulty,i32 spell,i32 score,const char* name){switch(action){case 0:return p->records.mark_stage(character,stage,difficulty);case 1:return p->records.count_play(character,legacy);case 2:return p->records.stage_clear(character,stage,difficulty);case 3:return p->records.finished_run(character,legacy,difficulty,score!=0);case 4:return p->records.spell_score(character,legacy,spell,score);case 5:return p->records.spell_begin(character,legacy,spell,difficulty!=0,name);case 6:return p->records.spell_capture(character,legacy,spell,difficulty!=0);default:return p->records.unlock_music(stage);}}
API(records_high_score) void records_high_score(RecordStoreFixture* p,i32 scope,i32 character,bool legacy,i32 stage,i32 difficulty,i32 spell,i32* out){const auto row=p->records.high_score({RecordScope(scope),character,difficulty,stage,spell,legacy});out[0]=row.score;out[1]=row.extra;}
API(records_title) void records_title(RecordStoreFixture* p,i32 character,u32* out){const auto title=p->records.title_records();out[0]=title.extra_available();out[1]=title.extra_character(character);for(u32 m=0;m<2;m++)for(u32 d=0;d<5;d++)out[2+m*5+d]=title.clears[character][m][d];for(u32 d=0;d<5;d++)for(u32 s=0;s<6;s++)out[12+d*6+s]=title.practice_stages[character][d][s];out[42]=p->records.music_mask();}
API(records_open) bool records_open(RecordStoreFixture* p,const u8* data,u32 size){return p->records.open(data,size);}
API(records_save) bool records_save(RecordStoreFixture* p){return p->records.save(p->output);}
API(records_data) const u8* records_data(RecordStoreFixture* p){return p->output.data();}
API(records_size) u32 records_size(RecordStoreFixture* p){return p->output.size();}
API(records_error) const char* records_error(RecordStoreFixture* p){return p->records.error.empty()?(p->initialize.error.empty()?p->completion.error.c_str():p->initialize.error.c_str()):p->records.error.c_str();}
API(records_insert_score) i32 records_insert_score(RecordStoreFixture* p,i32 character,bool legacy,i32 difficulty,i32 score,i32 stage,i32 continues,double delivered,double requested,i32 deaths){RunScoreSubmission row;row.score=score;row.stage=stage;row.continues=continues;row.timestamp={0x12345678,signed_bits(0x9abcdef0)};row.slowdown=score_slowdown(delivered,requested);row.deaths=deaths;return p->records.insert_score(character,legacy,difficulty,row);}
API(records_score_name) bool records_score_name(RecordStoreFixture* p,i32 character,bool legacy,i32 difficulty,i32 slot,const char* value){std::array<char,9> name{};std::memcpy(name.data(),value,9);return p->records.score_name(character,legacy,difficulty,slot,name);}

#include "../../cpp/game/RunInitialization.hpp"
#include "../../cpp/game/RunCompletion.hpp"
#include "../../cpp/game/RunStageExit.hpp"
#include "../../cpp/game/RunStageFlow.hpp"
struct RunConstructionFixture:RunSessionFixture,RunConstructionServices,RunCompletionServices,RunStageExitServices {
 Rng record_random;RecordStore records;RunInitialization construction;std::unique_ptr<RunCompletion> completion;bool next_queued=false;u32 prepared_stage=0,loaded_stage=0;std::array<u32,2> loaded_player{};
 explicit RunConstructionFixture(StageAssetFixture& a):RunSessionFixture(a),construction(run,progress,records,replay,session,*this){records.reset(record_random);records.checkpoint_remove=[](i32,i32){return true;};}
 bool prepare_pause_menu()override{return available;}bool prepare_checkpoint_storage()override{return available;}bool restore_enemy_checkpoint(StageGameplay& s)override{return s.restore();}
 bool load_stage_theme(i32 s)override{loaded_stage=s;return available;}bool load_player_theme(i32 s,bool boss)override{loaded_player[boss?1:0]=s;return available;}
 bool prepare_ending(StageGameplay&)override{return available;}bool finish_replay()override{return RunSessionFixture::finish_replay();}bool finish_practice()override{return available;}bool queue_next_stage()override{next_queued=true;return available;}bool prepare_next_stage(i32 stage)override{prepared_stage=stage;return available;}
 bool unlock_current_music()override{const auto* d=stage_definition(progress.stage);return d&&records.unlock_music(d->music_unlock[0]);}
 bool unlock_music(i32 index)override{return records.unlock_music(index);}
 std::vector<i32> stage_exit_events;std::unique_ptr<RunStageExit> stage_exit;std::vector<u8> saved_records;
 std::unique_ptr<RunStageFlow> flow;
 bool stop_checkpoint_worker()override{stage_exit_events.push_back(1);return available;}
 bool save_records()override{stage_exit_events.push_back(2);return available&&records.save(saved_records);}
 bool clear_session_links()override{stage_exit_events.push_back(3);return available;}
 bool reset_transition()override{stage_exit_events.push_back(4);return available;}
 bool disable_pause_callbacks()override{stage_exit_events.push_back(5);return available;}
 bool queue_exit_music(i32 mode)override{stage_exit_events.push_back(10+mode);return available;}
 bool reset_audio_slots()override{stage_exit_events.push_back(6);return available;}
 bool initial(u32 stage,i32 character,u32 flags,i32 difficulty,const u8* bytes,u32 size){progress.difficulty=difficulty;progress.starting_stage=stage;progress.new_run=true;progress.replay=bytes!=nullptr;if(!run.construct(stage,character,&requested_chapter))return false;run.scene()->battle.session.mode_flags=flags;StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};if(!construction.initialize(camera,0,bytes,size))return false;session.driver()->auto_focus=config.auto_focus();bind_completion();return true;}
 void bind_completion(){StageCamera camera;camera.fov=.6f;camera.direction={0,0,1};flow=std::make_unique<RunStageFlow>(run,session,progress,records,construction,replay,*this,*this);flow->bind(camera,&requested_chapter);}
 bool next(){if(!flow||!flow->pending()||prepared_stage!=u32(progress.stage))return false;stage_exit_events.clear();return flow->advance();}
};
API(run_construction_create) RunConstructionFixture* run_construction_create(StageAssetFixture* assets){return new RunConstructionFixture(*assets);}
API(run_construction_delete) void run_construction_delete(RunConstructionFixture* p){delete p;}
API(run_construction_load) bool run_construction_load(RunConstructionFixture* p,u32 stage,i32 character,u32 flags,i32 difficulty,const u8* bytes,u32 size){return p->initial(stage,character,flags,difficulty,bytes,size);}
API(run_construction_next) bool run_construction_next(RunConstructionFixture* p){return p->next();}
API(run_construction_complete) bool run_construction_complete(RunConstructionFixture* p){return p->run.scene()->messages.request(-2);}
API(run_construction_error) const char* run_construction_error(RunConstructionFixture* p){if(p->flow&&!p->flow->error.empty())return p->flow->error.c_str();if(p->stage_exit&&!p->stage_exit->error.empty())return p->stage_exit->error.c_str();if(!p->construction.error.empty())return p->construction.error.c_str();if(p->completion&&!p->completion->error.empty())return p->completion->error.c_str();if(!p->records.error.empty())return p->records.error.c_str();return run_session_error(p);}
API(run_construction_records) RecordStore* run_construction_records(RunConstructionFixture* p){return &p->records;}
API(run_construction_record_field) void* run_construction_record_field(RunConstructionFixture* p,i32 character,i32 mode,i32 difficulty,i32 stage,i32 kind){auto& ch=p->records.characters[character];if(kind==0)return &ch.modes[mode].plays;if(kind==1)return &ch.practice[difficulty][stage];if(kind==2)return &ch.modes[mode].without_continue[difficulty];if(kind==3)return &ch.modes[mode].clears[difficulty];if(kind==4)return p->records.music.data();return const_cast<u8*>(p->records.export_blocks().settings.data());}
API(run_construction_queue) u32 run_construction_queue(RunConstructionFixture* p,u32 field){return field==0?p->flow&&p->flow->pending():field==1?p->prepared_stage:field==2?p->loaded_stage:field==3?p->loaded_player[0]:p->loaded_player[1];}
API(run_construction_exit_event_count) u32 run_construction_exit_event_count(RunConstructionFixture* p){return p->stage_exit_events.size();}
API(run_construction_exit_events) const i32* run_construction_exit_events(RunConstructionFixture* p){return p->stage_exit_events.data();}
API(run_construction_saved_size) u32 run_construction_saved_size(RunConstructionFixture* p){return p->saved_records.size();}
API(run_construction_exit_presentation) const SessionExitPresentation* run_construction_exit_presentation(RunConstructionFixture* p){return p->flow?p->flow->exit_presentation():nullptr;}
API(run_construction_step) bool run_construction_step(RunConstructionFixture* p,u32 held,u32 pressed,float fps,bool background_finished){SessionGameplayInput input;input.scene.battle.held=held;input.scene.battle.pressed=pressed;input.scene.battle.focus_allowed=true;input.scene.background.flags=1;input.fps=fps;input.physical_pressed=pressed;input.background_finished=background_finished;p->run.scene()->battle.player->life.invulnerability.set(100000);return p->flow&&p->flow->step(input);}
API(player_finish_stage_options) bool player_finish_stage_options(PlayerFixture* p){return p->player.finish_stage_options();}

API(run_session_sample_count) u32 run_session_sample_count(RunSessionFixture* p,u32 stage){auto* r=p->replay.live();const auto* s=r?r->stage(stage):nullptr;return s?s->inputs.size():0;}

#include "../../cpp/game/PauseActivation.hpp"
#include "../../cpp/game/PauseRestoration.hpp"
struct PauseActivationFixture:PauseActivationServices,PauseRestorationServices {
 PauseState pause;SessionState progress;PlayerLifeSession player;i32 frame_skip=7;std::vector<i32> events;bool available=true;
 std::array<u8,0x3f4> projection{};PauseActivation activation{pause,progress,player,frame_skip,*this};
 PauseRestoration restoration{pause,progress,frame_skip,*this};
 bool event(i32 id){events.push_back(id);return available;}
 bool record_elapsed_play_time()override{return event(1);}
 bool menu_visual(u32& handle,i32 script,i32 label)override{events.insert(events.end(),{2,script,label});handle=0x1111;return available;}
 bool reset_audio_slots()override{return event(3);}bool sound(i32 id)override{events.insert(events.end(),{4,id});return available;}
 bool suspend_music()override{return event(5);}bool finish_audio_requests()override{return event(6);}
 bool capture_background(u32& handle,bool field)override{events.insert(events.end(),{7,field?1:0});handle=0x2222;return available;}
 bool freeze_dialogue()override{return event(8);}bool freeze_chapter_result()override{return event(9);}
 bool preserve_current_music(std::string& wave,double& position)override{wave="native-current.wav";position=123.125;return available;}
 bool start_game_over_music()override{events.insert(events.end(),{11,12});return available;}
 bool replay_destination(bool selection)override{events.insert(events.end(),{13,selection?2:4});return available;}
 bool restart_elapsed_play_time()override{return event(1);}
 bool interrupt_pause_visual(u32 handle,i32 label)override{events.insert(events.end(),{4,i32(handle),label});return available;}
 bool resume_dialogue()override{return event(2);}bool resume_chapter_result()override{return event(3);}
 void configure(u32 flags,i32 chapter,i32 replay,float rate,i32 screen,u32 state_flags){
  pause=PauseState{};pause.age.set(111);pause.selection_age.set(222);pause.screen=PauseScreen(screen);pause.previous=PauseScreen::GameOver;pause.phase=7;pause.menu.disabled_count=0;pause.result_mode=9;pause.flags=state_flags;pause.menu_animation=66;pause.snapshot_animation=77;pause.front_bank=99;pause.saved_rate=.33f;pause.saved_frame_skip=9;pause.saved_music="old-wave";pause.saved_music_position=7.125;
  progress={};progress.scene_flags=0x4080;progress.chapter=chapter;progress.transition=replay;progress.rate=rate;player.mode_flags=flags;frame_skip=7;events.clear();activation.error.clear();restoration.error.clear();
 }
 const u8* bytes(){projection.fill(0);auto put=[&](u32 at,const auto& value){std::memcpy(projection.data()+at,&value,sizeof(value));};put(0xc,pause.age);put(0x20,pause.selection_age);put(0x108,pause.menu.disabled_count);put(0x1e4,pause.menu_animation);put(0x1e8,pause.snapshot_animation);put(0x1ec,pause.screen);put(0x1f0,pause.previous);put(0x1f4,pause.phase);put(0x1fc,pause.result_mode);put(0x208,pause.saved_frame_skip);put(0x2e0,pause.saved_rate);put(0x2e4,pause.saved_music_position);std::memcpy(projection.data()+0x2ec,pause.saved_music.c_str(),pause.saved_music.size()+1);put(0x3ec,pause.flags);put(0x3f0,pause.front_bank);return projection.data();}
};
API(pause_activation_create) PauseActivationFixture* pause_activation_create(){return new PauseActivationFixture;}
API(pause_activation_delete) void pause_activation_delete(PauseActivationFixture* p){delete p;}
API(pause_activation_configure) void pause_activation_configure(PauseActivationFixture* p,u32 flags,i32 chapter,i32 replay,float rate,i32 screen,u32 state_flags){p->configure(flags,chapter,replay,rate,screen,state_flags);}
API(pause_activation_open) bool pause_activation_open(PauseActivationFixture* p,i32 entrance,bool selection){return p->activation.open(PauseEntrance(entrance),101,selection);}
API(pause_activation_state) const u8* pause_activation_state(PauseActivationFixture* p){return p->bytes();}
API(pause_activation_field) void* pause_activation_field(PauseActivationFixture* p,u32 kind){if(kind==0)return &p->progress;if(kind==1)return &p->player;if(kind==2)return &p->frame_skip;return &p->available;}
API(pause_activation_events) const i32* pause_activation_events(PauseActivationFixture* p){return p->events.data();}
API(pause_activation_event_count) u32 pause_activation_event_count(PauseActivationFixture* p){return p->events.size();}
API(pause_activation_error) const char* pause_activation_error(PauseActivationFixture* p){return p->activation.error.c_str();}
API(pause_restoration_run) bool pause_restoration_run(PauseActivationFixture* p){p->events.clear();return p->restoration.restore();}
API(pause_restoration_error) const char* pause_restoration_error(PauseActivationFixture* p){return p->restoration.error.c_str();}

#include "../../cpp/game/PauseNameEditor.hpp"
struct PauseNameFixture:PauseActivationFixture,PauseNameServices {
 Rng random;RecordStore records;PauseNameEditor editor{pause,progress,player,records,*this};MenuCursorFixture grid_projection;
 std::vector<i32> sounds;std::array<char,9> written{};i32 written_slot=-1;u32 result_menus=0;
 PauseNameFixture(){records.reset(random);}
 bool sound(i32 value)override{sounds.push_back(value);return available;}
 bool write_replay(i32 slot,const std::array<char,9>& value)override{written_slot=slot;written=value;return available;}
 bool prepare_results_menu()override{result_menus++;pause.select_phase(6);return available;}
 bool setup(i32 phase,i32 cursor,i32 length,i32 age,i32 slot,u32 mode,const char* value){
  configure(mode,43,0,.75f,2,2);progress.character=1;progress.difficulty=2;records.reset(random);pause.phase=phase;pause.menu.count=25;pause.menu.wrapping=true;pause.menu.select(slot);pause.names.count=91;pause.names.wrapping=true;pause.names.select(cursor);pause.names.previous=cursor;pause.name_length=length;std::memcpy(pause.name.data(),value,9);pause.age.set(age);written={};written_slot=-1;result_menus=0;sounds.clear();editor.error.clear();return true;
 }
};
API(pause_name_create) PauseNameFixture* pause_name_create(){return new PauseNameFixture;}
API(pause_name_delete) void pause_name_delete(PauseNameFixture* p){delete p;}
API(pause_name_configure) bool pause_name_configure(PauseNameFixture* p,i32 phase,i32 cursor,i32 length,i32 age,i32 slot,u32 mode,const char* value){return p->setup(phase,cursor,length,age,slot,mode,value);}
API(pause_name_update) bool pause_name_update(PauseNameFixture* p,u32 pressed,u32 repeated){return p->editor.update(pressed,repeated);}
API(pause_name_grid) const u8* pause_name_grid(PauseNameFixture* p){p->grid_projection.menu=p->pause.names;return menu_cursor_bytes(&p->grid_projection);}
API(pause_name_text) const char* pause_name_text(PauseNameFixture* p){return p->pause.name.data();}
API(pause_name_length) i32 pause_name_length(PauseNameFixture* p){return p->pause.name_length;}
API(pause_name_phase) i32 pause_name_phase(PauseNameFixture* p){return p->pause.phase;}
API(pause_name_sound_count) u32 pause_name_sound_count(PauseNameFixture* p){return p->sounds.size();}
API(pause_name_sounds) const i32* pause_name_sounds(PauseNameFixture* p){return p->sounds.data();}
API(pause_name_written) const char* pause_name_written(PauseNameFixture* p){return p->written.data();}
API(pause_name_written_slot) i32 pause_name_written_slot(PauseNameFixture* p){return p->written_slot;}
API(pause_name_result_menus) u32 pause_name_result_menus(PauseNameFixture* p){return p->result_menus;}
API(pause_name_blocks) const u8* pause_name_blocks(PauseNameFixture* p,u32 settings){const auto& file=p->records.export_blocks();return settings?file.settings.data():file.characters[0].data();}
API(pause_name_error) const char* pause_name_error(PauseNameFixture* p){return p->editor.error.c_str();}

#include "../../cpp/game/PauseScoreRegistration.hpp"
struct PauseScoreFixture:PauseNameFixture,PauseScoreServices {
 ItemScoreState score;PauseScoreDetails details;u32 captures=0;MenuCursorFixture menu_projection;
 PauseScoreRegistration registration{pause,progress,player,score,records,editor,*this};
 bool capture_score_details(PauseScoreDetails& value)override{captures++;value=details;return available;}
 bool setup_result(const u8* bytes,i32 character,i32 difficulty,i32 stage,i32 continues,i32 deaths,u32 mode,u32 points,i32 result,double delivered,double requested){
  configure(mode,3,0,.5f,2,0);records.import_blocks(bytes,bytes+5*0xa4a0);progress.character=character;progress.subcharacter=0;progress.difficulty=difficulty;progress.stage=stage;progress.continues=continues;progress.stage_deaths[0]=deaths;score.score=signed_bits(points);pause.result_mode=result;pause.name_not_required=false;pause.name.fill('x');pause.name[8]=0;pause.name_length=3;details.timestamp={0x12345678,signed_bits(0x9abcdef0)};details.delivered=delivered;details.requested=requested;captures=0;registration.error.clear();editor.error.clear();return records.error.empty();
 }
};
API(pause_score_create) PauseScoreFixture* pause_score_create(){return new PauseScoreFixture;}
API(pause_score_delete) void pause_score_delete(PauseScoreFixture* p){delete p;}
API(pause_score_configure) bool pause_score_configure(PauseScoreFixture* p,const u8* bytes,i32 character,i32 difficulty,i32 stage,i32 continues,i32 deaths,u32 mode,u32 score,i32 result,double delivered,double requested){return p->setup_result(bytes,character,difficulty,stage,continues,deaths,mode,score,result,delivered,requested);}
API(pause_score_register) bool pause_score_register(PauseScoreFixture* p){return p->registration.register_result();}
API(pause_score_name_not_required) bool pause_score_name_not_required(PauseScoreFixture* p){return p->pause.name_not_required;}
API(pause_score_menu) const u8* pause_score_menu(PauseScoreFixture* p){p->menu_projection.menu=p->pause.menu;return menu_cursor_bytes(&p->menu_projection);}
API(pause_score_captures) u32 pause_score_captures(PauseScoreFixture* p){return p->captures;}
API(pause_score_stage) i32 pause_score_stage(PauseScoreFixture* p){return p->progress.stage;}
API(pause_score_error) const char* pause_score_error(PauseScoreFixture* p){return p->registration.error.c_str();}

#include "../../cpp/game/PauseMenu.hpp"
struct PauseMenuFixture:PauseScoreFixture,PauseMenuServices {
 AnmManager& animations;PauseMenu controller;u32 catalog_reads=0,replay_preparations=0,options_opens=0,options_closes=0,actions=0;bool options_done=false,completed_replay=false;float options_offset=0;
 explicit PauseMenuFixture(AnmManagerFixture& a):animations(a.manager),controller(pause,progress,player,score,records,a.manager,*this){}
 bool sound(i32 id)override{return PauseNameFixture::sound(id);}
 bool capture_score_details(PauseScoreDetails& value)override{return PauseScoreFixture::capture_score_details(value);}
 bool synchronize_result_score()override{if(u32(score.score)>u32(progress.high_score))progress.high_score=score.score;return available;}
 bool read_replay_slot(i32,std::shared_ptr<Replay>& value)override{catalog_reads++;value.reset();return available;}
 bool save_named_replay(i32 slot,const std::array<char,9>& value)override{return PauseNameFixture::write_replay(slot,value);}
 bool prepare_replay_save(bool completed)override{replay_preparations++;completed_replay=completed;return available;}
 bool open_options(float offset)override{options_opens++;options_offset=offset;return available;}
 bool options_finished()const override{return options_done;}
 bool close_options()override{options_closes++;options_done=false;return available;}
 bool finish_pause_selection()override{actions++;return available;}
 bool setup_menu(i32 phase,i32 age,i32 screen,i32 slot,u32 mode,i32 continues,bool replay,u32 flags,i32 budget,i32 script){
  configure(mode,43,replay?1:0,.75f,screen,flags);pause.phase=phase;pause.age.set(age);pause.front_bank=101;pause.menu.count=5;pause.menu.wrapping=true;pause.menu.select(slot);pause.retry_controls=false;progress.continues=continues;progress.continue_budget=budget;score.score=0;progress.high_score=0;records.reset(random);pause.name.fill(' ');pause.name[8]=0;pause.name_length=0;pause.menu_animation=animations.create_overlay(101,script);pause.snapshot_animation=0;catalog_reads=replay_preparations=options_opens=options_closes=actions=0;options_done=false;sounds.clear();return pause.menu_animation!=0;
 }
};
API(pause_menu_create) PauseMenuFixture* pause_menu_create(AnmManagerFixture* a){return new PauseMenuFixture(*a);}
API(pause_menu_delete) void pause_menu_delete(PauseMenuFixture* p){delete p;}
API(pause_menu_configure) bool pause_menu_configure(PauseMenuFixture* p,i32 phase,i32 age,i32 screen,i32 slot,u32 mode,i32 continues,bool replay,u32 flags,i32 budget,i32 script){return p->setup_menu(phase,age,screen,slot,mode,continues,replay,flags,budget,script);}
API(pause_menu_update) bool pause_menu_update(PauseMenuFixture* p,u32 pressed,u32 repeated){p->sounds.clear();return p->controller.update(pressed,repeated);}
API(pause_menu_tick) void pause_menu_tick(PauseMenuFixture* p,float rate){p->pause.age.tick(&rate);p->pause.selection_age.tick(&rate);}
API(pause_menu_state) const u8* pause_menu_state(PauseMenuFixture* p){p->bytes();p->menu_projection.menu=p->pause.menu;std::memcpy(p->projection.data()+0x34,menu_cursor_bytes(&p->menu_projection),0xd8);p->grid_projection.menu=p->pause.names;std::memcpy(p->projection.data()+0x10c,menu_cursor_bytes(&p->grid_projection),0xd8);std::memcpy(p->projection.data()+0x1f8,&p->pause.name_length,4);i32 required=p->pause.name_not_required,retry=p->pause.retry_controls;std::memcpy(p->projection.data()+0x200,&required,4);std::memcpy(p->projection.data()+0x204,&retry,4);std::memcpy(p->projection.data()+0x2d4,p->pause.name.data(),9);return p->projection.data();}
API(pause_menu_options_done) void pause_menu_options_done(PauseMenuFixture* p,bool done){p->options_done=done;}
API(pause_menu_count) u32 pause_menu_count(PauseMenuFixture* p,u32 kind){switch(kind){case 0:return p->catalog_reads;case 1:return p->replay_preparations;case 2:return p->options_opens;case 3:return p->options_closes;default:return p->actions;}}
API(pause_menu_error) const char* pause_menu_error(PauseMenuFixture* p){return p->controller.error.c_str();}

API(pause_menu_points) void pause_menu_points(PauseMenuFixture* p,u32 points){p->score.score=signed_bits(points);}
API(pause_menu_choose) void pause_menu_choose(PauseMenuFixture* p,i32 cursor){p->pause.menu.select(cursor);}
API(pause_menu_name_choose) void pause_menu_name_choose(PauseMenuFixture* p,i32 cursor){p->pause.names.select(cursor);}

#include "../../cpp/game/PauseActions.hpp"
struct PauseActionsFixture final:PauseActivationFixture,PauseGameplayActions,PauseActionServices {
 AnmManager& animations;ItemScoreState score;PauseActions controller;bool selection=false;std::vector<i32> action_events;std::array<u32,17> projection_fields{};
 explicit PauseActionsFixture(AnmManagerFixture& a):animations(a.manager),controller(pause,progress,player,score,a.manager,restoration,*this,*this,frame_skip){}
 bool ae(i32 value){action_events.push_back(value);return available;}
 bool resume_looping_sounds()override{return ae(1);}bool resume_music()override{return ae(2);}
 bool resume_saved_music(const std::string& wave,double position)override{return wave==pause.saved_music&&position==pause.saved_music_position&&ae(3);}
 bool reset_audio_slots()override{return ae(4);}bool destination(PauseDestination d)override{action_events.insert(action_events.end(),{5,i32(d)});return available;}
 bool restore_checkpoint()override{return ae(6);}bool options_changed()override{return ae(7);}
 bool life_hud(i32 a,i32 b)override{action_events.insert(action_events.end(),{8,a,b});return available;}
 bool bomb_hud(i32 a,i32 b)override{action_events.insert(action_events.end(),{9,a,b});return available;}
 bool power_notice()override{return ae(10);}bool resume_dialogue()override{return pause.screen==PauseScreen::Pause?event(2):ae(11);}bool resume_chapter_result()override{return pause.screen==PauseScreen::Pause?event(3):ae(12);}
 bool configure_action(u32 mode,i32 screen,i32 item,i32 stage,i32 chapter,bool replay,i32 result,i32 continues,i32 power,i32 max_power,i32 power_step,bool return_selection){
  configure(mode,chapter,replay?1:0,.75f,screen,4);pause.phase=16;pause.age.set(12);pause.menu.count=5;pause.menu.wrapping=true;pause.menu.select(item);pause.result_mode=result;pause.menu_animation=pause.snapshot_animation=0;progress.stage=stage;progress.continues=continues;progress.continue_budget=5;progress.scene_flags=0x4090;player.extra_lives=-1;player.life_pieces=3;player.bombs=0;player.bomb_pieces=2;player.power=power;player.power_step=power_step;score.score=123456;score.max_power=max_power;selection=return_selection;events.clear();action_events.clear();return true;
 }
 const u32* fields(){auto& b=projection_fields;b={u32(player.extra_lives),u32(player.life_pieces),u32(player.bombs),u32(player.bomb_pieces),u32(player.power),u32(player.power_step),player.mode_flags,u32(score.score),u32(score.max_power),u32(progress.continues),u32(progress.continue_budget),progress.scene_flags,float_to_bits(progress.rate),u32(frame_skip),float_to_bits(pause.saved_rate),pause.menu_animation,pause.snapshot_animation};return b.data();}
};
API(pause_actions_create) PauseActionsFixture* pause_actions_create(AnmManagerFixture* a){return new PauseActionsFixture(*a);}
API(pause_actions_delete) void pause_actions_delete(PauseActionsFixture* p){delete p;}
API(pause_actions_configure) bool pause_actions_configure(PauseActionsFixture* p,u32 mode,i32 screen,i32 item,i32 stage,i32 chapter,bool replay,i32 result,i32 continues,i32 power,i32 max_power,i32 power_step,bool selection){return p->configure_action(mode,screen,item,stage,chapter,replay,result,continues,power,max_power,power_step,selection);}
API(pause_actions_execute) bool pause_actions_execute(PauseActionsFixture* p){return p->controller.execute(p->selection);}
API(pause_actions_fields) const u32* pause_actions_fields(PauseActionsFixture* p){return p->fields();}
API(pause_actions_events) const i32* pause_actions_events(PauseActionsFixture* p){return p->action_events.data();}
API(pause_actions_event_count) u32 pause_actions_event_count(PauseActionsFixture* p){return p->action_events.size();}
API(pause_actions_error) const char* pause_actions_error(PauseActionsFixture* p){return p->controller.error.c_str();}

#include "../../cpp/game/RunPlayTime.hpp"
struct RunPlayTimeFixture final:RunClock {
 Rng random;RecordStore records;SessionState progress;PlayerLifeSession player;RunPlayTime clock{records,progress,player,*this};std::array<double,2> values{};u32 reads=0;
 RunPlayTimeFixture(){records.reset(random);}
 bool seconds(double& value)override{value=values[reads++&1];return true;}
 bool setup(const u8* bytes,i32 character,u32 mode,bool replay,double origin,double first,double second){records.import_blocks(bytes,bytes+5*0xa4a0);progress.character=character;progress.subcharacter=0;progress.transition=replay?1:0;player.mode_flags=mode;clock.origin=origin;values={first,second};reads=0;clock.error.clear();return records.error.empty();}
};
API(run_time_create) RunPlayTimeFixture* run_time_create(){return new RunPlayTimeFixture;}
API(run_time_delete) void run_time_delete(RunPlayTimeFixture* p){delete p;}
API(run_time_configure) bool run_time_configure(RunPlayTimeFixture* p,const u8* data,i32 character,u32 mode,bool replay,double origin,double first,double second){return p->setup(data,character,mode,replay,origin,first,second);}
API(run_time_account) bool run_time_account(RunPlayTimeFixture* p,i32 destination){return p->clock.account(destination);}
API(run_time_rebase) bool run_time_rebase(RunPlayTimeFixture* p){return p->clock.rebase();}
API(run_time_reads) u32 run_time_reads(RunPlayTimeFixture* p){return p->reads;}
API(run_time_origin) double run_time_origin(RunPlayTimeFixture* p){return p->clock.origin;}
API(run_time_blocks) const u8* run_time_blocks(RunPlayTimeFixture* p,u32 settings){const auto& file=p->records.export_blocks();return settings?file.settings.data():file.characters[0].data();}
API(run_time_error) const char* run_time_error(RunPlayTimeFixture* p){return p->clock.error.c_str();}

#include "../../cpp/game/RunPause.hpp"
struct RunPauseFixture final:RunConstructionFixture,RunPausePlatform {
 std::unique_ptr<RunPause> pause_controller;double clock_seconds=0;std::vector<i32> pause_events;bool options_done=false;std::array<std::vector<u8>,25> replay_files;std::array<char,9> last_name{};i32 last_slot=-1,exit_destination=0;
 explicit RunPauseFixture(StageAssetFixture& a):RunConstructionFixture(a){}
 bool seconds(double& value)override{value=clock_seconds;return available;}
 bool pe(i32 id){pause_events.push_back(id);return available;}
 bool sound(i32 value)override{pause_events.insert(pause_events.end(),{1,value});return available;}
 bool suspend_music()override{return pe(2);}bool finish_audio_requests()override{return pe(3);}
 bool capture_background(StageGameplay&,AnmManager&,u32& handle,bool field)override{handle=0;pause_events.insert(pause_events.end(),{4,field?1:0});return available;}
 bool preserve_current_music(std::string& wave,double& position)override{wave="fixture-stage";position=clock_seconds;return available;}
 bool start_game_over_music()override{return pe(5);}bool resume_looping_sounds()override{return pe(6);}bool resume_music()override{return pe(7);}
 bool resume_saved_music(const std::string&,double)override{return pe(8);}bool reset_audio_slots()override{return pe(9);}
 bool destination(PauseDestination value)override{exit_destination=i32(value);return pe(10);}
 bool capture_score_details(PauseScoreDetails& out)override{out.timestamp={1700000000,0};out.delivered=out.requested=1;return available;}
 bool read_replay_slot(i32 slot,std::shared_ptr<Replay>& out)override{out.reset();if(slot<0||slot>=25)return false;if(replay_files[slot].empty())return true;out=std::make_shared<Replay>();return out->open(replay_files[slot].data(),replay_files[slot].size());}
 bool save_named_replay(i32 slot,const std::array<char,9>& name)override{auto* live=replay.live();if(!live||slot<0||slot>=25)return false;ReplayExportDetails d;d.score=run.scene()->battle.score.score;d.elapsed=d.total=clock_seconds;if(!live->write(name.data(),d,true))return false;replay_files[slot]=live->output();last_slot=slot;last_name=name;return true;}
 bool prepare_replay_save(bool completed)override{auto* live=replay.live();if(!live)return false;live->finish(1700000000,progress.stage,completed);return true;}
 bool open_options(float)override{options_done=false;return pe(11);}bool options_finished()const override{return options_done;}bool close_options()override{options_done=false;return pe(12);}
 i32 scene_destination()const override{return exit_destination;}
 bool attach_pause(){if(!run.scene()||!session.driver())return false;pause_controller=std::make_unique<RunPause>(*run.scene(),*session.driver(),progress,records,assets.animations,*this,session.driver()->runtime.frame_skip,run.scene_assets()->front);return pause_controller->prepare();}
 bool paused_step(u32 held,u32 pressed,u32 repeated,bool focus){if(!pause_controller)return false;pause_controller->controls({pressed,repeated,focus,true});clock_seconds+=1./60.;return run_session_step(this,held,pressed,60,false)&&pause_controller->frame.error.empty();}
};
API(run_pause_create) RunPauseFixture* run_pause_create(StageAssetFixture* a){return new RunPauseFixture(*a);}
API(run_pause_delete) void run_pause_delete(RunPauseFixture* p){delete p;}
API(run_pause_attach) bool run_pause_attach(RunPauseFixture* p){return p->attach_pause();}
API(run_pause_step) bool run_pause_step(RunPauseFixture* p,u32 held,u32 pressed,u32 repeated,bool focus){return p->paused_step(held,pressed,repeated,focus);}
API(run_pause_open) bool run_pause_open(RunPauseFixture* p,i32 entrance){return p->pause_controller&&p->pause_controller->open(PauseEntrance(entrance));}
API(run_pause_game_over) bool run_pause_game_over(RunPauseFixture* p){return p->run.scene()->battle.game_over_begin&&p->run.scene()->battle.game_over_begin();}
API(run_pause_state) u32 run_pause_state(RunPauseFixture* p,u32 kind){const auto& s=p->pause_controller->state;switch(kind){case 0:return u32(s.screen);case 1:return s.phase;case 2:return s.age.current;case 3:return s.menu.cursor;case 4:return s.menu.disabled_count;case 5:return s.menu_animation;case 6:return p->exit_destination;default:return p->last_slot;}}
API(run_pause_name) const char* run_pause_name(RunPauseFixture* p){return p->last_name.data();}
API(run_pause_name_choose) void run_pause_name_choose(RunPauseFixture* p,i32 cursor){p->pause_controller->state.names.select(cursor);}
API(run_pause_replay_data) const u8* run_pause_replay_data(RunPauseFixture* p,i32 slot){return p->replay_files[slot].data();}
API(run_pause_replay_size) u32 run_pause_replay_size(RunPauseFixture* p,i32 slot){return p->replay_files[slot].size();}
API(run_pause_error) const char* run_pause_error(RunPauseFixture* p){if(!p->pause_controller)return "Run pause is unattached";if(!p->assets.animations.error.empty())return p->assets.animations.error.c_str();if(!p->pause_controller->frame.error.empty())return p->pause_controller->frame.error.c_str();if(!p->pause_controller->error.empty())return p->pause_controller->error.c_str();return run_construction_error(p);}

API(run_pause_choose) void run_pause_choose(RunPauseFixture* p,i32 cursor){p->pause_controller->state.menu.select(cursor);}
API(run_pause_die) bool run_pause_die(RunPauseFixture* p){auto& b=p->run.scene()->battle;b.session.extra_lives=0;b.score.score=0;return b.player->life.hit();}
API(run_pause_random) const Rng* run_pause_random(RunPauseFixture* p){return &p->assets.environment.game_rng;}
API(run_pause_record_clock) i32 run_pause_record_clock(RunPauseFixture* p){return p->replay.live()?p->replay.live()->frame_clock():-1;}

#include "../../cpp/game/PauseFrame.hpp"
struct PauseFrameFixture final:PauseMenuFixture {
 FrameScheduler schedule;Timer session_age;PauseFrame frame{pause,progress,player,session_age,activation,controller,schedule,101};
 explicit PauseFrameFixture(AnmManagerFixture& a):PauseMenuFixture(a){}
 bool sound(i32 id)override{return PauseActivationFixture::sound(id);}
 void setup_frame(u32 mode,u32 flags,float rate,i32 screen,i32 age){configure(mode,43,0,rate,screen,5);pause.phase=5;progress.scene_flags=flags;session_age.set(age);}
};
API(pause_frame_create) PauseFrameFixture* pause_frame_create(AnmManagerFixture* a){return new PauseFrameFixture(*a);}
API(pause_frame_delete) void pause_frame_delete(PauseFrameFixture* p){delete p;}
API(pause_frame_configure) void pause_frame_configure(PauseFrameFixture* p,u32 mode,u32 flags,float rate,i32 screen,i32 age){p->setup_frame(mode,flags,rate,screen,age);}
API(pause_frame_update) bool pause_frame_update(PauseFrameFixture* p,u32 pressed,u32 repeated,bool focus,bool enabled){return p->frame.update({pressed,repeated,focus,enabled});}
API(pause_frame_error) const char* pause_frame_error(PauseFrameFixture* p){return p->frame.error.c_str();}

#include "../../cpp/game/PauseDraw.hpp"
struct PauseDrawFixture final:PauseNameFixture,HudDrawServices,ReplayCalendarServices {
 AnmManager& animations;std::array<std::shared_ptr<Replay>,25> files{};std::array<u8,0xa4> live{};AnmVm captured;PauseDraw draw;std::vector<HudTextDraw> texts;ReplayCalendar date;std::vector<i64> times;
 explicit PauseDrawFixture(AnmManagerFixture& a):animations(a.manager),draw(pause,progress,player,records,animations,files){}
 bool hud_text(const HudTextDraw& value)override{texts.push_back(value);return available;}
 bool calendar(i64 timestamp,ReplayCalendar& out)override{times.push_back(timestamp);out=date;return available;}
 void setup_draw(i32 screen,i32 phase,u32 flags,u32 mode,i32 selected,i32 cursor,i32 length,i32 age,bool required,i32 character,i32 rank,i32 credit){pause.screen=PauseScreen(screen);pause.phase=phase;pause.flags=flags;pause.menu.cursor=selected;pause.names.cursor=cursor;pause.name_length=length;pause.age.set(age);pause.name_not_required=required;progress.character=character;progress.subcharacter=0;progress.difficulty=rank;progress.continue_budget=credit;player.mode_flags=mode;draw.error.clear();}
 void snapshot(i32 kind,u32 color){animations.retire(pause.snapshot_animation);captured.visual.color=0x11223344;pause.snapshot_animation=0;if(kind==1)pause.snapshot_animation=0x12345678;else if(kind>=2){auto* root=animations.registry.allocate();root->source_script=52;pause.snapshot_animation=animations.registry.submit(*root,4);if(kind>=3){auto* child=animations.registry.allocate();child->source_script=kind==3?57:56;child->visual.color=color;animations.registry.submit(*child,4);animations.registry.attach(*child,*root);}}}
};
API(pause_draw_create) PauseDrawFixture* pause_draw_create(AnmManagerFixture* a){return new PauseDrawFixture(*a);}
API(pause_draw_delete) void pause_draw_delete(PauseDrawFixture* p){p->animations.retire(p->pause.snapshot_animation);delete p;}
API(pause_draw_configure) void pause_draw_configure(PauseDrawFixture* p,i32 screen,i32 phase,u32 flags,u32 mode,i32 selected,i32 cursor,i32 length,i32 age,bool required,i32 character,i32 rank,i32 credit){p->setup_draw(screen,phase,flags,mode,selected,cursor,length,age,required,character,rank,credit);}
API(pause_draw_snapshot) void pause_draw_snapshot(PauseDrawFixture* p,i32 kind,u32 color){p->snapshot(kind,color);}
API(pause_draw_run) bool pause_draw_run(PauseDrawFixture* p){p->texts.clear();p->times.clear();return p->draw.draw(*p,*p,&p->live,&p->captured);}
API(pause_draw_count) u32 pause_draw_count(PauseDrawFixture* p){return p->texts.size();}
API(pause_draw_text) const char* pause_draw_text(PauseDrawFixture* p,u32 index){return p->texts[index].text.c_str();}
API(pause_draw_style) const HudTextStyle* pause_draw_style(PauseDrawFixture* p,u32 index){return &p->texts[index].style;}
API(pause_draw_position) const Vec3* pause_draw_position(PauseDrawFixture* p,u32 index){return &p->texts[index].position;}
API(pause_draw_date) ReplayCalendar* pause_draw_date(PauseDrawFixture* p){return &p->date;}
API(pause_draw_times) const i64* pause_draw_times(PauseDrawFixture* p){return p->times.data();}
API(pause_draw_time_count) u32 pause_draw_time_count(PauseDrawFixture* p){return p->times.size();}
API(pause_draw_live) u8* pause_draw_live(PauseDrawFixture* p){return p->live.data();}
API(pause_draw_snapshot_handle) u32 pause_draw_snapshot_handle(PauseDrawFixture* p){return p->pause.snapshot_animation;}
API(pause_draw_snapshot_color) u32 pause_draw_snapshot_color(PauseDrawFixture* p){return p->captured.visual.color;}
API(pause_draw_file) bool pause_draw_file(PauseDrawFixture* p,u32 slot,const u8* data,u32 size){if(slot>=25)return false;p->files[slot]=std::make_shared<Replay>();return p->files[slot]->open(data,size);}
API(pause_draw_records) bool pause_draw_records(PauseDrawFixture* p,const u8* data){return p->records.import_blocks(data,data+5*0xa4a0);}
API(pause_draw_error) const char* pause_draw_error(PauseDrawFixture* p){return p->draw.error.c_str();}

#include "../../cpp/game/SoundEffects.hpp"
struct SoundEffectsFixture final:SoundOutput {
 std::array<u8,77> available{},playing{};std::vector<i32> events;SoundEffects effects{*this};std::array<i32,77*4> projection{};
 SoundEffectsFixture(){available.fill(1);}
 bool sound_available(u32 id)const override{return available[id]!=0;}
 bool sound_playing(u32 id)override{events.insert(events.end(),{1,i32(id),playing[id]});return playing[id]!=0;}
 void sound_stop(u32 id)override{events.insert(events.end(),{2,i32(id)});playing[id]=0;}
 void sound_position(u32 id,u32 frame)override{events.insert(events.end(),{3,i32(id),i32(frame)});}
 void sound_pan(u32 id,i32 value)override{events.insert(events.end(),{4,i32(id),value});}
 void sound_volume(u32 id,i32 value)override{events.insert(events.end(),{5,i32(id),value});}
 void sound_play(u32 id,u32 flags)override{events.insert(events.end(),{6,i32(id),i32(flags)});playing[id]=1;}
 const i32* fields(){for(u32 id=0;id<77;id++){auto& v=effects.voices[id];projection[id*4]=v.renewal;projection[id*4+1]=v.pan;projection[id*4+2]=v.resume;projection[id*4+3]=i32(v.definition-sound_definitions.data());}return projection.data();}
};
API(sound_effects_create) SoundEffectsFixture* sound_effects_create(){return new SoundEffectsFixture;}
API(sound_effects_delete) void sound_effects_delete(SoundEffectsFixture* p){delete p;}
API(sound_effects_enqueue) bool sound_effects_enqueue(SoundEffectsFixture* p,i32 id,i32 pan){return p->effects.enqueue(id,pan);}
API(sound_effects_positioned) bool sound_effects_positioned(SoundEffectsFixture* p,i32 id,float x){return p->effects.positioned(id,x);}
API(sound_effects_stop) bool sound_effects_stop(SoundEffectsFixture* p,i32 id){p->events.clear();return p->effects.stop(id);}
API(sound_effects_process) void sound_effects_process(SoundEffectsFixture* p){p->events.clear();p->effects.process();}
API(sound_effects_suspend) void sound_effects_suspend(SoundEffectsFixture* p){p->events.clear();p->effects.suspend();}
API(sound_effects_resume) void sound_effects_resume(SoundEffectsFixture* p){p->events.clear();p->effects.resume();}
API(sound_effects_volume) i32 sound_effects_volume(i32 base,i32 master){return SoundEffects::adjusted_volume(base,master);}
API(sound_effects_configure) void sound_effects_configure(SoundEffectsFixture* p,bool enabled,i32 volume){p->effects.enabled=enabled;p->effects.master_volume=volume;}
API(sound_effects_queue) SoundQueue* sound_effects_queue(SoundEffectsFixture* p){return &p->effects.state;}
API(sound_effects_voices) const i32* sound_effects_voices(SoundEffectsFixture* p){return p->fields();}
API(sound_effects_available) u8* sound_effects_available(SoundEffectsFixture* p){return p->available.data();}
API(sound_effects_playing) u8* sound_effects_playing(SoundEffectsFixture* p){return p->playing.data();}
API(sound_effects_event_count) u32 sound_effects_event_count(SoundEffectsFixture* p){return p->events.size();}
API(sound_effects_events) const i32* sound_effects_events(SoundEffectsFixture* p){return p->events.data();}
API(sound_effects_error) const char* sound_effects_error(SoundEffectsFixture* p){return p->effects.error.c_str();}

#include "../../cpp/game/MusicLayout.hpp"
API(music_layout_create) MusicLayout* music_layout_create(){return new MusicLayout;}
API(music_layout_delete) void music_layout_delete(MusicLayout* p){delete p;}
API(music_layout_open) bool music_layout_open(MusicLayout* p,const u8* bytes,u32 size){return p->open(bytes,size);}
API(music_layout_count) u32 music_layout_count(MusicLayout* p){return p->count();}
API(music_layout_index) i32 music_layout_index(MusicLayout* p,const char* value){return p->original_index(value);}
API(music_layout_error) const char* music_layout_error(MusicLayout* p){return p->error.c_str();}
API(music_layout_name) const char* music_layout_name(MusicLayout* p,u32 i){auto* t=p->track(i);return t?t->filename.c_str():"";}
API(music_layout_fields) const u32* music_layout_fields(MusicLayout* p,u32 i){static std::array<u32,10> v;auto* t=p->track(i);if(!t)return nullptr;v={t->offset,t->length,t->loop_start,t->loop_end,t->rate,t->channels,t->alignment,t->bits,u32(t->frames()),u32(t->loop_frame())};return v.data();}
API(music_adjusted_volume) i32 music_adjusted_volume(i32 base,i32 master){return adjusted_music_volume(base,master);}

#include "../../cpp/game/MusicCommands.hpp"
struct MusicCommandsFixture final:MusicCommandOutput {
 MusicLayout layout;MusicCommands commands{layout,*this};bool ready=true,busy=false,release=false,waiting=false,cached=false;i32 fill_result=0,fade=0;std::vector<i32> events;
 bool music_ready()const override{return ready;}bool music_busy()const override{return busy;}bool music_has_intro()const override{return true;}bool release_pending()const override{return release;}bool release_waiting()override{if(!waiting)release=false;return waiting;}
 static i32 hash(const std::string& value){u32 n=2166136261;for(u8 b:value){n^=b;n*=16777619;}return signed_bits(n);}
 void preload_reset()override{events.push_back(9);}bool prepare_music(i32 slot,const std::string& value)override{events.insert(events.end(),{11,slot,hash(value)});return true;}
 bool cached_music(i32 slot)override{events.insert(events.end(),{10,slot});return cached;}
 void stop_music_stream(bool close)override{fade=0;events.insert(events.end(),{1,close?1:0});}
 void rewind_music_stream()override{events.push_back(14);}
 bool load_music(const std::string& file,i32 index)override{events.insert(events.end(),{2,index});return true;}void select_music(i32 index)override{events.insert(events.end(),{3,index});}
 i32 fill_music(bool alternate,bool initial)override{events.insert(events.end(),{4,alternate?1:0,initial?1:0});return fill_result;}
 void start_music_stream()override{events.push_back(5);}void signal_music_release()override{events.push_back(12);}void close_music_stream()override{events.push_back(13);ready=false;}
 void fade_music_stream(i32 frames)override{fade=frames;}void pause_music_stream(bool pause)override{events.insert(events.end(),{7,pause?1:0});}
 void refresh_music_volume()override{events.push_back(6);}void switch_music_wave(i32 index)override{events.insert(events.end(),{8,index});}
};
API(music_commands_create) MusicCommandsFixture* music_commands_create(){return new MusicCommandsFixture;}
API(music_commands_delete) void music_commands_delete(MusicCommandsFixture* p){delete p;}
API(music_commands_layout) bool music_commands_layout(MusicCommandsFixture* p,const u8* bytes,u32 size){return p->layout.open(bytes,size);}
API(music_commands_enqueue) bool music_commands_enqueue(MusicCommandsFixture* p,i32 code,i32 value,const char* text){return p->commands.enqueue(code,value,text);}
API(music_commands_process) bool music_commands_process(MusicCommandsFixture* p){p->events.clear();return p->commands.process();}
API(music_commands_configure) void music_commands_configure(MusicCommandsFixture* p,bool alternate,u32 mode,bool ready,bool busy,bool release,bool waiting,bool cached,i32 fill){p->commands.alternate=alternate;p->commands.music_mode=u8(mode);p->ready=ready;p->busy=busy;p->release=release;p->waiting=waiting;p->cached=cached;p->fill_result=fill;}
API(music_commands_queue) const MusicRequest* music_commands_queue(MusicCommandsFixture* p){return p->commands.requests.data();}
API(music_commands_count) u32 music_commands_count(MusicCommandsFixture* p){return p->events.size();}
API(music_commands_events) const i32* music_commands_events(MusicCommandsFixture* p){return p->events.data();}
API(music_commands_wave) const char* music_commands_wave(MusicCommandsFixture* p){return p->commands.current_wave.c_str();}
API(music_commands_prepared) const char* music_commands_prepared(MusicCommandsFixture* p,u32 slot){return p->commands.prepared[slot].c_str();}
API(music_commands_fade) i32 music_commands_fade(MusicCommandsFixture* p){return p->fade;}
API(music_commands_ready) bool music_commands_ready(MusicCommandsFixture* p){return p->ready;}
API(music_commands_release) bool music_commands_release(MusicCommandsFixture* p){return p->release;}
API(music_commands_error) const char* music_commands_error(MusicCommandsFixture* p){return p->commands.error.c_str();}

#include "../../cpp/game/TitleFrame.hpp"
struct TitleFrameFixture final:TitleFrameServices {
 TitleMenuFixture& title;Rng random;RecordStore records;FrameScheduler scheduler;TitleFrame frame;std::array<std::shared_ptr<Replay>,3> demos;std::vector<i32> events;std::array<i32,16> fields{};
 explicit TitleFrameFixture(TitleMenuFixture& t):title(t),frame(t.state,t.progress,t.player,records,t.animations,*this,scheduler){records.reset(random);}
 static i32 hash(const std::string& text){u32 v=2166136261;for(u8 b:text){v^=b;v*=16777619;}return signed_bits(v);}
 bool release_transient_animations()override{events.push_back(1);return true;}bool clear_title_overlay()override{events.push_back(2);return true;}
 bool read_demo(i32 index,std::shared_ptr<Replay>& out)override{events.insert(events.end(),{3,index});out=demos[index];return bool(out);}
 bool begin_demo(const ReplayStartRequest& r)override{events.insert(events.end(),{4,r.stage,r.character,r.subcharacter,r.difficulty,r.selection,hash(r.filename)});return true;}
 bool queue_title_music(i32 code,i32 value,const std::string& name)override{events.insert(events.end(),{5,code,value,hash(name)});return true;}
 bool clear_current_wave()override{events.push_back(6);return true;}bool reset_replay_selection()override{events.push_back(7);return true;}bool return_practice_transition()override{events.push_back(8);return true;}
 bool update_title_menu(TitleScreen screen,const TitleFrameInput&)override{events.insert(events.end(),{9,i32(screen)});return true;}bool draw_title_menu(TitleScreen screen)override{events.insert(events.end(),{10,i32(screen)});return true;}
 bool title_exit(i32 destination)override{events.insert(events.end(),{11,destination});return true;}bool fade_out_title_music()override{events.push_back(12);return true;}
 const i32* project(){fields={frame.demo_idle,frame.demo_index,frame.saved_difficulty,frame.music_age,title.state.return_reason,i32(title.player.mode_flags),title.progress.stage,title.progress.starting_stage,title.progress.character,title.progress.subcharacter,title.progress.difficulty,frame.drawing()?1:0,records.music[0]};return fields.data();}
};
API(title_frame_create) TitleFrameFixture* title_frame_create(TitleMenuFixture* t){return new TitleFrameFixture(*t);}
API(title_frame_delete) void title_frame_delete(TitleFrameFixture* f){delete f;}
API(title_frame_configure) void title_frame_configure(TitleFrameFixture* f,i32 screen,i32 reason,i32 idle,i32 index,i32 saved,i32 music,bool alternate,i32 difficulty,float rate){f->title.state.screen=TitleScreen(screen);f->title.state.return_reason=reason;f->frame.demo_idle=idle;f->frame.demo_index=index;f->frame.saved_difficulty=saved;f->frame.music_age=music;f->frame.alternate_audio=alternate;f->title.progress.difficulty=difficulty;f->title.progress.rate=rate;}
API(title_frame_demo) bool title_frame_demo(TitleFrameFixture* f,u32 index,const u8* data,u32 size){if(index>=3)return false;f->demos[index]=std::make_shared<Replay>();return f->demos[index]->open(data,size);}
API(title_frame_update) bool title_frame_update(TitleFrameFixture* f,u32 held,u32 pressed,u32 repeated,u32 flags){f->events.clear();return f->frame.update({held,pressed,repeated,flags});}
API(title_frame_draw) bool title_frame_draw(TitleFrameFixture* f){f->events.clear();return f->frame.draw();}
API(title_frame_fields) const i32* title_frame_fields(TitleFrameFixture* f){return f->project();}
API(title_frame_events) const i32* title_frame_events(TitleFrameFixture* f){return f->events.data();}
API(title_frame_count) u32 title_frame_count(TitleFrameFixture* f){return f->events.size();}
API(title_frame_error) const char* title_frame_error(TitleFrameFixture* f){return f->frame.error.c_str();}

#include "../../cpp/game/TitlePracticeDraw.hpp"
struct TitlePracticeDrawFixture final:HudDrawServices {
 TitleFrameFixture& title;TitlePracticeDraw draw;std::vector<HudTextDraw> texts;
 explicit TitlePracticeDrawFixture(TitleFrameFixture& f):title(f),draw(f.title.state,f.title.progress,f.records){}
 bool hud_text(const HudTextDraw& text)override{texts.push_back(text);return true;}
};
API(title_practice_draw_create) TitlePracticeDrawFixture* title_practice_draw_create(TitleFrameFixture* f){return new TitlePracticeDrawFixture(*f);}
API(title_practice_draw_delete) void title_practice_draw_delete(TitlePracticeDrawFixture* f){delete f;}
API(title_practice_draw_configure) void title_practice_draw_configure(TitlePracticeDrawFixture* f,i32 phase,i32 age,i32 cursor,i32 character,i32 rank){auto& t=f->title.title;t.state.substate=phase;t.state.age.set(age);t.state.menu.cursor=cursor;t.progress.character=character;t.progress.subcharacter=0;t.progress.difficulty=rank;}
API(title_practice_draw_record) void title_practice_draw_record(TitlePracticeDrawFixture* f,i32 character,i32 rank,i32 stage,i32 score,u32 visited){auto& r=f->title.records.characters[character].practice[rank][stage];r.score=score;r.visited=u8(visited);}
API(title_practice_draw_run) bool title_practice_draw_run(TitlePracticeDrawFixture* f){f->texts.clear();return f->draw.draw(*f);}
API(title_practice_draw_count) u32 title_practice_draw_count(TitlePracticeDrawFixture* f){return f->texts.size();}
API(title_practice_draw_text) const char* title_practice_draw_text(TitlePracticeDrawFixture* f,u32 i){return f->texts[i].text.c_str();}
API(title_practice_draw_style) const HudTextStyle* title_practice_draw_style(TitlePracticeDrawFixture* f,u32 i){return &f->texts[i].style;}
API(title_practice_draw_position) const Vec3* title_practice_draw_position(TitlePracticeDrawFixture* f,u32 i){return &f->texts[i].position;}
API(title_practice_draw_error) const char* title_practice_draw_error(TitlePracticeDrawFixture* f){return f->draw.error.c_str();}

#include "../../cpp/game/TitleResultsMenu.hpp"
struct TitleResultsFixture final:TitleResultsServices,HudDrawServices,ReplayCalendarServices {
 TitleFrameFixture& frame;ItemScoreState points;TitleResultsMenu menu;std::vector<i32> events;std::vector<HudTextDraw> texts;ReplayCalendar date;std::vector<i64> timestamps;PauseScoreDetails details;std::array<i32,8> fields{};
 explicit TitleResultsFixture(TitleFrameFixture& f):frame(f),menu(f.title.state,f.title.progress,f.title.player,points,f.records,f.title.animations,*this){}
 bool sound(i32 id)override{events.insert(events.end(),{4,id});return true;}
 bool music(const std::string& wave,i32 track)override{events.insert(events.end(),{1,TitleFrameFixture::hash(wave),track});return frame.records.unlock_music(track);}
 bool release_live_replay()override{events.push_back(3);return true;}
 bool capture_score_details(PauseScoreDetails& out)override{events.push_back(2);out=details;return true;}
 bool hud_text(const HudTextDraw& text)override{texts.push_back(text);return true;}
 bool calendar(i64 timestamp,ReplayCalendar& out)override{timestamps.push_back(timestamp);out=date;return true;}
 const i32* project(){fields={menu.names.cursor,menu.names.previous,menu.names.count,menu.names.wrapping?1:0,menu.name_length,menu.name_not_required?1:0,frame.title.progress.stage,frame.title.progress.starting_stage};return fields.data();}
};
API(title_results_create) TitleResultsFixture* title_results_create(TitleFrameFixture* f){return new TitleResultsFixture(*f);}
API(title_results_delete) void title_results_delete(TitleResultsFixture* f){delete f;}
API(title_results_configure) void title_results_configure(TitleResultsFixture* f,i32 character,i32 difficulty,u32 mode,i32 score,i32 continues,i32 deaths){auto& t=f->frame.title;t.state.screen=TitleScreen::Records;t.progress.character=character;t.progress.subcharacter=0;t.progress.difficulty=difficulty;t.progress.continues=continues;t.progress.stage_deaths[0]=deaths;t.player.mode_flags=mode;f->points.score=score;}
API(title_results_records) bool title_results_records(TitleResultsFixture* f,const u8* bytes){return f->frame.records.import_blocks(bytes,bytes+5*0xa4a0);}
API(title_results_blocks) const u8* title_results_blocks(TitleResultsFixture* f,u32 settings){const auto& file=f->frame.records.export_blocks();return settings?file.settings.data():file.characters[0].data();}
API(title_results_details) PauseScoreDetails* title_results_details(TitleResultsFixture* f){return &f->details;}
API(title_results_date) ReplayCalendar* title_results_date(TitleResultsFixture* f){return &f->date;}
API(title_results_name) char* title_results_name(TitleResultsFixture* f){return f->menu.name.data();}
API(title_results_fields) const i32* title_results_fields(TitleResultsFixture* f){return f->project();}
API(title_results_choose) void title_results_choose(TitleResultsFixture* f,i32 cursor){f->menu.names.select(cursor);}
API(title_results_update) bool title_results_update(TitleResultsFixture* f,u32 pressed,u32 repeated){f->events.clear();return f->menu.update(pressed,repeated);}
API(title_results_events) const i32* title_results_events(TitleResultsFixture* f){return f->events.data();}
API(title_results_count) u32 title_results_count(TitleResultsFixture* f){return f->events.size();}
API(title_results_draw) bool title_results_draw(TitleResultsFixture* f){f->texts.clear();f->timestamps.clear();return f->menu.draw(*f,*f);}
API(title_results_text_count) u32 title_results_text_count(TitleResultsFixture* f){return f->texts.size();}
API(title_results_text) const char* title_results_text(TitleResultsFixture* f,u32 i){return f->texts[i].text.c_str();}
API(title_results_style) const HudTextStyle* title_results_style(TitleResultsFixture* f,u32 i){return &f->texts[i].style;}
API(title_results_position) const Vec3* title_results_position(TitleResultsFixture* f,u32 i){return &f->texts[i].position;}
API(title_results_timestamps) const i64* title_results_timestamps(TitleResultsFixture* f){return f->timestamps.data();}
API(title_results_timestamp_count) u32 title_results_timestamp_count(TitleResultsFixture* f){return f->timestamps.size();}
API(title_results_error) const char* title_results_error(TitleResultsFixture* f){return f->menu.error.c_str();}

#include "../../cpp/game/LegacyTextFormat.hpp"
API(legacy_decimal_tenth_text) const char* legacy_decimal_tenth_text(float value){static std::string text;text=legacy_decimal_tenth(value);return text.c_str();}

#include "../../cpp/game/TitleReplaySave.hpp"
struct TitleReplaySaveFixture final:TitleReplaySaveServices,HudDrawServices,ReplayCalendarServices {
 TitleResultsFixture& result;TitleReplaySave menu;std::array<std::shared_ptr<Replay>,25> disk{};std::shared_ptr<Replay> saved_file;std::array<u8,0xa4> live{};std::vector<i32> events;std::vector<HudTextDraw> texts;std::vector<i64> timestamps;ReplayCalendar date;i64 clock=1700000000;std::array<i32,8> fields{};
 explicit TitleReplaySaveFixture(TitleResultsFixture& f):result(f),menu(f.frame.title.state,f.frame.title.progress,f.frame.records,f.frame.title.animations,f.menu,*this){}
 bool read_slot(i32 slot,std::shared_ptr<Replay>& out)override{events.insert(events.end(),{5,slot});out=disk[slot];return true;}
 bool sound(i32 id)override{events.insert(events.end(),{4,id});return true;}
 bool prepare_live_replay(bool completed)override{events.insert(events.end(),{6,completed?1:0});std::memcpy(live.data()+0xc,&clock,8);const u32 clear=completed?8:result.frame.title.progress.stage;std::memcpy(live.data()+0x98,&clear,4);return true;}
 bool save_slot(i32 slot,const std::array<char,9>& name)override{events.insert(events.end(),{7,slot,TitleFrameFixture::hash(std::string(name.data()))});disk[slot]=saved_file;return true;}
 bool release_live_replay()override{events.push_back(3);return true;}
 bool music(const std::string& name,i32 track)override{events.insert(events.end(),{1,TitleFrameFixture::hash(name),track});return result.frame.records.unlock_music(track);}
 bool hud_text(const HudTextDraw& text)override{texts.push_back(text);return true;}
 bool calendar(i64 time,ReplayCalendar& out)override{timestamps.push_back(time);out=date;return true;}
 const i32* project(){auto& n=result.menu;auto& p=result.frame.title.progress;fields={n.names.cursor,n.names.previous,n.names.count,n.names.wrapping?1:0,n.name_length,menu.selected,p.stage,p.starting_stage};return fields.data();}
};
API(title_replay_save_create) TitleReplaySaveFixture* title_replay_save_create(TitleResultsFixture* f){return new TitleReplaySaveFixture(*f);}
API(title_replay_save_delete) void title_replay_save_delete(TitleReplaySaveFixture* f){delete f;}
API(title_replay_save_disk) bool title_replay_save_disk(TitleReplaySaveFixture* f,i32 slot,const u8* bytes,u32 size){auto file=std::make_shared<Replay>();if(!file->open(bytes,size))return false;if(slot<0)f->saved_file=std::move(file);else if(slot<25)f->disk[slot]=std::move(file);else return false;return true;}
API(title_replay_save_live) u8* title_replay_save_live(TitleReplaySaveFixture* f){return f->live.data();}
API(title_replay_save_date) ReplayCalendar* title_replay_save_date(TitleReplaySaveFixture* f){return &f->date;}
API(title_replay_save_update) bool title_replay_save_update(TitleReplaySaveFixture* f,u32 pressed,u32 repeated){f->events.clear();return f->menu.update(pressed,repeated);}
API(title_replay_save_fields) const i32* title_replay_save_fields(TitleReplaySaveFixture* f){return f->project();}
API(title_replay_save_events) const i32* title_replay_save_events(TitleReplaySaveFixture* f){return f->events.data();}
API(title_replay_save_count) u32 title_replay_save_count(TitleReplaySaveFixture* f){return f->events.size();}
API(title_replay_save_draw) bool title_replay_save_draw(TitleReplaySaveFixture* f){f->texts.clear();f->timestamps.clear();return f->menu.draw(*f,*f,&f->live);}
API(title_replay_save_text_count) u32 title_replay_save_text_count(TitleReplaySaveFixture* f){return f->texts.size();}
API(title_replay_save_text) const char* title_replay_save_text(TitleReplaySaveFixture* f,u32 i){return f->texts[i].text.c_str();}
API(title_replay_save_style) const HudTextStyle* title_replay_save_style(TitleReplaySaveFixture* f,u32 i){return &f->texts[i].style;}
API(title_replay_save_position) const Vec3* title_replay_save_position(TitleReplaySaveFixture* f,u32 i){return &f->texts[i].position;}
API(title_replay_save_timestamps) const i64* title_replay_save_timestamps(TitleReplaySaveFixture* f){return f->timestamps.data();}
API(title_replay_save_timestamp_count) u32 title_replay_save_timestamp_count(TitleReplaySaveFixture* f){return f->timestamps.size();}
API(title_replay_save_error) const char* title_replay_save_error(TitleReplaySaveFixture* f){return f->menu.error.c_str();}

#include "../../cpp/game/TitleCheat.hpp"
struct TitleCheatFixture {Rng random;RecordStore records;TitleCheat cheat{records};TitleKeyboard keyboard;bool unlocked=false;TitleCheatFixture(){records.reset(random);}};
API(title_cheat_create) TitleCheatFixture* title_cheat_create(){return new TitleCheatFixture;}
API(title_cheat_delete) void title_cheat_delete(TitleCheatFixture* f){delete f;}
API(title_cheat_keyboard) u8* title_cheat_keyboard(TitleCheatFixture* f){return f->keyboard.keys.data();}
API(title_cheat_records) bool title_cheat_records(TitleCheatFixture* f,const u8* data){return f->records.import_blocks(data,data+5*0xa4a0);}
API(title_cheat_blocks) const u8* title_cheat_blocks(TitleCheatFixture* f,u32 settings){const auto& file=f->records.export_blocks();return settings?file.settings.data():file.characters[0].data();}
API(title_cheat_unlock) bool title_cheat_unlock(TitleCheatFixture* f){return f->records.unlock_all();}
API(title_cheat_configure) void title_cheat_configure(TitleCheatFixture* f,i32 sequence,i32 idle){f->cheat.sequence=sequence;f->cheat.idle=idle;}
API(title_cheat_update) bool title_cheat_update(TitleCheatFixture* f,u32 pressed,i32 format){f->keyboard.format=format;return f->cheat.update(pressed,f->keyboard,f->unlocked);}
API(title_cheat_sequence) i32 title_cheat_sequence(TitleCheatFixture* f){return f->cheat.sequence;}
API(title_cheat_idle) i32 title_cheat_idle(TitleCheatFixture* f){return f->cheat.idle;}
API(title_cheat_unlocked) bool title_cheat_unlocked(TitleCheatFixture* f){return f->unlocked;}
API(title_cheat_current) const u8* title_cheat_current(TitleCheatFixture* f){return f->cheat.raw_current().data();}
API(title_cheat_previous) const u8* title_cheat_previous(TitleCheatFixture* f){return f->cheat.raw_previous().data();}
API(title_cheat_error) const char* title_cheat_error(TitleCheatFixture* f){return f->cheat.error.c_str();}

#include "../../cpp/game/TitlePlayerData.hpp"
struct TitlePlayerDataFixture final:TitlePlayerDataServices,HudDrawServices,ReplayCalendarServices {
 TitleResultsFixture& result;TitlePlayerData menu;TitleKeyboard keys;std::vector<i32> sounds;std::vector<DialogueText> bitmaps;std::vector<HudTextDraw> texts;std::vector<i64> timestamps;ReplayCalendar date;MenuCursorFixture projection;std::array<i32,3> fields{};
 explicit TitlePlayerDataFixture(TitleResultsFixture& r):result(r),menu(r.frame.title.state,r.frame.records,r.frame.title.animations,*this){}
 bool sound(i32 id)override{sounds.push_back(id);return true;}
 bool text(AnmVm&,const DialogueText& value)override{bitmaps.push_back(value);return true;}
 bool keyboard(TitleKeyboard& out)override{out=keys;return true;}
 bool hud_text(const HudTextDraw& value)override{texts.push_back(value);return true;}
 bool calendar(i64 time,ReplayCalendar& out)override{timestamps.push_back(time);out=date;return true;}
};
API(title_player_data_create) TitlePlayerDataFixture* title_player_data_create(TitleResultsFixture* r){return new TitlePlayerDataFixture(*r);}
API(title_player_data_delete) void title_player_data_delete(TitlePlayerDataFixture* f){delete f;}
API(title_player_data_update) bool title_player_data_update(TitlePlayerDataFixture* f,u32 pressed,u32 repeated){f->sounds.clear();f->bitmaps.clear();return f->menu.update(pressed,repeated);}
API(title_player_data_cursors) const u8* title_player_data_cursors(TitlePlayerDataFixture* f,u32 pages){f->projection.menu=pages?f->menu.pages:f->menu.ranks;return menu_cursor_bytes(&f->projection);}
API(title_player_data_fields) const i32* title_player_data_fields(TitlePlayerDataFixture* f){f->fields={f->menu.displayed_spells,f->menu.cheat.sequence,f->menu.cheat.idle};return f->fields.data();}
API(title_player_data_sound_count) u32 title_player_data_sound_count(TitlePlayerDataFixture* f){return f->sounds.size();}
API(title_player_data_sounds) const i32* title_player_data_sounds(TitlePlayerDataFixture* f){return f->sounds.data();}
API(title_player_data_bitmap_count) u32 title_player_data_bitmap_count(TitlePlayerDataFixture* f){return f->bitmaps.size();}
API(title_player_data_bitmap) const char* title_player_data_bitmap(TitlePlayerDataFixture* f,u32 i){return f->bitmaps[i].bytes.c_str();}
API(title_player_data_bitmap_handle) u32 title_player_data_bitmap_handle(TitlePlayerDataFixture* f,u32 i){return f->bitmaps[i].handle;}
API(title_player_data_bitmap_color) u32 title_player_data_bitmap_color(TitlePlayerDataFixture* f,u32 i){return f->bitmaps[i].color;}
API(title_player_data_keyboard) TitleKeyboard* title_player_data_keyboard(TitlePlayerDataFixture* f){return &f->keys;}
API(title_player_data_date) ReplayCalendar* title_player_data_date(TitlePlayerDataFixture* f){return &f->date;}
API(title_player_data_draw) bool title_player_data_draw(TitlePlayerDataFixture* f){f->texts.clear();f->timestamps.clear();return f->menu.draw(*f,*f);}
API(title_player_data_text_count) u32 title_player_data_text_count(TitlePlayerDataFixture* f){return f->texts.size();}
API(title_player_data_text) const char* title_player_data_text(TitlePlayerDataFixture* f,u32 i){return f->texts[i].text.c_str();}
API(title_player_data_style) const HudTextStyle* title_player_data_style(TitlePlayerDataFixture* f,u32 i){return &f->texts[i].style;}
API(title_player_data_position) const Vec3* title_player_data_position(TitlePlayerDataFixture* f,u32 i){return &f->texts[i].position;}
API(title_player_data_timestamps) const i64* title_player_data_timestamps(TitlePlayerDataFixture* f){return f->timestamps.data();}
API(title_player_data_timestamp_count) u32 title_player_data_timestamp_count(TitlePlayerDataFixture* f){return f->timestamps.size();}
API(title_player_data_error) const char* title_player_data_error(TitlePlayerDataFixture* f){return f->menu.error.c_str();}

#include "../../cpp/game/TitleScene.hpp"
struct TitleSceneFixture final:TitleScenePlatform {
 AnmManagerFixture& bank;SessionState progress;PlayerLifeSession player;ItemScoreState points;RecordStore records;Rng random;TitleSelectionSettings selection;TitleAudioSettings audio;TitleControllerSettings bindings;MusicComments comments;FrameScheduler scheduler;std::unique_ptr<TitleScene> scene;std::vector<i32> events;std::vector<HudTextDraw> captions;std::vector<DialogueText> bitmaps;std::array<std::shared_ptr<Replay>,3> demos{};std::array<std::shared_ptr<Replay>,25> slots{};TitleKeyboard keys;std::array<i32,14> fields{};bool pending_page=false;bool checkpoint=false;i32 resume_stage=1,destination=-1;
 TitleSceneFixture(AnmManagerFixture& a,const u8* bytes,u32 size):bank(a){records.reset(random);if(comments.open(bytes,size)){scene=std::make_unique<TitleScene>(progress,player,points,records,selection,audio,bindings,comments,a.manager,*this,scheduler);scene->state.return_reason=0;}}
 bool sound(i32 id)override{events.insert(events.end(),{1,id});return true;}
 bool text(AnmVm&,const DialogueText& value)override{bitmaps.push_back(value);return true;}
 bool hud_text(const HudTextDraw& value)override{captions.push_back(value);return true;}
 bool keyboard(TitleKeyboard& value)override{value=keys;return true;}
 bool calendar(i64,ReplayCalendar& value)override{value={2026,10,2,20,36};return true;}
 bool capture_score_details(PauseScoreDetails& value)override{value={{1700000000,0},600,600};return true;}
 bool music(const std::string& name)override{events.insert(events.end(),{2,TitleFrameFixture::hash(name)});return true;}
 bool music(const std::string& name,i32 track)override{events.insert(events.end(),{3,track,TitleFrameFixture::hash(name)});return records.unlock_music(track);}
 bool music_command(i32 code)override{events.insert(events.end(),{4,code});return true;}bool start_title_music()override{events.insert(events.end(),{4,0});return true;}
 bool volume(i32 music,i32 sound,i32 attenuation)override{events.insert(events.end(),{5,music,sound,attenuation});return true;}
 bool save(const TitleControllerSettings&)override{events.push_back(6);return true;}
 bool checkpoint_available(i32,i32,bool& value)override{value=checkpoint;return true;}
 bool checkpoint_stage(i32,i32,i32& value)override{value=resume_stage;return true;}
 bool reset_resume_selection()override{events.push_back(7);return true;}
 bool prepare_game_music()override{events.push_back(8);return true;}
 bool begin_transition(u32& value)override{value=0;events.push_back(9);return true;}
 bool start_game(i32 stage)override{events.insert(events.end(),{10,stage});destination=13;return true;}
 bool request_catalog()override{events.push_back(11);scene->replay_catalog_ready();return true;}
 bool release_catalog()override{events.push_back(12);return true;}
 bool fade_music(float)override{events.push_back(13);return true;}
 bool transition_size(float,float)override{return true;}
 bool start_replay(const ReplayStartRequest& r)override{events.insert(events.end(),{14,r.stage});destination=13;return true;}
 bool clear_page()override{events.push_back(15);return true;}
 bool request_page(i32 page)override{events.insert(events.end(),{16,page});pending_page=true;return true;}
 bool upload_page(i32 page)override{events.insert(events.end(),{17,page});return true;}
 bool read_slot(i32 slot,std::shared_ptr<Replay>& value)override{value=slots[slot];return true;}
 bool prepare_live_replay(bool)override{return true;}
 bool save_slot(i32,const std::array<char,9>&)override{return true;}
 bool release_live_replay()override{events.push_back(18);return true;}
 bool release_transient_animations()override{events.push_back(19);return true;}
 bool clear_title_overlay()override{events.push_back(20);return true;}
 bool read_demo(i32 index,std::shared_ptr<Replay>& value)override{events.insert(events.end(),{21,index});value=demos[index];return bool(value);}
 bool begin_demo(const ReplayStartRequest& r)override{events.insert(events.end(),{22,r.stage});destination=13;return true;}
 bool queue_title_music(i32 code,i32 value,const std::string& name)override{events.insert(events.end(),{23,code,value,TitleFrameFixture::hash(name)});return true;}
 bool clear_current_wave()override{events.push_back(24);return true;}
 bool reset_replay_selection()override{events.push_back(25);return true;}
 bool return_practice_transition()override{events.push_back(26);return true;}
 bool title_exit(i32 value)override{events.insert(events.end(),{27,value});destination=value;return true;}
 bool fade_out_title_music()override{events.push_back(28);return true;}
 const i32* project(){auto& s=scene->state;fields={i32(s.screen),s.substate,s.age.current,s.menu.cursor,s.menu.count,s.menu.depth,progress.character,progress.difficulty,progress.stage,i32(player.mode_flags),destination,scene->frame.demo_idle,scene->frame.music_age,i32(s.flags)};return fields.data();}
};
API(title_scene_create) TitleSceneFixture* title_scene_create(AnmManagerFixture* a,const u8* bytes,u32 size){return new TitleSceneFixture(*a,bytes,size);}
API(title_scene_delete) void title_scene_delete(TitleSceneFixture* f){delete f;}
API(title_scene_step) bool title_scene_step(TitleSceneFixture* f,u32 held,u32 pressed,u32 repeated){if(!f->scene)return false;f->events.clear();f->captions.clear();f->bitmaps.clear();if(f->pending_page){f->pending_page=false;if(!f->scene->manual_page_ready())return false;}f->scene->controls({held,pressed,repeated,0});return f->scheduler.update()>=0&&f->scheduler.draw()>=0;}
API(title_scene_fields) const i32* title_scene_fields(TitleSceneFixture* f){return f->project();}
API(title_scene_state) TitleState* title_scene_state(TitleSceneFixture* f){return &f->scene->state;}
API(title_scene_select) void title_scene_select(TitleSceneFixture* f,i32 cursor){f->scene->state.menu.select(cursor);}
API(title_scene_screen) void title_scene_screen(TitleSceneFixture* f,i32 screen){f->scene->state.change_screen(TitleScreen(screen));}
API(title_scene_unlock) bool title_scene_unlock(TitleSceneFixture* f){return f->records.unlock_all();}
API(title_scene_event_count) u32 title_scene_event_count(TitleSceneFixture* f){return f->events.size();}
API(title_scene_events) const i32* title_scene_events(TitleSceneFixture* f){return f->events.data();}
API(title_scene_caption_count) u32 title_scene_caption_count(TitleSceneFixture* f){return f->captions.size();}
API(title_scene_bitmap_count) u32 title_scene_bitmap_count(TitleSceneFixture* f){return f->bitmaps.size();}
API(title_scene_error) const char* title_scene_error(TitleSceneFixture* f){return !f->scene?f->comments.error.c_str():f->scene->error.empty()?f->scene->frame.error.c_str():f->scene->error.c_str();}

#include "../../cpp/game/ScreenViews.hpp"
API(screen_view_prepare) void screen_view_prepare(const GraphicsViewport* viewport,float fov,ScreenView* out){*out=screen_view(*viewport,fov);}
API(screen_view_create) ScreenView* screen_view_create(){return new ScreenView;}
API(screen_view_delete) void screen_view_delete(ScreenView* value){delete value;}
API(screen_view_matrix) const Matrix4* screen_view_matrix(ScreenView* value,u32 kind){return kind?&value->camera.projection:&value->camera.view;}
struct AnmDrawScheduleFixture final:AnmDrawServices {
 AnmManagerFixture& animations;AnmRendererFixture& rendering;FrameScheduler scheduler;AnmDrawSchedule draw;std::vector<i32> events;std::array<u32,4> layers{};
 AnmDrawScheduleFixture(AnmManagerFixture& a,AnmRendererFixture& r):animations(a),rendering(r),draw(scheduler,a.manager,r.renderer,r.graphics,*this){for(i32 layer=0;layer<42;layer++){auto* vm=a.manager.registry.allocate();vm->environment=&a.environment;vm->visual.flags=3|(16u<<25);vm->visual.layer=layer;vm->visual.sprite_size={2,2};vm->variables.position={float(layer+10),10,0};a.manager.registry.submit(*vm,layer>=35?4:0);}}
 bool camera(DrawCamera id,bool refresh)override{events.insert(events.end(),{i32(id),i32(refresh)});AnmCamera camera;camera.view.identity();camera.projection.identity();rendering.renderer.set_camera(camera);rendering.renderer.offset={float(i32(id)*7+3),float(i32(id)*11+5)};return true;}
};
API(anm_draw_schedule_create) AnmDrawScheduleFixture* anm_draw_schedule_create(AnmManagerFixture* a,AnmRendererFixture* r){return new AnmDrawScheduleFixture(*a,*r);}
API(anm_draw_schedule_delete) void anm_draw_schedule_delete(AnmDrawScheduleFixture* f){delete f;}
API(anm_draw_schedule_draw) bool anm_draw_schedule_draw(AnmDrawScheduleFixture* f){f->events.clear();const auto result=f->scheduler.draw();f->rendering.renderer.flush();return result>=0;}
API(anm_draw_schedule_events) const i32* anm_draw_schedule_events(AnmDrawScheduleFixture* f){return f->events.data();}
API(anm_draw_schedule_count) u32 anm_draw_schedule_count(AnmDrawScheduleFixture* f){return f->events.size();}
API(anm_draw_schedule_error) const char* anm_draw_schedule_error(AnmDrawScheduleFixture* f){return f->draw.error.c_str();}

API(screen_views_prepare) void screen_views_prepare(AnmRenderer* renderer,const AnmEnvironment* environment,u32 index,ScreenView* out){ScreenViews views(*renderer,*environment);*out=views.view(DrawCamera(index));}
API(screen_view_viewport) const GraphicsViewport* screen_view_viewport(ScreenView* value){return &value->viewport;}
API(anm_environment_dimensions) u32* anm_environment_dimensions(AnmEnvironment* value){return &value->screen_width;}
API(music_commands_preload_track) bool music_commands_preload_track(MusicCommandsFixture* p,i32 slot,const char* stem){return p->commands.preload_track(slot,stem);}

#include "../../cpp/game/EndingScript.hpp"
struct EndingScriptFixture final:EndingServices {
 MessageProgram program;EndingScript ending{*this};std::vector<i32> events;std::vector<DialogueText> texts;u32 next=100;
 static i32 hash(const std::string& name){u32 h=2166136261;for(u8 ch:name){h^=ch;h*=16777619;}return signed_bits(h);}
 bool create_text(i32 script,u32& h)override{h=next++;events.insert(events.end(),{1,script,signed_bits(h)});return true;}
 bool text(u32 h,const std::string& value,u32 color)override{texts.push_back({h,value,0,0,color});events.insert(events.end(),{2,signed_bits(h),signed_bits(color),hash(value)});return true;}
 bool interrupt(u32 h,i32 label)override{events.insert(events.end(),{3,signed_bits(h),label});return true;}
 bool retire(u32& h)override{events.insert(events.end(),{4,signed_bits(h)});h=0;return true;}
 bool load_animation(i32 bank,const std::string& name)override{events.insert(events.end(),{5,bank,hash(name)});return true;}
 bool create_animation(i32 bank,i32 script,u32& h)override{h=next++;events.insert(events.end(),{6,bank,script,signed_bits(h)});return true;}
 bool loading_overlay()override{events.push_back(7);return true;}bool end_loading_overlay()override{events.push_back(8);return true;}
 bool read_staff(const std::string& name,MessageProgram& out)override{events.insert(events.end(),{9,hash(name)});return out.open(staff_bytes.data(),staff_bytes.size());}
 bool prepare_music(const std::string& name)override{events.insert(events.end(),{10,hash(name)});return true;}bool start_music(i32 track)override{events.insert(events.end(),{11,track});return true;}
 bool fade_music(i32 seconds)override{events.insert(events.end(),{12,seconds});return true;}bool sound(i32 id)override{events.insert(events.end(),{13,id});return true;}
 bool shake(i32 kind,i32 amount)override{events.insert(events.end(),{14,kind,amount});return true;}bool reset_caption_state()override{events.push_back(15);return true;}
 std::vector<u8> staff_bytes;std::array<i32,29> projected{};
 const i32* fields(){auto& s=ending.state;projected[0]=signed_bits(s.instruction_offset);projected[1]=signed_bits(s.flags);projected[2]=s.line;projected[3]=signed_bits(s.color);for(u32 i=0;i<5;i++)projected[4+i]=signed_bits(s.text_handles[i]);for(u32 i=0;i<4;i++)projected[9+i]=s.banks[i];for(u32 i=0;i<16;i++)projected[13+i]=signed_bits(s.sprites[i]);return projected.data();}
};
API(ending_script_create) EndingScriptFixture* ending_script_create(){return new EndingScriptFixture;}
API(ending_script_delete) void ending_script_delete(EndingScriptFixture* p){delete p;}
API(ending_script_initialize) bool ending_script_initialize(EndingScriptFixture* p,const u8* data,u32 size){p->events.clear();p->texts.clear();if(!p->program.open(data,size)||p->program.scripts.empty())return false;return p->ending.initialize(p->program.scripts[0]);}
API(ending_script_step) bool ending_script_step(EndingScriptFixture* p,u32 held,u32 pressed,u32 held_frames,float rate){p->events.clear();p->texts.clear();return p->ending.step({held,pressed,held_frames,rate});}
API(ending_script_configure) void ending_script_configure(EndingScriptFixture* p,i32 difficulty,i32 continues,u32 mode,u32 first){p->ending.run={difficulty,0,mode,first,continues};}
API(ending_script_staff) void ending_script_staff(EndingScriptFixture* p,const u8* data,u32 size){p->staff_bytes.assign(data,data+size);}
API(ending_script_ready) bool ending_script_ready(EndingScriptFixture* p,i32 bank){p->events.clear();p->texts.clear();return p->ending.animation_ready(bank);}
API(ending_script_events) const i32* ending_script_events(EndingScriptFixture* p){return p->events.data();}
API(ending_script_event_count) u32 ending_script_event_count(EndingScriptFixture* p){return p->events.size();}
API(ending_script_fields) const i32* ending_script_fields(EndingScriptFixture* p){return p->fields();}
API(ending_script_timer) const Timer* ending_script_timer(EndingScriptFixture* p,u32 kind){return kind==0?&p->ending.state.age:kind==1?&p->ending.state.clock:&p->ending.state.wait;}
API(ending_script_finished) bool ending_script_finished(EndingScriptFixture* p){return p->ending.finished;}
API(ending_script_error) const char* ending_script_error(EndingScriptFixture* p){return p->ending.error.empty()?p->program.error.c_str():p->ending.error.c_str();}
API(ending_script_text_count) u32 ending_script_text_count(EndingScriptFixture* p){return p->texts.size();}
API(ending_script_text_bytes) const char* ending_script_text_bytes(EndingScriptFixture* p,u32 index){return p->texts[index].bytes.c_str();}
API(records_begin_ending) bool records_begin_ending(RecordStoreFixture* p,i32 index,i32 difficulty,u32* first){return p->records.begin_ending(index,difficulty,*first);}
API(ending_index) i32 ending_index_export(i32 character,i32 subcharacter,u32 mode,i32 continues){return ending_index(character,subcharacter,mode,continues);}

#include "../../cpp/game/EndingFrame.hpp"
struct EndingFrameFixture final:EndingDestination {
 EndingScriptFixture& owner;FrameScheduler scheduler;EndingFrame frame;i32 destination=-1;
 explicit EndingFrameFixture(EndingScriptFixture& value):owner(value),frame(value.ending,*this,scheduler){}
 bool ending_destination(i32 value)override{destination=value;return true;}
};
API(ending_frame_create) EndingFrameFixture* ending_frame_create(EndingScriptFixture* p){return new EndingFrameFixture(*p);}
API(ending_frame_delete) void ending_frame_delete(EndingFrameFixture* p){delete p;}
API(ending_frame_update) i32 ending_frame_update(EndingFrameFixture* p,u32 held,u32 pressed,u32 held_frames,float rate,u32 system){p->destination=-1;p->owner.events.clear();p->owner.texts.clear();p->frame.controls({held,pressed,held_frames,rate},system);return p->frame.update();}
API(ending_frame_fields) i32 ending_frame_fields(EndingFrameFixture* p,u32 kind){return kind?p->destination:p->frame.frames;}
API(ending_frame_error) const char* ending_frame_error(EndingFrameFixture* p){return p->frame.error.c_str();}

#include "../../cpp/game/EndingScene.hpp"
struct EndingSceneFixture final:EndingScenePlatform {
 AnmManagerFixture& animation;Rng random;RecordStore records;SessionState progress;FrameScheduler scheduler;
 std::array<FrameCallback,2> updates;std::unique_ptr<EndingScene> scene;
 std::unordered_map<std::string,std::vector<u8>> messages;std::vector<DialogueText> texts;std::vector<i32> events;
 u32 loading_handle=0;std::string pending_name;i32 pending_bank=-1,destination=-1;std::string error;
 explicit EndingSceneFixture(AnmManagerFixture& a):animation(a){records.reset(random);for(u32 i=0;i<2;i++){auto& cb=updates[i];cb.owner=this;cb.enabled=true;cb.run=i?[](void* p){return static_cast<EndingSceneFixture*>(p)->animation.manager.update(false)?1:5;}:[](void* p){return static_cast<EndingSceneFixture*>(p)->animation.manager.update(true)?1:5;};scheduler.add(cb,FramePass::Update,i?34:9);}}
 ~EndingSceneFixture(){scene.reset();for(auto& cb:updates)scheduler.remove(cb);}
 bool read_message(const std::string& name,MessageProgram& out)override{const auto at=messages.find(name);events.insert(events.end(),{1,EndingScriptFixture::hash(name)});return at!=messages.end()&&out.open(at->second.data(),at->second.size());}
 bool text(AnmVm&,const DialogueText& request)override{texts.push_back(request);events.insert(events.end(),{2,signed_bits(request.handle),signed_bits(request.color),EndingScriptFixture::hash(request.bytes)});return true;}
 bool request_animation(i32 bank,const std::string& name)override{pending_bank=bank;pending_name=name;events.insert(events.end(),{3,bank,EndingScriptFixture::hash(name)});return true;}
 bool prepare_animation(AnmResource&)override{return true;}bool cancel_animation_request()override{pending_bank=-1;pending_name.clear();return true;}
 bool loading_overlay()override{if(!loading_handle)loading_handle=animation.manager.create(5,17,-1,0,{960,784,0});return loading_handle!=0;}
 bool end_loading_overlay()override{if(!animation.manager.interrupt(loading_handle,1))return false;loading_handle=0;return true;}
 bool prepare_music(const std::string& name)override{events.insert(events.end(),{4,EndingScriptFixture::hash(name)});return true;}bool start_music(i32 track)override{events.insert(events.end(),{5,track});return true;}
 bool fade_music(i32 seconds)override{events.insert(events.end(),{6,seconds});return true;}bool sound(i32 id)override{events.insert(events.end(),{7,id});return true;}
 bool shake(i32 kind,i32 amount)override{events.insert(events.end(),{8,kind,amount});return true;}bool reset_caption_state()override{return true;}
 bool ending_destination(i32 value)override{destination=value;return true;}
 bool initialize(i32 character,i32 difficulty,i32 continues,u32 mode){progress.character=character;progress.difficulty=difficulty;progress.continues=continues;scene=std::make_unique<EndingScene>(animation.manager,records,*this,scheduler);return scene->initialize(progress,mode,continues);}
 bool step(u32 held,u32 pressed,u32 held_frames,float rate,u32 flags){events.clear();texts.clear();animation.manager.rate=rate;scene->controls({held,pressed,held_frames,rate},flags);if(scheduler.update()<0){error=scene->error.empty()?scene->frame.error.empty()?animation.manager.error:scene->frame.error:scene->error;return false;}return true;}
};
API(ending_scene_create) EndingSceneFixture* ending_scene_create(AnmManagerFixture* a){return new EndingSceneFixture(*a);}
API(ending_scene_delete) void ending_scene_delete(EndingSceneFixture* f){delete f;}
API(ending_scene_message) void ending_scene_message(EndingSceneFixture* f,const char* name,const u8* bytes,u32 size){f->messages[name].assign(bytes,bytes+size);}
API(ending_scene_initialize) bool ending_scene_initialize(EndingSceneFixture* f,i32 character,i32 difficulty,i32 continues,u32 mode){return f->initialize(character,difficulty,continues,mode);}
API(ending_scene_step) bool ending_scene_step(EndingSceneFixture* f,u32 held,u32 pressed,u32 held_frames,float rate,u32 flags){return f->step(held,pressed,held_frames,rate,flags);}
API(ending_scene_error) const char* ending_scene_error(EndingSceneFixture* f){return f->error.empty()?f->scene->error.c_str():f->error.c_str();}
API(ending_scene_pending_bank) i32 ending_scene_pending_bank(EndingSceneFixture* f){return f->pending_bank;}
API(ending_scene_pending_name) const char* ending_scene_pending_name(EndingSceneFixture* f){return f->pending_name.c_str();}
API(ending_scene_ready) bool ending_scene_ready(EndingSceneFixture* f,const u8* bytes,u32 size){const auto bank=f->pending_bank;f->pending_bank=-1;f->pending_name.clear();return f->scene->animation_ready(bank,bytes,size);}
API(ending_scene_state) EndingState* ending_scene_state(EndingSceneFixture* f){return &f->scene->script.state;}
API(ending_scene_timer) const Timer* ending_scene_timer(EndingSceneFixture* f,u32 kind){auto& s=f->scene->script.state;return kind==0?&s.age:kind==1?&s.clock:&s.wait;}
API(ending_scene_field) i32 ending_scene_field(EndingSceneFixture* f,u32 kind){auto& s=f->scene->script.state;switch(kind){case 0:return signed_bits(s.instruction_offset);case 1:return signed_bits(s.flags);case 2:return s.line;case 3:return signed_bits(s.color);case 4:return f->scene->frame.frames;case 5:return f->destination;default:return signed_bits(f->scene->script.run.first_seen_flags);}}
API(ending_scene_handles) const u32* ending_scene_handles(EndingSceneFixture* f,u32 kind){return kind?f->scene->script.state.sprites.data():f->scene->script.state.text_handles.data();}
API(ending_scene_music) const u8* ending_scene_music(EndingSceneFixture* f){return f->records.music.data();}
API(ending_scene_events) const i32* ending_scene_events(EndingSceneFixture* f){return f->events.data();}
API(ending_scene_event_count) u32 ending_scene_event_count(EndingSceneFixture* f){return f->events.size();}
API(ending_scene_text_count) u32 ending_scene_text_count(EndingSceneFixture* f){return f->texts.size();}
API(ending_scene_text_bytes) const char* ending_scene_text_bytes(EndingSceneFixture* f,u32 index){return f->texts[index].bytes.c_str();}

API(ending_scene_seen) void ending_scene_seen(EndingSceneFixture* f,u32 ending,u32 staff){f->records.endings.fill(u8(ending));f->records.ending_seen=u8(staff);}

#include "../../cpp/game/AsciiFrame.hpp"
struct AsciiFrameFixture final:AnmDrawServices {
 AsciiTextFixture& captions;AnmRendererFixture& rendering;ScreenViews views;FrameScheduler scheduler;AsciiFrame frame;std::vector<i32> cameras;
 AsciiFrameFixture(AsciiTextFixture& t,AnmRendererFixture& r,AnmManagerFixture& a):captions(t),rendering(r),views(r.renderer,a.environment),frame(scheduler,t.text,r.renderer,*this){}
 bool camera(DrawCamera index,bool refresh)override{cameras.push_back(i32(index));return views.camera(index,refresh);}
};
API(ascii_frame_create) AsciiFrameFixture* ascii_frame_create(AsciiTextFixture* t,AnmRendererFixture* r,AnmManagerFixture* a){return new AsciiFrameFixture(*t,*r,*a);}
API(ascii_frame_delete) void ascii_frame_delete(AsciiFrameFixture* p){delete p;}
API(ascii_frame_update) i32 ascii_frame_update(AsciiFrameFixture* p){return p->scheduler.update();}
API(ascii_frame_draw) i32 ascii_frame_draw(AsciiFrameFixture* p){p->cameras.clear();return p->scheduler.draw();}
API(ascii_frame_camera) void ascii_frame_camera(AsciiFrameFixture* p,i32 id){p->views.camera(DrawCamera(id),false);}
API(ascii_frame_cameras) const i32* ascii_frame_cameras(AsciiFrameFixture* p){return p->cameras.data();}
API(ascii_frame_camera_count) u32 ascii_frame_camera_count(AsciiFrameFixture* p){return p->cameras.size();}
API(ascii_frame_frames) i32 ascii_frame_frames(AsciiFrameFixture* p){return p->frame.frames;}
API(ascii_frame_error) const char* ascii_frame_error(AsciiFrameFixture* p){return p->frame.error.c_str();}

API(item_manager_draw) u32 item_manager_draw(ItemManagerFixture* p,AnmRendererFixture* r){return p->manager.draw(r->renderer);}
API(bullet_scene_draw) u32 bullet_scene_draw(BulletSceneFixture* p,AnmRendererFixture* r){return p->scene.draw(r->renderer);}
API(bullet_visual_draw) u32 bullet_visual_draw(BulletVisualFixture* p,AnmRendererFixture* r){return p->visual.draw(r->renderer);}

API(anm_renderer_flush) void anm_renderer_flush(AnmRendererFixture* p){p->renderer.flush();}
API(bullet_scene_overlay) AnmVm* bullet_scene_overlay(BulletSceneFixture* p,u32 slot){auto* visual=p->scene.visual(slot);return visual?&visual->overlay:nullptr;}

API(player_draw) u32 player_draw(PlayerFixture* p,AnmRendererFixture* r){return p->player.draw(r->renderer);}

#include "../../cpp/game/SpellDraw.hpp"
struct SpellDrawFixture final:HudDrawServices {
 AnmManagerFixture& animations;SpellCardFixture spell;RecordStore records;SpellDrawContext context;std::vector<HudTextDraw> text;std::string error;
 explicit SpellDrawFixture(AnmManagerFixture& a):animations(a){}
 bool hud_text(const HudTextDraw& value)override{text.push_back(value);return true;}
};
API(spell_draw_create) SpellDrawFixture* spell_draw_create(AnmManagerFixture* a){return new SpellDrawFixture(*a);}
API(spell_draw_delete) void spell_draw_delete(SpellDrawFixture* p){delete p;}
API(spell_draw_spell) SpellCardFixture* spell_draw_spell(SpellDrawFixture* p){return &p->spell;}
API(spell_draw_style) HudTextStyle* spell_draw_style(SpellDrawFixture* p){return &p->context.style;}
API(spell_draw_configure) void spell_draw_configure(SpellDrawFixture* p,i32 character,i32 subcharacter,u32 flags,i32 captures,i32 attempts){p->context.character=character;p->context.subcharacter=subcharacter;p->context.mode_flags=flags;auto& record=p->records.characters[character+subcharacter].modes[(flags&0x300)==0?1:0].spells[p->spell.spell.identifier];record.captures[(flags&0x30)==0x20?1:0]=captures;record.attempts[(flags&0x30)==0x20?1:0]=attempts;}
API(spell_draw_step) u32 spell_draw_step(SpellDrawFixture* p){p->text.clear();return draw_spell_card(p->spell.spell,p->animations.manager,p->records,p->context,*p,p->error);}
API(spell_draw_count) u32 spell_draw_count(SpellDrawFixture* p){return p->text.size();}
API(spell_draw_request) const HudTextDraw* spell_draw_request(SpellDrawFixture* p,u32 i){return i<p->text.size()?&p->text[i]:nullptr;}
API(spell_draw_text) const char* spell_draw_text(SpellDrawFixture* p,u32 i){return i<p->text.size()?p->text[i].text.c_str():"";}
API(spell_draw_error) const char* spell_draw_error(SpellDrawFixture* p){return p->error.c_str();}

#include "../../cpp/game/ScreenCompositor.hpp"
struct ScreenCompositorFixture {
 AnmManagerFixture& animation;AnmRendererFixture& render;FrameScheduler scheduler;ScreenViews views;ScreenCompositor compositor;
 ScreenCompositorFixture(AnmManagerFixture& a,AnmRendererFixture& r):animation(a),render(r),views(r.renderer,a.environment),compositor(scheduler,a.manager,a.environment,r.renderer,r.graphics,views){r.graphics.targets_enabled=true;compositor.main={a.manager.resource(5),0};compositor.alternate={a.manager.resource(5),1};}
};
API(screen_compositor_create) ScreenCompositorFixture* screen_compositor_create(AnmManagerFixture* a,AnmRendererFixture* r){return new ScreenCompositorFixture(*a,*r);}
API(screen_compositor_delete) void screen_compositor_delete(ScreenCompositorFixture* p){delete p;}
API(screen_compositor_capture) void screen_compositor_capture(ScreenCompositorFixture* p,u32 i,AnmVm* vm){p->compositor.captures[i]=vm;}
API(screen_compositor_configure) void screen_compositor_configure(ScreenCompositorFixture* p,u32 active,u32 color,u32 width,u32 height,float scale){p->compositor.active=active;p->compositor.clear_color=color;p->animation.environment.screen_width=width;p->animation.environment.screen_height=height;p->animation.environment.resolution_scale=scale;p->views.configure(p->animation.environment);p->render.graphics.target_events.clear();p->render.graphics.target=0;p->render.graphics.history.clear();p->render.graphics.submissions.clear();p->render.graphics.calls=0;}
API(screen_compositor_offset) void screen_compositor_offset(ScreenCompositorFixture* p,u32 index,float x,float y){p->views.view(DrawCamera(index)).offset={x,y};}
API(screen_compositor_view_offset) const Vec2* screen_compositor_view_offset(ScreenCompositorFixture* p,u32 index){return &p->views.view(DrawCamera(index)).offset;}
API(screen_compositor_pass) u32 screen_compositor_pass(ScreenCompositorFixture* p,u32 i){return p->compositor.pass(i);}
API(screen_compositor_events) const u32* screen_compositor_events(ScreenCompositorFixture* p){return p->render.graphics.target_events.data();}
API(screen_compositor_event_count) u32 screen_compositor_event_count(ScreenCompositorFixture* p){return p->render.graphics.target_events.size();}
API(screen_compositor_size) const Vec2* screen_compositor_size(ScreenCompositorFixture* p){return &p->compositor.playfield_size;}
API(screen_compositor_error) const char* screen_compositor_error(ScreenCompositorFixture* p){return p->compositor.error.c_str();}
API(screen_compositor_replacement) void screen_compositor_replacement(ScreenCompositorFixture* p,u32 index,u32 enabled){p->compositor.replacement[index]=enabled?std::function<bool()>([p,index](){p->render.graphics.target_event(3,index,nullptr);return true;}):std::function<bool()>();}

#include "../../cpp/game/StageDrawFrame.hpp"
struct StageDrawFrameFixture final:th15::StageDrawServices {
 th15::FrameScheduler scheduler;th15::ScreenViews views;th15::StageDrawFrame draw;std::vector<i32> fades;
 StageDrawFrameFixture(StageSceneFixture& s,AnmManagerFixture& a,AnmRendererFixture& r):views(r.renderer,a.environment),draw(scheduler,s.scene,a.manager,r.renderer,r.graphics,views,*this){r.graphics.targets_enabled=true;}
 bool background_fade(i32 duration,i32 update_priority,i32 draw_priority)override{fades.insert(fades.end(),{duration,update_priority,draw_priority});return true;}
};
API(stage_draw_frame_create) StageDrawFrameFixture* stage_draw_frame_create(StageSceneFixture* s,AnmManagerFixture* a,AnmRendererFixture* r){return new StageDrawFrameFixture(*s,*a,*r);}
API(stage_draw_frame_delete) void stage_draw_frame_delete(StageDrawFrameFixture* p){delete p;}
API(stage_draw_frame_configure) void stage_draw_frame_configure(StageDrawFrameFixture* p,StageSceneFixture* s,const GraphicsViewport* viewport,u32 flags,u32 tint,i32 age,float rate,float x,float y){p->draw.viewport=*viewport;p->draw.state.flags=flags;p->draw.state.tint_color=tint;p->draw.state.transition.set(age);p->draw.rate=rate;p->views.view(th15::DrawCamera::Playfield).offset={x,y};s->scene.draw_objects=(flags&1)!=0;p->fades.clear();}
API(stage_draw_frame_pass) u32 stage_draw_frame_pass(StageDrawFrameFixture* p,u32 pass){return p->draw.draw(pass);}
API(stage_draw_frame_state) const th15::StageDrawState* stage_draw_frame_state(StageDrawFrameFixture* p){return &p->draw.state;}
API(stage_draw_frame_fades) const i32* stage_draw_frame_fades(StageDrawFrameFixture* p){return p->fades.data();}
API(stage_draw_frame_fade_count) u32 stage_draw_frame_fade_count(StageDrawFrameFixture* p){return p->fades.size();}
API(stage_draw_frame_error) const char* stage_draw_frame_error(StageDrawFrameFixture* p){return p->draw.error.c_str();}
API(anm_renderer_target_events) const u32* anm_renderer_target_events(AnmRendererFixture* p){return p->graphics.target_events.data();}
API(anm_renderer_target_event_count) u32 anm_renderer_target_event_count(AnmRendererFixture* p){return p->graphics.target_events.size();}
API(anm_renderer_clear_target_events) void anm_renderer_clear_target_events(AnmRendererFixture* p){p->graphics.target_events.clear();}

#include "../../cpp/game/ScreenFade.hpp"
struct ScreenFadeFixture {th15::FrameScheduler scheduler;th15::AnmEnvironment environment;th15::ScreenFade fade;ScreenFadeFixture(AnmRendererFixture& r,i32 duration,u32 color=0,bool cover=false,bool full=false):fade(scheduler,r.renderer,r.graphics,environment,duration,20,10,color,cover,full){}};
API(screen_fade_create) ScreenFadeFixture* screen_fade_create(AnmRendererFixture* r,i32 duration){return new ScreenFadeFixture(*r,duration);}
API(screen_fade_create_mode) ScreenFadeFixture* screen_fade_create_mode(AnmRendererFixture* r,i32 duration,u32 mode,u32 color){return new ScreenFadeFixture(*r,duration,color,mode==2||mode==5,mode==0||mode==5);}
API(screen_fade_delete) void screen_fade_delete(ScreenFadeFixture* p){delete p;}
API(screen_fade_state) th15::ScreenFadeState* screen_fade_state(ScreenFadeFixture* p){return &p->fade.state;}
API(screen_fade_configure) void screen_fade_configure(ScreenFadeFixture* p,float rate,u32 shutdown,u32 width,u32 height){p->fade.rate=rate;p->fade.shutdown=shutdown;p->environment.screen_width=width;p->environment.screen_height=height;}
API(screen_fade_mode) void screen_fade_mode(ScreenFadeFixture* p,u32 covering,u32 fullscreen,u32 game,u32 flags){p->fade.covering=covering;p->fade.full_screen=fullscreen;p->fade.game_available=game;p->fade.game_flags=flags;}
API(screen_fade_update) i32 screen_fade_update(ScreenFadeFixture* p){return p->fade.update();}
API(screen_fade_draw) u32 screen_fade_draw(ScreenFadeFixture* p){return p->fade.draw();}

#include "../../cpp/game/ScreenTargets.hpp"
struct ScreenTargetsFixture {th15::ScreenTargets targets;ScreenTargetsFixture(AnmManagerFixture& a,ScreenCompositorFixture& c,i32 bank):targets(a.manager,a.environment,c.views,c.compositor,bank){}};
API(screen_targets_create) ScreenTargetsFixture* screen_targets_create(AnmManagerFixture* a,ScreenCompositorFixture* c,i32 bank){return new ScreenTargetsFixture(*a,*c,bank);}
API(screen_targets_delete) void screen_targets_delete(ScreenTargetsFixture* p){delete p;}
API(screen_targets_prepare) u32 screen_targets_prepare(ScreenTargetsFixture* p){return p->targets.prepare();}
API(screen_targets_deactivate) void screen_targets_deactivate(ScreenTargetsFixture* p){p->targets.deactivate();}
API(screen_targets_capture) AnmVm* screen_targets_capture(ScreenTargetsFixture* p,u32 index){return index<4?&p->targets.captures[index]:nullptr;}
API(screen_targets_error) const char* screen_targets_error(ScreenTargetsFixture* p){return p->targets.error.c_str();}

#include "../../cpp/game/GameDraw.hpp"
struct RunDrawFixture final:StageDrawServices,ReplayCalendarServices {
 RunPauseFixture run;AnmRendererFixture rendering;ScreenViews views;AsciiText captions;std::unique_ptr<AnmDrawSchedule> animation_drawing;std::unique_ptr<AsciiFrame> caption_frame;std::unique_ptr<ScreenCompositor> compositor;std::unique_ptr<ScreenTargets> targets;std::unique_ptr<GameDraw> drawing;std::vector<std::unique_ptr<ScreenFade>> fades;std::array<u8,0xa4> replay_header{};std::array<u32,5> statistics{};std::string error;
 explicit RunDrawFixture(StageAssetFixture& a):run(a),views(rendering.renderer,a.environment),captions(a.animations,a.environment){rendering.graphics.targets_enabled=true;}
 bool calendar(i64,ReplayCalendar& out)override{out={2026,10,2,12,34};return true;}
 bool background_fade(i32 duration,i32 up,i32 dp)override{auto* s=run.run.scene();if(!s)return false;fades.push_back(std::make_unique<ScreenFade>(s->frame_scheduler(),rendering.renderer,rendering.graphics,run.assets.environment,duration,up,dp));return true;}
 ~RunDrawFixture(){fades.clear();drawing.reset();targets.reset();compositor.reset();caption_frame.reset();animation_drawing.reset();}
 bool attach(){auto* s=run.run.scene();const auto* a=run.run.scene_assets();if(!s||!a||!run.attach_pause()||!captions.initialize(a->ascii))return false;run.assets.environment.screen_offsets={320,16,320,16};auto& scheduler=s->frame_scheduler();views.configure(run.assets.environment);animation_drawing=std::make_unique<AnmDrawSchedule>(scheduler,run.assets.animations,rendering.renderer,rendering.graphics,views);caption_frame=std::make_unique<AsciiFrame>(scheduler,captions,rendering.renderer,views);compositor=std::make_unique<ScreenCompositor>(scheduler,run.assets.animations,run.assets.environment,rendering.renderer,rendering.graphics,views);targets=std::make_unique<ScreenTargets>(run.assets.animations,run.assets.environment,views,*compositor,a->text);drawing=std::make_unique<GameDraw>(*s,run.progress,run.records,run.assets.animations,rendering.renderer,rendering.graphics,views,captions,*this,*this);drawing->pause=run.pause_controller.get();drawing->captured=&targets->captures[2];drawing->replay_header=&replay_header;return targets->prepare();}
 bool step(u32 held,u32 pressed,u32 repeated){if(!run.paused_step(held,pressed,repeated,false))return false;for(auto& fade:fades)fade->rate=run.progress.rate;rendering.renderer.invalidate();rendering.graphics.history.clear();rendering.graphics.submissions.clear();rendering.graphics.target_events.clear();rendering.graphics.calls=0;compositor->clear_color=run.run.scene()->background.color;if(!drawing->draw()){error=drawing->error.empty()?drawing->background.error:drawing->error;return false;}rendering.renderer.flush();if(!compositor->error.empty()||!animation_drawing->error.empty()||!caption_frame->error.empty()){error=!compositor->error.empty()?compositor->error:!animation_drawing->error.empty()?animation_drawing->error:caption_frame->error;return false;}statistics[0]++;statistics[1]+=rendering.graphics.calls;for(const auto& b:rendering.graphics.history)statistics[2]+=b.size();statistics[3]+=rendering.graphics.target_events.size()/8;statistics[4]=captions.count();return true;}
};
API(run_draw_create) RunDrawFixture* run_draw_create(StageAssetFixture* a){return new RunDrawFixture(*a);}
API(run_draw_delete) void run_draw_delete(RunDrawFixture* p){delete p;}
API(run_draw_run) RunPauseFixture* run_draw_run(RunDrawFixture* p){return &p->run;}
API(run_draw_attach) u32 run_draw_attach(RunDrawFixture* p){return p->attach();}
API(run_draw_step) u32 run_draw_step(RunDrawFixture* p,u32 held,u32 pressed,u32 repeated){return p->step(held,pressed,repeated);}
API(run_draw_statistics) const u32* run_draw_statistics(RunDrawFixture* p){return p->statistics.data();}
API(run_draw_error) const char* run_draw_error(RunDrawFixture* p){if(!p->rendering.renderer.error.empty())return p->rendering.renderer.error.c_str();if(p->compositor&&!p->compositor->error.empty())return p->compositor->error.c_str();if(p->animation_drawing&&!p->animation_drawing->error.empty())return p->animation_drawing->error.c_str();if(p->caption_frame&&!p->caption_frame->error.empty())return p->caption_frame->error.c_str();if(p->drawing&&!p->drawing->background.error.empty())return p->drawing->background.error.c_str();if(!p->error.empty())return p->error.c_str();if(p->targets&&!p->targets->error.empty())return p->targets->error.c_str();if(!p->captions.error.empty())return p->captions.error.c_str();return run_pause_error(&p->run);}

API(popup_draw_renderer) u32 popup_draw_renderer(PopupFixture* p,AnmRendererFixture* r,const Vec3* player){struct Services final:PopupDrawServices{PopupFixture& fixture;AnmRenderer& renderer;Services(PopupFixture& p,AnmRenderer& r):fixture(p),renderer(r){}bool draw_glyph(AnmVm& vm)override{return renderer.draw_glyph(vm)!=-2;}bool draw_text(const Vec3& p,u32 color,const std::string& text)override{return fixture.draw_text(p,color,text);}}services(*p,r->renderer);p->texts.clear();const bool ok=p->manager.draw(services,*player);r->renderer.flush();return ok;}

API(run_gameplay_graze) u32 run_gameplay_graze(RunGameplayFixture* p,const Vec3* position){return p->run.scene()->battle.record_graze(*position);}
API(run_gameplay_graze_flash) void run_gameplay_graze_flash(RunGameplayFixture* p){p->run.scene()->battle.graze_flash();}
API(run_gameplay_graze_resonance) void run_gameplay_graze_resonance(RunGameplayFixture* p,float value){p->run.scene()->battle.graze_resonance(value);}
API(run_gameplay_graze_timer) Timer* run_gameplay_graze_timer(RunGameplayFixture* p){return &p->run.scene()->battle.player->motion.barrier_timer;}
API(run_gameplay_graze_scale) float* run_gameplay_graze_scale(RunGameplayFixture* p){return &p->run.scene()->battle.items->motion_scale;}
API(run_gameplay_graze_bank) i32 run_gameplay_graze_bank(RunGameplayFixture* p){return p->run.scene_assets()->effect;}
API(run_gameplay_graze_animation) AnmVm* run_gameplay_graze_animation(RunGameplayFixture* p){auto& r=p->assets.animations.registry;const auto count=r.ordered_count(false);return count?r.find(r.ordered_handle(false,count-1)):nullptr;}

API(run_gameplay_visual_rng) Rng* run_gameplay_visual_rng(RunGameplayFixture* p){return &p->assets.random;}
API(run_gameplay_audio_events) const u32* run_gameplay_audio_events(RunGameplayFixture* p){return p->events.data();}
API(run_gameplay_audio_event_count) u32 run_gameplay_audio_event_count(RunGameplayFixture* p){return p->events.size();}

struct SharedRunFixture {FrameScheduler scheduler;RunGameplayFixture run;std::array<FrameCallback,2> callbacks;u32 updates=0,draws=0;explicit SharedRunFixture(StageAssetFixture& assets):run(assets,&scheduler){for(u32 i=0;i<2;i++){auto& c=callbacks[i];c.owner=this;c.enabled=true;if(i==0)c.run=[](void* p)->i32{static_cast<SharedRunFixture*>(p)->updates++;return 1;};else c.run=[](void* p)->i32{static_cast<SharedRunFixture*>(p)->draws++;return 1;};scheduler.add(c,i?FramePass::Draw:FramePass::Update,i?83:4);}}~SharedRunFixture(){for(auto& c:callbacks)scheduler.remove(c);}};
API(shared_run_create) SharedRunFixture* shared_run_create(StageAssetFixture* a){return new SharedRunFixture(*a);}
API(shared_run_delete) void shared_run_delete(SharedRunFixture* p){delete p;}
API(shared_run_gameplay) RunGameplayFixture* shared_run_gameplay(SharedRunFixture* p){return &p->run;}
API(shared_run_updates) u32 shared_run_updates(SharedRunFixture* p){return p->updates;}
API(shared_run_draws) u32 shared_run_draws(SharedRunFixture* p){return p->draws;}
API(shared_run_draw) i32 shared_run_draw(SharedRunFixture* p){return p->scheduler.draw();}

#include "../../cpp/game/SceneCapture.hpp"
struct SceneCaptureFixture final:SceneCaptureServices {
 AnmManagerFixture& a;SceneCapture capture;std::vector<i32> calls;
 SceneCaptureFixture(AnmManagerFixture& v,i32 bank):a(v),capture(v.manager,v.environment,*this,bank){capture.pause_source={v.manager.resource(bank),3};}
 bool copy(const ScreenSurface& source,const std::array<i32,4>& from,const AnmResource& destination,u32 texture,const std::array<i32,4>& to)override{calls.push_back(source.resource?i32(source.texture):-1);calls.push_back(i32(texture));calls.insert(calls.end(),from.begin(),from.end());calls.insert(calls.end(),to.begin(),to.end());return true;}
};
API(scene_capture_create) SceneCaptureFixture* scene_capture_create(AnmManagerFixture* a,i32 bank){return new SceneCaptureFixture(*a,bank);}
API(scene_capture_delete) void scene_capture_delete(SceneCaptureFixture* p){delete p;}
API(scene_capture_run) bool scene_capture_run(SceneCaptureFixture* p,u32* handle,bool results){return p->capture.capture(*handle,results);}
API(scene_capture_queue) bool scene_capture_queue(SceneCaptureFixture* p,u32 handle,i32 x,i32 y,i32 w,i32 h){return p->capture.queue(handle,x,y,w,h);}
API(scene_capture_finish) bool scene_capture_finish(SceneCaptureFixture* p){return p->capture.finish_frame();}
API(scene_capture_pending) u32 scene_capture_pending(SceneCaptureFixture* p){return p->capture.pending();}
API(scene_capture_calls) i32* scene_capture_calls(SceneCaptureFixture* p){return p->calls.data();}
API(scene_capture_count) u32 scene_capture_count(SceneCaptureFixture* p){return p->calls.size();}
API(scene_capture_reset) void scene_capture_reset(SceneCaptureFixture* p){p->calls.clear();p->capture.discard();}
API(scene_capture_error) const char* scene_capture_error(SceneCaptureFixture* p){return p->capture.error.c_str();}
API(scene_capture_configure) void scene_capture_configure(SceneCaptureFixture* p,float scale,i32 x,i32 y){p->a.environment.resolution_scale=scale;p->a.environment.screen_offsets[0]=x;p->a.environment.screen_offsets[1]=y;}

struct ApplicationRunAssetsFixture {
 StageAssetFixture& assets;FrameScheduler scheduler;std::unordered_map<std::string,i32> shared;std::unique_ptr<RunGameplayFixture> run;std::string error;
 explicit ApplicationRunAssetsFixture(StageAssetFixture& a):assets(a){}
 bool initialize(){const std::pair<const char*,i32> common[]={{"text.anm",0},{"ascii.anm",2}};for(const auto& row:common){std::vector<u8> bytes;if(!assets.source.read(row.first,bytes)||!assets.animations.load(row.second,bytes.data(),bytes.size())){error=assets.animations.error;return false;}shared.emplace(row.first,row.second);}return assets.animations.sprite_fallback(2);}
 bool begin(u32 stage,i32 character){run.reset();assets.animations.update(false);assets.animations.update(true);run=std::make_unique<RunGameplayFixture>(assets,&scheduler,&shared);return run_gameplay_load(run.get(),stage,character);}
};
API(application_assets_create) ApplicationRunAssetsFixture* application_assets_create(StageAssetFixture* a){return new ApplicationRunAssetsFixture(*a);}
API(application_assets_delete) void application_assets_delete(ApplicationRunAssetsFixture* p){delete p;}
API(application_assets_initialize) bool application_assets_initialize(ApplicationRunAssetsFixture* p){return p->initialize();}
API(application_assets_begin) bool application_assets_begin(ApplicationRunAssetsFixture* p,u32 stage,i32 character){return p->begin(stage,character);}
API(application_assets_run) RunGameplayFixture* application_assets_run(ApplicationRunAssetsFixture* p){return p->run.get();}
API(application_assets_common) AnmResource* application_assets_common(ApplicationRunAssetsFixture* p,i32 bank){return p->assets.animations.resource(bank);}
API(application_assets_bank) i32 application_assets_bank(ApplicationRunAssetsFixture* p,u32 index){auto* a=p->run->run.scene_assets();return index?a->ascii:a->text;}
API(application_assets_clear) void application_assets_clear(ApplicationRunAssetsFixture* p){p->run.reset();p->assets.animations.update(false);p->assets.animations.update(true);}
API(application_assets_error) const char* application_assets_error(ApplicationRunAssetsFixture* p){return p->error.c_str();}

struct ScreenMotionFixture {FrameScheduler scheduler;Rng random;ScreenShakeContext context;std::array<Vec2,2> offsets{};std::unique_ptr<ScreenMotionFrame> motion;};
API(screen_motion_create) ScreenMotionFixture* screen_motion_create(u32 nudge,i32 a,i32 b,i32 d,i32 e){auto* p=new ScreenMotionFixture;auto ctx=[p](){return p->context;};auto sink=[p](Vec2 world,Vec2 screen){p->offsets={world,screen};};if(nudge)p->motion=std::make_unique<ScreenMotionFrame>(p->scheduler,p->random,ScreenNudgeSpec{a,b,d},ctx,sink);else p->motion=std::make_unique<ScreenMotionFrame>(p->scheduler,p->random,ScreenShakeSpec{a,b,d,e},ctx,sink);return p;}
API(screen_motion_delete) void screen_motion_delete(ScreenMotionFixture* p){delete p;}
API(screen_motion_field) void* screen_motion_field(ScreenMotionFixture* p,u32 field){return field?static_cast<void*>(p->offsets.data()):static_cast<void*>(&p->random);}
API(screen_motion_active) u32 screen_motion_active(ScreenMotionFixture* p){return p->motion->active;}
API(screen_motion_update) i32 screen_motion_update(ScreenMotionFixture* p,u32 shutdown,u32 game,u32 flags,float rate,float scale){p->offsets={};p->context={shutdown!=0,game!=0,flags,rate,scale};return p->scheduler.update();}

#include "../../cpp/game/CheckpointFile.hpp"
struct CheckpointFileFixture {CheckpointFile file;std::vector<u8> output;};
API(checkpoint_file_create) CheckpointFileFixture* checkpoint_file_create(){return new CheckpointFileFixture;}
API(checkpoint_file_delete) void checkpoint_file_delete(CheckpointFileFixture* p){delete p;}
API(checkpoint_file_header) u8* checkpoint_file_header(CheckpointFileFixture* p){return p->file.header.bytes.data();}
API(checkpoint_file_header_create) void checkpoint_file_header_create(CheckpointFileFixture* p,i32 low,i32 high,i32 c,i32 d,i32 s,i32 chapter,const i32* retries,u32 flags){std::array<i32,10> values;std::copy_n(retries,10,values.begin());p->file.header=CheckpointHeader::create(i64(u64(u32(high))<<32|u32(low)),c,d,s,chapter,values,flags);}
API(checkpoint_file_compatible) u32 checkpoint_file_compatible(CheckpointFileFixture* p,i32 c,i32 d,u32 flags){return p->file.header.compatible(c,d,flags);}
API(checkpoint_file_section) const u8* checkpoint_file_section(CheckpointFileFixture* p,u32 id){return id<9?p->file.sections[id].data():nullptr;}
API(checkpoint_file_section_size) u32 checkpoint_file_section_size(CheckpointFileFixture* p,u32 id){return id<9?p->file.sections[id].size():0;}
API(checkpoint_file_set_section) u32 checkpoint_file_set_section(CheckpointFileFixture* p,u32 id,const u8* b,u32 n){if(id>=9||n>CheckpointFile::max_payload||(!b&&n))return 0;p->file.sections[id].assign(b,b+n);return 1;}
API(checkpoint_file_encode) u32 checkpoint_file_encode(CheckpointFileFixture* p){return p->file.encode(p->output);}
API(checkpoint_file_open) u32 checkpoint_file_open(CheckpointFileFixture* p,const u8* b,u32 n){return p->file.open(b,n);}
API(checkpoint_file_update_header) u32 checkpoint_file_update_header(CheckpointFileFixture* p,u8* b,u32 n){std::vector<u8> v(b,b+n);if(!CheckpointFile::update_header(v,p->file.header))return 0;std::copy(v.begin(),v.end(),b);return 1;}
API(checkpoint_file_output) const u8* checkpoint_file_output(CheckpointFileFixture* p){return p->output.data();}
API(checkpoint_file_output_size) u32 checkpoint_file_output_size(CheckpointFileFixture* p){return p->output.size();}
API(checkpoint_file_error) const char* checkpoint_file_error(CheckpointFileFixture* p){return p->file.error.c_str();}

#include "../../cpp/game/AnmFile.hpp"
struct AnmFileFixture {AnmFile codec;AnmFile::Block block;};
API(anm_file_create) AnmFileFixture* anm_file_create(){return new AnmFileFixture;}
API(anm_file_delete) void anm_file_delete(AnmFileFixture* p){delete p;}
API(anm_file_block) const u8* anm_file_block(AnmFileFixture* p){return p->block.data();}
API(anm_file_write) u32 anm_file_write(AnmFileFixture* p,AnmVm* vm,AnmManagerFixture* m){return p->codec.write(*vm,m->manager,p->block);}
API(anm_file_read) u32 anm_file_read(AnmFileFixture* p,const u8* b,u32 size,AnmVm* vm,AnmManagerFixture* m){return p->codec.read(b,size,*vm,m->manager);}
API(anm_file_error) const char* anm_file_error(AnmFileFixture* p){return p->codec.error.c_str();}

struct AnmTreeFileFixture {std::vector<u8> bytes;u32 consumed=0;};
API(anm_tree_file_create) AnmTreeFileFixture* anm_tree_file_create(){return new AnmTreeFileFixture;}
API(anm_tree_file_delete) void anm_tree_file_delete(AnmTreeFileFixture* p){delete p;}
API(anm_tree_file_write) u32 anm_tree_file_write(AnmTreeFileFixture* f,AnmCheckpoint* p,u32 handle,AnmManagerFixture* m){f->bytes.clear();return p->write_tree(handle,m->manager,f->bytes);}
API(anm_tree_file_read) u32 anm_tree_file_read(AnmTreeFileFixture* f,AnmCheckpoint* p,const u8* bytes,u32 n,AnmManagerFixture* m){return p->read_tree(bytes,n,f->consumed,m->manager);}
API(anm_tree_file_bytes) const u8* anm_tree_file_bytes(AnmTreeFileFixture* f){return f->bytes.data();}
API(anm_tree_file_size) u32 anm_tree_file_size(AnmTreeFileFixture* f){return f->bytes.size();}
API(anm_tree_file_consumed) u32 anm_tree_file_consumed(AnmTreeFileFixture* f){return f->consumed;}

struct PlayerCheckpointFileFixture {std::vector<u8> bytes;u32 consumed=0;};
API(player_checkpoint_file_create) PlayerCheckpointFileFixture* player_checkpoint_file_create(){return new PlayerCheckpointFileFixture;}
API(player_checkpoint_file_delete) void player_checkpoint_file_delete(PlayerCheckpointFileFixture* p){delete p;}
API(player_checkpoint_file_write) i32 player_checkpoint_file_write(PlayerCheckpointFileFixture* f,PlayerCheckpoint* p,AnmManagerFixture* m){return p->write_file(f->bytes,m->manager);}
API(player_checkpoint_file_read) i32 player_checkpoint_file_read(PlayerCheckpointFileFixture* f,PlayerCheckpoint* p,const u8* bytes,u32 n,AnmManagerFixture* m){return p->read_file(bytes,n,f->consumed,m->manager);}
API(player_checkpoint_file_bytes) const u8* player_checkpoint_file_bytes(PlayerCheckpointFileFixture* f){return f->bytes.data();}
API(player_checkpoint_file_size) u32 player_checkpoint_file_size(PlayerCheckpointFileFixture* f){return f->bytes.size();}
API(player_checkpoint_file_consumed) u32 player_checkpoint_file_consumed(PlayerCheckpointFileFixture* f){return f->consumed;}

API(effect_checkpoint_file_write) i32 effect_checkpoint_file_write(PlayerCheckpointFileFixture* f,EffectCheckpoint* p,AnmManagerFixture* m){return p->write_file(f->bytes,m->manager);}
API(effect_checkpoint_file_read) i32 effect_checkpoint_file_read(PlayerCheckpointFileFixture* f,EffectCheckpoint* p,const u8* data,u32 n,AnmManagerFixture* m){return p->read_file(data,n,f->consumed,m->manager);}
API(popup_checkpoint_file_write) void popup_checkpoint_file_write(PlayerCheckpointFileFixture* f,PopupFixture* p){p->manager.write_checkpoint_file(f->bytes);}
API(popup_checkpoint_file_read) i32 popup_checkpoint_file_read(PopupFixture* p,const u8* data,u32 n){return p->manager.read_checkpoint_file(data,n);}

API(stage_checkpoint_file_write) i32 stage_checkpoint_file_write(PlayerCheckpointFileFixture* f,StageCheckpoint* p){return p->write_file(f->bytes);}
API(stage_checkpoint_file_read) i32 stage_checkpoint_file_read(PlayerCheckpointFileFixture* f,StageCheckpoint* p,const u8* data,u32 n){return p->read_file(data,n,f->consumed);}

API(item_checkpoint_file_write) i32 item_checkpoint_file_write(PlayerCheckpointFileFixture* f,ItemCheckpoint* p,AnmManagerFixture* m){return p->write_file(f->bytes,m->manager);}
API(item_checkpoint_file_read) i32 item_checkpoint_file_read(PlayerCheckpointFileFixture* f,ItemCheckpoint* p,const u8* data,u32 n,AnmManagerFixture* m){return p->read_file(data,n,f->consumed,m->manager);}

API(bullet_checkpoint_file_write) i32 bullet_checkpoint_file_write(PlayerCheckpointFileFixture* f,BulletCheckpoint* p){return p->write_file(f->bytes);}
API(bullet_checkpoint_file_read) i32 bullet_checkpoint_file_read(PlayerCheckpointFileFixture* f,BulletCheckpoint* p,const u8* data,u32 n){return p->read_file(data,n,f->consumed);}
API(bullet_scene_reflection_bounds) Vec2* bullet_scene_reflection_bounds(BulletSceneFixture* p){return &p->scene.manager.context.reflection_bounds;}

API(bullet_scene_manager_resource) void bullet_scene_manager_resource(BulletSceneFixture* p,AnmManagerFixture* m,i32 bank){p->scene.resource=m->manager.resource(bank);}

API(bomb_checkpoint_file_write) i32 bomb_checkpoint_file_write(PlayerCheckpointFileFixture* f,BombCheckpoint* p){return p->write_file(f->bytes);}
API(bomb_checkpoint_file_read) i32 bomb_checkpoint_file_read(PlayerCheckpointFileFixture* f,BombCheckpoint* p,const u8* data,u32 n){return p->read_file(data,n,f->consumed);}

API(chapter_checkpoint_file_write) i32 chapter_checkpoint_file_write(PlayerCheckpointFileFixture* f,ChapterCheckpointFixture* p){return p->checkpoint.write_file(f->bytes);}
API(chapter_checkpoint_file_read) i32 chapter_checkpoint_file_read(PlayerCheckpointFileFixture* f,ChapterCheckpointFixture* p,const u8* data,u32 n){return p->checkpoint.read_file(data,n,f->consumed);}

#include "../../cpp/game/EclFile.hpp"
API(ecl_snapshot_file_write) i32 ecl_snapshot_file_write(PlayerCheckpointFileFixture* f,EclThreadsSnapshot* p){f->bytes.assign(0x1210,0);EclFile codec;return codec.write_threads(*p,f->bytes,0,0x1210);}
API(ecl_snapshot_file_read) i32 ecl_snapshot_file_read(PlayerCheckpointFileFixture* f,EclThreadsSnapshot* p,const u8* data,u32 n,EclProgram* program){EclFile codec;u32 children=0;if(!codec.read_threads(data,n,*p,*program,0x1210,children))return 0;f->consumed=0x1210+children;return 1;}
API(ecl_snapshot_file_error) const char* ecl_snapshot_file_error(){return "Invalid serialized ECL snapshot";}

API(bullet_scene_transform_payload) void bullet_scene_transform_payload(BulletSceneFixture* p,u32 slot,u32 index,const char* text){if(slot>=BulletManager::capacity||index>=18)return;auto& t=p->scene.manager.state(slot)->transforms[index];t.payload.assign(text,text+std::strlen(text)+1);}
API(bullet_scene_transform_payload_bytes) const char* bullet_scene_transform_payload_bytes(BulletSceneFixture* p,u32 slot,u32 index){if(slot>=BulletManager::capacity||index>=18)return "";const auto& t=p->scene.manager.state(slot)->transforms[index];return t.payload.empty()?"":reinterpret_cast<const char*>(t.payload.data());}

API(enemy_checkpoint_file_write) i32 enemy_checkpoint_file_write(PlayerCheckpointFileFixture* f,EnemyCheckpoint* p){return p->write_file(f->bytes);}
API(enemy_checkpoint_file_read) i32 enemy_checkpoint_file_read(PlayerCheckpointFileFixture* f,EnemyCheckpoint* p,const u8* data,u32 n){return p->read_file(data,n,f->consumed);}
API(enemy_manager_saved_field) void* enemy_manager_saved_field(EnemyManagerFixture* p,u32 index){switch(index){case 0:return &p->world.manager_flags;case 1:return &p->world.shot_damage;case 2:return &p->world.other_damage;case 3:return p->world.integer_registers.data();case 4:return p->world.float_registers.data();case 5:return p->world.counters.data();default:return p->world.boss_ids.data();}}

API(enemy_manager_attach_grid) u32 enemy_manager_attach_grid(EnemyManagerFixture* p,u32 index,ScreenGridObjectFixture* g){auto* enemy=p->manager.at(index);if(!enemy)return 0;enemy->state.distortion_mesh=g->mesh;enemy->state.distortion=g->state;return 1;}
API(enemy_manager_grid_field) void* enemy_manager_grid_field(EnemyManagerFixture* p,u32 index,u32 kind){auto* e=p->manager.at(index);if(!e)return nullptr;auto& m=e->state.distortion_mesh;return kind==0?static_cast<void*>(&e->state.distortion):kind==1?static_cast<void*>(m.grid.vertices.data()):static_cast<void*>(m.grid.sampling.data());}
API(enemy_manager_grid_handle) u32 enemy_manager_grid_handle(EnemyManagerFixture* p,u32 index,u32 slot){auto* e=p->manager.at(index);if(!e)return 0;return slot==0?e->state.distortion_mesh.capture_handle:slot<=16?e->state.distortion_mesh.strip_handles[slot-1]:0;}

API(battle_checkpoint_file_write) i32 battle_checkpoint_file_write(PlayerCheckpointFileFixture* f,BattleCheckpointFixture* p,u32 flags){return p->checkpoint.write_file(f->bytes,1700000000,flags);}
API(battle_checkpoint_file_read) i32 battle_checkpoint_file_read(PlayerCheckpointFileFixture* f,BattleCheckpointFixture* p,const u8* bytes,u32 size,u32 flags){return p->checkpoint.read_file(bytes,size,flags);}

API(anm_manager_name) u32 anm_manager_name(AnmManagerFixture* p,i32 id,const char* name){return p->manager.name_resource(id,name);}
API(anm_file_read_mapped) u32 anm_file_read_mapped(AnmFileFixture* p,const u8* data,u32 size,AnmVm* vm,AnmManagerFixture* m,i32 saved,i32 current){const auto* before=m->manager.checkpoint_banks;std::unordered_map<i32,i32> map{{saved,current}};m->manager.checkpoint_banks=&map;const bool result=p->codec.read(data,size,*vm,m->manager);m->manager.checkpoint_banks=before;return result;}

#include "../../cpp/game/GameKeyboard.hpp"
API(game_keyboard_keys) u32 game_keyboard_keys(const u8* bytes){bool keys[256]{};for(u32 i=0;i<256;i++)keys[i]=(bytes[i]&128)!=0;return keyboard_keys(keys);}
API(player_motion_touch) void player_motion_touch(PlayerMotionFixture* p,i32 mode,float x,float y){p->motion.touch={mode,x,y};}
API(recording_touch_tick) i32 recording_touch_tick(ReplayRecordingFixture* p,u32 held,u32 pressed,u32 released,float fps,u32 paused,i32 mode,float x,float y){return p->recording.tick({u16(held),u16(pressed),u16(released)},fps,paused,{mode,x,y});}
API(recording_uses_touch) u32 recording_uses_touch(ReplayRecordingFixture* p){return p->recording.uses_touch();}
API(replay_uses_touch) u32 replay_uses_touch(Replay* p){return p->uses_touch();}

#include "../../cpp/game/TextLayout.hpp"
API(text_layout_spell_offset) i32 text_layout_spell_offset(float width,i32 font,u32 length){AnmResource resource;AnmSprite sprite;sprite.width=width;sprite.height=64;AnmVm vm;vm.resource=&resource;vm.sprite=&sprite;DialogueText request;request.font=font;request.bytes.assign(length,'X');request.right_aligned=true;TextLayout layout;return TextLayout::dialogue(vm,request,false,layout)?layout.style.offset:INT32_MIN;}
