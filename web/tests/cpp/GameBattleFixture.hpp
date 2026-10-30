#pragma once
#include "../../cpp/game/GameBattle.hpp"
#include "../../cpp/game/GameInput.hpp"
// Captures presentation only. Simulation owners and all cross-owner contacts
// below are the production GameBattle implementation.
struct GameBattleFixture final:BattleWorldServices,DialogueServices {
    AnmManagerFixture& animations;std::vector<u32> events,dialogue_events;FrameScheduler scheduler;GameInput dialogue_input;
    struct Observer {GameBattleFixture* fixture=nullptr;u32 index=0;FrameCallback callback;};
    std::array<Observer,9> observers;std::array<std::array<Rng,2>,9> random_history;GameBattle battle;
    EnemyVariableFixture enemy_view;std::vector<BulletStateFixture> bullet_views;SpellRecords spell_records;u32 background_flags=1;std::string spell_name;
    GameBattleFixture(AnmManagerFixture& a,EclProgram& e,const ShtResource& s,i32 character):animations(a),battle(a.manager,a.environment,a.environment.game_rng,a.random,*this,e,s,character,{10,7,8,30,2,{{7,8,-1,-1,-1,-1}}}),bullet_views(BulletManager::capacity){for(u32 i=0;i<BulletManager::capacity;i++)bullet_views[i].external_state=battle.bullet_scene->manager.state(i);battle.attach(scheduler);for(u32 i=0;i<observers.size();i++){auto& o=observers[i];o.fixture=this;o.index=i;o.callback.owner=&o;o.callback.enabled=true;o.callback.run=[](void* ptr)->i32{auto& v=*static_cast<Observer*>(ptr);v.fixture->random_history[v.index]={{v.fixture->animations.environment.game_rng,v.fixture->animations.random}};return 1;};scheduler.add(o.callback,FramePass::Update,GameBattle::update_priorities[i]+1);}}
    bool audio(i32 id,float,PlayerShots::SoundAction action,bool queued)override{events.insert(events.end(),{1,u32(id),u32(action),u32(queued)});return true;}
    bool life_hud(i32 a,i32 b)override{events.insert(events.end(),{2,u32(a),u32(b)});return true;}
    bool bomb_hud(i32 a,i32 b)override{events.insert(events.end(),{3,u32(a),u32(b)});return true;}
    bool game_over()override{events.push_back(4);return true;}bool notice(i32 n)override{events.insert(events.end(),{5,u32(n)});return true;}
    bool popup(const Vec3&,i32 n,u32 color)override{events.insert(events.end(),{6,u32(n),color});return true;}
    bool trophy(i32 n)override{events.insert(events.end(),{7,u32(n)});return true;}
    bool scene_message(i32)override{return false;}bool message_complete(bool&)override{return false;}bool dialogue_present()const noexcept override{return false;}
    bool spell_background_visible(bool value)override{background_flags=(background_flags&~1u)|(value?1:0);return true;}
    bool spell_prepare_hud(bool)override{return true;}
    bool spell_title(AnmVm&,const std::string& name)override{spell_name=name;return true;}
    bool spell_result(i32 value,bool failed)override{events.insert(events.end(),{8,u32(value),u32(failed)});return true;}
    bool chapter(i32)override{return false;}
    bool boss_segment(i32,i32,float,u32)override{return false;}bool boss_segments(i32)override{return false;}bool stage_logo()override{return false;}bool background_interrupt(i32)override{return false;}
    bool clear_dialogue_field()override{return false;}
    bool dialogue_text(AnmVm& vm,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string& value)override{dialogue_events.insert(dialogue_events.end(),{2,animations.manager.registry.handle(vm),color,shadow,u32(style),u32(x),u32(y),u32(value.size())});for(auto ch:value)dialogue_events.push_back(u8(ch));return true;}
    bool dialogue_sound(i32)override{return false;}
    bool dialogue_boss_music()override{dialogue_events.push_back(4);return true;}
    bool dialogue_music_fade(i32 seconds)override{dialogue_events.insert(dialogue_events.end(),{5,u32(seconds)});return true;}
    bool dialogue_next_stage()override{dialogue_events.push_back(6);return true;}
    bool dialogue_boss_name()override{return false;}bool dialogue_notice(u32)override{return false;}
};
API(battle_create) GameBattleFixture* battle_create(AnmManagerFixture* a,EclProgram* e,ShtResource* s,i32 character){return new GameBattleFixture(*a,*e,*s,character);}
API(battle_delete) void battle_delete(GameBattleFixture* p){delete p;}
API(battle_initialize) u32 battle_initialize(GameBattleFixture* p){return p->battle.initialize();}
API(battle_spawn) u32 battle_spawn(GameBattleFixture* p,const char* name,const u8* data){auto r=spawn_request(data);r.routine=name;return p->battle.spawn(r);}
API(battle_step) u32 battle_step(GameBattleFixture* p,u32 held,u32 pressed,float rate,u32 flags,u32 phase_mask){p->dialogue_input.update(held);BattleFrame f;f.held=held;f.pressed=pressed;f.rate=rate;f.game_flags=flags;f.shoot_frames=p->dialogue_input.duration[0];f.skip_frames=p->dialogue_input.duration[9];f.player_enabled=phase_mask&1;f.bomb_enabled=phase_mask&2;f.enemies_enabled=phase_mask&4;f.lasers_enabled=phase_mask&8;f.bullets_enabled=phase_mask&16;f.items_enabled=phase_mask&32;f.spirits_enabled=phase_mask&64;return p->battle.step(f);}
API(battle_error) const char* battle_error(GameBattleFixture* p){return p->battle.error.c_str();}
API(battle_events) const u32* battle_events(GameBattleFixture* p){return p->events.data();}
API(battle_event_count) u32 battle_event_count(GameBattleFixture* p){return p->events.size();}
API(battle_clear_events) void battle_clear_events(GameBattleFixture* p){p->events.clear();}
API(battle_player_field) void* battle_player_field(GameBattleFixture* p,u32 field){auto& g=p->battle;auto& v=*g.player;switch(field){case 0:return &g.session;case 1:return &v.life.state;case 2:return &v.life.age;case 3:return &v.life.invulnerability;case 4:return &v.frame.input_age;case 5:return &v.frame.ready_age;case 6:return &v.motion.position;case 7:return &v.motion.position_fixed;case 8:return &v.motion.behavior_flags;case 9:return v.damage.sources.data();case 10:return &v.damage.cursor;case 11:return v.shots.shots.data();case 12:return &v.frame.bounds.hit;case 13:return &v.frame.bounds.item_near;case 14:return &v.frame.bounds.graze;case 15:return &v.frame.bounds.item_full;case 16:return &g.score;case 17:return &g.items->roaring.state;case 18:return &g.items->roaring.visuals;case 19:return &g.spell;case 20:return &g.graze_total;case 21:return &g.graze_chapter;case 22:return &g.effects.cursor;case 23:return g.effects.handles.data();default:return nullptr;}}
API(battle_counter) i32* battle_counter(GameBattleFixture* p,u32 index){return &p->battle.enemy_world.counter(index);}
API(battle_roaring_counter) i32* battle_roaring_counter(GameBattleFixture* p,u32 index){return index?&p->battle.items->roaring.break_chain:&p->battle.items->roaring.break_count;}
API(battle_commit_death) u32 battle_commit_death(GameBattleFixture* p){return p->battle.player->life.commit_death();}
API(battle_hit) void battle_hit(GameBattleFixture* p){p->battle.player->hit();}
API(battle_reset_player) u32 battle_reset_player(GameBattleFixture* p){return p->battle.player->reset_for_stage();}
API(battle_roar_break) u32 battle_roar_break(GameBattleFixture* p,u32 death){return p->battle.items->end_roaring(death!=0);}
API(battle_collect_spirit) u32 battle_collect_spirit(GameBattleFixture* p,i32 kind){const bool ok=p->battle.items->roaring.collect(kind);p->battle.items->before_player();return ok;}
API(battle_item) u32 battle_item(GameBattleFixture* p,i32 kind,float x,float y){return p->battle.items->spawn(kind,{x,y,0},-1.57079637f,1.25f);}
API(battle_enemy_count) u32 battle_enemy_count(GameBattleFixture* p){return p->battle.enemies->count();}
API(battle_enemy_view) EnemyVariableFixture* battle_enemy_view(GameBattleFixture* p,u32 index){auto* r=p->battle.enemies->at(index);if(!r)return nullptr;p->enemy_view.enemy=r->state;p->enemy_view.world=p->battle.enemy_world;return &p->enemy_view;}
API(battle_enemy_context) EclContext* battle_enemy_context(GameBattleFixture* p,u32 index,u32 thread){auto* r=p->battle.enemies->at(index);return r?r->scripts.at(thread):nullptr;}
API(battle_bullet_list) u32 battle_bullet_list(GameBattleFixture* p,u32 kind,u32 slot){auto& m=p->battle.bullet_scene->manager;return kind?m.next_active(slot):m.first_active();}
API(battle_bullet_view) BulletStateFixture* battle_bullet_view(GameBattleFixture* p,u32 slot){return &p->bullet_views[slot];}
API(battle_emit_test_bullet) u32 battle_emit_test_bullet(GameBattleFixture* p,float x,float y){BulletShooter shooter;shooter.reset();shooter.position={x,y,0};shooter.flags=0;shooter.speed=0;return p->battle.bullet_scene->emit(shooter,0);}
API(battle_bullet_vm) AnmVm* battle_bullet_vm(GameBattleFixture* p,u32 slot){return &p->battle.bullet_scene->visual(slot)->body;}
API(battle_bullet_history) i32* battle_bullet_history(GameBattleFixture* p){return p->battle.bullet_scene->manager.graze_history.data();}
API(battle_spirit_count) u32 battle_spirit_count(GameBattleFixture* p){return p->battle.items->spirits.count();}
API(battle_spirit) AnimalSpiritState* battle_spirit(GameBattleFixture* p,u32 index){auto& s=p->battle.items->spirits;return s.find(s.order(index));}
API(battle_item_state) const ItemState* battle_item_state(GameBattleFixture* p,u32 id){auto* slot=p->battle.items->items.find(id);return slot?&slot->state:nullptr;}
API(battle_rng) Rng* battle_rng(GameBattleFixture* p,u32 visual){return visual?&p->animations.random:&p->animations.environment.game_rng;}
API(battle_phase_rng) Rng* battle_phase_rng(GameBattleFixture* p,u32 phase,u32 visual){return &p->random_history[phase][visual];}
API(battle_damage) DamageSources* battle_damage(GameBattleFixture* p){return &p->battle.player->damage;}
API(battle_bomb_begin) u32 battle_bomb_begin(GameBattleFixture* p){return p->battle.bomb->begin();}
API(battle_bomb_age) Timer* battle_bomb_age(GameBattleFixture* p){return &p->battle.bomb->age;}
API(battle_screen_offset) Vec2* battle_screen_offset(GameBattleFixture* p,u32 screen){return screen?&p->battle.screen_effects.screen_offset:&p->battle.screen_effects.world_offset;}
API(battle_screen_count) u32 battle_screen_count(GameBattleFixture* p){return p->battle.screen_effects.count();}
API(battle_spell_configure) u32 battle_spell_configure(GameBattleFixture* p,u32 scene){return p->battle.configure_spell(p->spell_records,{2,6,8,{{20,scene?10:0,false,scene?20:-1,scene?18:-1}}});}
API(battle_hud_configure) u32 battle_hud_configure(GameBattleFixture* p,u32 scene){auto& g=p->battle;return g.configure_hud({g.enemy_world.mode_flags,0,g.stage,g.character,g.session.animal,g.score.difficulty,0,true,false},{5,2,11,{{scene?0:-1,-1}}});}
API(battle_dialogue_configure) u32 battle_dialogue_configure(GameBattleFixture* p,MessageResource* file){auto& g=p->battle;DialogueResources r;r.character=g.character;r.enemy={{7,8,12,20,-1,-1}};return g.configure_dialogue(*StageConfiguration::get(g.stage),*file,*p,r);}
API(battle_dialogue_present) u32 battle_dialogue_present(GameBattleFixture* p){return p->battle.dialogue_present();}
API(battle_dialogue_event_count) u32 battle_dialogue_event_count(GameBattleFixture* p){return p->dialogue_events.size();}
API(battle_dialogue_events) const u32* battle_dialogue_events(GameBattleFixture* p){return p->dialogue_events.data();}
API(battle_dialogue_field) void* battle_dialogue_field(GameBattleFixture* p,u32 field){auto* d=p->battle.dialogue.get();if(!d)return nullptr;switch(field){case 0:return &d->id;case 1:return &d->age;case 2:return &d->script_age;case 3:return &d->wait;case 4:return &d->player_portrait;case 5:return d->boss_portraits.data();case 6:return &d->name;case 7:return d->text_handles.data();case 8:return &d->boss_label;case 9:return &d->bubble;case 10:return &d->complete_pulse;case 11:return &d->flags;case 12:return &d->line;case 13:return &d->cleared;case 14:return &d->speaker;case 15:return d->colors.data();case 16:return &d->position;case 17:return &d->width;case 18:return &d->bubble_style;case 19:return &d->portrait_mode;default:return nullptr;}}
API(battle_hud_field) void* battle_hud_field(GameBattleFixture* p,u32 field){auto* h=p->battle.hud.get();if(!h)return nullptr;switch(field){case 0:return &h->flags;case 1:return h->gauges.data();case 2:return &h->current_seconds;case 3:return &h->previous_seconds;case 4:return h->phase_handles.data();default:return nullptr;}}
API(battle_set_boss) void battle_set_boss(GameBattleFixture* p,u32 index){if(auto* e=p->battle.enemies->at(index))p->battle.enemy_world.boss_ids[0]=e->state.id;}
API(battle_spell_begin) u32 battle_spell_begin(GameBattleFixture* p,i32 id,i32 duration,const char* name){return p->battle.begin_spell({id,duration,0,name});}
API(battle_spell_finish) u32 battle_spell_finish(GameBattleFixture* p){return p->battle.finish_spell();}
API(battle_spell_clock) Timer* battle_spell_clock(GameBattleFixture* p){return p->battle.spell_card?&p->battle.spell_card->age:nullptr;}
API(battle_spell_background) u32* battle_spell_background(GameBattleFixture* p){return &p->background_flags;}
API(battle_shot_damage) u32 battle_shot_damage(GameBattleFixture* p,EnemyVariableFixture* e,u32 preview,u32* out){EnemyShotDamage r;r.position=e->enemy.last_hit_position;if(!p->battle.combat->shot_damage(e->enemy,preview!=0,r))return 0;out[0]=u32(r.amount);out[1]=r.direct;return 1;}
