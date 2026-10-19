#pragma once
#include "ApplicationState.hpp"
namespace th10 {
struct ApplicationFrameEnvironment {
    AnmManager** animations;
    i32* pending_screen;
    const u32* background_color;
    Camera* world_camera;
    virtual void update_audio()=0;
    virtual void update_input()=0;
    virtual i32 process_loading()=0;
    virtual i32 transition(ApplicationState& application)=0;
    virtual void configure_camera(Camera& camera)=0;
    virtual void set_viewport(void* device,const CameraViewport& viewport)=0;
    virtual void clear(u32 color)=0;
    virtual void flush()=0;
};
struct ApplicationFrame {
    ApplicationState& application;ApplicationFrameEnvironment& environment;
    i32 update();
    i32 begin_draw();
    i32 finish_draw();
};
}
