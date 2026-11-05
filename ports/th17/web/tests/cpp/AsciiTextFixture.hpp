#pragma once
#include "../../cpp/game/AsciiText.hpp"
struct AsciiTextFixture final:AsciiGlyphServices {AsciiText text;HudTextDraw request;std::vector<AnmVm> glyphs;explicit AsciiTextFixture(AnmManagerFixture& a):text(a.manager,a.environment){}bool ascii_glyph(AnmVm& vm)override{glyphs.push_back(vm);return true;}};
API(ascii_text_create) AsciiTextFixture* ascii_text_create(AnmManagerFixture* a){return new AsciiTextFixture(*a);}
API(ascii_text_delete) void ascii_text_delete(AsciiTextFixture* p){delete p;}
API(ascii_text_initialize) u32 ascii_text_initialize(AsciiTextFixture* p,i32 bank){return p->text.initialize(bank);}
API(ascii_text_position) Vec3* ascii_text_position(AsciiTextFixture* p){return &p->request.position;}
API(ascii_text_style) HudTextStyle* ascii_text_style(AsciiTextFixture* p){return &p->request.style;}
API(ascii_text_enqueue) u32 ascii_text_enqueue(AsciiTextFixture* p,const char* value,i32 lifetime,u32 shadow){p->request.text=value;return p->text.enqueue(p->request,lifetime,shadow);}
API(ascii_text_update) void ascii_text_update(AsciiTextFixture* p){p->text.update();}
API(ascii_text_count) u32 ascii_text_count(AsciiTextFixture* p){return p->text.count();}
API(ascii_text_entry_position) const Vec3* ascii_text_entry_position(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?&e->text.position:nullptr;}
API(ascii_text_entry_style) const HudTextStyle* ascii_text_entry_style(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?&e->text.style:nullptr;}
API(ascii_text_entry_value) const char* ascii_text_entry_value(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?e->text.text.c_str():nullptr;}
API(ascii_text_entry_lifetime) i32 ascii_text_entry_lifetime(AsciiTextFixture* p,u32 i){auto* e=p->text.entry(i);return e?e->lifetime:-1;}
API(ascii_text_draw) u32 ascii_text_draw(AsciiTextFixture* p,i32 space){p->glyphs.clear();return p->text.draw(space,*p);}
API(ascii_text_glyph_count) u32 ascii_text_glyph_count(AsciiTextFixture* p){return p->glyphs.size();}
API(ascii_text_glyph) AnmVm* ascii_text_glyph(AsciiTextFixture* p,u32 i){return i<p->glyphs.size()?&p->glyphs[i]:nullptr;}
API(ascii_text_error) const char* ascii_text_error(AsciiTextFixture* p){return p->text.error.c_str();}

API(ascii_quad_positions) u32 ascii_quad_positions(AnmVm* p,Vec3* output){Vec3 positions[4];if(!anm_quad_positions(*p,positions))return 0;std::memcpy(output,positions,sizeof positions);return 1;}
