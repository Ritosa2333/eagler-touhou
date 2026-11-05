#pragma once
#include "GraphicsStartup.hpp"
namespace th10 {
struct ClearScreenEnvironment {
    void** device;AnmManager** animations;CameraViewport* viewport;PresentationParameters* parameters;
    virtual void flush(AnmManager& animations)=0;
    virtual void set_viewport(void* device,const CameraViewport& viewport)=0;
    virtual void clear(void* device,u32 color)=0;
    virtual i32 present(void* device)=0;
    virtual i32 reset(void* device,PresentationParameters& parameters)=0;
};
// The original submits twice. Keep the progress between submissions so a
// browser can yield without retaining the C++ call stack across frames.
struct ClearScreen {
    u32 color=0,submitted=0;i32 result=0;
    static void prepare(ClearScreenEnvironment& environment);
    bool advance(ClearScreenEnvironment& environment);
};
}
