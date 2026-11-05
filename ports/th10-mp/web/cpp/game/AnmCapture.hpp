#pragma once
#include "AnmRegistry.hpp"
namespace th10 {
struct CaptureRectangle {i32 left,top,width,height;};
// First 0x2c bytes of AnmManager. Rendering consumes the pending request later.
struct AnmCapture {
    i32 reserved_000,target_file;
    CaptureRectangle source,destination;
    u32 flags;
    i32 from_animation(u32 animation,const CaptureRectangle& source,const AnmRegistry& registry) noexcept;
    i32 request(i32 file,u32 flags,const CaptureRectangle& source,const CaptureRectangle& destination) noexcept;
};
static_assert(sizeof(AnmCapture)==0x2c);
struct AnmCaptureEnvironment {
    virtual void release_surface(void* surface)=0;
    virtual void free_pixels(void* pixels)=0;
};
// The three arrays start at AnmManager + 0x3ad4e0.
struct AnmCaptureBuffers {
    void* textures[32];
    void* surfaces[32];
    void* pixels[32];
    void release(i32 slot,AnmCaptureEnvironment& environment);
};
static_assert(sizeof(AnmCaptureBuffers)==0x180);
}
