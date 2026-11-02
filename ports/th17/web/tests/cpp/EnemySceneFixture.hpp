#pragma once
#include "../../cpp/game/EnemyCallbacks.hpp"
struct EnemySceneFixture final:EnemySceneHost,EnemySpawnHost {
    EnemyVariableFixture& enemy;std::vector<u32> events;std::string name;bool message_done=true;i32 segments=0;
    explicit EnemySceneFixture(EnemyVariableFixture& e):enemy(e){e.commands.scene.host=this;e.commands.scene.spawning=this;}
    ~EnemySceneFixture(){enemy.commands.scene.host=nullptr;enemy.commands.scene.spawning=nullptr;}
    bool spawn(const EnemySpawnRequest&)override{return false;}bool boss_exists(i32)const noexcept override{return false;}
    bool clear_field(bool suppress)override{events.insert(events.end(),{5,u32(suppress)});return true;}
    bool nudge(i32 duration,i32 first,i32 last)override{events.insert(events.end(),{1,u32(duration),u32(first),u32(last)});return true;}
    bool scene_message(i32 id)override{events.insert(events.end(),{2,u32(id)});return true;}
    bool message_complete(bool& value)override{events.push_back(6);value=message_done;return true;}
    bool cancel_all_bullets(i32 reward)override{events.insert(events.end(),{3,u32(reward)});return true;}
    bool cancel_all_lasers(i32 reward,bool honor)override{events.insert(events.end(),{4,u32(reward),u32(honor)});return true;}
    bool cancel_bullets_circle(const Vec3& p,float r,i32 reward,bool honor)override{events.insert(events.end(),{15,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(r),u32(reward),u32(honor)});return true;}
    bool cancel_lasers_circle(const Vec3& p,float r,i32 reward,bool honor)override{events.insert(events.end(),{16,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z),float_to_bits(r),u32(reward),u32(honor)});return true;}
    bool clear_lasers_with_rewards()override{events.push_back(17);return true;}
    bool begin_spell(const SpellStartRequest& r)override{events.insert(events.end(),{7,u32(r.id),u32(r.time),u32(r.mode)});name=r.name;return true;}
    bool finish_spell()override{events.push_back(8);return true;}
    bool chapter(i32)override{return true;}
    bool boss_segment(i32 boss,i32 index,float fraction,u32 color)override{events.insert(events.end(),{9,u32(boss),u32(index),float_to_bits(fraction),color});return true;}
    bool boss_segments(i32 count)override{segments=count;return true;}
    bool stage_logo()override{events.push_back(10);return true;}
    bool tracked_effect(i32 script,const Vec3& p)override{events.insert(events.end(),{11,u32(script),float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z)});return true;}
    bool effect(const EnemyEffectRequest& r)override{events.insert(events.end(),{18,u32(r.resource),u32(r.script),float_to_bits(r.position.x),float_to_bits(r.position.y),float_to_bits(r.position.z),r.ordering});return true;}
    bool immediate_update(EnemyState&,EnemyUpdateRule r)override{events.insert(events.end(),{19,u32(r)});return true;}
    bool retire_distortion(EnemyState&)override{events.push_back(20);return true;}
    bool begin_distortion(EnemyState&)override{events.push_back(21);return true;}
    bool drop_items(EnemyState& e)override{const auto& p=e.motion.position;events.insert(events.end(),{12,float_to_bits(p.x),float_to_bits(p.y),float_to_bits(p.z)});return true;}
    bool background_interrupt(i32 label)override{events.insert(events.end(),{13,u32(label)});return true;}
    bool bullet_bounds(const Vec2& p)override{events.insert(events.end(),{14,float_to_bits(p.x),float_to_bits(p.y)});return true;}
};
API(enemy_scene_fixture_create) EnemySceneFixture* enemy_scene_fixture_create(EnemyVariableFixture* p){return new EnemySceneFixture(*p);}
API(enemy_scene_fixture_delete) void enemy_scene_fixture_delete(EnemySceneFixture* p){delete p;}
API(enemy_scene_fixture_reset) void enemy_scene_fixture_reset(EnemySceneFixture* p,u32 complete){p->events.clear();p->name.clear();p->message_done=complete!=0;}
API(enemy_scene_fixture_events) const u32* enemy_scene_fixture_events(EnemySceneFixture* p){return p->events.data();}
API(enemy_scene_fixture_count) u32 enemy_scene_fixture_count(EnemySceneFixture* p){return p->events.size();}
API(enemy_scene_fixture_name) const char* enemy_scene_fixture_name(EnemySceneFixture* p){return p->name.c_str();}
API(enemy_scene_fixture_segments) i32 enemy_scene_fixture_segments(EnemySceneFixture* p){return p->segments;}
API(enemy_scene_fixture_rule) i32 enemy_scene_fixture_rule(EnemySceneFixture* p){return i32(p->enemy.enemy.update_rule);}
API(enemy_scene_fixture_rule_state) i32 enemy_scene_fixture_rule_state(EnemySceneFixture* p){return p->enemy.enemy.update_rule_state;}
API(enemy_scene_fixture_distortion) EnemyDistortionState* enemy_scene_fixture_distortion(EnemySceneFixture* p){return &p->enemy.enemy.distortion;}
API(enemy_callback_run) u32 enemy_callback_run(EnemyVariableFixture* p,BulletManagerFixture* b,i32 rule){p->enemy.update_rule=EnemyUpdateRule(rule);return enemy_update_rule(p->enemy,b->manager,p->random);}
API(bullet_callback_transform_bytes) const u8* bullet_callback_transform_bytes(BulletStateFixture* p){static BulletShooter shot;shot.transforms=(p->external_state?*p->external_state:p->state).transforms;return shooter_bytes(shot)+40;}
