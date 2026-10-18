#pragma once
#include "Textures.hpp"
namespace th10::browser {
struct TextureResample {
    static i32 point(PixelSurface& output,const TextureRect& destination,const PixelSurface& input,const TextureRect& source) noexcept;
    static i32 triangle(PixelSurface& output,const TextureRect& destination,const PixelSurface& input,const TextureRect& source,bool wrap_x=true,bool wrap_y=true,bool dither=false) noexcept;
};
}
