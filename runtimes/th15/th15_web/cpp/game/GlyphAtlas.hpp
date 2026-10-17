#pragma once
#include "TextureImage.hpp"
#include <array>
#include <string>
#include <unordered_map>
namespace th15 {
class GlyphAtlas {
    std::array<std::vector<u8>,13> fonts;
    std::vector<u8> encoding,blend;
    // Coverage is immutable after loading; reuse it for all outline passes.
    std::array<std::unordered_map<u32,std::vector<u8>>,13> decoded;
public:
    bool load(u32 slot,const u8*,u32);
    bool ready()const;
    // color is the original COLORREF ordering; destination is ARGB4444.
    bool draw(TextureImage&,i32 x,i32 y,u32 font,u32 color,const std::string&);
};
}
