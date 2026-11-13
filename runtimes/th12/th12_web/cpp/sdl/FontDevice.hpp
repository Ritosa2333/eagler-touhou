#pragma once
#include "GraphicsDevice.hpp"
#include "../game/TextRaster.hpp"
#include "../game/FontBitmapTh14.hpp"
#include "../game/Dialogue.hpp"
#if defined(TH_ENABLE_THCRAP)
#include <SDL3_ttf/SDL_ttf.h>
#include <array>
#include "ThcrapLayout.hpp"
#endif
namespace th11::sdl {
class FontDevice {
    GraphicsDevice& graphics;TextRaster raster;bool ready=false;
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
    // Set when the last call found the animation without a sprite: not an error the caller has to
    // stop on, but a request it can keep for the frame the plate's own walk binds one.
    bool waiting=false;
    bool initialize(const FontBitmapTh14* bitmap=nullptr);
    bool text(AnmVm&,const DialogueText&);
};
}
