#pragma once
#include "ApplicationState.hpp"
#include "HighResolutionClock.hpp"
namespace th10 {
struct SystemSettingsEnvironment {
    virtual void system_parameter(u32 action,u32 value,void* output,u32 flags)=0;
    virtual void query_frequency(i64& frequency)=0;
    virtual void query_counter(i64& counter)=0;
    virtual void restore_input_method()=0;
};
struct SystemSettings {
    u32 saved[3];
    void initialize(HighResolutionClock& clock,SystemSettingsEnvironment& environment);
    void restore(SystemSettingsEnvironment& environment);
};
struct ApplicationLoopEnvironment {
    ApplicationState* application;AnmManager** animations;
    const u8* frame_skip;double* frame_duration;u32* graphics_state;
    u32* fog_enabled;
    virtual Extended time()=0;
    virtual void sleep(u32 milliseconds)=0;
    virtual void flush()=0;
    virtual void configure_flat(Camera& camera)=0;
    virtual void set_viewport(void* device,const CameraViewport& viewport)=0;
    virtual i32 update()=0;
    virtual void update_audio()=0;
    virtual void stop_loader()=0;
    virtual i32 begin_scene(void* device)=0;
    virtual void draw()=0;
#ifdef TH_NATIVE_PLATFORM
    virtual i32 set_fog_enabled(bool enabled)=0;
#else
    virtual i32 render_state(void* device,u32 state,u32 value)=0;
#endif
    virtual void clear_texture(void* device)=0;
    virtual void end_scene(void* device)=0;
    virtual void present()=0;
};
#pragma pack(push,4)
struct ApplicationLoop {
    u8 reserved_000[0x14],skipped_frames,reserved_015[0x23];
    double sampled_time,previous_time,next_frame_time;
    i32 step(ApplicationLoopEnvironment& environment);
    static i32 disable_fog(ApplicationState& application,u32& enabled,ApplicationLoopEnvironment& environment);
};
#pragma pack(pop)
static_assert(sizeof(ApplicationLoop)==0x50&&offsetof(ApplicationLoop,sampled_time)==0x38);
}
