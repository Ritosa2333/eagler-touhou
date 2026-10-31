#pragma once
#include "../../cpp/game/RunGameplay.hpp"
struct RunGameplayFixture final:StageGameplayServices {
    AnmManagerFixture& animations;GameRun run;GameRecords records;SpellRecords spells;GameInput controls;std::vector<i32> events;
    RunGameplay game;
    RunGameplayFixture(AssetSourceFixture& files,AnmManagerFixture& a,i32 character,i32 animal):animations(a),game(files,a.manager,a.environment,a.environment.game_rng,a.random,run,records,spells,*this){run.character=character;run.animal=animal;}
    bool audio(i32,float,PlayerShots::SoundAction,bool)override{return true;}bool popup(const Vec3&,i32,u32)override{return true;}bool trophy(i32)override{return true;}
    bool game_over()override{run.scene_flags|=0x10;return true;}bool finish_practice()override{return true;}bool record_play_time()override{return true;}bool return_to_title()override{return true;}
    bool dialogue_text(AnmVm&,u32,u32,i32,i32,i32,const std::string&)override{return true;}bool spell_title(AnmVm&,const std::string&)override{return true;}
    bool music_control(i32 operation,i32 value)override{events.insert(events.end(),{1,operation,value});return true;}bool music_fade(i32)override{return true;}
    bool unlock_music(i32 value)override{events.insert(events.end(),{2,value});return true;}bool transition(const SceneTransition& value)override{events.insert(events.end(),{3,value.mode,value.frames,value.layer});return true;}
    i32 loading_state()const noexcept override{return 0;}bool retire_loading()override{return true;}bool flush_audio()override{return true;}bool resume_environment(double)override{return true;}
    StageCamera camera(){StageCamera value;value.fov=.6f;value.direction={0,0,1};return value;}
};
API(run_gameplay_create) RunGameplayFixture* run_gameplay_create(AssetSourceFixture* f,AnmManagerFixture* a,i32 character,i32 animal){return new RunGameplayFixture(*f,*a,character,animal);}
API(run_gameplay_delete) void run_gameplay_delete(RunGameplayFixture* p){delete p;}
API(run_gameplay_load) u32 run_gameplay_load(RunGameplayFixture* p,i32 stage){return p->game.load(stage,p->camera());}
API(run_gameplay_next) u32 run_gameplay_next(RunGameplayFixture* p,i32 stage){return p->game.next(stage,p->camera());}
API(run_gameplay_complete_stage) u32 run_gameplay_complete_stage(RunGameplayFixture* p){return p->game.scene()->progression->next_stage();}
API(run_gameplay_step) u32 run_gameplay_step(RunGameplayFixture* p,u32 held){p->controls.update(held);StageGameplayFrame frame;frame.battle.held=held;frame.battle.pressed=p->controls.pressed;return p->game.update_previous(1)&&p->game.scene()->step(frame);}
API(run_gameplay_release_previous) u32 run_gameplay_release_previous(RunGameplayFixture* p){return p->game.release_previous();}
API(run_gameplay_error) const char* run_gameplay_error(RunGameplayFixture* p){return p->game.error.empty()?p->game.scene()?p->game.scene()->error.c_str():"No scene":p->game.error.c_str();}
API(run_gameplay_field) void* run_gameplay_field(RunGameplayFixture* p,u32 field){auto* scene=p->game.scene();auto& battle=scene->battle;switch(field){case 0:return battle.player.get();case 1:return &battle.session;case 2:return &battle.score;case 3:return &battle.player->motion.position;case 4:return &battle.player->life.age;case 5:return battle.hud.get();case 6:return &p->run.scene_flags;case 7:return &scene->controller->age;case 8:return &scene->background_draw;case 9:return p->game.previous_scene()?&p->game.previous_scene()->background_draw:nullptr;case 10:return &battle.items->roaring.state;case 11:return &p->records.starts;case 12:return &battle.player->life.invulnerability;default:return nullptr;}}
API(run_gameplay_enemy_count) u32 run_gameplay_enemy_count(RunGameplayFixture* p){return p->game.scene()->battle.enemies->count();}
API(run_gameplay_spawn_spirit) u32 run_gameplay_spawn_spirit(RunGameplayFixture* p){return p->game.scene()->battle.items->spirits.spawn(15,{100,200,0},0,false);}
API(run_gameplay_spirit_count) u32 run_gameplay_spirit_count(RunGameplayFixture* p){return p->game.scene()->battle.items->spirits.count();}
API(run_gameplay_events) const i32* run_gameplay_events(RunGameplayFixture* p){return p->events.data();}
API(run_gameplay_event_count) u32 run_gameplay_event_count(RunGameplayFixture* p){return p->events.size();}
