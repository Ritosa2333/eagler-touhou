#include "../../cpp/game/Manual.hpp"
struct ManualFixture final:ManualServices {
 Manual panel;std::vector<i32> events;explicit ManualFixture(AnmManager& a):panel(a,*this){}
 bool sound(i32 id)override{events.insert(events.end(),{1,id});return true;}bool clear_page()override{events.push_back(2);return true;}
 bool request_page(i32 index)override{events.insert(events.end(),{3,index});return true;}bool upload_page(i32 index)override{events.insert(events.end(),{4,index});return true;}
 std::array<u32,14> values{};
};
API(manual_create) ManualFixture* manual_create(AnmManagerFixture* a){return new ManualFixture(a->manager);}
API(manual_delete) void manual_delete(ManualFixture* p){delete p;}
API(manual_ready) i32 manual_ready(ManualFixture* p){return p->panel.page_loaded();}
API(manual_update) i32 manual_update(ManualFixture* p,u32 pressed,u32 repeated,float rate){p->events.clear();return p->panel.update(pressed,repeated,rate);}
API(manual_values) const u32* manual_values(ManualFixture* p){auto& v=p->values;v[0]=p->panel.mode;v[1]=p->panel.phase;v[2]=p->panel.finished;v[3]=p->panel.page;v[4]=p->panel.menu.wrap;std::copy(p->panel.choices.begin(),p->panel.choices.end(),v.begin()+5);return v.data();}
API(manual_age) Timer* manual_age(ManualFixture* p){return &p->panel.age;}
API(manual_cursor) const u8* manual_cursor(ManualFixture* p){return reinterpret_cast<const u8*>(&p->panel.menu);}
API(manual_event_count) u32 manual_event_count(ManualFixture* p){return p->events.size();}
API(manual_events) const i32* manual_events(ManualFixture* p){return p->events.data();}
API(manual_error) const char* manual_error(ManualFixture* p){return p->panel.error.c_str();}

#include "../../cpp/game/TitleManualMenu.hpp"
struct TitleManualFixture {TitleManualMenu menu;TitleManualFixture(TitleMainFixture& t,ManualFixture& m):menu(t.state,t.animations,m.panel){t.state.screen=TitleScreen::Manual;}};
API(title_manual_create) TitleManualFixture* title_manual_create(TitleMainFixture* t,ManualFixture* m){return new TitleManualFixture(*t,*m);}
API(title_manual_delete) void title_manual_delete(TitleManualFixture* p){delete p;}
API(title_manual_update) i32 title_manual_update(TitleManualFixture* p){return p->menu.update();}
API(manual_finished) void manual_finished(ManualFixture* p,u32 value){p->panel.finished=value;}
API(title_manual_error) const char* title_manual_error(TitleManualFixture* p){return p->menu.error.c_str();}

