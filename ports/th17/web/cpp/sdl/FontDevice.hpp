#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextLayout.hpp"
#include <SDL3_ttf/SDL_ttf.h>
namespace th17::sdl {
class FontDevice {GraphicsDevice& graphics;TextRaster raster;std::array<bool,15> loaded{};bool load(u32 slot);
 std::string font_path;
 std::array<TTF_Font*,9> localized_fonts{},japanese_fallback{};
 bool has_japanese_fallback=false;
 bool localization_checked=false,ttf_initialized=false;
 bool load_localized_font();
 bool write_localized(TextureImage&,const TextLayout&,const TextureText&);
public:
 std::string error;u32 writes=0;bool fallback=false;
 explicit FontDevice(GraphicsDevice& g):graphics(g){}
 ~FontDevice();
 bool translated_message(const std::string&)const;
 bool initialize();bool preload(i32 style);bool text(AnmVm&,const TextureText&);
 bool dialogue(AnmVm&,u32 color,u32 outline,i32 style,i32 x,i32 spacing,const std::string&);
 bool spell_title(AnmVm&,const std::string&,bool utf8);
};
}
