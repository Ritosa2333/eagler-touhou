// Shared SDL3 application lifecycle. Platform code supplies files and inputs;
// Game owns menus, stages, playback and rendering on desktop and Web alike.
#include "Game.hpp"
#include "FrameClock.hpp"
#include "RuntimeOptions.hpp"
#include <SDL3/SDL.h>
#include <array>
#include <cmath>
#include <memory>
#include <vector>
#ifdef TH12_MULTIPLAYER
#include "multiplayer/Session.hpp"
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace th12host {
namespace {
u32 input_bit(int key){
    switch(key){case 90:return InShot;case 88:return InBomb;case 16:return InFocus;case 17:return InSkip;
    case 37:return InLeft;case 38:return InUp;case 39:return InRight;case 40:return InDown;
    case 27:return InPause;case 13:return InEnter;case 82:return InRetry;default:return 0;}
}
int virtual_key(SDL_Keycode key){
    if(key>=SDLK_A&&key<=SDLK_Z)return int(key-SDLK_A)+'A';
    if(key>=SDLK_F1&&key<=SDLK_F12)return int(key-SDLK_F1)+112;
    switch(key){case SDLK_LEFT:return 37;case SDLK_UP:return 38;case SDLK_RIGHT:return 39;case SDLK_DOWN:return 40;
    case SDLK_RETURN:return 13;case SDLK_ESCAPE:return 27;case SDLK_TAB:return 9;case SDLK_BACKSPACE:return 8;
    case SDLK_LSHIFT:case SDLK_RSHIFT:return 16;case SDLK_LCTRL:case SDLK_RCTRL:return 17;default:return 0;}
}
struct Application {
    SDL_Window* window=nullptr;SDL_GLContext context=nullptr;
    std::unique_ptr<Game> game;std::vector<SDL_Gamepad*> pads;FrameClock clock;
    u32 virtual_held=0,controls=0,pulses=0;int motion=0,x=0,y=0;
    bool focused=true,stop=false,vsync=false;
    ~Application(){
        game.reset();for(auto* pad:pads)SDL_CloseGamepad(pad);
        if(context)SDL_GL_DestroyContext(context);if(window)SDL_DestroyWindow(window);SDL_Quit();
    }
    void add(SDL_JoystickID id){
        for(auto* pad:pads)if(SDL_GetGamepadID(pad)==id)return;
        if(SDL_IsGamepad(id))if(auto* pad=SDL_OpenGamepad(id))pads.push_back(pad);
    }
    void remove(SDL_JoystickID id){for(auto it=pads.begin();it!=pads.end();){if(SDL_GetGamepadID(*it)==id){SDL_CloseGamepad(*it);it=pads.erase(it);}else ++it;}}
    PadSample sample(){
        PadSample out;if(!focused)return out;
        auto& held=out.keyboard;held=virtual_held|controls;out.pressed=pulses;pulses=0;
#ifndef __EMSCRIPTEN__
        const bool* k=SDL_GetKeyboardState(nullptr);
        if(k[SDL_SCANCODE_LEFT])held|=InLeft;if(k[SDL_SCANCODE_RIGHT])held|=InRight;
        if(k[SDL_SCANCODE_UP])held|=InUp;if(k[SDL_SCANCODE_DOWN])held|=InDown;
        if(k[SDL_SCANCODE_Z])held|=InShot;if(k[SDL_SCANCODE_X])held|=InBomb;
        if(k[SDL_SCANCODE_LSHIFT]||k[SDL_SCANCODE_RSHIFT])held|=InFocus;
        if(k[SDL_SCANCODE_LCTRL]||k[SDL_SCANCODE_RCTRL])held|=InSkip;
        if(k[SDL_SCANCODE_ESCAPE])held|=InPause;if(k[SDL_SCANCODE_R])held|=InRetry;
        if(k[SDL_SCANCODE_RETURN]&&!k[SDL_SCANCODE_LALT]&&!k[SDL_SCANCODE_RALT])held|=InEnter;
        for(auto* pad:pads)if(SDL_GamepadConnected(pad)){
            const float px=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTX)/32768.f,py=SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFTY)/32768.f;
            if(px<-.3f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_LEFT))out.directions|=InLeft;
            if(px>.3f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_RIGHT))out.directions|=InRight;
            if(py<-.3f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_UP))out.directions|=InUp;
            if(py>.3f||SDL_GetGamepadButton(pad,SDL_GAMEPAD_BUTTON_DPAD_DOWN))out.directions|=InDown;
            const SDL_GamepadButton map[]{SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,SDL_GAMEPAD_BUTTON_LEFT_SHOULDER,
                SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER,SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH};
            for(unsigned b=0;b<7;++b)if(SDL_GetGamepadButton(pad,map[b]))out.buttons|=1u<<b;
            if(SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_LEFT_TRIGGER)>9830)out.buttons|=1u<<2;
            if(SDL_GetGamepadAxis(pad,SDL_GAMEPAD_AXIS_RIGHT_TRIGGER)>9830)out.buttons|=1u<<3;
        }
