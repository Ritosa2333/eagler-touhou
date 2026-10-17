#pragma once
#include "AnmResource.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th11 {
struct TextureImage {
    u32 width=0,height=0,pitch=0;u16 packed_format=0;
    touhou::graphics::PixelFormat format=touhou::graphics::PixelFormat::Bgra8;
    std::vector<u8> pixels;
    bool load(const AnmTexture&,bool low_color=false);
};
}
