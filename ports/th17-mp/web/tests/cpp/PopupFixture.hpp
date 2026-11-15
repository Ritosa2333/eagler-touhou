#pragma once
#include "../../cpp/game/PopupManager.hpp"
struct PopupFixture final:PopupDrawServices {
    PopupManager manager;
    std::vector<AnmVm> glyphs;
    std::vector<HudTextDraw> texts;
    explicit PopupFixture(AnmManager& a):manager(a){}
    bool popup_glyph(AnmVm& vm)override{glyphs.push_back(vm);return true;}
    bool popup_text(const HudTextDraw& text)override{texts.push_back(text);return true;}
};
API(popup_create) PopupFixture* popup_create(AnmManagerFixture* a){return new PopupFixture(a->manager);}
API(popup_delete) void popup_delete(PopupFixture* p){delete p;}
API(popup_initialize) u32 popup_initialize(PopupFixture* p,i32 bank){return p->manager.initialize(bank);}
API(popup_entries) PopupEntry* popup_entries(PopupFixture* p){return p->manager.entries.data();}
API(popup_cursor) u32 popup_cursor(PopupFixture* p){return p->manager.cursor();}
API(popup_number) void popup_number(PopupFixture* p,const Vec3* position,i32 value,u32 color){p->manager.number(*position,value,color);}
API(popup_update) void popup_update(PopupFixture* p,float rate){p->manager.update(rate);}
API(popup_draw) u32 popup_draw(PopupFixture* p,const Vec3* player){p->glyphs.clear();p->texts.clear();return p->manager.draw(*p,*player);}
API(popup_glyph_count) u32 popup_glyph_count(PopupFixture* p){return p->glyphs.size();}
API(popup_glyph) AnmVm* popup_glyph(PopupFixture* p,u32 i){return &p->glyphs[i];}
API(popup_text_count) u32 popup_text_count(PopupFixture* p){return p->texts.size();}
API(popup_text_position) const Vec3* popup_text_position(PopupFixture* p,u32 i){return &p->texts[i].position;}
API(popup_text_style) const HudTextStyle* popup_text_style(PopupFixture* p,u32 i){return &p->texts[i].style;}
API(popup_text_value) const char* popup_text_value(PopupFixture* p,u32 i){return p->texts[i].text.c_str();}
API(popup_error) const char* popup_error(PopupFixture* p){return p->manager.error.c_str();}
