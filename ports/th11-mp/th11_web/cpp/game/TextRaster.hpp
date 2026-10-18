#pragma once
#include "GlyphAtlas.hpp"
#include "ImageResample.hpp"
namespace th11 {
struct TextStyle {i32 offset=0,height=17,font=0;u32 color=0xffffff,outline=0,spacing=0;bool plain=false;};
class TextRaster {
public:
    GlyphAtlas glyphs;TextureImage scratch;
    TextRaster();
    bool rasterize(const std::string&,const TextStyle&);
    bool write(TextureImage&,const ImageRect&,const std::string&,const TextStyle&);
    static void bleed(TextureImage&,u32 rows);
};
}
