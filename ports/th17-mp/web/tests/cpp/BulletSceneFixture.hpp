#pragma once
#include "../../cpp/game/BulletScene.hpp"
#include "../../cpp/game/BulletArea.hpp"
struct BulletVisualFixture {BulletState state;Rng random;BulletVisual visual{state,random};};
API(bullet_visual_create) BulletVisualFixture* bullet_visual_create(){return new BulletVisualFixture;}
API(bullet_visual_delete) void bullet_visual_delete(BulletVisualFixture* p){delete p;}
API(bullet_visual_bind) u32 bullet_visual_bind(BulletVisualFixture* p,AnmResource* resource,i32 type,i32 color,float rate){const auto* spec=bullet_appearance(type);if(!spec)return 0;p->state.sprite_type=i16(type);p->state.color=i16(color);p->visual.resource=resource;p->visual.rate=rate;return p->visual.bind(spec->script,spec->overlay);}
API(bullet_visual_vm) AnmVm* bullet_visual_vm(BulletVisualFixture* p,u32 overlay){return overlay?nullptr:&p->visual.body;}
API(bullet_visual_rng) Rng* bullet_visual_rng(BulletVisualFixture* p){return &p->random;}
API(bullet_visual_interrupt) u32 bullet_visual_interrupt(BulletVisualFixture* p,i32 id){return p->visual.interrupt(id);}
API(bullet_visual_tick) i32 bullet_visual_tick(BulletVisualFixture* p,u32 overlay){return p->visual.tick(overlay);}
API(bullet_visual_state_field) void* bullet_visual_state_field(BulletVisualFixture* p,u32 field){switch(field){case 0:return &p->state.visual_flags;case 1:return &p->state.visual_jitter;case 2:return &p->state.graze_color;case 3:return &p->state.hit_interrupt;default:return &p->state.spawn_animation_finished;}}
API(bullet_visual_error) const char* bullet_visual_error(BulletVisualFixture* p){return p->visual.error.c_str();}
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
API(bullet_scene_draw) u32 bullet_scene_draw(BulletSceneFixture* p,AnmRendererFixture* r){return p->scene.draw(r->renderer);}
API(bullet_area_select) u32 bullet_area_select(BulletStateFixture* p,u32 kind,const Vec3* center,float radius,const Vec2* size,float angle){const auto& s=p->external_state?*p->external_state:p->state;return kind==2?bullet_rectangle_area(s,*center,*size,angle):bullet_circle_area(s,*center,radius,kind!=0);}
