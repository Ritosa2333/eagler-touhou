#pragma once
#include "HudDrawData.hpp"
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
namespace th15 {
struct AsciiTextEntry {HudTextDraw text;i32 lifetime=0;bool shadow=false;};
struct AsciiGlyphServices {virtual ~AsciiGlyphServices()=default;virtual bool ascii_glyph(AnmVm&)=0;};
// One typed text queue and one reusable glyph; the shared sprite renderer owns
// vertex generation and batching. No original device/font objects are used.
class AsciiText final:public HudDrawServices {
 AnmManager& animations;AnmEnvironment& environment;AnmVm glyph;std::vector<AsciiTextEntry> entries;
 bool draw_entry(const AsciiTextEntry&,AsciiGlyphServices&);
public:
 std::string error;i32 spacing=9;explicit AsciiText(AnmManager& a,AnmEnvironment& e):animations(a),environment(e){entries.reserve(320);}
 bool initialize(i32 bank);bool enqueue(const HudTextDraw&,i32 lifetime=0,bool shadow=false);bool hud_text(const HudTextDraw& text)override{return enqueue(text,0,text.style.shadow);}
 void update();bool draw(i32 coordinate_space,AsciiGlyphServices&);bool draw(i32 coordinate_space,AnmRenderer&);
 void scene_origin(bool enabled)noexcept{glyph.visual.render_flags=(glyph.visual.render_flags&~0xc0000u)|(enabled?0x80000u:0u);}
 u32 count()const noexcept{return entries.size();}const AsciiTextEntry* entry(u32 i)const noexcept{return i<entries.size()?&entries[i]:nullptr;}
};
}
