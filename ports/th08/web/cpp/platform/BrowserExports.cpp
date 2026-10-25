#include "BrowserRuntime.hpp"
#include <cstdlib>
#include <algorithm>
#include <cstring>
#include "../game/ValidationScene.hpp"
#ifdef TH_SDL3
#include "../sdl/GraphicsHost.hpp"
#include "../sdl/ThpracUi.hpp"
#endif
using namespace th08;
#ifdef __EMSCRIPTEN__
#define EX(name) __attribute__((export_name(name)))
#else
#define EX(name)
#endif
namespace {
void validation_scene_snapshot(BrowserRuntime* r,u32* out){
    std::fill(out,out+16,0);out[0]=1;if(!r)return;
    const auto& a=r->app;const auto& g=a.game;const auto& gate=g.enemies.validation_scene();
    out[1]=gate.enabled();out[3]=a.in_game()&&!a.loading_game()&&g.ready();
    out[4]=g.paused||g.menus.context.pause_state||g.dialogue.present()||
        g.player_state.context.game_over||(g.globals.game_flags&0x60);
    out[5]=(g.globals.game_flags&8)!=0||a.session.practice.replay;
    out[6]=g.enemies.population.active_count();
    out[7]=g.projectile_pool.live_bullet_count();
    for(const auto& laser:g.projectile_pool.lasers)out[8]+=laser.in_use!=0;
    out[2]=out[3]&&!out[4]&&!out[5]&&!out[6]&&!out[7]&&!out[8];
    out[9]=g.globals.stage;out[10]=g.enemies.state.frames;out[11]=g.control.state.frames;
    out[12]=gate.withheld_updates();out[13]=g.program.timeline_count();
    out[14]=a.session.practice.assisted;out[15]=u8(g.player_state.life.state);
}
u32 deathbomb_float_word(float value){u32 word=0;static_assert(sizeof(word)==sizeof(value));std::memcpy(&word,&value,sizeof(word));return word;}
bool validation_deathbomb_write_lock_valid(const BrowserRuntime& r){
    const auto& receipt=r.validation_scene_writes.receipt();
    return r.validation_scene_used&&receipt.locked&&!receipt.load_scope&&receipt.committed_bytes==0;
}
bool validation_deathbomb_quiet_world(const GameplayScene& g){
    if(g.enemies.population.active_count()!=0||g.projectile_pool.live_bullet_count()!=0||g.player_state.bomb.active||
       g.menus.context.pause_state||g.dialogue.present()||g.player_state.context.game_over||g.player_state.context.pause)return false;
    for(const auto& laser:g.projectile_pool.lasers)if(laser.in_use)return false;
    for(const auto& shot:g.player_state.shots.shots)if(shot.state)return false;
    return true;
}
bool validation_custom_practice(const GameApplication& app){
    const auto& p=app.session.practice;
    return validation_custom_practice_eligible(p.enabled,p.active,p.replay,p.run.mode,
        practice_runtime_stage(p.run),i32(app.game.globals.stage));
}
void validation_deathbomb_snapshot(BrowserRuntime* r,u32* out){
    std::fill(out,out+deathbomb_stimulus_word_count,0);out[0]=deathbomb_stimulus_version;if(!r)return;
    const auto& app=r->app;const auto& g=app.game;const auto& receipt=g.bullets.validation_deathbomb_receipt();
    const auto& gate=g.enemies.validation_scene();const bool lock_valid=validation_deathbomb_write_lock_valid(*r);
    const bool fixture=app.in_game()&&!app.loading_game()&&g.ready()&&gate.enabled()&&r->validation_scene_used;
    const bool quiet=validation_deathbomb_quiet_world(g);
    const auto& input=g.player.consumed_input();
    out[1]=app.in_game()&&!app.loading_game()&&g.ready();out[2]=gate.enabled();out[3]=lock_valid;
    out[4]=g.bullets.validation_deathbomb_eligible(fixture,lock_valid,quiet,validation_custom_practice(app));
    out[5]=receipt.request_status;out[6]=receipt.request_sequence;out[7]=receipt.projectile_status;
    out[8]=receipt.collision_source;out[9]=receipt.collision_sequence;out[10]=receipt.consumer_sequence_at_collision;
    out[11]=input.sequence;out[12]=input.valid;out[13]=u32(i32(g.player_state.life.state));
    out[14]=u32(g.player_state.life.timer.current);out[15]=u32(g.player_state.life.predead_count);
    out[16]=g.player_state.life.deathbomb;out[17]=u32(g.player_state.bomb.cooldown);
    out[18]=u32(g.player_state.bomb.active);out[19]=u32(g.player_state.bomb.type);
    out[20]=u32(g.player_state.bomb.triggered);out[21]=u32(g.player_state.bomb.consumed);
    out[22]=u32(g.player_state.context.bombs);out[23]=g.player_state.bomb_input.regular_bombs;
    out[24]=u32(i32(app.session.numbers.lives));out[25]=u32(i32(app.session.numbers.deaths));
    out[26]=u32(i32(app.session.numbers.bombs_used));out[27]=u32(i32(app.session.numbers.bombs_used_stage));
    out[28]=deathbomb_float_word(g.player_state.motion.movement.position.x);
    out[29]=deathbomb_float_word(g.player_state.motion.movement.position.y);
    out[30]=g.projectile_pool.live_bullet_count();out[31]=receipt.life_state_before_collision;
    out[32]=receipt.life_state_after_collision;out[33]=receipt.collision_result;
    out[34]=g.bullets.validation_deathbomb_projectile_state();
}
}
extern "C" {
// Preparation is explicit and monotonic for this world. There is no operation
// that resumes a suppressed timeline in a world whose logical clock advanced.
EX("validation_scene_prepare") bool browser_validation_scene_prepare(BrowserRuntime* r){
    if(!r)return false;u32 state[16];validation_scene_snapshot(r,state);
    if(!r->app.game.enemies.validation_scene().begin(state[3],state[5],state[4],state[6],state[7],state[8]))return false;
    r->validation_scene_used=true;r->validation_scene_writes.arm();r->app.session.practice.assisted=true;return true;
}
EX("validation_scene_state") const u32* browser_validation_scene_state(BrowserRuntime* r){
    static u32 out[16];validation_scene_snapshot(r,out);return out;
}
EX("validation_scene_arm") bool browser_validation_scene_arm(BrowserRuntime* r){
    if(!r||!r->app.active()||r->app.in_game()||r->app.loading_game()||r->validation_scene_used||
       r->validation_scene_request_pending||r->app.game.enemies.validation_scene().enabled())return false;
    r->validation_scene_request_pending=true;r->validation_scene_request_status=1;
    r->validation_scene_accepted_before_first_update=false;
    if(++r->validation_scene_request_generation==0)++r->validation_scene_request_generation;
    return true;
}
// Lifecycle evidence is a separate ABI so validation_scene_state's 16 words
// keep their original meanings for existing readers.
EX("validation_scene_arm_state") const u32* browser_validation_scene_arm_state(BrowserRuntime* r){
    static u32 out[8]{};std::fill(out,out+8,0);out[0]=1;if(!r)return out;
    const auto& gate=r->app.game.enemies.validation_scene();out[1]=r->validation_scene_request_status;
    out[2]=r->validation_scene_request_generation;out[3]=r->validation_scene_request_pending;
    out[4]=r->validation_scene_accepted_before_first_update;out[5]=gate.first_timeline_withheld();
    out[6]=gate.update_started();out[7]=r->validation_scene_used;return out;
}
EX("validation_scene_write_state_v1") const u32* browser_validation_scene_write_state_v1(BrowserRuntime* r){
    static u32 out[8]{};std::fill(out,out+8,0);out[0]=1;if(!r)return out;
    const auto& receipt=r->validation_scene_writes.receipt();out[1]=receipt.locked;out[2]=receipt.load_scope;
    out[3]=receipt.attempts;out[4]=receipt.ram_overrides;out[5]=receipt.discarded_bytes;
    out[6]=receipt.committed_bytes;out[7]=receipt.rejected_writes;return out;
}
// Deathbomb fixture stimuli are unavailable until the native disposable scene
// and its write lock are both active. The snapshot only copies current owners.
EX("validation_deathbomb_hit_v1") bool browser_validation_deathbomb_hit_v1(BrowserRuntime* r){
    if(!r)return false;auto& app=r->app;auto& g=app.game;const auto& gate=g.enemies.validation_scene();
    const bool fixture=app.in_game()&&!app.loading_game()&&g.ready()&&gate.enabled()&&r->validation_scene_used;
    return g.bullets.request_validation_deathbomb_hit(fixture,validation_deathbomb_write_lock_valid(*r),
        validation_deathbomb_quiet_world(g),validation_custom_practice(app));
}
EX("validation_deathbomb_state_v1") const u32* browser_validation_deathbomb_state_v1(BrowserRuntime* r){
    static u32 out[deathbomb_stimulus_word_count]{};validation_deathbomb_snapshot(r,out);return out;
}
EX("practice_enable") void browser_practice_enable(BrowserRuntime* r,bool enabled){if(r&&!r->app.in_game()){auto& p=r->app.session.practice;p.enabled=enabled;if(!enabled)p.menu=p.accepted=p.active=false;}}
EX("practice_status") const i32* browser_practice_status(BrowserRuntime* r){static i32 out[7]{};if(!r)return out;const auto& p=r->app.session.practice;
    out[0]=p.menu;out[1]=r->app.title.context.difficulty;out[2]=r->app.title.context.character;out[3]=p.active;out[4]=p.replay;out[5]=p.cheats;out[6]=p.assisted;return out;}
// Versioned read-only practice snapshot. Header words 0..17 are version,
// validity mask, practice flags, title screen/phase/cursor and selected game
// identity. Offsets 20/46/72 hold configured/run/replay-candidate's explicit
// 26-word PracticeConfig schema; offsets 100..127 are values consumed by the
// active GameApplication/GameplayScene. Values are copied from their current
// owners and never trigger configuration or gameplay transitions.
EX("practice_snapshot_v1") const double* browser_practice_snapshot_v1(BrowserRuntime* r){
    static double out[128];std::fill(out,out+128,0.0);out[0]=1.0;if(!r)return out;
    const auto& a=r->app;const auto& p=a.session.practice;const auto& g=a.game;const auto& n=a.session.numbers;
    const bool configured=p.enabled,run=p.accepted||p.active,candidate=p.replay_candidate_valid;
    const bool applied=p.active&&a.in_game()&&g.ready()&&practice_runtime_stage(p.run)==i32(g.globals.stage);
    out[1]=(configured?1:0)|(run?2:0)|(candidate?4:0)|(applied?8:0);
    out[2]=p.enabled;out[3]=p.menu;out[4]=p.accepted;out[5]=p.active;out[6]=p.replay;
    out[7]=p.assisted;out[8]=p.replay_candidate_valid;out[9]=a.in_game();
    out[10]=i32(a.title.menus.state.currentScreen);out[11]=a.title.menus.state.currentScreenState;
    out[12]=a.title.menus.state.cursor;out[13]=g.globals.stage;out[14]=a.title.context.difficulty;
    out[15]=a.title.context.character;out[16]=a.title.context.shotType;out[17]=g.globals.game_flags;
    out[18]=a.title.menus.state.stateTimer;
    out[19]=2u|(a.title.confirmation_ready()?1u:0u);
    if(configured)p.configured.encode(out+20);
    if(run)p.run.encode(out+46);
    if(candidate)p.replay_candidate.encode(out+72);
    if(applied){
        out[100]=n.score;out[101]=n.display_score;out[102]=n.lives;out[103]=n.bombs;out[104]=n.power;
        out[105]=n.gauge;out[106]=n.gauge_copy;out[107]=n.graze;out[108]=n.graze_stage;
        out[109]=n.points;out[110]=n.points_stage;out[111]=n.time_orbs;out[112]=n.total_time_orbs;
        out[113]=n.point_value;out[114]=n.clock_time;out[115]=a.session.rank.value;
        out[116]=a.session.rank.minimum;out[117]=a.session.rank.maximum;
        out[118]=g.enemies.population.practice_familiar;out[119]=g.enemies.state.frames;
        out[120]=g.control.state.frames;out[121]=u8(g.player_state.life.state);
        out[122]=p.familiar_pending;out[123]=p.cheats;out[124]=p.everlasting_bgm;
        out[125]=g.globals.stage;out[126]=g.globals.difficulty;out[127]=g.globals.shot;
    }
    return out;
}
// Captured once by GameplayScene::load immediately after apply_practice and
// before bind_jobs. This separate ABI preserves practice_snapshot_v1's live
// values while a Replay advances score and other counters.
EX("practice_initial_snapshot_v1") const double* browser_practice_initial_snapshot_v1(BrowserRuntime* r){
    static const double unavailable[80]={1.0};
    return r?r->app.game.practice_initial_snapshot_data():unavailable;
}
// Same 80-word load-boundary layout, for ordinary and Practice gameplay.
// Validity bits: 1=initialized, 2=Replay, 4=custom Practice; run config is
// present only for custom Practice. Reads never advance or reset the game.
EX("gameplay_initial_snapshot_v1") const double* browser_gameplay_initial_snapshot_v1(BrowserRuntime* r){
    static const double unavailable[80]={1.0};
    return r?r->app.game.gameplay_initial_snapshot_data():unavailable;
}
// Navigation observation only: no menu input, string lookup or game transition.
EX("language_ui_snapshot_v1") const double* browser_language_ui_snapshot_v1(BrowserRuntime* r){
    static double out[20]{};std::fill(out,out+20,0.);out[0]=1;if(!r)return out;
    const auto& app=r->app;const auto& title=app.title;const auto& result=app.results.controls.state;
    out[1]=1;out[2]=app.supervisor.state.active;out[3]=app.active();out[4]=app.loading_game();
    out[5]=title.menus.state.currentScreen;out[6]=title.menus.state.currentScreenState;out[7]=title.menus.state.cursor;out[8]=title.confirmation_ready();
    out[9]=app.results.active();out[10]=result.currentState;out[11]=result.menuDepth;out[12]=result.cursor;out[13]=result.frameTimer;out[14]=result.frameTimer2;
    out[15]=app.in_game();
    if(app.supervisor.state.active==1){out[16]=title.context.input.current;out[17]=title.context.input.previous;}
    else if(app.results.active()){out[16]=app.results.controls.context.input.current;out[17]=app.results.controls.context.input.previous;}
    out[18]=app.game.display.dialogue.lines[0].color1.a;out[19]=app.game.display.dialogue.lines[1].color1.a;return out;
}
EX("practice_tracker_status") const i32* browser_practice_tracker_status(BrowserRuntime* r){static i32 out[7]{};std::fill(out,out+7,0);if(!r)return out;
    const auto& app=r->app;if(!app.in_game())return out;const auto& n=app.session.numbers;
    out[0]=1;out[1]=app.game.globals.shot;out[2]=Scalar::truncate(n.deaths);out[3]=app.session.practice.tracker_dissolve_count;
    out[4]=Scalar::truncate(n.bombs_used);out[5]=n.captured_spells;out[6]=app.session.practice.tracker_last_spell_captures;return out;}
// Trainer consumers: resources, actual tagged collision and boss clocks/damage.
// Reads only copy existing owners; no menu or gameplay actions occur here.
EX("practice_effects_snapshot_v1") const double* browser_practice_effects_snapshot_v1(BrowserRuntime* r){
    static double out[48]{};std::fill(out,out+48,0.);out[0]=1;if(!r||!r->app.in_game())return out;
    const auto& a=r->app;const auto& g=a.game;const auto& n=a.session.numbers;const auto& p=a.session.practice;
    out[1]=1;out[2]=g.control.state.frames;out[3]=g.globals.stage;out[4]=p.cheats;
    out[5]=n.lives;out[6]=n.bombs;out[7]=n.power;out[8]=n.deaths;out[9]=n.bombs_used;
    out[10]=p.tracker_dissolve_count;out[11]=n.captured_spells;out[12]=p.tracker_last_spell_captures;
    out[13]=g.player_state.life.state;out[14]=g.player_state.life.predead_count;
    out[15]=g.player_state.bomb.active;out[16]=g.player_state.bomb.type;out[17]=g.player_state.bomb.consumed;
    const auto& input=g.player.consumed_input();out[18]=input.sequence;out[19]=input.valid;
    const auto& hit=g.bullets.validation_hit_observation();for(unsigned i=0;i<hit.size();i++)out[20+i]=hit[i];
    for(unsigned i=0;i<2;i++)if(const auto* boss=g.globals.boss_slots[i]){
        auto* b=out+25+i*8;b[0]=1;b[1]=boss->life;b[2]=boss->timeout;b[3]=boss->lifetime.current;
        b[4]=boss->last_damage;b[5]=boss->flags;b[6]=reinterpret_cast<std::uintptr_t>(boss);
    }
    out[41]=p.active;out[42]=p.replay;out[43]=p.accepted;out[44]=p.enabled;out[45]=g.globals.game_flags;
    return out;
}
#ifdef TH_SDL3
EX("practice_controls_snapshot_v1") const double* browser_practice_controls_snapshot_v1(BrowserRuntime* r){static double out[1024]{};std::fill(out,out+1024,0.);out[0]=1;if(r)ThpracUi::controls_observation(*r,out,1024);return out;}
EX("practice_ui_snapshot_v1") const u32* browser_practice_ui_snapshot_v1(BrowserRuntime* r){static u32 out[128]{};std::fill(out,out+128,0u);out[0]=1;if(r)ThpracUi::observation(out,128);return out;}
#endif
EX("view_options_snapshot_v1") const double* browser_view_options_snapshot_v1(BrowserRuntime* r){
    static double out[32]{};std::fill(out,out+32,0.);out[0]=1;if(!r||!r->app.in_game())return out;
    const auto& g=r->app.game;const auto& p=g.player_state;const auto& m=p.motion;const auto* effect=m.form.focus_effect;
    out[1]=1;out[2]=g.always_hitbox;out[3]=p.input.always_hitbox;out[4]=m.form.focused;out[5]=effect!=nullptr;
    if(effect){out[6]=effect->visible;out[7]=effect->flags;out[8]=effect->scriptIndex;out[9]=effect->activeSpriteIndex;
        out[10]=effect->color1.a;out[11]=effect->pos.x;out[12]=effect->pos.y;}
    out[13]=m.movement.position.x;out[14]=m.movement.position.y;out[15]=p.input.buttons;
    for(unsigned i=0;i<3;i++){out[16+i*2]=m.movement.half_boxes[i].x;out[17+i*2]=m.movement.half_boxes[i].y;}
    out[22]=g.enemies.state.frames;return out;
}
// Frozen actual ECL bytes after PracticeRuntime application, before gameplay jobs.
EX("practice_program_snapshot_v1") const u32* browser_practice_program_snapshot_v1(BrowserRuntime* r){
    static u32 out[4]{};std::fill(out,out+4,0u);out[0]=1;if(!r)return out;
    const auto& bytes=r->app.game.practice_initial_program();if(bytes.empty())return out;
    out[1]=1;out[2]=u32(reinterpret_cast<std::uintptr_t>(bytes.data()));out[3]=u32(bytes.size());return out;
}
EX("practice_configure") bool browser_practice_configure(BrowserRuntime* r,const double* words,u32 count,bool accept){
    if(!r||!r->app.session.practice.enabled||r->app.in_game())return false;auto& p=r->app.session.practice;PracticeConfig config;
    if(!config.decode(words,count)||(accept&&!p.menu))return false;p.configured=config;
    if(accept){p.run=config;p.accepted=true;}return true;
}
EX("practice_cancel") void browser_practice_cancel(BrowserRuntime* r){if(!r||!r->app.session.practice.menu)return;auto& p=r->app.session.practice;p.menu=p.accepted=false;
    r->app.title.menus.state.cursor=r->app.title.context.character;r->app.title.menus.ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);}
