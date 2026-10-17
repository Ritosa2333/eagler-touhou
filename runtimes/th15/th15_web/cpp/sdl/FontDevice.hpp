#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextLayout.hpp"
#if defined(TH_ENABLE_THCRAP)
#include <SDL3_ttf/SDL_ttf.h>
#include "ThcrapLayout.hpp"
#endif
namespace th15::sdl {
class FontDevice {GraphicsDevice& graphics;TextRaster raster;std::array<bool,15> loaded{};bool load(u32 slot);
#if defined(TH_ENABLE_THCRAP)
    std::array<TTF_Font*,9> localized{};bool localization_checked=false,english_details=false;
    std::vector<int> layout_tabs;
    bool initialize_localized();
    LayoutLine layout(const std::string&,int);
    void font_style(int,const std::string&);
    bool write_unicode(TextureImage&,const TextRect&,const std::string&,const TextStyle&);
#endif
public:
    std::string error;u32 writes=0;bool fallback=false;
    explicit FontDevice(GraphicsDevice& g):graphics(g){}
    ~FontDevice();
    bool initialize();bool preload(i32 style);bool text(AnmVm&,const DialogueText&);
    i32 extent(const std::string&,i32);
};
}