#endif
        return out;
    }
    void pointer(int type,int id,float px,float py){
        int w=640,h=480;SDL_GetWindowSize(window,&w,&h);const float scale=std::min(w/640.f,h/480.f);
        if(scale>0)game->practice_pointer(type,id,(px-(w-640*scale)/2)/scale,(py-(h-480*scale)/2)/scale);
    }
};
Application* active=nullptr;
std::string failure_reason;
const char* stop_source="initialization";
SDL_AppResult failure(const std::string& why){
    failure_reason=why;stop_source="native-failure";
    host::log("%s",why.c_str());
#ifdef __EMSCRIPTEN__
    EM_ASM({Module.th12Error?.(UTF8ToString($0));},why.c_str());
#else
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Touhou 12",why.c_str(),active?active->window:nullptr);
#endif
    return SDL_APP_FAILURE;
}
}
}
using namespace th12host;

extern "C" SDL_AppResult SDL_AppInit(void** state,int argc,char** argv){
    failure_reason.clear();stop_source="initialization";
    auto app=std::make_unique<Application>();active=app.get();*state=app.release();
#ifdef __EMSCRIPTEN__
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_CANVAS_SELECTOR,"#screen");
#endif
    for(int i=1;i<argc;++i){const std::string arg=argv[i];
        if(arg=="--high-refresh")runtime_options.limit_presentation=false;
        if(arg=="--always-hitbox")runtime_options.always_hitbox=true;
        if(arg=="--no-thprac")runtime_options.thprac=false;
        if(arg=="--no-music")runtime_options.music=false;
        if(arg=="--full-music")runtime_options.full_music=true;
    }
    if(!SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_GAMEPAD))return failure(SDL_GetError());
    host::locate_data(argc,argv);
    host::log("TH12 SDL3: data %s; saves %s",host::paths().data.c_str(),host::paths().save.c_str());
    for(int backend=0;backend<2&&!active->context;++backend){
#ifdef __EMSCRIPTEN__
        if(backend)break;
#endif
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,backend?SDL_GL_CONTEXT_PROFILE_CORE:SDL_GL_CONTEXT_PROFILE_ES);
#ifdef __APPLE__
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,backend?4:3);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,backend?1:0);
#else
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,backend?3:0);
#endif
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
        active->window=SDL_CreateWindow("Touhou 12 - Undefined Fantastic Object",960,720,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_HIGH_PIXEL_DENSITY);
        if(active->window)active->context=SDL_GL_CreateContext(active->window);
        if(!active->context&&active->window){SDL_DestroyWindow(active->window);active->window=nullptr;}
    }
    if(!active->context)return failure(std::string("GLES 3 / desktop OpenGL: ")+SDL_GetError());
    active->vsync=SDL_GL_SetSwapInterval(1);
    int count=0;if(auto* ids=SDL_GetGamepads(&count)){for(int i=0;i<count;++i)active->add(ids[i]);SDL_free(ids);}
    active->game=std::make_unique<Game>();if(!active->game->boot())return failure(active->game->error);
