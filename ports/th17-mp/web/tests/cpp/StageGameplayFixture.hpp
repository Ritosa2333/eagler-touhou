#pragma once
#include "../../cpp/game/StageGameplay.hpp"
struct StageGameplayFixture final:StageGameplayServices {
    AnmManagerFixture& animations;GameRun run;GameRecords records;SpellRecords spells;
    GameInput controls;std::vector<u32> events;StageGameplay game;EnemyVariableFixture enemy_view;BulletStateFixture bullet_view;
    struct Observer {StageGameplayFixture* fixture=nullptr;u32 index=0;FrameCallback callback;};
    struct Phase {std::array<Rng,2> random;std::array<u32,2> animations;};std::array<Observer,12> observers;std::array<Phase,12> phases;
    static GameRun selection(i32 stage,i32 character,i32 animal,i32 difficulty){GameRun r;r.stage=stage;r.character=character;r.animal=animal;r.difficulty=difficulty;r.scoring.mode_flags=2;return r;}
    // Sample background after the live priority-20 screen effects. Popup update
    // at 21 consumes neither RNG nor pooled animations, so this point matches
    // the native fixture's background-plus-screen-effects sample exactly.
    StageGameplayFixture(StageAssetsFixture& a,AnmManagerFixture& anm,i32 stage,i32 character,i32 animal,i32 difficulty):animations(anm),run(selection(stage,character,animal,difficulty)),game(a.assets,anm.manager,anm.environment,anm.environment.game_rng,anm.random,run,records,spells,*this){const i32 priority[]={11,21,24,26,28,29,30,31,32,33,35,36};for(u32 i=0;i<observers.size();i++){auto& o=observers[i];o.fixture=this;o.index=i;o.callback.owner=&o;o.callback.enabled=true;o.callback.run=[](void* p)->i32{auto& o=*static_cast<Observer*>(p);auto& f=*o.fixture;f.phases[o.index]={{{f.animations.environment.game_rng,f.animations.random}},{{f.animations.manager.registry.ordered_count(false),f.animations.manager.registry.ordered_count(true)}}};return 1;};game.frame_scheduler().add(o.callback,FramePass::Update,priority[i]);}}
    ~StageGameplayFixture(){for(auto& o:observers)game.frame_scheduler().remove(o.callback);}
    bool audio(i32 id,float,PlayerShots::SoundAction action,bool queued)override{events.insert(events.end(),{1,u32(id),u32(action),u32(queued)});return true;}
    bool popup(const Vec3&,i32 amount,u32 color)override{events.insert(events.end(),{2,u32(amount),color});return true;}
    bool trophy(i32 id)override{events.insert(events.end(),{3,u32(id)});return true;}
    bool game_over()override{events.push_back(4);run.scene_flags|=0x10;return true;}
    bool finish_practice()override{events.push_back(5);return true;}
    bool record_play_time()override{events.push_back(6);return true;}
    bool return_to_title()override{events.push_back(7);return true;}
    bool dialogue_text(AnmVm& vm,u32 color,u32 shadow,i32 style,i32 x,i32 y,const std::string& text)override{events.insert(events.end(),{8,animations.manager.registry.handle(vm),color,shadow,u32(style),u32(x),u32(y),u32(text.size())});for(u8 ch:text)events.push_back(ch);return true;}
    bool spell_title(AnmVm& vm,const std::string& text)override{events.insert(events.end(),{9,animations.manager.registry.handle(vm),u32(text.size())});for(u8 ch:text)events.push_back(ch);return true;}
    bool music_control(i32 op,i32 value)override{events.insert(events.end(),{10,u32(op),u32(value)});return true;}
    bool music_fade(i32 seconds)override{events.insert(events.end(),{11,u32(seconds)});return true;}
    bool unlock_music(i32 id)override{events.insert(events.end(),{12,u32(id)});return true;}
    bool transition(const SceneTransition& spec)override{events.insert(events.end(),{13,u32(spec.mode),u32(spec.frames),u32(spec.layer),spec.color});return true;}
    i32 loading_state()const noexcept override{return 0;}bool retire_loading()override{return true;}
    bool flush_audio()override{events.push_back(14);return true;}
    bool resume_environment(double)override{events.push_back(15);return true;}
};
API(stage_gameplay_create) StageGameplayFixture* stage_gameplay_create(StageAssetsFixture* a,AnmManagerFixture* m,i32 stage,i32 character,i32 animal,i32 difficulty){return new StageGameplayFixture(*a,*m,stage,character,animal,difficulty);}
API(stage_gameplay_delete) void stage_gameplay_delete(StageGameplayFixture* p){delete p;}
inline StageCamera gameplay_fixture_camera(StageGameplayFixture& p){p.animations.environment.screen_offsets={320,16,320,16};p.animations.environment.playfield_origin={128,16};StageCamera camera;camera.fov=.6;camera.direction={0,0,1};return camera;}
API(stage_gameplay_prepare) u32 stage_gameplay_prepare(StageGameplayFixture* p){return p->game.prepare(gameplay_fixture_camera(*p));}
API(stage_gameplay_prepare_replay) u32 stage_gameplay_prepare_replay(StageGameplayFixture* p,const ReplayStageState* replay){return p->game.prepare(gameplay_fixture_camera(*p),replay);}
API(stage_gameplay_capture_run) void stage_gameplay_capture_run(StageGameplayFixture* p,ReplayRunState* state){*state=p->game.capture_run();}
API(stage_gameplay_step) u32 stage_gameplay_step(StageGameplayFixture* p,u32 held,float rate){p->controls.update(held);StageGameplayFrame f;f.battle.held=held;f.battle.pressed=p->controls.pressed;f.battle.shoot_frames=p->controls.duration[0];f.battle.skip_frames=p->controls.duration[9];f.battle.rate=rate;const bool result=p->game.step(f);return result;}
API(stage_gameplay_error) const char* stage_gameplay_error(StageGameplayFixture* p){return p->game.error.c_str();}
API(stage_gameplay_field) void* stage_gameplay_field(StageGameplayFixture* p,u32 field){auto& b=p->game.battle;switch(field){case 0:return &b.session;case 1:return &b.score;case 2:return &b.player->motion.position;case 3:return &b.player->life.state;case 4:return &b.player->life.age;case 5:return &b.player->life.invulnerability;case 6:return &b.player->motion.position_fixed;case 7:return &b.player->motion.behavior_flags;case 8:return &b.spell;case 9:return p->game.controller?&p->game.controller->age:nullptr;case 10:return p->game.controller?p->game.controller->counters.data():nullptr;case 11:return &p->run.scene_flags;case 12:return &p->game.background.script.state.camera.animation_delta;case 13:return &b.enemy_world.boss_seconds;case 14:return &p->game.background_flags;case 15:return &b.items->roaring.state;case 16:return &b.effects.cursor;case 17:return b.effects.handles.data();case 18:return b.bullet_scene->manager.graze_history.data();case 19:return &b.graze_total;case 20:return &b.player->frame.input_age;case 21:return &b.player->frame.ready_age;case 22:return b.player->shots.shots.data();case 23:return &b.player->shots.reward_counter;case 24:return b.player->damage.sources.data();case 25:return &b.player->damage.cursor;case 26:return &b.items->roaring.visuals;case 27:return &b.enemy_world.current_chapter;case 28:return &b.enemy_world.requested_chapter;case 29:return &b.enemies->timer;case 30:return &p->game.background.script.state.timer;default:return nullptr;}}
API(stage_gameplay_events) const u32* stage_gameplay_events(StageGameplayFixture* p){return p->events.data();}
API(stage_gameplay_event_count) u32 stage_gameplay_event_count(StageGameplayFixture* p){return p->events.size();}
API(stage_gameplay_clear_events) void stage_gameplay_clear_events(StageGameplayFixture* p){p->events.clear();}
API(stage_gameplay_enemy_count) u32 stage_gameplay_enemy_count(StageGameplayFixture* p){return p->game.battle.enemies->count();}
API(stage_gameplay_enemy) EnemyVariableFixture* stage_gameplay_enemy(StageGameplayFixture* p,u32 index){auto* r=p->game.battle.enemies->at(index);if(!r)return nullptr;p->enemy_view.enemy=r->state;p->enemy_view.world=p->game.battle.enemy_world;return &p->enemy_view;}
API(stage_gameplay_enemy_context) EclContext* stage_gameplay_enemy_context(StageGameplayFixture* p,u32 index,u32 thread){auto* r=p->game.battle.enemies->at(index);return r?r->scripts.at(thread):nullptr;}
API(stage_gameplay_bullet_count) u32 stage_gameplay_bullet_count(StageGameplayFixture* p){auto& m=p->game.battle.bullet_scene->manager;u32 count=0;for(u32 i=m.first_active();i!=BulletManager::none;i=m.next_active(i))count++;return count;}
API(stage_gameplay_advance) u32 stage_gameplay_advance(StageGameplayFixture* p){return p->game.battle.enemies->clear_field(false);}
API(stage_gameplay_screen) i32 stage_gameplay_screen(StageGameplayFixture* p){return p->run.screen;}
API(stage_gameplay_run) GameRun* stage_gameplay_run(StageGameplayFixture* p){return &p->run;}
API(stage_gameplay_player_field) void* stage_gameplay_player_field(StageGameplayFixture* p,u32 field){auto& g=p->game.battle;auto& v=*g.player;switch(field){case 0:return &g.session;case 1:return &v.life.state;case 2:return &v.life.age;case 3:return &v.life.invulnerability;case 4:return &v.frame.input_age;case 5:return &v.frame.ready_age;case 6:return &v.motion.position;case 7:return &v.motion.position_fixed;case 8:return &v.motion.behavior_flags;case 9:return v.damage.sources.data();case 10:return &v.damage.cursor;case 11:return v.shots.shots.data();case 12:return &v.frame.bounds.hit;case 13:return &v.frame.bounds.item_near;case 14:return &v.frame.bounds.graze;case 15:return &v.frame.bounds.item_full;case 16:return &g.score;case 17:return &g.items->roaring.state;case 18:return &g.items->roaring.visuals;case 19:return &g.spell;case 20:return &g.graze_total;case 21:return &g.graze_chapter;case 22:return &g.effects.cursor;case 23:return g.effects.handles.data();default:return nullptr;}}
API(stage_gameplay_rng) Rng* stage_gameplay_rng(StageGameplayFixture* p,u32 visual){return visual?&p->animations.random:&p->animations.environment.game_rng;}
API(stage_gameplay_bullet_list) u32 stage_gameplay_bullet_list(StageGameplayFixture* p,u32 kind,u32 slot){auto& m=p->game.battle.bullet_scene->manager;return kind?m.next_active(slot):m.first_active();}
API(stage_gameplay_bullet_view) BulletStateFixture* stage_gameplay_bullet_view(StageGameplayFixture* p,u32 slot){p->bullet_view.external_state=p->game.battle.bullet_scene->manager.state(slot);return &p->bullet_view;}
API(stage_gameplay_bullet_vm) AnmVm* stage_gameplay_bullet_vm(StageGameplayFixture* p,u32 slot){return &p->game.battle.bullet_scene->visual(slot)->body;}
API(stage_gameplay_laser_count) u32 stage_gameplay_laser_count(StageGameplayFixture* p){return p->game.battle.laser_scene->manager.count();}
API(stage_gameplay_laser_field) void* stage_gameplay_laser_field(StageGameplayFixture* p,u32 index,u32 field){
    auto& scene=*p->game.battle.laser_scene;if(field==6)return scene.origin_at(index);
    void* result=nullptr;scene.manager.each([&](LaserObject& laser){if(index--==0)switch(field){case 0:result=&laser.flags;break;case 1:result=&laser.state;break;case 2:result=&laser.age;break;case 3:result=&laser.outside_delay;break;case 4:result=&laser.outside_count;break;case 5:result=&laser.id;break;}});return result;
}
API(stage_gameplay_spirit_count) u32 stage_gameplay_spirit_count(StageGameplayFixture* p){return p->game.battle.items->spirits.count();}
API(stage_gameplay_spirit) AnimalSpiritState* stage_gameplay_spirit(StageGameplayFixture* p,u32 index){auto& s=p->game.battle.items->spirits;return s.find(s.order(index));}
API(stage_gameplay_hud_field) void* stage_gameplay_hud_field(StageGameplayFixture* p,u32 field){auto* h=p->game.battle.hud.get();if(!h)return nullptr;switch(field){case 0:return &h->flags;case 1:return h->gauges.data();case 2:return &h->current_seconds;case 3:return &h->previous_seconds;case 4:return h->phase_handles.data();case 5:return &h->displayed_score;default:return nullptr;}}
API(stage_gameplay_boss_ids) u32* stage_gameplay_boss_ids(StageGameplayFixture* p){return p->game.battle.enemy_world.boss_ids.data();}
API(stage_gameplay_bullet_bounds) Vec2* stage_gameplay_bullet_bounds(StageGameplayFixture* p){return &p->game.battle.bullet_scene->manager.context.reflection_bounds;}
API(stage_gameplay_enemy_counter) i32* stage_gameplay_enemy_counter(StageGameplayFixture* p,u32 index){return index<3?&p->game.battle.enemy_world.counter(index):nullptr;}
API(stage_gameplay_phase) void* stage_gameplay_phase(StageGameplayFixture* p,u32 phase){return &p->phases[phase];}
API(stage_gameplay_popup_entries) PopupEntry* stage_gameplay_popup_entries(StageGameplayFixture* p){return p->game.popups.entries.data();}
API(stage_gameplay_popup_cursor) u32 stage_gameplay_popup_cursor(StageGameplayFixture* p){return p->game.popups.cursor();}

