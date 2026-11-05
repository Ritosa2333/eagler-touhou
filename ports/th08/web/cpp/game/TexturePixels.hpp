#pragma once
#include "Types.hpp"
#include <vector>
namespace th08 {
struct PixelFormat {
    u32 bytes=0, red_mask=0, green_mask=0, blue_mask=0, alpha_mask=0;
    u32 red_shift=0, green_shift=0, blue_shift=0, alpha_shift=0;
};
class TexturePixels {
public:
    u32 width=0,height=0,format=0;
    std::vector<u8> pixels;
    bool create(u32 width,u32 height,u32 format);
    bool from_anm(const u8* bytes,u32 size,u32 requested_format,bool force_16bit);
    bool from_rgba(const u8* rgba,u32 width,u32 height,u32 format);
    std::vector<u8> rgba() const;
    static PixelFormat describe(u32 format);
    static u32 anm_format(u32 format,bool force_16bit);
    static bool convert(u8* output,u32 output_format,const u8* source,u32 source_format,u32 count);
};
}
