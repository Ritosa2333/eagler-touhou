#pragma once
#include "GraphicsState.hpp"
#include <cstdlib>
namespace touhou::graphics {
// Original resource files and the comparison harness encode pixel storage as
// integers. Decode at the resource boundary; the renderer stores PixelFormat.
inline PixelFormat asset_pixel_format(u32 code) {
    switch(code) {
    case 20:return PixelFormat::Bgr8;case 21:return PixelFormat::Bgra8;case 22:return PixelFormat::Bgrx8;
    case 23:return PixelFormat::Rgb565;case 24:return PixelFormat::Xrgb1555;case 25:return PixelFormat::Argb1555;
    case 26:return PixelFormat::Argb4444;case 28:return PixelFormat::Alpha8;case 50:return PixelFormat::Luminance8;
    case 51:return PixelFormat::LuminanceAlpha8;case 75:return PixelFormat::Depth24Stencil8;
    case 77:return PixelFormat::Depth24;case 80:return PixelFormat::Depth16;
    default:std::abort();
    }
}
inline u32 asset_pixel_code(PixelFormat f) {
    constexpr u32 codes[]{20,21,22,23,24,25,26,28,50,51,80,77,75};return codes[u32(f)];
}
constexpr u32 pixel_bytes(PixelFormat f) {
    return f==PixelFormat::Bgr8?3:f==PixelFormat::Rgb565||f==PixelFormat::Xrgb1555||f==PixelFormat::Argb1555||f==PixelFormat::Argb4444||f==PixelFormat::LuminanceAlpha8?2:f==PixelFormat::Alpha8||f==PixelFormat::Luminance8?1:4;
}
}