EX("practice_cheats") bool browser_practice_cheats(BrowserRuntime* r,u32 mask){if(!r||!r->app.in_game()||!r->app.session.practice.enabled||r->app.session.practice.replay||mask>127)return false;auto& p=r->app.session.practice;p.cheats=mask;if(mask)p.assisted=true;return true;}
EX("resident_hits") u32 browser_resident_hits(BrowserRuntime* r){return r->app.library.resident_hits();}
EX("preload_stats") const u32* browser_preload_stats(BrowserRuntime* r){static u32 out[2];out[0]=r->app.library.preload_count();out[1]=r->app.library.preload_hits();return out;}
EX("allocate") void* browser_allocate(u32 n){return std::calloc(n,1);}
EX("deallocate") void browser_deallocate(void* p){std::free(p);}
EX("create") BrowserRuntime* browser_create(){auto* r=new BrowserRuntime();
#ifdef TH_SDL3
if(!sdl_attach(r)){delete r;return nullptr;}
#endif
return r;}
EX("destroy") void browser_destroy(BrowserRuntime* r){delete r;
#ifdef TH_SDL3
sdl_detach();
#endif
}
EX("asset") bool browser_asset(BrowserRuntime* r,const char* p,const u8* b,u32 n){return r&&r->put(p,b,n);}
EX("archive") bool browser_archive(BrowserRuntime* r,const u8* b,u32 n){return r&&r->put_archive(b,n);}
EX("font") bool browser_font(BrowserRuntime* r,i32 k,const u8* b,u32 n){return r&&r->put_font(k,b,n);}
EX("image") bool browser_image(BrowserRuntime* r,const char* p,u32 w,u32 h,const u8* b,u32 n){return r&&r->put_image(p,w,h,b,n);}
EX("file") const u8* browser_file(BrowserRuntime* r,const char* p){return r?r->file(p).data():nullptr;}
EX("file_size") u32 browser_file_size(BrowserRuntime* r,const char* p){return r?r->file(p).size():0;}
EX("texture") const BrowserTexture* browser_texture(BrowserRuntime* r,u32 h){return r?r->texture(h):nullptr;}
EX("backbuffer") u32 browser_backbuffer(BrowserRuntime* r){return r?r->backbuffer():0;}
EX("initialize") bool browser_initialize(BrowserRuntime* r){return r&&r->initialize();}
EX("step") bool browser_step(BrowserRuntime* r,bool render){return r&&r->step(render);}
EX("audio_tick") bool browser_audio_tick(BrowserRuntime* r,u32 now){return r&&r->audio_tick(now);}
EX("status") i32 browser_status(BrowserRuntime* r,i32 k){return r?r->status(k):-1;}
EX("keyboard") u8* browser_keyboard(BrowserRuntime* r){return r?r->keyboard_state():nullptr;}
EX("controller") void browser_controller(BrowserRuntime* r,i32 x,i32 y,const u8* b,u32 n,bool available){if(r)r->controller_state(x,y,b,n,available);}
EX("close") void browser_close(BrowserRuntime* r){if(r)r->app.close();}
EX("save") bool browser_save(BrowserRuntime* r){return r&&!r->validation_scene_used&&r->app.save_score();}
// Finalize the current recording using the same owner as the result screen.
EX("save_replay") bool browser_save_replay(BrowserRuntime* r,i32 slot,const char* name){return r&&!r->validation_scene_used&&r->app.finalize_replay(slot,name);}
EX("touch_target") void browser_touch_target(BrowserRuntime* r,i32 mode,float x,float y){if(r)r->motion.target(mode,x,y);}
// Read-only touch metadata for the browser's movement target and controls.
EX("touch_state") void browser_touch_state(BrowserRuntime* r,float* out){
    std::fill(out,out+12,0);if(!r)return;auto& a=r->app;auto& g=a.game;auto& p=g.player_state;
    if(a.in_game()&&(g.globals.game_flags&8)){out[0]=3;return;}
    if(!a.in_game()||a.loading_game()||!g.ready()||g.paused||g.menus.context.pause_state||p.context.game_over||(g.globals.game_flags&0x60))return;
    out[0]=g.dialogue.present()?2:1;out[1]=p.life.state!=1&&p.life.state!=2;out[2]=g.globals.stage+1;
    const auto& m=p.motion.movement;out[3]=m.position.x;out[4]=m.position.y;
    out[5]=g.shots[0].settings().normal_speed*g.player.timing.rate;
    out[6]=g.shots[1].settings().focus_speed*g.player.timing.rate;
    out[7]=p.input.minimum.x;out[8]=p.input.minimum.y;out[9]=out[7]+p.input.extent.x;out[10]=out[8]+p.input.extent.y;
}
// Read-only sizes of the next original movement step. Bombs can force the
// player form and change either axis multiplier independently of Shift.
EX("touch_motion") void browser_touch_motion(BrowserRuntime* r,bool slow,float* out){
    std::fill(out,out+6,0);if(!r||!r->app.in_game())return;
    auto& g=r->app.game;const auto& p=g.player_state;const auto& m=p.motion.movement;
    const bool focused=p.bomb.active?(p.bomb.type&1)!=0:slow;
    const auto& profile=g.player.profile(focused);
    const float straight=focused?profile.focus_speed:profile.normal_speed,diagonal=focused?profile.focus_diagonal:profile.normal_diagonal;
    const auto step=[&](float speed,float multiplier){return (number((number(speed)*number(multiplier)).to_float())*number(g.player.timing.rate)).to_float();};
    out[0]=step(straight,m.multiplier.x);out[1]=step(straight,m.multiplier.y);
    out[2]=step(diagonal,m.multiplier.x);out[3]=step(diagonal,m.multiplier.y);
    // The original priority-17 recording job publishes keys after the player
    // (priority 9). Account for the movement already queued for this frame,
    // without changing that order or writing to any live gameplay state.
    auto predicted=m;
    if(!(g.globals.game_flags&8)&&g.recording.ready()&&p.life.state!=1&&p.life.state!=2){
        const u16 pending=g.recording.input.current;
        const bool pending_focus=p.bomb.active?(p.bomb.type&1)!=0:(pending&4)!=0;
        move_player(predicted,g.player.profile(false),g.player.profile(true),pending_focus,p.input.character,pending,p.input.minimum,p.input.extent,g.player.timing,nullptr,false);
    }
    out[4]=predicted.position.x;out[5]=predicted.position.y;
}
EX("touch_action_state_v1") const u32* browser_touch_action_state_v1(BrowserRuntime* r){
    static u32 out[TouchActionStateV1::word_count]{};
    if(!r){std::fill(out,out+TouchActionStateV1::word_count,0);out[TouchActionStateV1::Version]=TouchActionStateV1::version;return out;}
    const auto* state=r->app.game.touch_actions().data();
    std::copy(state,state+TouchActionStateV1::word_count,out);
    return out;
}
EX("trace") const u32* browser_trace(BrowserRuntime* r){
    static u32 out[68];auto& a=r->app;out[0]=a.game.playback.stream.frame;out[1]=a.game.globals.stage;
    out[2]=a.game.enemies.state.frames;out[3]=a.game.control.state.frames;out[4]=a.game.globals.game_flags;
    out[5]=a.session.random.seed;out[6]=a.session.random.calls;std::memcpy(out+7,&a.game.player_state.motion.movement.position,8);
    out[9]=u8(a.game.player_state.life.state);out[10]=a.game.playback.input.current;std::memcpy(out+11,&a.session.numbers,228);return out;
}
EX("diagnostics") const i32* browser_diagnostics(BrowserRuntime* r){
    static i32 out[16];std::fill(out,out+16,0);if(!r)return out;
    const auto& a=r->app;const auto& g=a.game;
    out[0]=a.supervisor.state.active;out[1]=a.supervisor.state.target;out[2]=a.supervisor.state.previous;
    out[3]=a.loading_game();out[4]=g.control.state.load_state;out[5]=g.control.state.load_frames;
    out[6]=g.menus.context.pause_state;out[7]=g.menus.context.show_retry;out[8]=g.player_state.context.game_over;
    out[9]=g.menus.context.supervisor_state;out[10]=g.control.state.next_scene;out[11]=g.player_state.input.buttons;
    out[12]=a.session.numbers.clock_time;out[13]=number(a.session.numbers.lives).truncate_int();
    out[14]=g.dialogue_context.flags;out[15]=g.control.state.sticky_input;return out;
}
EX("visual_diagnostics") const u32* browser_visual_diagnostics(BrowserRuntime* r){
    static u32 out[4]{};if(!r)return out;
    out[0]=r->app.ascii.state.blindness_color;
    std::memcpy(out+1,&r->app.ascii.state.blindness_radius,4);
    out[2]=r->app.game.globals.stage;out[3]=r->app.game.enemies.state.frames;return out;
}
EX("menu_diagnostics") const i32* browser_menu_diagnostics(BrowserRuntime* r){
    static i32 out[12]{};if(!r)return out;const auto& a=r->app;
    const auto& s=a.results.controls.state;const auto& pause=a.ascii.state.pause;
    out[0]=s.currentState;out[1]=s.cursor;out[2]=s.selectedReplay;out[3]=s.selectedCharacter;out[4]=s.frameTimer;
    out[5]=a.game.recording.ready();out[6]=a.game.menus.context.lockable_backbuffer;
    out[7]=pause.state;out[8]=pause.frames;out[9]=pause.background.visible;out[10]=pause.background.color1.d3dColor;
    out[11]=a.session.numbers.retries;return out;
}
// These requests select production scene boundaries in an explicitly armed
// fixture. They do not set the movement marker, result, score or Replay bytes.
EX("validation_run_arm_v1") i32 browser_validation_run_arm_v1(BrowserRuntime* r){
    if(!r||r->app.in_game()||r->app.loading_game()||r->validation_scene_used||
        r->app.game.enemies.validation_scene().enabled()||r->validation_run_boundaries_armed)return 0;
    r->validation_run_boundaries_armed=true;r->validation_run_transition_count=0;return 1;
}
EX("validation_run_transition_v1") i32 browser_validation_run_transition_v1(BrowserRuntime* r,u32 request,u32 run,u32 stage){
    if(!r||!r->validation_run_boundaries_armed||r->validation_scene_used||
        !r->app.in_game()||r->app.loading_game()||r->validation_run_sequence!=run)return 0;
    auto& g=r->app.game;
    if(u32(g.globals.stage)!=stage||g.paused||g.dialogue.present()||g.enemies.validation_scene().enabled()||
        g.globals.game_flags&(1|2|8|0x4000)||r->app.session.practice.active||r->app.session.practice.assisted)return 0;
    if(request==1&&r->validation_run_transition_count==0){
        g.menus.context.supervisor_state=i32(Scene::NextStage);
    }else if(request==2&&r->validation_run_transition_count==1){
        g.menus.context.supervisor_state=i32(Scene::GameResults);
    }else return 0;
    ++r->validation_run_transition_count;return 1;
}
EX("validation_run_state_v1") const double* browser_validation_run_state_v1(BrowserRuntime* r){
    static double out[17]{};std::fill(out,out+17,0.);out[0]=1;if(!r)return out;
    const auto& a=r->app;const auto& g=a.game;const auto& result=a.results.controls.state;
    out[1]=1;out[2]=r->validation_run_boundaries_armed;out[3]=r->validation_run_sequence;
    out[4]=a.supervisor.state.active;out[5]=a.in_game();out[6]=a.loading_game();out[7]=g.globals.stage;
    out[8]=g.globals.game_flags;out[9]=a.session.practice.active;out[10]=r->motion.cheat_movement_used;
    out[11]=g.player_state.motion.movement.position.x;out[12]=g.player_state.motion.movement.position.y;
    out[13]=a.results.active()&&result.hscr.header.magic==fourcc('H','S','C','R');
    if(out[13])out[14]=result.hscr.lag;
    out[15]=r->validation_run_transition_count;out[16]=g.enemies.validation_scene().enabled();return out;
}
}
