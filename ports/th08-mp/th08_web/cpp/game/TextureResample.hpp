#pragma once
#include "TexturePixels.hpp"
namespace th08 {
struct TextureRect { i32 left=0,top=0,right=0,bottom=0; };
struct PixelSurface { u32 format=0,width=0,height=0,pitch=0; u8* pixels=nullptr; };
struct TextureResample {
    static bool valid(const PixelSurface&,const TextureRect&) noexcept;
    static i32 point(PixelSurface&,const TextureRect&,const PixelSurface&,const TextureRect&) noexcept;
    static i32 triangle(PixelSurface&,const TextureRect&,const PixelSurface&,const TextureRect&,
                        bool wrap_x=true,bool wrap_y=true,bool dither=false) noexcept;
};
}
