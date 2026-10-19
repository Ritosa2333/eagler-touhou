#pragma once
#include "GlyphAtlas.hpp"
namespace th15 {
struct TextRect {i32 left=0,top=0,right=0,bottom=0;};
struct TextStyle {i32 height=17,font=0,offset=0,spacing=0;u32 color=0xffffff,outline=0;bool shadow=true,fallback=false;};
class TextRaster {
public:
    GlyphAtlas glyphs;TextureImage scratch;
    TextRaster();
    static u32 font_index(i32 style,bool fallback)noexcept;
    static bool invert_alpha(TextureImage&,u32 rows);
    static bool expand_alpha(TextureImage&,u32 rows);
    static bool bleed(TextureImage&,u32 rows);
    bool rasterize(const std::string&,const TextStyle&);
    bool write(TextureImage&,const TextRect&,const std::string&,const TextStyle&);
};
}
