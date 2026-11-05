#pragma once
#include "ApplicationState.hpp"
#include "ApplicationConfig.hpp"
#include "AudioManager.hpp"
#include "GameLog.hpp"
#include "GraphicsStartup.hpp"
namespace th10 {
struct StartupAllocationPool {void* allocations[10240];u32 used;};
struct WindowMessage {u32 window,message,parameter,detail,time; i32 x,y;};
enum class DriverNotice {Starting,NoGraphicsDriver,Restarting,LogSeparator};
enum class DriverAction {Continue,InitializeGraphics,Frame,Done};
struct ApplicationDriverEnvironment {
    u32* instance;u32* application_instance;ApplicationState* application;ApplicationConfig* configuration;
    StartupAllocationPool** allocation_pool;UpdateChain** chain;AnmManager** animations;AudioManager* audio;void** graphics_driver;void** device;void** midi;
    u32* window;u32* quit;u8* skipped_frames;i32* reset_frames;double* initial_times[4];GameLog* log;PresentationParameters* parameters;
    virtual void* allocate(u32 size)=0;
    virtual void release_object(void* object)=0;
    virtual void release_bytes(void* bytes)=0;
    virtual void initialize_lock(u32 index)=0;
    virtual void delete_lock(u32 index)=0;
    virtual void notice(DriverNotice message)=0;
    virtual i32 check_instance()=0;
    virtual void save_system_settings()=0;
    virtual void restore_system_settings()=0;
    virtual i32 load_configuration()=0;
    virtual void save_configuration()=0;
    virtual i32 choose_display(u32 instance)=0;
    virtual void checksum()=0;
    virtual void* create_graphics_driver()=0;
    virtual i32 create_window(u32 instance)=0;
    virtual void start_audio(u32 window)=0;
    virtual void initialize_input()=0;
    virtual void initialize_animations(AnmManager& animations)=0;
    virtual void release_animations(AnmManager& animations)=0;
    virtual void enable_input_method(bool enabled)=0;
    virtual void show_cursor(bool visible)=0;
    virtual void clear_cursor()=0;
    virtual Extended time()=0;
    virtual void foreground(u32 window)=0;
    virtual i32 install_callbacks()=0;
    virtual bool peek_message(WindowMessage& message)=0;
    virtual void translate_message(WindowMessage& message)=0;
    virtual void dispatch_message(WindowMessage& message)=0;
    virtual i32 cooperative_level(void* device)=0;
    virtual void release_capture_textures(AnmManager& animations)=0;
    virtual i32 reset_device(void* device,PresentationParameters& parameters)=0;
    virtual void configure_graphics_defaults()=0;
    virtual void shutdown_application()=0;
    virtual void clear_chain(UpdateChain& chain)=0;
    virtual i32 update_audio()=0;
    virtual void join_audio()=0;
    virtual void release_audio()=0;
    virtual void release_device(void* device)=0;
    virtual void hide_window(u32 window)=0;
    virtual void shrink_window(u32 window)=0;
    virtual void destroy_window(u32 window)=0;
    virtual void stop_midi(void* midi)=0;
    virtual void release_midi(void* midi)=0;
    virtual void show_log(const char* log)=0;
    virtual void save_log(const char* log,u32 length)=0;
};
struct ApplicationDriver {
    enum class Phase {Boot,CreateSession,AfterGraphics,Running,AfterFrame,Shutdown,DrainAudio,Cleanup,Finish,Done};
    Phase phase=Phase::Boot;i32 result=0;u32 instance=0;WindowMessage message{};
    // Graphics startup and the frame callback may yield. Their result is
    // supplied on the next advance, without keeping a native call stack alive.
    DriverAction advance(ApplicationDriverEnvironment& environment,i32 callback_result=0);
};
static_assert(sizeof(StartupAllocationPool)==0xa004&&sizeof(WindowMessage)==28);
}
