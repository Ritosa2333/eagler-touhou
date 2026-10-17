#pragma once
#include "AnmResource.hpp"
#include "../../../portable/sdl/GraphicsState.hpp"
namespace th09 {
// Pixels remain in their original packed storage through texture upload. The
// graphics backend receives a format name and never interprets ANM format IDs.
class TextureImage {
public:
    u32 width=0,height=0,pitch=0;
    touhou::graphics::PixelFormat format=touhou::graphics::PixelFormat::Bgra8;
    std::vector<u8> pixels;
    bool load(const AnmTextureSource&,const u8*,u32 size,bool low_color=false);
};
}
