// Development-only probes of the same complete application used by the browser.
#include "../../cpp/sdl/ApplicationState.hpp"
#include <emscripten.h>
using namespace th17;
namespace {std::unique_ptr<sdl::ApplicationState> app;std::string last_error,music_name;}
EM_JS(void, application_draw_marker, (i32 value), {if(globalThis.th17DrawMarker)globalThis.th17DrawMarker(value);});
extern "C" {
EMSCRIPTEN_KEEPALIVE void application_test_draw_profile(){
 if(!app||!app->scene())return;
 static std::array<FrameCallback,7> callbacks;static i32 priorities[]={2,13,24,43,56,85,88};
 for(u32 i=0;i<callbacks.size();i++){auto& c=callbacks[i];c.enabled=true;c.owner=&priorities[i];c.run=[](void* value)->i32{app->renderer.flush();app->graphics.backend.flush();application_draw_marker(*static_cast<i32*>(value));return 1;};app->scene()->frame_scheduler().add(c,FramePass::Draw,priorities[i]);}
}
EMSCRIPTEN_KEEPALIVE i32 application_initialize(i32 device,u32 scale){app=std::make_unique<sdl::ApplicationState>();app->graphics.render_scale=scale>=2?2:1;if(!app->initialize(device!=0)){last_error=app->failure;app.reset();return 0;}return 1;}
EMSCRIPTEN_KEEPALIVE i32 application_step(u32 held,u32 pressed,u32 repeated,float fps,i32 focus){if(!app)return 0;const bool ok=app->step(held,pressed,repeated,fps,focus!=0);app->graphics.backend.commit();return ok;}
// Every update and draw still runs; only the final browser-window presentation
// is combined, as in BrowserRuntime's bounded catch-up loop. Protection is an
// explicit test aid for reaching each real boss and script exit without deaths.
EMSCRIPTEN_KEEPALIVE i32 application_test_combat_batch(u32 ticks){if(!app||!app->scene()||ticks>120)return 0;const i32 stage=app->progress.stage;app->graphics.backend.defer=true;bool ok=true;for(u32 i=0;i<ticks&&ok&&app->scene()&&app->progress.stage==stage;i++){app->scene()->battle.player->life.invulnerability.set(60000);const u32 age=app->frames,held=1|0x200|(age%180<90?0x10:0x20)|(age%120<60?8:0);ok=app->step(held,held&~app->physical_input.held,0,60,false);}app->graphics.backend.commit();return ok;}
EMSCRIPTEN_KEEPALIVE const char* application_error(){return app?app->failure.c_str():last_error.c_str();}
EMSCRIPTEN_KEEPALIVE const i32* application_state(){return app?app->projection():nullptr;}
EMSCRIPTEN_KEEPALIVE const i32* application_loading_state(){static std::array<i32,8> value;value.fill(0);if(app){auto* vm=app->animations.registry.find(app->transition_overlay);value={i32(app->loading_destination!=AppScene::Uninitialized),i32(app->loading_destination),i32(app->loading_frames),i32(app->loading_duration),i32(app->loading_handle),i32(vm&&vm->geometry.overlay!=nullptr),i32(app->environment.game_rng.seed),i32(app->environment.game_rng.calls)};}return value.data();}
EMSCRIPTEN_KEEPALIVE const i32* application_manual_state(){static std::array<i32,5> value;value.fill(-1);if(app&&app->pause_manual){const auto& m=*app->pause_manual;value={m.mode,m.phase,m.age.current,m.menu.selected,truncate_int(m.offset_x)};}return value.data();}
EMSCRIPTEN_KEEPALIVE const i32* application_pause_state(){static std::array<i32,7> value;value.fill(-1);if(app&&app->pause){const auto& p=app->pause->state;value={i32(p.screen),p.phase,p.menu.selected,p.menu.count,p.names.selected,p.name_length,p.age.current};}return value.data();}
EMSCRIPTEN_KEEPALIVE void application_title_select(i32 selected){if(app&&app->title)app->title->state.menu.select(selected);}
EMSCRIPTEN_KEEPALIVE void application_name_select(i32 selected){if(app&&app->pause)app->pause->state.names.select(selected);}
EMSCRIPTEN_KEEPALIVE const i32* application_world_state(){static std::array<i32,24> v;v.fill(0);if(app&&app->scene()){auto& s=*app->scene();auto& b=s.battle;v={s.controller->counters[0],b.player->motion.position_fixed.x,b.player->motion.position_fixed.y,b.player->life.state,b.session.extra_lives,b.session.life_pieces,b.session.bombs,b.session.bomb_pieces,b.session.power,b.session.deaths,b.score.score,b.score.point_value,b.graze_total,b.enemy_world.current_chapter,i32(b.enemies->count()),i32(b.bullet_scene->manager.visible_count),app->environment.game_rng.seed,i32(app->environment.game_rng.calls),i32(app->visual.seed),i32(app->visual.calls),i32(s.background.drawn_instances),i32(s.background.culled_instances),i32(s.background.drawn_primitives),i32(s.background_flags)};}return v.data();}
// Unmodified replay inputs, no protection or forced scene completion. As with
// the production catch-up path, updates/draws run and only presentation waits.
EMSCRIPTEN_KEEPALIVE i32 application_test_replay_batch(u32 ticks,i32* output){if(!app||!output||ticks>240)return 0;app->graphics.backend.defer=true;for(u32 i=0;i<ticks;i++){if(!app->step(0,0,0,60,false)){app->graphics.backend.commit();return 0;}std::copy_n(application_world_state(),24,output+i*24);}app->graphics.backend.commit();return 1;}
EMSCRIPTEN_KEEPALIVE const float* application_camera_state(){return app&&app->scene()?&app->scene()->background.script.state.camera.position.x:nullptr;}
EMSCRIPTEN_KEEPALIVE const PlayerShot* application_shots(){return app&&app->scene()?app->scene()->battle.player->shots.shots.data():nullptr;}
EMSCRIPTEN_KEEPALIVE const DamageSource* application_damage(){return app&&app->scene()?app->scene()->battle.player->damage.sources.data():nullptr;}
EMSCRIPTEN_KEEPALIVE i32 application_prepare_replay(i32 cleared){return app&&app->prepare_live_replay(cleared!=0);}
EMSCRIPTEN_KEEPALIVE i32 application_save_replay(i32 slot,const char* name){if(!app||!name||std::strlen(name)>8)return 0;std::array<char,9> text{};std::memcpy(text.data(),name,std::strlen(name));return app->save_slot(slot,text);}
EMSCRIPTEN_KEEPALIVE void application_keyboard_key(u32 key,u32 down){if(app&&key<256){app->keyboard_state.format=2;app->keyboard_state.keys[key]=down?128:0;}}
EMSCRIPTEN_KEEPALIVE const char* application_music(){if(!app)return "";double seconds=0;app->audio_device.current_music(music_name,seconds);return music_name.c_str();}
// Observe the actual texture owned by GraphicsDevice, independently of the
// logical ANM bank. The integration test compares its bytes to the native atlas.
EMSCRIPTEN_KEEPALIVE const u32* application_test_ascii_image(){static std::array<u32,8> value;value.fill(0);if(!app)return nullptr;auto* bank=app->animations.resource(app->shared.at("ascii.anm"));if(!bank||bank->textures.empty())return nullptr;auto* image=app->graphics.pixels(app->graphics.texture(*bank,0));if(!image)return nullptr;value={bank->textures[0].width,bank->textures[0].height,image->width,image->height,image->pitch,image->packed_format,u32(image->pixels.size()),u32(bank->sprites.size())};return value.data();}
EMSCRIPTEN_KEEPALIVE const u8* application_test_ascii_pixels(){if(!app)return nullptr;auto* bank=app->animations.resource(app->shared.at("ascii.anm"));if(!bank)return nullptr;auto* image=app->graphics.pixels(app->graphics.texture(*bank,0));return image?image->pixels.data():nullptr;}
EMSCRIPTEN_KEEPALIVE const AnmSprite* application_test_ascii_sprites(){if(!app)return nullptr;auto* bank=app->animations.resource(app->shared.at("ascii.anm"));return bank?bank->sprites.data():nullptr;}
EMSCRIPTEN_KEEPALIVE const u8* application_pixels(){if(!app)return nullptr;app->graphics.backend.read(sdl::GraphicsDevice::screen);return app->graphics.pixels(sdl::GraphicsDevice::screen)->pixels.data();}
EMSCRIPTEN_KEEPALIVE const touhou::sdl::Statistics* application_graphics_stats(){return app?&app->graphics.backend.stats:nullptr;}
EMSCRIPTEN_KEEPALIVE void application_test_staged_uploads(i32 value){if(app){app->graphics.backend.flush();app->graphics.backend.staged_uploads=value!=0;}}
EMSCRIPTEN_KEEPALIVE const u32* application_test_audio_stats(){return app?app->audio_device.statistics():nullptr;}
EMSCRIPTEN_KEEPALIVE void application_test_protection(i32 frames){if(app&&app->scene())app->scene()->battle.player->life.invulnerability.set(frames);}
// Observe the real ordered drawing passes while normal application updates and
// draws run. These temporary callbacks never alter the camera or game state.
EMSCRIPTEN_KEEPALIVE i32 application_test_shake_frames(u32 held,u32 pressed,u32 ticks,float* output){
 if(!app||!app->scene()||!output||!ticks||ticks>240)return 0;
 auto* scene=app->scene();auto& scheduler=scene->frame_scheduler();
 struct Probe {u32 index;float* sample;sdl::ApplicationState* app;};std::array<Probe,4> probes;std::array<FrameCallback,4> callbacks;const i32 priorities[]={2,13,47,60};
 for(u32 i=0;i<4;i++){probes[i]={i,output,app.get()};auto& c=callbacks[i];c.owner=&probes[i];c.enabled=true;c.run=[](void* p)->i32{auto& probe=*static_cast<Probe*>(p);if(!probe.index){auto& effects=probe.app->scene()->battle.screen_effects;probe.sample[0]=effects.world_offset.x;probe.sample[1]=effects.world_offset.y;probe.sample[2]=effects.screen_offset.x;probe.sample[3]=effects.screen_offset.y;}else{const auto offset=probe.app->renderer.offset;probe.sample[2+probe.index*2]=offset.x;probe.sample[3+probe.index*2]=offset.y;}return 1;};scheduler.add(c,FramePass::Draw,priorities[i]);}
 app->graphics.backend.defer=true;bool ok=true;
 for(u32 frame=0;frame<ticks&&ok;frame++){
  float* sample=output+frame*16;std::fill_n(sample,16,-9999.f);for(auto& probe:probes)probe.sample=sample;
  ok=app->step(held,frame?0:pressed,0,60,false);if(app->scene()!=scene){ok=false;break;}
  auto& effects=scene->battle.screen_effects;sample[10]=effects.world_offset.x;sample[11]=effects.world_offset.y;sample[12]=effects.screen_offset.x;sample[13]=effects.screen_offset.y;sample[14]=float(effects.count());sample[15]=float(scene->battle.session.bomb_state);
 }
 if(app->scene()==scene)for(auto& c:callbacks)scheduler.remove(c);app->graphics.backend.commit();return ok;
}
EMSCRIPTEN_KEEPALIVE i32 application_test_nudge(i32 duration,i32 first,i32 last){return app&&app->scene()&&app->scene()->battle.screen_effects.nudge({duration,first,last});}
EMSCRIPTEN_KEEPALIVE i32 application_test_game_over(){if(!app||!app->scene()||!app->pause)return 0;app->scene()->battle.session.extra_lives=-1;return app->pause->open(PauseEntrance::GameOver);}
EMSCRIPTEN_KEEPALIVE i32 application_test_death(){return app&&app->scene()&&app->scene()->battle.player->life.commit_death();}
EMSCRIPTEN_KEEPALIVE i32 application_complete_stage(){return app&&app->scene()&&app->scene()->progression->next_stage();}
EMSCRIPTEN_KEEPALIVE i32 application_test_ending(i32 index){if(!app||!app->title)return 0;return app->preview_ending(index);}
EMSCRIPTEN_KEEPALIVE const i32* application_ending_state(){static std::array<i32,6> value;value.fill(-1);if(app&&app->ending){const auto& e=*app->ending;value={1,i32(e.script.finished),i32(e.script.state.flags),e.script.state.clock.current,e.script.state.line,e.frame.frames};}return value.data();}
EMSCRIPTEN_KEEPALIVE i32 application_unlock(){return app&&app->records.unlock_all();}
EMSCRIPTEN_KEEPALIVE i32 application_test_replay(const char* name,i32 stage){if(!app||!app->title||!name||!app->files.replay(name,app->selected_replay)||!app->selected_replay)return 0;ReplayStageState entry;if(!app->selected_replay->initial_state(stage,entry))return 0;auto& p=app->progress;p.stage=p.initial_stage=stage;p.character=entry.run.character;p.animal=entry.run.animal;p.difficulty=entry.run.difficulty;p.spell_id=entry.run.spell_id;p.scoring.mode_flags=0;p.demo_slot=-1;app->flow.pending=AppScene::ReplayStart;return 1;}
// Run the supplied recording's held keys through the live input/recorder path.
// This never changes player vulnerability or forces any stage destination.
EMSCRIPTEN_KEEPALIVE i32 application_test_live_from_replay(const char* name,i32 stage){if(!app||!app->title||!name)return 0;std::shared_ptr<Replay> source;if(!app->files.replay(name,source)||!source)return 0;ReplayStageState entry;if(!source->initial_state(stage,entry))return 0;auto& p=app->progress;p.stage=p.initial_stage=stage;p.character=entry.run.character;p.animal=entry.run.animal;p.difficulty=entry.run.difficulty;p.spell_id=entry.run.spell_id;p.scoring.mode_flags=0;p.demo_slot=-1;app->environment.game_rng.seed=entry.seed;app->visual.seed=entry.seed;app->flow.pending=AppScene::Game;return 1;}
EMSCRIPTEN_KEEPALIVE i32 application_test_live_batch(const u16* held,u32 ticks,i32* output){if(!app||!held||!output||ticks>240)return 0;app->graphics.backend.defer=true;for(u32 i=0;i<ticks;i++){if(!app->step(held[i],held[i]&~app->physical_input.held,0,60,false)){app->graphics.backend.commit();return 0;}std::copy_n(application_world_state(),24,output+i*26);output[i*26+24]=app->recording?app->recording->current_stage():0;output[i*26+25]=app->recording?app->recording->frame_clock():-1;}app->graphics.backend.commit();return 1;}
EMSCRIPTEN_KEEPALIVE i32 application_test_replay_trace(u32 ticks,i32* output){if(!app||!output||ticks>240)return 0;app->graphics.backend.defer=true;for(u32 i=0;i<ticks;i++){if(!app->step(0,0,0,60,false)){app->graphics.backend.commit();return 0;}std::copy_n(application_world_state(),24,output+i*26);output[i*26+24]=app->selected_replay?app->selected_replay->selected_stage():0;output[i*26+25]=app->selected_replay?app->selected_replay->frame():-1;}app->graphics.backend.commit();return 1;}
EMSCRIPTEN_KEEPALIVE i32 application_test_recording_frames(){return app&&app->recording?app->recording->frame_clock():-1;}
EMSCRIPTEN_KEEPALIVE i32 application_test_recording_stage(){return app&&app->recording?app->recording->current_stage():0;}
EMSCRIPTEN_KEEPALIVE i32 application_test_replay_stage(){return app&&app->selected_replay?app->selected_replay->selected_stage():0;}
EMSCRIPTEN_KEEPALIVE const u8* application_test_player_diagnostic(i32 field){if(!app||!app->scene())return nullptr;auto& p=*app->scene()->battle.player;switch(field){case 0:return reinterpret_cast<const u8*>(&p.frame.collision);case 1:return reinterpret_cast<const u8*>(&p.frame.bounds);case 2:return reinterpret_cast<const u8*>(&p.motion);case 3:return reinterpret_cast<const u8*>(&p.life.invulnerability);default:return nullptr;}}
EMSCRIPTEN_KEEPALIVE u32 application_test_player_diagnostic_size(i32 field){switch(field){case 0:return sizeof(PlayerCollision);case 1:return sizeof(PlayerBounds);case 2:return offsetof(PlayerMotion,options);case 3:return sizeof(Timer);default:return 0;}}
EMSCRIPTEN_KEEPALIVE const BulletState* application_test_bullet_diagnostic(u32 slot){return app&&app->scene()?app->scene()->battle.bullet_scene->manager.state(slot):nullptr;}
EMSCRIPTEN_KEEPALIVE u32 application_test_bullet_diagnostic_size(){return sizeof(BulletState);}
EMSCRIPTEN_KEEPALIVE u32 application_test_bullet_next(u32 slot){if(!app||!app->scene())return BulletManager::none;auto& b=app->scene()->battle.bullet_scene->manager;return slot==BulletManager::none?b.first_active():b.next_active(slot);}
EMSCRIPTEN_KEEPALIVE void application_test_bullet_bounds(float x,float y){if(app&&app->scene())app->scene()->battle.bullet_scene->manager.context.reflection_bounds={x,y};}
EMSCRIPTEN_KEEPALIVE const u32* application_test_simulation_hashes(){static std::array<u32,2> hashes;if(!app||!app->scene())return nullptr;auto& b=app->scene()->battle;hashes.fill(2166136261u);auto add=[&](u32 kind,const void* data,u32 size){const auto* p=static_cast<const u8*>(data);for(u32 i=0;i<size;i++)hashes[kind]=(hashes[kind]^p[i])*16777619u;};auto& bullets=b.bullet_scene->manager;for(u32 slot=bullets.first_active();slot!=BulletManager::none;slot=bullets.next_active(slot)){const auto& s=*bullets.state(slot);add(0,&s.motion,sizeof s.motion);add(0,&s.flags,sizeof s.flags);add(0,&s.phase,sizeof s.phase);add(0,&s.lifetime.current,4);add(0,&s.collision_delay,4);add(0,&s.hitbox,sizeof s.hitbox);}for(u32 i=0;i<b.enemies->count();i++){const auto& e=b.enemies->at(i)->state;add(1,&e.motion,sizeof e.motion);add(1,&e.life,4);add(1,&e.flags,4);add(1,e.integer_registers.data(),sizeof e.integer_registers);add(1,e.float_registers.data(),sizeof e.float_registers);}return hashes.data();}
EMSCRIPTEN_KEEPALIVE void application_close(){app.reset();}
}
