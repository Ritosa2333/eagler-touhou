// TH08 text layout/outline rules adapted from the MIT reference and checked
// against the local 1.00d executable. Glyph coverage is original GDI data.
#pragma once
#include "JapaneseFonts.hpp"
namespace th08 {
class TextRasterBuffer {
public:
    u32 width=0,height=0,format=0,pitch=0;
    std::vector<u8> pixels;
    bool create(i32 width,i32 height,u32 format);
    bool invert_alpha(i32 x,i32 y,i32 sprite_width,i32 font_height,bool soft);
    PixelSurface surface() {return {format,width,height+1,pitch,pixels.data()};}
};
class TextRaster {
public:
    explicit TextRaster(JapaneseFonts& fonts):fonts(fonts){}
    bool render(PixelSurface& output,i32 x,i32 y,i32 sprite_width,i32 sprite_height,i32 font_height,i32 font_width,
                u32 color,u32 outline,const u8* text,u32 size,bool bold);
    const TextRasterBuffer& last_buffer() const noexcept {return scratch;}
private:
    JapaneseFonts& fonts;
    TextRasterBuffer scratch;
};
}