#ifdef __EMSCRIPTEN__
    EM_ASM({Module.th12Started?.();});
#endif
    stop_source="running";return SDL_APP_CONTINUE;
}
extern "C" SDL_AppResult SDL_AppIterate(void* state){
    auto& app=*static_cast<Application*>(state);if(app.stop)return SDL_APP_SUCCESS;
    const double now=host::seconds(),period=app.game->logical_interval();
    double (*pace)(double)=nullptr;
#ifdef TH12_MULTIPLAYER
    if(th12::multiplayer::active)pace=[](double elapsed){return th12::multiplayer::session.network.PacedElapsed(elapsed*1000)/1000;};
#endif
    const unsigned ticks=app.clock.advance(now,period,pace);
    if(!ticks&&!app.clock.present(now,runtime_options.limit_presentation)){
#ifndef __EMSCRIPTEN__
        SDL_Delay(1);
#endif
        return SDL_APP_CONTINUE;
    }
    if(ticks)app.clock.present(now,false);
#ifdef __EMSCRIPTEN__
    if(ticks)EM_ASM({Module.th12BeforeFrame?.();});
#endif
    app.game->set_motion(app.motion,app.x,app.y);
    if(!app.game->frame(app.sample(),ticks,now*1000,app.clock.alpha(period))){
        if(!app.game->error.empty())return failure(app.game->error);
        stop_source="menu-quit";return SDL_APP_SUCCESS;
    }
    int w=0,h=0;SDL_GetWindowSizeInPixels(app.window,&w,&h);
    if(w>0&&h>0){const double start=host::seconds();app.game->present(w,h);SDL_GL_SwapWindow(app.window);app.game->presented((host::seconds()-start)*1000);}
#ifdef __EMSCRIPTEN__
    EM_ASM({Module.th12Frame?.();});
#endif
#ifndef __EMSCRIPTEN__
    if(!app.vsync)SDL_Delay(1);
#endif
    return SDL_APP_CONTINUE;
}
extern "C" SDL_AppResult SDL_AppEvent(void* state,SDL_Event* event){
    auto& app=*static_cast<Application*>(state);
    switch(event->type){
    case SDL_EVENT_QUIT:stop_source="sdl-quit";return SDL_APP_SUCCESS;
    case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
    case SDL_EVENT_WINDOW_ENTER_FULLSCREEN:
    case SDL_EVENT_WINDOW_LEAVE_FULLSCREEN:app.clock.reset();break;
    case SDL_EVENT_GAMEPAD_ADDED:app.add(event->gdevice.which);break;
    case SDL_EVENT_GAMEPAD_REMOVED:app.remove(event->gdevice.which);break;
    case SDL_EVENT_WINDOW_FOCUS_LOST:
#ifndef __EMSCRIPTEN__
        app.focused=false;app.virtual_held=app.controls=0;app.motion=0;app.clock.reset();app.game->focus_lost();app.game->suspend_audio(true);break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:app.focused=true;app.clock.reset();app.game->suspend_audio(false);break;
#else
        app.clock.reset();break;
    case SDL_EVENT_WINDOW_FOCUS_GAINED:app.clock.reset();break;
#endif
#ifndef __EMSCRIPTEN__
    case SDL_EVENT_KEY_DOWN:case SDL_EVENT_KEY_UP:{
        const bool down=event->type==SDL_EVENT_KEY_DOWN;const int key=virtual_key(event->key.key);
        if(key)app.game->practice_key(key,down);
        if(down&&!event->key.repeat&&event->key.key==SDLK_RETURN&&(event->key.mod&SDL_KMOD_ALT))SDL_SetWindowFullscreen(app.window,!(SDL_GetWindowFlags(app.window)&SDL_WINDOW_FULLSCREEN));
        break;}
    case SDL_EVENT_MOUSE_MOTION:app.pointer(1,0,event->motion.x,event->motion.y);break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:case SDL_EVENT_MOUSE_BUTTON_UP:if(event->button.button==SDL_BUTTON_LEFT)app.pointer(event->type==SDL_EVENT_MOUSE_BUTTON_DOWN?0:2,0,event->button.x,event->button.y);break;
    case SDL_EVENT_MOUSE_WHEEL:app.game->practice_wheel(event->wheel.mouse_x,event->wheel.mouse_y,event->wheel.x,event->wheel.y);break;
#endif
    default:break;
    }
    return SDL_APP_CONTINUE;
}
extern "C" void SDL_AppQuit(void* state,SDL_AppResult result){
#ifdef __EMSCRIPTEN__
    const auto* app=static_cast<Application*>(state);
    const std::string final_state=app&&app->game?app->game->host_state():"{}";
#endif
    delete static_cast<Application*>(state);active=nullptr;
#ifdef __EMSCRIPTEN__
    EM_ASM({Module.th12Stopped?.($0,UTF8ToString($1),{source:UTF8ToString($2),state:JSON.parse(UTF8ToString($3))});},result==SDL_APP_SUCCESS?0:1,failure_reason.c_str(),stop_source,final_state.c_str());
#endif
}
extern "C" {
#ifdef TH12_MULTIPLAYER
int th12_host_mp_stage(int stage,int character,int shot,int difficulty,int story_character,int story_shot){return active&&active->game&&active->game->multiplayer_stage(stage,character,shot,difficulty,story_character,story_shot);}
int th12_host_mp_ready(){return active&&active->game&&active->game->multiplayer_ready();}
void th12_host_mp_start(int replay,int local,int visible){if(active){active->game->multiplayer_start(replay!=0,local,visible!=0);active->clock.reset();}}
void th12_host_mp_finish(int save){if(active)active->game->multiplayer_finish(save!=0);}
void th12_host_mp_room(){if(active)active->game->multiplayer_room();}
void th12_host_mp_sound(int id,int pan,int stop){if(active)active->game->multiplayer_sound(id,pan,stop!=0);}
#endif
void th12_host_key(int key,int down){if(!active)return;const u32 bit=input_bit(key);if(down)active->virtual_held|=bit;else active->virtual_held&=~bit;active->game->practice_key(key,down!=0);}
void th12_host_controls(unsigned held,int mode,int x,int y){if(!active)return;active->controls=held&0x2803ff;active->motion=mode>=0&&mode<=4?mode:0;active->x=x;active->y=y;}
void th12_host_pressed(unsigned pressed){if(active)active->pulses|=pressed&0x2803ff;}
int th12_host_pointer(int type,int id,float x,float y,int desktop){return active&&active->game?active->game->practice_pointer(type,id,x,y,desktop!=0):0;}
void th12_host_wheel(float x,float y,float horizontal,float vertical){if(active&&active->game)active->game->practice_wheel(x,y,horizontal,vertical);}
const char* th12_host_state(){return active&&active->game?active->game->host_state().c_str():"{\"scene\":\"closed\"}";}
void th12_host_stop(){if(active){active->game->flush_profile();active->stop=true;stop_source="host-stop";}}
void th12_host_audio(int paused){if(active){if(paused)active->game->focus_lost();active->game->suspend_audio(paused!=0);active->clock.reset();}}
void th12_host_flush(){if(active)active->game->flush_profile();}
void th12_host_refresh_music(){if(active)active->game->refresh_music();}
void th12_host_clear(int pointers){if(active){if(pointers){active->controls=active->pulses=0;active->motion=0;}else active->virtual_held=0;active->game->clear_inputs(pointers!=0);}}
void th12_host_mobile(int enabled,int visible){if(active)active->game->mobile_panels(enabled!=0,visible!=0);}
void th12_host_options(int limited,int hitbox,int thprac,int language,int music){runtime_options={limited!=0,hitbox!=0,thprac!=0,std::clamp(language,0,2),music!=0};}
void th12_host_music_mode(int full){runtime_options.full_music=full!=0;}
int th12_host_playback(int action,const char* path,int value){return active&&active->game&&active->game->playback(action,path,value);}
}
