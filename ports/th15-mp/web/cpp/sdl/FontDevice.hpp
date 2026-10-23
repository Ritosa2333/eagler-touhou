#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextLayout.hpp"
#include <eagler/text/RuntimeFontRaster.hpp>
#if defined(TH_ENABLE_THCRAP)
#include <SDL3_ttf/SDL_ttf.h>
#include "ThcrapLayout.hpp"
#endif
namespace th15::sdl {
class FontDevice {GraphicsDevice& graphics;TextRaster raster;
    eagler::text::RuntimeFontRaster runtimeFonts{{24,400},{28,400},{32,400},{40,400},{32,600},{40,600},{15,700},{15,700},{36,400},{42,400},{48,400},{60,400},{16,400}};
    bool fontsReady=false;
    std::vector<int> layout_tabs;
#if defined(TH_ENABLE_THCRAP)
    std::array<TTF_Font*,9> localized{};bool localization_checked=false,english_details=false;
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
    const std::vector<int>& rollback_layout()const{return layout_tabs;}
    void restore_layout(const std::vector<int>&);
};
}
