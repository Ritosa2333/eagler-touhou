#pragma once
#include "ApplicationConfig.hpp"
#include "Camera.hpp"
namespace th10 {
struct DisplayMode {u32 width,height,refresh,format;};
struct PresentationParameters {
    u32 width,height,format,count,multisample,quality,swap_effect,window,windowed,automatic_depth,depth_format,flags,refresh,interval;
};
enum class GraphicsNotice {DefaultDepth,Vsync,Immediate,HardwareRetry,SoftwareVertexRetry,RefreshFallback,VsyncFallback,DeviceFailed,ReferenceDevice,SoftwareVertexDevice,HardwareDevice,NoTextureAlpha,SmallTextureLimit,NoArgbTexture};
struct GraphicsStartupEnvironment {
    void** driver;void** device;u32* window;ApplicationConfig* settings;u32* engine_flags;const u8* alternate_launch;i32* disable_vsync;i32* fixed_refresh;u32* quit;u32* reset_state;
    PresentationParameters* parameters;Matrix4* view;Matrix4* projection;CameraViewport* viewport;u32* capabilities;
    virtual void display_mode(void* driver,DisplayMode& mode)=0;
    virtual i32 create_device(void* driver,u32 type,u32 window,u32 behavior,PresentationParameters& parameters,void** device)=0;
    virtual void release(void* object)=0;
    virtual void notice(GraphicsNotice message)=0;
    virtual void look_at(Matrix4& matrix,const Vec3& eye,const Vec3& target,const Vec3& up)=0;
    virtual void perspective(Matrix4& matrix,float fov,float aspect,float near_plane,float far_plane)=0;
    virtual void set_transform(void* device,MatrixParameter kind,const Matrix4& matrix)=0;
    virtual void get_viewport(void* device,CameraViewport& viewport)=0;
    virtual void get_capabilities(void* device,u32* capabilities)=0;
    virtual i32 check_argb_texture(void* driver,u32 format)=0;
    virtual void configure_defaults()=0;
};
struct GraphicsStartup {
    GraphicsStartupEnvironment& environment;
    // Clearing the two swap-chain buffers presents twice. The driver yields
    // between those presents, then calls finish before returning to startup.
    i32 initialize();
    void finish() noexcept {*environment.quit=0;*environment.reset_state=0;}
};
static_assert(sizeof(PresentationParameters)==56);
}
