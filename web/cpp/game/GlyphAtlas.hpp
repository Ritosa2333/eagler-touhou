#pragma once
#include "TextureImage.hpp"
#include <array>
#include <string>
namespace th11 {
class GlyphAtlas {
    std::array<std::vector<u8>,4> fonts;
    std::vector<u8> encoding,blend;
public:
    using Renderer=bool(*)(void*,TextureImage&,i32,i32,u32,u32,const std::string&);
    void renderer(void* owner,Renderer draw){rendererOwner=owner;rendererDraw=draw;}
    bool load(u32 slot,const u8*,u32);
    bool ready()const;
    // color is the original COLORREF ordering; destination is ARGB4444.
    bool draw(TextureImage&,i32 x,i32 y,u32 font,u32 color,const std::string&);
private:
    void* rendererOwner=nullptr;Renderer rendererDraw=nullptr;
};
}
