#pragma once
#include "HudDrawData.hpp"
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
namespace th13 {
struct AsciiTextEntry {HudTextDraw text;i32 lifetime=0;bool shadow=false;};
struct AsciiGlyphServices {virtual ~AsciiGlyphServices()=default;virtual bool ascii_glyph(AnmVm&)=0;};
// One typed text queue and one reusable glyph; the shared sprite renderer owns
// vertex generation and batching. No original device/font objects are used.
// 字形 sprite 序号来自原版 0x4042a0：字符项 = (ch*9+C)*8 字节偏移的 0x48=72 字节
// slot 表（FUN_0046e770 自 ANM sprite 表顺序展开），C/9 为基序号：
// font0=ch-32、font1=ch+66、font2/3=ch+179（小写 ch+147）、font4/5=ch+222、
// font6/7=ch+237；特殊字符为独立 slot（见 AsciiText.cpp），序号即 ascii.anm
// 归档内 sprite 全局序。
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
