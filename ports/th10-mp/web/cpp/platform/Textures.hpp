#pragma once
#include "Graphics.hpp"
#include "../game/TexturePlatform.hpp"
namespace th10::browser {
struct PixelCopy {
    static i32 copy(PixelSurface& destination,const TextureRect& target,const u8* source,u32 source_format,i32 source_pitch,const TextureRect& region) noexcept;
};
struct Textures final:TexturePlatform {
    GraphicsDevice& device;
    explicit Textures(GraphicsDevice& device,const u32& flags):device(device){display_flags=&flags;}
    i32 create_surface_texture(AnmTexture& texture,u32 width,u32 height,u32 format) override;
    i32 decode_source_texture(AnmTexture&,u32,u32,u32,u32) override;
    void* get_surface(void* texture) override;
    TextureDescription describe_surface(void* surface) override;
    TextureLock lock_surface(void* surface) override;
    void unlock_surface(void* surface) override;
    void release_surface(void* surface) override;
    i32 upload_surface(void* surface,const u8* bytes,u32 format,i32 pitch,const TextureRect& rectangle) override;
};
}
