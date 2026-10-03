#pragma once
#include "ApplicationState.hpp"
#include "AnmCapture.hpp"
namespace th10 {
struct CaptureRequests {AnmCapture texture;CaptureRectangle screen_source,screen_destination;};
static_assert(sizeof(CaptureRequests)==0x4c);
struct PresentationEnvironment {
    ApplicationState* application;AnmManager** animations;
    const u16* pressed_keys;i32* reset_frames;void* presentation_parameters;
    virtual i32 present(void* device)=0;
    virtual void reset_device(void* device,void* parameters)=0;
    virtual void release_surface(void* surface)=0;
#ifdef TH_NATIVE_PLATFORM
    virtual void configure_graphics()=0;
#else
    virtual void render_state(void* device,u32 setting,u32 value)=0;
    virtual void texture_stage(void* device,u32 stage,u32 setting,u32 value)=0;
    virtual void sampler_state(void* device,u32 sampler,u32 setting,u32 value)=0;
#endif
    virtual void capture_texture(AnmManager& animations,i32 file,u32 flags,CaptureRectangle source,CaptureRectangle destination)=0;
    virtual void capture_screen(AnmManager& animations,i32 slot,CaptureRectangle source,CaptureRectangle destination)=0;
    virtual void create_directory(const char* path)=0;
    virtual bool file_exists(const char* path)=0;
    virtual void save_screenshot(ApplicationState& application,const char* path)=0;
};
struct Presentation {
    PresentationEnvironment& environment;
    void configure_defaults();
    void release_textures(AnmManager& animations);
    void process_captures(AnmManager& animations);
    void submit();
};
}
