#pragma once
#include "AnmFile.hpp"
namespace th10 {
struct PixelSurface {
    u32 format,width,height;
    i32 pitch;
    u8* pixels;
    void repair_edges() noexcept;
};
struct TextureDescription {u32 format,width,height;};
struct TextureLock {i32 pitch;u8* pixels;};
struct TextureRect {i32 left,top,right,bottom;};
struct TexturePlatform {
    const u32* display_flags;
    virtual i32 create_surface_texture(AnmTexture& texture,u32 width,u32 height,u32 format)=0;
    virtual i32 decode_source_texture(AnmTexture& texture,u32 width,u32 height,u32 format,u32 color_key)=0;
    virtual void* get_surface(void* texture)=0;
    virtual TextureDescription describe_surface(void* surface)=0;
    virtual TextureLock lock_surface(void* surface)=0;
    virtual void unlock_surface(void* surface)=0;
    virtual void release_surface(void* surface)=0;
    virtual i32 upload_surface(void* surface,const u8* bytes,u32 format,i32 pitch,const TextureRect& rectangle)=0;
};
}
