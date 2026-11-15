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
    static constexpr int th14_font_count=DialogueText::th14_spell_title_style+1;
    std::array<TTF_Font*,PORT_TITLE==14?th14_font_count:4> localized{};
    static constexpr int font_index(int n){return n>=0&&n<(PORT_TITLE==14?th14_font_count:3)?n:3;}
#if PORT_TITLE == 14
    std::array<TTF_Font*,th14_font_count> fallback{};
    std::vector<u16> cp932;
    std::string decode(const std::string&) const;
#endif
    std::vector<int> layout_tabs;
    LayoutLine layout(const std::string&,int);
    void font_style(int,const std::string&);
    bool write_unicode(TextureImage&,const ImageRect&,const std::string&,const TextStyle&,i32 width_limit);
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
    i32 measure(const std::string&,int);
};
}
