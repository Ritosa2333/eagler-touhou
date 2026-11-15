#include "../../cpp/game/TitlePlayerData.hpp"
#include "../../cpp/game/TitleTrophies.hpp"
struct TitleDataFixture final:TitlePlayerDataServices,TitleTrophyServices,HudDrawServices,ReplayCalendarServices {
 struct Text {u32 handle;TextureText request;};TitleMainFixture& t;RecordStore records;TitleSpellMemory memory;TrophyCatalog catalog;TitleKeyboard keys;TitlePlayerData data;TitleSpellPractice practice;TitleTrophies trophies;
 std::vector<Text> texts;std::vector<i32> events;std::vector<HudTextDraw> captions;ReplayCalendar date{2026,10,23,20,36};
 explicit TitleDataFixture(TitleMainFixture& title):t(title),data(t.state,records,t.animations,*this),practice(t.state,t.selection,t.settings,records,t.animations,*this,memory),trophies(t.state,records,catalog,t.animations,*this){}
 bool sound(i32 i)override{events.push_back(i);return true;}bool keyboard(TitleKeyboard& out)override{out=keys;return true;}
 bool text(AnmVm& vm,const TextureText& out)override{texts.push_back({t.animations.registry.handle(vm),out});return true;}
 bool fade_menu_music()override{events.push_back(1001);return true;}bool show_loading_indicator()override{events.push_back(1002);return true;}
 bool begin_transition(u32& out)override{out=t.animations.create(0,18,-1,0);events.push_back(1003);return out!=0;}
 bool start_game(i32 stage)override{events.insert(events.end(),{1004,stage});return true;}i32 checkpoint_key()const override{return 0;}
 bool preview_ending(i32 i)override{events.insert(events.end(),{1005,i});return true;}bool trophy_stage(i32 c,i32 a)override{events.insert(events.end(),{1006,c,a});return true;}
 bool hud_text(const HudTextDraw& out)override{captions.push_back(out);return true;}bool calendar(i64,ReplayCalendar& out)override{out=date;return true;}
};
API(title_data_create) TitleDataFixture* title_data_create(TitleMainFixture* t){return new TitleDataFixture(*t);}
API(title_data_delete) void title_data_delete(TitleDataFixture* p){delete p;}
API(title_data_import) bool title_data_import(TitleDataFixture* p,const u8* chars,const u8* settings){return p->records.import_blocks(chars,settings);}
API(title_data_update) bool title_data_update(TitleDataFixture* p,u32 pressed,u32 repeated,u32 kind){p->texts.clear();p->events.clear();return kind==1?p->practice.update(pressed,repeated):kind==2?p->trophies.update(pressed,repeated):p->data.update(pressed,repeated);}
API(title_data_draw) bool title_data_draw(TitleDataFixture* p,u32 kind){p->captions.clear();return kind==1?p->practice.draw(*p):kind==2?p->trophies.draw(*p):kind==3?p->t.stage.draw(p->records.game,*p):p->data.draw(*p,*p);}
API(title_data_field) void* title_data_field(TitleDataFixture* p,u32 k){switch(k){case 0:return &p->data.ranks;case 1:return &p->data.pages;case 2:return &p->data.kind;case 3:return &p->practice.stage;case 4:return p->practice.spells.data();case 5:return &p->memory;case 6:return &p->keys;case 7:return &p->trophies.saved_selection;case 8:return &p->trophies.description_age;default:return nullptr;}}
API(title_data_text_count) u32 title_data_text_count(TitleDataFixture* p){return p->texts.size();}
API(title_data_text_bytes) const char* title_data_text_bytes(TitleDataFixture* p,u32 i){return p->texts[i].request.bytes.c_str();}
API(title_data_text_value) i32 title_data_text_value(TitleDataFixture* p,u32 i,u32 k){auto& a=p->texts[i];return k==0?a.handle:k==1?a.request.color:k==2?a.request.font:a.request.centered;}
API(title_data_events) const i32* title_data_events(TitleDataFixture* p){return p->events.data();}
API(title_data_event_count) u32 title_data_event_count(TitleDataFixture* p){return p->events.size();}
API(title_data_error) const char* title_data_error(TitleDataFixture* p,u32 kind){return kind==1?p->practice.error.c_str():kind==2?p->trophies.error.c_str():p->data.error.c_str();}
API(title_data_trophies) bool title_data_trophies(TitleDataFixture* p,const u8* bytes,u32 n,Rng* visual){return p->catalog.open(bytes,n,visual);}
API(title_data_caption_count) u32 title_data_caption_count(TitleDataFixture* p){return p->captions.size();}
API(title_data_caption) const char* title_data_caption(TitleDataFixture* p,u32 i){return p->captions[i].text.c_str();}
API(title_data_caption_style) const HudTextStyle* title_data_caption_style(TitleDataFixture* p,u32 i){return &p->captions[i].style;}
API(title_data_caption_position) const Vec3* title_data_caption_position(TitleDataFixture* p,u32 i){return &p->captions[i].position;}
