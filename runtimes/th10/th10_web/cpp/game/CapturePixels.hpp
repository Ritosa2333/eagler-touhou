#pragma once
#include "AnmCapture.hpp"
#include "AnmManager.hpp"
#include "TexturePlatform.hpp"
namespace th10 {
struct CaptureSize {i32 width,height;u8 reserved[20];};
struct CapturePoint {i32 x,y;};
struct CapturePixelEnvironment : AnmCaptureEnvironment {
    void** device;const u32* format;
    virtual void flush(AnmManager& manager)=0;
    virtual i32 back_buffer(void* device,void** output)=0;
    virtual i32 texture_surface(void* texture,void** output)=0;
    virtual i32 render_target(void* device,i32 width,i32 height,u32 format,void** output)=0;
    virtual i32 offscreen_surface(void* device,i32 width,i32 height,u32 format,void** output)=0;
    virtual i32 copy_surface(void* destination,const TextureRect* destination_rect,void* source,const TextureRect* source_rect,u32 filter)=0;
    virtual void update_surface(void* device,void* source,const TextureRect& source_rect,void* destination,const CapturePoint& destination_point)=0;
};
struct CapturePixels {
    AnmManager& manager;CapturePixelEnvironment& environment;
    AnmCaptureBuffers& buffers() noexcept {return *reinterpret_cast<AnmCaptureBuffers*>(manager.resource_state+4);}
    CaptureSize& size(i32 slot) noexcept {return reinterpret_cast<CaptureSize*>(manager.resource_state+0x204)[slot];}
    bool restore_texture(i32 slot);
    void restore(i32 slot,i32 left,i32 top,CapturePoint destination);
    void restore_rectangle(i32 slot,CaptureRectangle source,CapturePoint destination);
    void capture_texture(i32 file,i32 texture,CaptureRectangle source,CaptureRectangle destination);
    void copy_texture(i32 destination_file,i32 destination_texture,i32 source_file,i32 source_texture,const TextureRect* destination,const TextureRect* source);
    void capture_screen(i32 slot,CaptureRectangle source,CaptureRectangle destination);
};
static_assert(sizeof(CaptureSize)==0x1c);
}
