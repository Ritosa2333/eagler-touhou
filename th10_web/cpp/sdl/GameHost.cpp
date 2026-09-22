#include "../platform/Application.hpp"
#include "ThpracUi.hpp"
#include "../game/PracticeConfig.hpp"
#include <SDL3/SDL.h>
#include <emscripten.h>
#include "../../../portable/input/TouchController.hpp"
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <set>
#include <cmath>
#include <vector>
using namespace th10;using namespace th10::browser;
EM_JS(int, th10_keyboard_gamepad_dpad, (), {
    if (!navigator.getGamepads) return 0;
    let bits = 0;
    for (const pad of navigator.getGamepads()) {
        if (!pad || !pad.buttons || pad.buttons.length < 16) continue;
        if (!/keyboard|\bkb\b/i.test(String(pad.id || 0))) continue;
        if (pad.buttons[12]?.pressed) bits |= 1;
        if (pad.buttons[13]?.pressed) bits |= 2;
        if (pad.buttons[14]?.pressed) bits |= 4;
        if (pad.buttons[15]?.pressed) bits |= 8;
    }
    return bits;
});
extern "C" {
FileSystem* files_create();void files_destroy(FileSystem*);u32 files_attach(FileSystem*,const char*);
Input* input_create();void input_destroy(Input*);GameState* game_state_create(Input*,u32);void game_state_destroy(GameState*);
GraphicsDevice* graphics_create(const GraphicsPresentation*,u32);void graphics_destroy(GraphicsDevice*);
AnimationEngine* animation_engine_create(FileSystem*,GraphicsDevice*,Rng*,Rng*,float*);void animation_engine_destroy(AnimationEngine*);
Fonts* fonts_create(GraphicsDevice*,Rng*,u32);void fonts_destroy(Fonts*);Audio* audio_create(FileSystem*);void audio_destroy(Audio*);
ScreenEffects* effects_create(AnimationEngine*,const u32*,const u32*);void effects_destroy(ScreenEffects*);
Application* application_create(FileSystem*,Input*,GameState*,AnimationEngine*,Fonts*,Audio*,ScreenEffects*);void application_destroy(Application*);
void application_touch_state(Application*,u32*);void sdl_loop_start(Application*);void sdl_loop_stop();void sdl_files_root(u32);
void sdl_audio_shutdown();void sdl_fonts_shutdown();void sdl_shutdown();
}
namespace {
struct Session {
    FileSystem* files=nullptr;Input* input=nullptr;GameState* state=nullptr;GraphicsDevice* device=nullptr;
    AnimationEngine* animation=nullptr;Fonts* fonts=nullptr;Audio* audio=nullptr;ScreenEffects* effects=nullptr;Application* app=nullptr;
    Rng random{},visual{};float rate=1;u32 quitting=0;
    ~Session(){sdl_loop_stop();if(app)application_destroy(app);if(effects)effects_destroy(effects);if(audio)audio_destroy(audio);sdl_audio_shutdown();if(fonts)fonts_destroy(fonts);sdl_fonts_shutdown();if(animation)animation_engine_destroy(animation);if(device)graphics_destroy(device);if(state)game_state_destroy(state);if(input)input_destroy(input);if(files)files_destroy(files);sdl_shutdown();}
};
std::unique_ptr<Session> session;
SDL_Gamepad* controllers[2]{};
constexpr SDL_GamepadButton gamepad_slots[]={SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH,SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,SDL_GAMEPAD_BUTTON_BACK,SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_LEFT_STICK,SDL_GAMEPAD_BUTTON_RIGHT_STICK,SDL_GAMEPAD_BUTTON_GUIDE};
constexpr SDL_GamepadAxis gamepad_axes[]={SDL_GAMEPAD_AXIS_LEFTX,SDL_GAMEPAD_AXIS_LEFTY,SDL_GAMEPAD_AXIS_RIGHTX,SDL_GAMEPAD_AXIS_RIGHTY,SDL_GAMEPAD_AXIS_LEFT_TRIGGER,SDL_GAMEPAD_AXIS_RIGHT_TRIGGER};
void close_controllers(){for(auto*& p:controllers)if(p){SDL_CloseGamepad(p);p=nullptr;}}
void add_controller(SDL_JoystickID id){for(const auto* p:controllers)if(p&&SDL_GetGamepadID(const_cast<SDL_Gamepad*>(p))==id)return;for(auto*& p:controllers)if(!p){p=SDL_OpenGamepad(id);break;}}
void poll_controllers(InputSnapshot& input){for(u32 i=0;i<2;i++){auto* p=controllers[i];if(!p||!SDL_GamepadConnected(p))continue;input.connected[i]=1;auto& d=input.direct[i];auto& l=input.legacy[i];l.size=52;l.flags=255;
    for(int n=0;n<6;n++){const int value=SDL_GetGamepadAxis(p,gamepad_axes[n]);const double unit=value<0?value/32768.:value/32767.;d.axes[n]=i32(std::floor(unit*1000+.5));(&l.x)[n]=u32(std::floor((unit+1)*32767.5+.5));}
    for(u32 n=0;n<sizeof(gamepad_slots)/sizeof(*gamepad_slots);n++){const bool down=SDL_GetGamepadButton(p,gamepad_slots[n]);d.buttons[n]=down?128:0;if(down&&n<32)l.buttons|=1u<<n;}
    const int x=int(SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_RIGHT))-int(SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_LEFT)),y=int(SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_DOWN))-int(SDL_GetGamepadButton(p,SDL_GAMEPAD_BUTTON_DPAD_UP));
    if(x){d.axes[0]=x*1000;l.x=x>0?65535:0;}if(y){d.axes[1]=y*1000;l.y=y>0?65535:0;}
    l.pov=(x||y)?u32((int(std::round(std::atan2(double(x),double(-y))*180/3.141592653589793))+360)%360)*100:~0u;d.pov[0]=l.pov;for(int n=1;n<4;n++)d.pov[n]=~0u;if(l.buttons)l.button_number=__builtin_ctz(l.buttons)+1;
}}
struct Key {const char* code;const char* sdl;u32 scan,vk;bool hosted=false;SDL_Scancode native=SDL_SCANCODE_UNKNOWN;};
#include "../../../portable/input/KeyboardMap.inc"
touhou::input::TouchController gestures;
touhou::input::TouchState touch_state(){touhou::input::TouchState s;if(!session||!session->app)return s;u32 raw[8];application_touch_state(session->app,raw);float values[5];std::memcpy(values,raw+3,20);
    s.context=raw[0];s.instance=raw[1];s.ready=raw[2];s.x=values[0];s.y=values[1];s.fast=values[2];s.slow=values[3];s.min_x=-184;s.max_x=184;s.min_y=32;s.max_y=432;return s;}
