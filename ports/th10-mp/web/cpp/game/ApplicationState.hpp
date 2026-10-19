#pragma once
#include "BackgroundThread.hpp"
#include "AnmRegistry.hpp"
#include "GameEconomy.hpp"
#include "GameProgression.hpp"
#include "Camera.hpp"
namespace th10 {
struct ApplicationEnvironment;
struct GameSession;
struct TitleMenu;
struct Ending;
struct StartupScreen;
struct Replay;
struct ApplicationState {
    u8 reserved_000[8];
    void* device;
    void* input_driver;void* keyboard;void* controller;
    u32 reserved_018,controller_capabilities[11],window;
    u8 reserved_04c[0x150-0x4c];
    u32 display_flags;
    Camera world_camera,ui_camera;
    Camera* active_camera;
    u32 screen_space;
    i32 screen,pending_screen,previous_screen;
    u32 reserved_398;
    i32 new_game;
    u8 reserved_3a0[0x28];
    AnmFile* loading_animations;
    u32 engine_flags;
    u8 reserved_3d0[0x62c-0x3d0];
    BackgroundThread resource_loader;
    i32 frame_gate;
    u8 critical_sections[7][24];
    u8 lock_depth[7],reserved_6fb;
    i32 loading_state;
    u8 reserved_700[0x68];
    StartupScreen* startup;
    u8 reserved_76c[0x14];
    u32 background_color;
    void lock(u32 index,ApplicationEnvironment& environment);
    void unlock(u32 index,ApplicationEnvironment& environment);
    i32 begin_loading(CallbackToken callback,void* argument,ApplicationEnvironment& environment);
    void stop_loading(ApplicationEnvironment& environment);
    void show_loading(const Vec3& position,ApplicationEnvironment& environment);
    void finish_loading(bool success,ApplicationEnvironment& environment);
    i32 transition(ApplicationEnvironment& environment);
    static void shutdown_screens(ApplicationEnvironment& environment);
};
static_assert(offsetof(ApplicationState,loading_animations)==0x3c8&&offsetof(ApplicationState,resource_loader)==0x62c);
static_assert(offsetof(ApplicationState,world_camera)==0x154&&offsetof(ApplicationState,ui_camera)==0x26c&&offsetof(ApplicationState,active_camera)==0x384);
static_assert(offsetof(ApplicationState,critical_sections)==0x64c&&offsetof(ApplicationState,loading_state)==0x6fc&&sizeof(ApplicationState)==0x784);
struct ApplicationEnvironment : BackgroundThreadEnvironment {
    GameEconomy* game;
    GameSession** current_game;
    TitleMenu** current_title;
    Ending** current_ending;
    StartupScreen** current_startup;
    Replay** current_replay;
    const StageConfiguration* stages;
    const StageConfiguration** current_stage;
    AnmRegistry* registry;
    u32* loading_ids;
    i32 *return_menu,*loading_pause;
    virtual void enter_lock(ApplicationState& application,u32 index)=0;
    virtual void leave_lock(ApplicationState& application,u32 index)=0;
    virtual StartupScreen* create_startup_screen(ApplicationState& application)=0;
    virtual void destroy_screens(ApplicationState& application)=0;
    virtual void create_title()=0;
    virtual void destroy_title(TitleMenu* title)=0;
    virtual void create_game(i32 replay_mode)=0;
    virtual void destroy_game(GameSession* session)=0;
    virtual void create_ending()=0;
    virtual void destroy_ending(Ending* ending)=0;
    virtual void destroy_startup(StartupScreen* startup)=0;
    virtual void destroy_replay(Replay* replay)=0;
    virtual u32 create_loading_animation(AnmFile& file,i32 script)=0;
};
}
