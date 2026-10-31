#include "../../src/game/TextGlyphs.hpp"
#include "../../src/game/Items.hpp"
#include <cstring>
std::vector<th12::ascii_text::Glyph> glyphs;
float itemBounds[6];
extern "C" {
int font5_layout(const char* text,float x,float y,float sx,float sy,int ax,int ay,unsigned color){glyphs.clear();return th12::ascii_text::font5(text,x,y,0,sx,sy,ax,ay,color,glyphs)?int(glyphs.size()):-1;}
const void* font5_glyphs(){return glyphs.data();}
const float* pickup_bounds(int character,int updated){using V=th12::item_system::PlayerView;auto p=V::native(character,{0,0},false,1,updated?V::PlayerPhase::Updated:V::PlayerPhase::Initialized);itemBounds[0]=p.pickupHalfWidth;itemBounds[1]=p.pickupHalfHeight;p=V::native(character,{0,0},true);itemBounds[2]=p.nearHalfWidth;itemBounds[3]=p.nearHalfHeight;return itemBounds;}
}