World* touch_world(){return session&&session->app?session->app->world:nullptr;}
void cancel(){if(auto* world=touch_world()){world->motion.touch_cancel(world->state.game.stage);world->motion.target(0,0,0);}gestures.cancel_transient();}
void key(InputSnapshot& s,u32 scan,u32 vk){s.scan_keys[scan]=128;s.virtual_keys[vk]=128;if(vk>=160&&vk<=165)s.virtual_keys[16+(vk-160)/2]=128;}
void sync_touch_context(const touhou::input::TouchState& state){const int previous=gestures.current_context();if(previous!=state.context&&(previous==1||previous==2))if(auto* world=touch_world())world->motion.touch_cancel(world->state.game.stage);}
void touch(int type,int id,float x,float y){const auto state=touch_state();sync_touch_context(state);if((state.context==1||state.context==2))if(auto* world=touch_world())world->motion.touch_event(world->state.game.stage,type,id,x,y);gestures.pointer(type,id,x,y,SDL_GetTicks(),state,session&&session->input&&session->input->snapshot.virtual_keys[16]);}

}
extern "C" {
__attribute__((export_name("sdl_native_input"))) void sdl_native_input(Application* app){
    if(!session||session->app!=app)return;
    SDL_Event event;while(SDL_PollEvent(&event)){
#ifdef TH_ENABLE_THPRAC
        ThpracUi::process_event(event);
#endif
        if(event.type==SDL_EVENT_FINGER_CANCELED){cancel();continue;}
        if(event.type==SDL_EVENT_GAMEPAD_ADDED)add_controller(event.gdevice.which);
        if(event.type==SDL_EVENT_GAMEPAD_REMOVED)for(auto*& p:controllers)if(p&&SDL_GetGamepadID(p)==event.gdevice.which){SDL_CloseGamepad(p);p=nullptr;}
        if(event.type==SDL_EVENT_FINGER_DOWN||event.type==SDL_EVENT_FINGER_MOTION||event.type==SDL_EVENT_FINGER_UP)touch(event.type==SDL_EVENT_FINGER_DOWN?0:event.type==SDL_EVENT_FINGER_MOTION?1:2,int(event.tfinger.fingerID),event.tfinger.x,event.tfinger.y);
    }
    auto& snapshot=app->input.snapshot;std::memset(&snapshot,0,sizeof(snapshot));snapshot.focused=1;
    const bool* physical=SDL_GetKeyboardState(nullptr);
    for(const auto& k:keyboard_map)if(k.hosted||(k.native!=SDL_SCANCODE_UNKNOWN&&physical[k.native]))key(snapshot,k.scan,k.vk);
    const int keyboardDpad=th10_keyboard_gamepad_dpad();if(keyboardDpad&1)key(snapshot,0xc8,38);if(keyboardDpad&2)key(snapshot,0xd0,40);if(keyboardDpad&4)key(snapshot,0xcb,37);if(keyboardDpad&8)key(snapshot,0xcd,39);
    poll_controllers(snapshot);
    const auto state=touch_state();sync_touch_context(state);const auto sample=gestures.sample(state,SDL_GetTicks(),snapshot.virtual_keys[16],snapshot.virtual_keys[37]||snapshot.virtual_keys[38]||snapshot.virtual_keys[39]||snapshot.virtual_keys[40]);
    for(const auto& k:keyboard_map)if(sample.keys[k.vk]||(k.vk>=160&&k.vk<=165&&sample.keys[16+(k.vk-160)/2]))key(snapshot,k.scan,k.vk);
    if(app->world)app->world->motion.target(sample.motion,sample.x,sample.y);
#ifdef TH_ENABLE_THPRAC
    ThpracUi::update_input(*app);
    if(ThpracUi::captures_game_input())for(const int vk:{16,27,37,38,39,40,88,90})snapshot.virtual_keys[vk]=0;
#endif
}
#define EXPORT(name) __attribute__((export_name(name)))
EXPORT("sdl_game_open") Application* sdl_game_open(u32 chinese,u32 seed){
    gestures.begin_session();
    for(auto& k:keyboard_map)k.native=SDL_GetScancodeFromName(k.sdl);
    close_controllers();SDL_InitSubSystem(SDL_INIT_GAMEPAD);int controller_count=0;auto* ids=SDL_GetGamepads(&controller_count);for(int i=0;i<controller_count;i++)add_controller(ids[i]);SDL_free(ids);
    session=std::make_unique<Session>();auto& s=*session;sdl_files_root(chinese);u32 rng[]{seed,0};std::memcpy(&s.random,rng,8);std::memcpy(&s.visual,rng,8);
    s.files=files_create();if(!s.files||!files_attach(s.files,chinese?"th10c.dat":"th10.dat"))return nullptr;
    const u32 parameters[]{640,480,22,1,0,0,1,0,1,1,80,0,0,0};s.input=input_create();s.state=game_state_create(s.input,chinese);s.device=graphics_create(reinterpret_cast<const GraphicsPresentation*>(parameters),0x40);if(!s.device)return nullptr;
    s.animation=animation_engine_create(s.files,s.device,&s.random,&s.visual,&s.rate);s.fonts=fonts_create(s.device,&s.random,chinese);s.audio=audio_create(s.files);s.effects=effects_create(s.animation,&s.quitting,nullptr);
    s.app=application_create(s.files,s.input,s.state,s.animation,s.fonts,s.audio,s.effects);
#ifdef TH_ENABLE_THPRAC
    if(!s.app)return nullptr;
    if(!ThpracUi::initialize()){session.reset();return nullptr;}
#endif
    return s.app;
}
EXPORT("sdl_game_close") void sdl_game_close(){cancel();
#ifdef TH_ENABLE_THPRAC
    ThpracUi::shutdown();
#endif
    close_controllers();session.reset();}
EXPORT("sdl_key") void sdl_key(const char* code,u32 down){for(auto& k:keyboard_map)if(!std::strcmp(code,k.code)){k.hosted=down!=0;return;}}
EXPORT("sdl_keys_clear") void sdl_keys_clear(){for(auto& k:keyboard_map)k.hosted=false;cancel();gestures.reset();}
EXPORT("sdl_touch") void sdl_touch(u32 type,i32 id,float x,float y){
#ifdef TH_ENABLE_THPRAC
    if(ThpracUi::captures_game_input())ThpracUi::mouse(type==0?1:type==1?0:2,x*640.f,y*480.f);
#endif
    touch(type,id,x,y);
}
EXPORT("sdl_touch_cancel") void sdl_touch_cancel(){cancel();}
#ifdef TH_ENABLE_THPRAC
EXPORT("sdl_thprac_mouse") void sdl_thprac_mouse(u32 type,float x,float y){ThpracUi::mouse(int(type),x,y);}
EXPORT("practice_enable") void practice_enable(Application* app,bool enabled){
    if(!app||(app->world&&app->world->actors.session))return;auto& p=app->state.practice;p.enabled=enabled;
    if(!enabled)p.menu=p.accepted=p.active=false;
}
EXPORT("practice_status") const i32* practice_status(Application* app){static i32 out[7]{};if(!app)return out;const auto& p=app->state.practice;
    out[0]=p.menu;out[1]=app->state.game.difficulty;out[2]=app->state.game.character;out[3]=p.active;out[4]=p.replay;out[5]=p.cheats;out[6]=p.assisted;return out;
}
EXPORT("practice_configure") bool practice_configure(Application* app,const double* words,u32 count,bool accept){
    if(!app||!app->state.practice.enabled||(app->world&&app->world->actors.session))return false;auto& p=app->state.practice;PracticeConfig config;
    if(!config.decode(words,count)||(accept&&!p.menu))return false;p.configured=config;
    if(accept){p.run=config;p.accepted=true;}return true;
}
EXPORT("practice_cancel") void practice_cancel(Application* app){if(!app||!app->state.practice.menu)return;auto& p=app->state.practice;p.menu=p.accepted=false;
    if(app->title&&app->title->value){app->title->value->menu.select(app->state.game.stage);app->title->value->set_screen(8,&app->engine.speed);}
}
EXPORT("practice_cheats") bool practice_cheats(Application* app,u32 mask){
    if(!app||!(app->world&&app->world->actors.session)||!app->state.practice.enabled||app->state.practice.replay||mask>63)return false;auto& p=app->state.practice;p.cheats=mask;if(mask)p.assisted=true;return true;
}
#endif
EXPORT("sdl_touch_options") void sdl_touch_options(u32 on,u32 free,float speed){gestures.enabled=on;gestures.unlimited=free;gestures.sensitivity=std::clamp(speed,1.f,3.f);if(!on)cancel();}
EXPORT("sdl_touch_gestures") void sdl_touch_gestures(u32 two,u32 taps){gestures.two_finger=two;gestures.double_tap=taps;}
EXPORT("sdl_touch_mode") void sdl_touch_mode(u32 mode){if(gestures.set_mode(static_cast<int>(mode))&&session&&session->app&&session->app->world)session->app->world->motion.target(0,0,0);}
EXPORT("sdl_touch_controls") void sdl_touch_controls(u32 shoot,u32 slow,u32 bomb,u32 escape,float x,float y){gestures.controls(shoot,slow,bomb,escape,x,y);}
EXPORT("sdl_resource_stats") const u32* sdl_resource_stats(){static u32 out[4]{};if(session&&session->files){auto& f=*session->files;out[0]=f.cache_bytes;out[1]=f.cache_hits;out[2]=f.cache_misses;out[3]=f.decoded.size();}return out;}
EXPORT("sdl_game_status") const i32* sdl_game_status(){static i32 result[10]{};if(session&&session->app){auto& a=*session->app;result[0]=a.value.screen;result[1]=a.state.game.stage;result[2]=a.error;result[3]=a.state.game.lives;result[4]=a.state.game.power;result[5]=gestures.current_context();result[6]=gestures.active();result[7]=gestures.fire;result[8]=gestures.focus;result[9]=a.input.player_profiles[0].input.raw;}return result;}
}
