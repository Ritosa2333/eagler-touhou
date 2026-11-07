#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextLayout.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <eagler/text/RuntimeFontRaster.hpp>
namespace th17::sdl {
class FontDevice {GraphicsDevice& graphics;TextRaster raster;
 eagler::text::RuntimeFontRaster original_font{{24,400},{28,400},{32,400},{40,400},{32,600},{40,600},{15,700},{15,700},
     {36,400},{42,400},{48,400},{60,400},{16,400}};
 bool original_ready=false;
 std::string font_path;
 std::array<TTF_Font*,9> localized_fonts{},japanese_fallback{};
 bool has_japanese_fallback=false,english_music_details=false;
 bool localization_checked=false,ttf_initialized=false;
 bool load_localized_font();
 bool write_localized(TextureImage&,const TextLayout&,const TextureText&);
public:
 std::string error;u32 writes=0;bool fallback=false;
 explicit FontDevice(GraphicsDevice& g):graphics(g){}
 ~FontDevice();
 bool translated_message(const std::string&)const;
 bool initialize();bool text(AnmVm&,const TextureText&);
 bool dialogue(AnmVm&,u32 color,u32 outline,i32 style,i32 x,i32 spacing,const std::string&);
 bool spell_title(AnmVm&,const std::string&,bool utf8);
};
}
