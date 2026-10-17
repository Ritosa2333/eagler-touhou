#pragma once
#include "TextureResample.hpp"
#include <memory>
#ifdef TH_NATIVE_PLATFORM
#include <map>
#endif
namespace th08 {
// Prepared translations arrive as UTF-8 while original assets stay CP932.
// These helpers decode one buffer at the rasterization boundary; localization
// off never touches them.
bool utf8_valid(const u8* text,u32 size) noexcept;
u16 utf8_next(const u8*& cursor,const u8* end) noexcept;
// Display columns of a valid UTF-8 string: ASCII counts 1, anything else 2.
// Matches the column model the CP932 byte count expresses for original text.
u32 utf8_display_columns(const u8* text,u32 size) noexcept;
class Cp932 {
public:
    bool load(const u8* data,u32 size);
    u16 next(const u8*& cursor,const u8* end) const noexcept;
    bool loaded() const noexcept {return mapping.size()==65536;}
private:
    std::vector<u16> mapping;
};
class JapaneseFont {
public:
    struct Glyph {u32 code=0,offset=0;i16 advance=0,left=0,top=0;u16 width=0,height=0;};
    bool load(const u8*,u32);
    const Glyph* find(u16 code) const noexcept;
    const u8* coverage(const Glyph& g) const noexcept;
    i32 height=0,weight=0;
#ifdef TH_NATIVE_PLATFORM
    bool native=false;
    struct Raster {Glyph glyph;std::vector<u8> pixels;};
    mutable std::map<u16,Raster> rasters;
#endif
private:
    std::vector<Glyph> glyphs;
    std::vector<u8> bytes;
};
class JapaneseFonts {
public:
    Cp932 encoding;
    bool load_blend(const u8*,u32);
    bool load_font(const u8*,u32);
    const JapaneseFont* find(i32 height,i32 weight) const noexcept;
    bool draw(const JapaneseFont&,PixelSurface&,i32 x,i32 y,u32 color,const u8* text,u32 size) const;
private:
    std::vector<u8> blend;
    mutable std::vector<std::unique_ptr<JapaneseFont>> fonts;
};
}
