// TH08 title screen presentation, adapted from the MIT reference and checked
// against the original executable at the drawing and text queue boundaries.
#pragma once
#include "TitleMenus.hpp"
#include "AsciiManager.hpp"
namespace th08 {
class TitleView {
public:
    TitleView(TitleMenus& menus,AsciiManager& ascii,AnmRenderer& renderer)
        :menus(menus),state(menus.state),context(menus.context),config(menus.config),ascii(ascii),renderer(renderer){}
    i32 draw();
    i32 replays();
    i32 practice();
    i32 spell_stages();
    i32 spell_cards();
    i32 completion();
    void pie(const Vec3& position,u32 color,float fraction,float diameter);
private:
    TitleMenus& menus;TitleState& state;TitleContext& context;GameConfiguration& config;
    AsciiManager& ascii;AnmRenderer& renderer;
    template<class... Args> void add(const Vec3& pos,const char* format,Args... args){ascii.add_format(pos,context.software_texturing,format,args...);}
    void reset_text(){ascii.state.color=0xffffffff;ascii.state.selected=0;}
};
}
