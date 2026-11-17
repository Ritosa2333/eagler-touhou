#pragma once
#include "TextureImage.hpp"
#include "PortTitle.hpp"
#include <array>
#include <string>
namespace th11 {
class GlyphAtlas {
    std::array<std::vector<u8>,PORT_TITLE==14?8:4> fonts;
    std::vector<u8> encoding,blend;
public:
    bool load(u32 slot,const u8*,u32);
    bool ready()const;
    // color is the original COLORREF ordering; destination is ARGB4444.
    bool draw(TextureImage&,i32 x,i32 y,u32 font,u32 color,const std::string&);
};
}
