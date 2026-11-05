#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextSurface.hpp"
namespace th075::sdl {
class FontDevice {
 GraphicsDevice& graphics;GlyphBank bank,chineseBank;bool useChinese=false;
public:
 std::string error;
 explicit FontDevice(GraphicsDevice& g):graphics(g){}
 bool initialize();u32 create(const std::string&,const TextRasterOptions&);
 bool write(u32 texture,const std::string&,const TextRasterOptions&);
};
}
