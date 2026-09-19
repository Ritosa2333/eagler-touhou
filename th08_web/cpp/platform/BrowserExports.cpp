#include "BrowserRuntime.hpp"
#include <cstdlib>
#include <algorithm>
#ifdef TH_SDL3
#include "../sdl/GraphicsHost.hpp"
#endif
using namespace th08;
#ifdef __EMSCRIPTEN__
#define EX(name) __attribute__((export_name(name)))
#else
#define EX(name)
#endif
extern "C" {
EX("practice_enable") void browser_practice_enable(BrowserRuntime* r,bool enabled){if(r&&!r->app.in_game()){auto& p=r->app.session.practice;p.enabled=enabled;if(!enabled)p.menu=p.accepted=p.active=false;}}
EX("practice_status") const i32* browser_practice_status(BrowserRuntime* r){static i32 out[7]{};if(!r)return out;const auto& p=r->app.session.practice;
    out[0]=p.menu;out[1]=r->app.title.context.difficulty;out[2]=r->app.title.context.character;out[3]=p.active;out[4]=p.replay;out[5]=p.cheats;out[6]=p.assisted;return out;}
EX("practice_tracker_status") const i32* browser_practice_tracker_status(BrowserRuntime* r){static i32 out[7]{};std::fill(out,out+7,0);if(!r)return out;
    const auto& app=r->app;if(!app.in_game())return out;const auto& n=app.session.numbers;
    out[0]=1;out[1]=app.game.globals.shot;out[2]=Scalar::truncate(n.deaths);out[3]=app.session.practice.tracker_dissolve_count;
    out[4]=Scalar::truncate(n.bombs_used);out[5]=n.captured_spells;out[6]=app.session.practice.tracker_last_spell_captures;return out;}
EX("practice_configure") bool browser_practice_configure(BrowserRuntime* r,const double* words,u32 count,bool accept){
    if(!r||!r->app.session.practice.enabled||r->app.in_game())return false;auto& p=r->app.session.practice;PracticeConfig config;
    if(!config.decode(words,count)||(accept&&!p.menu))return false;p.configured=config;
    if(accept){p.run=config;p.accepted=true;}return true;
}
EX("practice_cancel") void browser_practice_cancel(BrowserRuntime* r){if(!r||!r->app.session.practice.menu)return;auto& p=r->app.session.practice;p.menu=p.accepted=false;
    r->app.title.menus.state.cursor=r->app.title.context.character;r->app.title.menus.ChangeCurrentScreen(TitleCurrentScreen_CharacterSelectPractice);}
EX("practice_cheats") bool browser_practice_cheats(BrowserRuntime* r,u32 mask){if(!r||!r->app.in_game()||!r->app.session.practice.enabled||r->app.session.practice.replay||mask>63)return false;auto& p=r->app.session.practice;p.cheats=mask;if(mask)p.assisted=true;return true;}
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
EX("save") bool browser_save(BrowserRuntime* r){return r&&r->app.save_score();}
// Finalize the current recording using the same owner as the result screen.
EX("save_replay") bool browser_save_replay(BrowserRuntime* r,i32 slot,const char* name){return r&&r->app.finalize_replay(slot,name);}
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
}
