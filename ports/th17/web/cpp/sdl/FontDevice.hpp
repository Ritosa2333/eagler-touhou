#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextLayout.hpp"
namespace th17::sdl {
class FontDevice {GraphicsDevice& graphics;TextRaster raster;std::array<bool,15> loaded{};bool load(u32 slot);
public:
 std::string error;u32 writes=0;bool fallback=false;
 explicit FontDevice(GraphicsDevice& g):graphics(g){}
 bool initialize();bool preload(i32 style);bool text(AnmVm&,const TextureText&);
 bool dialogue(AnmVm&,u32 color,u32 outline,i32 style,i32 x,i32 spacing,const std::string&);
 bool spell_title(AnmVm&,const std::string&);
};
}
