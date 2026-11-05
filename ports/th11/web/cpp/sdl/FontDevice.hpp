#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextRaster.hpp"
#include "../game/Dialogue.hpp"
#include <eagler/text/RuntimeFontRaster.hpp>
#if defined(TH_ENABLE_THCRAP)
#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include "ThcrapLayout.hpp"
#endif
namespace th11::sdl {
class FontDevice {
    GraphicsDevice& graphics;TextRaster raster;bool ready=false;
    eagler::text::RuntimeFontRaster runtimeFonts{{32,400},{32,600},{15,700},{15,700}};
#if defined(TH_ENABLE_THCRAP)
    std::array<TTF_Font*,4> localized{};
    std::vector<int> layout_tabs;
    LayoutLine layout(const std::string&,int);
    void font_style(int,const std::string&);
    bool write_unicode(TextureImage&,const ImageRect&,const std::string&,const TextStyle&);
#endif
public:
    explicit FontDevice(GraphicsDevice& g):graphics(g){}
    ~FontDevice();
    std::string error;
    u32 writes=0;
    bool initialize();
    bool text(AnmVm&,const DialogueText&);
};
}
