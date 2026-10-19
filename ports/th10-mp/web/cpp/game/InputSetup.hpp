#pragma once
#include "ApplicationState.hpp"
namespace th10 {
enum class InputSetupMessage : u32 {Driver,KeyboardFormat,KeyboardCooperative,KeyboardReady,ControllerReady};
struct InputSetupEnvironment {
    ApplicationState* global;
    const u8* input_interface;const u8* keyboard_guid;
    const void* keyboard_format;const void* controller_format;
    CallbackToken controller_callback,axis_callback;
    virtual u32 window_instance(u32 window)=0;
    virtual i32 create_driver(u32 instance,const u8* interface,void** output)=0;
    virtual i32 create_device(void* driver,const u8* guid,void** output)=0;
    virtual i32 set_format(void* device,const void* format)=0;
    virtual i32 cooperative(void* device,u32 window,u32 flags)=0;
    virtual void acquire(void* device)=0;
    virtual void release(void* device)=0;
    virtual void enumerate_controllers(void* driver,CallbackToken callback)=0;
    virtual void capabilities(void* device,u32* output)=0;
    virtual void enumerate_axes(void* device,CallbackToken callback)=0;
    virtual i32 set_axis_range(void* device,const u32* range)=0;
    virtual void report(InputSetupMessage message)=0;
};
struct InputSetup {
    ApplicationState& application;InputSetupEnvironment& environment;
    i32 initialize();
    void initialize_worker();
    void release_driver();
    bool select_controller(const u8* instance);
    bool configure_axis(const u8* object);
};
}
