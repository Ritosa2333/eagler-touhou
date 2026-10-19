#pragma once
#include "Types.hpp"
#include <map>
namespace th075 {
struct GlyphOutline {u16 width=0,height=0,stride=0;i16 left=0,top=0;const u8* coverage=nullptr;};
class GlyphBank {
 struct Entry {u16 width,height,stride;i16 left,top;u32 offset;};
 std::vector<u8> storage;std::map<std::pair<u16,u16>,Entry> entries;
public:
 std::string error;bool load(const void*,std::size_t);bool glyph(u16 height,u16 code,GlyphOutline&)const;
};
struct TextRasterOptions {
 u32 width=1024,height=256,wrapWidth=560,fontHeight=24; i32 characterSpacing=0,lineSpacing=0;
 u8 red=120,green=120,blue=255,lineLimit=5,bytesPerPixel=4;
};
class TextRaster {
 const GlyphBank& bank;const GlyphBank* chineseBank=nullptr;
public:
 std::string error;explicit TextRaster(const GlyphBank& bank,const GlyphBank* chinese=nullptr):bank(bank),chineseBank(chinese){}
 bool render(const std::string& cp932,const TextRasterOptions&,std::vector<u8>&);
};
}
